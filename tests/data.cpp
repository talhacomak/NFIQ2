#include <nfiq2_data.hpp>

#include <cstdio>
#include <iostream>
#include <stdexcept>

static void
require(bool condition, const char *message)
{
	if (!condition)
		throw std::runtime_error(message);
}

int
main()
{
	try {
		using NFIQ2::Data;
		using NFIQ2::DataString;
		using NFIQ2::DataTraits;
		uint8_t bytes[256];
		for (unsigned int i = 0; i < 256; ++i) {
			bytes[i] = static_cast<uint8_t>(i);
			require(DataTraits::to_char_type(
				    DataTraits::to_int_type(bytes[i])) ==
				bytes[i],
			    "byte conversion");
			require(!DataTraits::eq_int_type(
				    DataTraits::to_int_type(bytes[i]),
				    DataTraits::eof()),
			    "byte collides with EOF");
		}
		require(DataTraits::not_eof(DataTraits::eof()) !=
			DataTraits::eof(),
		    "not_eof");
		require(DataTraits::not_eof(255) == 255,
		    "not_eof preserves byte");
		Data all(bytes, sizeof(bytes));
		require(all.size() == 256 && all[0] == 0 && all[255] == 255,
		    "binary constructor");
		require(all.data()[all.size()] == 0, "string terminator");
		Data copied(all);
		copied[0] = 42;
		require(all[0] == 0 && copied[0] == 42, "independent copy");
		copied = all;
		require(copied == all, "copy assignment");
		Data fromString(all.substr(127, 3));
		require(fromString.size() == 3 && fromString[0] == 127 &&
			fromString[2] == 129,
		    "substring constructor");
		struct OtherTraits : DataTraits { };
		std::basic_string<uint8_t, OtherTraits> other(bytes,
		    sizeof(bytes));
		require(Data(other) == all, "constructor with other traits");
		const uint8_t low[] = { 0x7f, 0 };
		const uint8_t high[] = { 0x80, 0 };
		require(DataString(low) < DataString(high),
		    "unsigned ordering");
		require(all.find(static_cast<uint8_t>(0xff)) == 255,
		    "find high byte");
		require(all.find(static_cast<uint8_t>(0)) == 0,
		    "find zero byte");
		DataString repeated(4, static_cast<uint8_t>(0xff));
		require(repeated.size() == 4 && repeated[3] == 255,
		    "fill assignment");
		uint8_t overlap[] = { 1, 2, 3, 4, 5 };
		DataTraits::move(overlap + 1, overlap, 4);
		require(overlap[1] == 1 && overlap[4] == 4,
		    "overlapping move right");
		DataTraits::move(overlap, overlap + 1, 4);
		require(overlap[0] == 1 && overlap[3] == 4,
		    "overlapping move left");
		require(DataTraits::compare(nullptr, nullptr, 0) == 0,
		    "empty comparison");
		require(DataTraits::find(nullptr, 0, 1) == nullptr,
		    "empty search");
		const uint8_t binary[] = { 0, 0x7f, 0x80, 0xff };
		Data sample(binary, sizeof(binary));
		require(sample.toHexString() == "00 7F 80 FF", "hex encoding");
		require(sample.toBase64String() == "AH+A/w==",
		    "base64 known vector");
		for (unsigned int size = 0; size <= 256; ++size) {
			Data original(bytes, size), decoded;
			decoded.fromBase64String(original.toBase64String());
			require(decoded == original, "base64 round trip");
		}
		Data decoded;
		decoded.fromBase64String(" A H+A/w==\n");
		require(decoded == sample, "base64 whitespace");
		bool rejected = false;
		try {
			decoded.fromBase64String("?");
		} catch (const NFIQ2::Exception &) {
			rejected = true;
		}
		require(rejected, "invalid base64 rejected");
		const std::string filename = "nfiq2-data-test.bin";
		all.writeToFile(filename);
		Data loaded;
		loaded.readFromFile(filename);
		std::remove(filename.c_str());
		require(loaded == all, "binary file round trip");
		std::cout << "PASS: binary Data and traits regression tests\n";
		return 0;
	} catch (const std::exception &error) {
		std::cerr << error.what() << '\n';
		return 1;
	}
}
