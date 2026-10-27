#!/usr/bin/env bash
set -e

# ==============================================================================
# Vernal Edge Android Build & Packaging Pipeline
# Targets: arm64-v8a | NativeActivity | GLES 3.0 | AAudio
# ==============================================================================

ANDROID_SDK="${ANDROID_HOME:-/home/antix/Android/Sdk}"
ANDROID_NDK="${ANDROID_NDK_HOME:-/home/antix/Android/Sdk/ndk/29.0.14206865}"
BUILD_TOOLS="${ANDROID_SDK}/build-tools/35.0.0"
PLATFORM_JAR="${ANDROID_SDK}/platforms/android-35/android.jar"
KEYSTORE="${HOME}/.android/debug.keystore"

PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "${PROJECT_ROOT}"

echo "=========================================================="
echo " Building Vernal Edge ARM64 Android Native Library"
echo "=========================================================="

# 1. Compile native code with CMake & NDK Clang
cmake -B android/build_arm64 \
  -S android/app/src/main/cpp \
  -DCMAKE_TOOLCHAIN_FILE="${ANDROID_NDK}/build/cmake/android.toolchain.cmake" \
  -DANDROID_ABI=arm64-v8a \
  -DANDROID_PLATFORM=android-26 \
  -DCMAKE_BUILD_TYPE=Release

cmake --build android/build_arm64 -j$(nproc)

echo ""
echo "=========================================================="
echo " Packaging & Signing APK"
echo "=========================================================="

rm -rf android/build_apk
mkdir -p android/build_apk/lib/arm64-v8a

# 2. Compile resources
"${BUILD_TOOLS}/aapt2" compile --dir android/app/src/main/res -o android/build_apk/res.zip

# 3. Link APK
"${BUILD_TOOLS}/aapt2" link -I "${PLATFORM_JAR}" \
  --manifest android/app/src/main/AndroidManifest.xml \
  --min-sdk-version 26 \
  --target-sdk-version 34 \
  -o android/build_apk/base.apk \
  android/build_apk/res.zip

# 4. Copy shared library and add to APK
cp android/build_arm64/libmain.so android/build_apk/lib/arm64-v8a/
(cd android/build_apk && zip -u -r base.apk lib/)

# 5. Zipalign (4-byte alignment)
"${BUILD_TOOLS}/zipalign" -p -f 4 android/build_apk/base.apk android/VernalEdge-arm64.apk

# 6. Sign APK
"${BUILD_TOOLS}/apksigner" sign \
  --ks "${KEYSTORE}" \
  --ks-pass pass:android \
  --key-pass pass:android \
  android/VernalEdge-arm64.apk

# 7. Verify
"${BUILD_TOOLS}/apksigner" verify android/VernalEdge-arm64.apk

echo ""
echo "=========================================================="
echo " BUILD SUCCESSFUL!"
echo " Output: android/VernalEdge-arm64.apk ($(du -h android/VernalEdge-arm64.apk | cut -f1))"
echo "=========================================================="
