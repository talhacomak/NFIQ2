# Android verification — 2026-09-11

The stacked Android build and example branches were verified on Windows with
JDK 21, Gradle 8.13, Android Gradle Plugin 8.11.1, SDK CMake 3.31.6, Android
NDK 29.0.14206865, ARM64, and API 21 native binaries. The application requires
Android 9/API 28 or later.

## Default external model

The final verification command was run from the repository root:

```powershell
.\examples\android\gradlew.bat -p examples\android --no-daemon `
  :nfiq2-android:assembleDebug :app:assembleDebug `
  :app:testDebugUnitTest :app:assembleDebugAndroidTest :app:lintDebug
```

The build completed successfully. It produced:

- `examples/android/app/build/outputs/apk/debug/app-debug.apk` (9,981,242 bytes).
- `NFIQ2/NFIQ2Android/build/outputs/aar/nfiq2-android-debug.aar`
  (2,224,070 bytes).
- `examples/android/app/build/outputs/apk/androidTest/debug/app-debug-androidTest.apk`
  (4,629,173 bytes).
- `build-android-example/arm64-v8a-external/install_staging/nfiq2/lib/libnfiq2.a`
  (10,830,534 bytes and 24 object files).

The APK contains exactly one `nist_plain_tir-ink.yaml` asset and one
`lib/arm64-v8a/libnfiq2_jni.so`. The AAR contains the same JNI library and the
`gov.nist.nfiq2.Nfiq2` Java class.

`libnfiq2_jni.so` is an ELF64 AArch64 shared object. It exports
`Java_gov_nist_nfiq2_Nfiq2_initialize` and
`Java_gov_nist_nfiq2_Nfiq2_score`. Its runtime dependencies are limited to the
Android system libraries `android`, `log`, `z`, `dl`, `m`, and `c`; libc++ is
linked statically. The APK passed v2 signature verification and 16 KiB
`zipalign` verification. The merged manifest requests no permissions.

Four grayscale JVM tests passed with no failures, errors, or skipped tests.
Lint reported zero errors and four warnings: two pinned dependency versions,
one backup/data-extraction recommendation, and one plural-resource suggestion.
`clang-format --dry-run --Werror` passed for the JNI source, and
`git diff --check` passed.

## Embedded model

The optional embedded configuration was also built successfully:

```powershell
.\examples\android\gradlew.bat -p examples\android --no-daemon `
  '-Pnfiq2.embedModel=true' `
  :nfiq2-android:assembleDebug :app:assembleDebug
```

Its APK is 43,124,165 bytes and its AAR is 4,591,616 bytes. The APK contains
one ARM64 JNI library and no external YAML model asset.

## `ad-hoc-group` compatibility

The complete default-model Gradle command was repeated on a temporary merge of
the stacked Android branches with `origin/ad-hoc-group`. That branch raises the
root CMake requirement to 3.28 and changes dependency lookup to standard CMake
package discovery.

The first probe exposed two integration gaps: the example still selected SDK
CMake 3.22.1, and the nested NFIQ2 projects could not discover Android's
installed OpenCV package. The final changes select SDK CMake 3.31.6 and pass
`install_staging/nfiq2/sdk/native/jni` as `OpenCV_DIR` to the NFIQ2 library and
legacy API projects. After those changes, NFIQ2, NFIQ2Api, the JNI AAR, the app
APK, the test APK, JVM tests, and lint all completed successfully on the
temporary merge.

The temporary merge had one expected textual conflict in the Android flag
forwarding block because both branches edit the same lines. The test resolution
kept the required-variable validation, ABI, platform, STL, toolchain, NDK, and
the `ad-hoc-group` `ANDROID_TARGET` forwarding. No compatibility-only commit was
left in the PR branches.

## Device limitation

`adb devices -l` reported no attached device. The ARM64 instrumentation test APK
was compiled and packaged but not executed. The test initializes the external
model from application assets, expects score 54 for the repository's
`SFinGe_Test01.pgm` fixture on two consecutive calls, checks invalid buffer and
PPI handling, and verifies that the 500 PPI source setting survives Activity
recreation. Those assertions still require an ARM64 device or emulator for
runtime confirmation.
