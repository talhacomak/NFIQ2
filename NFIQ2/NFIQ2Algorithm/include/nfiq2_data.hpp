/*
 * This file is part of NIST Fingerprint Image Quality (NFIQ) 2. For more
 * information on this project, refer to:
 *   - https://nist.gov/services-resources/software/nfiq2
 *   - https://github.com/usnistgov/NFIQ2
 *
 * This work is in the public domain. For complete licensing details, refer to:
 *   - https://github.com/usnistgov/NFIQ2/blob/master/LICENSE.md
 */

#ifndef NFIQ2_DATA_HPP_
#define NFIQ2_DATA_HPP_

#include <nfiq2_exception.hpp>

#include <cstdint>
#include <cstring>
#include <fstream>
#include <string>
#include <vector>

namespace NFIQ2 {

/** Character traits for binary strings, including bytes above 0x7f. */
struct DataTraits {
	using char_type = uint8_t;
	using int_type = std::char_traits<char>::int_type;
	using off_type = std::char_traits<char>::off_type;
	using pos_type = std::char_traits<char>::pos_type;
	using state_type = std::char_traits<char>::state_type;

	static void assign(char_type &dest, const char_type &value) noexcept
	{
		dest = value;
	}
	static char_type *assign(char_type *dest, size_t count, char_type value)
	{
		if (count != 0)
			std::memset(dest, value, count);
		return dest;
	}
	static constexpr bool eq(char_type lhs, char_type rhs) noexcept
	{
		return lhs == rhs;
	}
	static constexpr bool lt(char_type lhs, char_type rhs) noexcept
	{
		return lhs < rhs;
	}
	static int compare(const char_type *lhs, const char_type *rhs,
	    size_t count)
	{
		return count == 0 ? 0 : std::memcmp(lhs, rhs, count);
	}
	static size_t length(const char_type *str)
	{
		size_t count = 0;
		while (str[count] != 0)
			++count;
		return count;
	}
	static const char_type *find(const char_type *str, size_t count,
	    const char_type &value)
	{
		return count == 0 ? nullptr :
				    static_cast<const char_type *>(
					std::memchr(str, value, count));
	}
	static char_type *move(char_type *dest, const char_type *src,
	    size_t count)
	{
		if (count != 0)
			std::memmove(dest, src, count);
		return dest;
	}
	static char_type *copy(char_type *dest, const char_type *src,
	    size_t count)
	{
		if (count != 0)
			std::memcpy(dest, src, count);
		return dest;
	}
	static constexpr int_type to_int_type(char_type value) noexcept
	{
		return value;
	}
	static constexpr char_type to_char_type(int_type value) noexcept
	{
		return static_cast<char_type>(value);
	}
	static constexpr bool eq_int_type(int_type lhs, int_type rhs) noexcept
	{
		return lhs == rhs;
	}
	static constexpr int_type eof() noexcept
	{
		return std::char_traits<char>::eof();
	}
	static constexpr int_type not_eof(int_type value) noexcept
	{
		return eq_int_type(value, eof()) ? 0 : value;
	}
};

/** Binary string with explicit traits, supported by modern libc++. */
using DataString = std::basic_string<uint8_t, DataTraits>;

/** Binary data */
class Data : public DataString {
    public:
	/** Default Data constructor. */
	Data();

	/**
	 * @brief
	 * Constructor with available pointer to data.
	 *
	 * @param pData
	 * Data pointer.
	 *
	 * @param dataSize
	 * Size of data at data pointer.
	 */
	Data(const uint8_t *pData, uint32_t dataSize);

	/** Copy constructor. */
	Data(const Data &otherData);

	/**
	 * @brief
	 * Constructor with string-based data.
	 *
	 * @param otherData
	 * Binary data in string format.
	 */
	explicit Data(const DataString &otherData);

	/** Copy bytes from strings using other traits or allocators. */
	template <typename Traits, typename Allocator>
	explicit Data(
	    const std::basic_string<uint8_t, Traits, Allocator> &otherData)
	    : DataString(otherData.data(), otherData.size())
	{
	}

	/** Destructor. */
	virtual ~Data();

	/**
	 * @brief
	 * Reads the content from the a file into this object.
	 *
	 * @param filename
	 * The path of the file that will be read.
	 *
	 * @throws NFIQ2::Exception
	 * File cannot be opened.
	 */
	void readFromFile(const std::string &filename);

	/**
	 * @brief
	 * Writes the content to a file.
	 *
	 * @param filename
	 * The path of the file that will be written to.
	 *
	 * @throws NFIQ2::Exception
	 * File cannot be opened.
	 */
	void writeToFile(const std::string &filename) const;

	/**
	 * @brief
	 * Generates a string in hexadecimal format of the buffer.
	 *
	 * @return
	 * The content of the buffer as hexadecimal string.
	 *
	 * @throws NFIQ2::Exception
	 * No data available in buffer.
	 */
	std::string toHexString() const;

	/**
	 * @brief
	 * Imports data from a Base64 encoded string.
	 *
	 * @param base64String
	 * The Base64 encoded string.
	 *
	 * @throws NFIQ2::Exception
	 * If invalid character is detected in string.
	 */
	void fromBase64String(const std::string &base64String);

	/**
	 * @brief
	 * Generates a string in Base64 format of the buffer.
	 *
	 * @return
	 * The content of the buffer as Base64 encoded string.
	 */
	std::string toBase64String() const;
};
} // namespace NFIQ

#endif /* NFIQ2_DATA_HPP_ */
