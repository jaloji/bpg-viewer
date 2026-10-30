cmake_minimum_required(VERSION 3.28)
project(bpg-viewer VERSION 1.0.0 LANGUAGES C CXX)

if(NOT WIN32)
    message(FATAL_ERROR "bpg-viewer builds a Windows WIC codec and needs Windows.")
endif()

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# Static CRT: the DLL is loaded into Explorer, so it must not need a VC++ redistributable.
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

# All binaries land in one folder next to each other.
foreach(kind RUNTIME LIBRARY ARCHIVE)
    set(CMAKE_${kind}_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
    foreach(config Debug Release RelWithDebInfo MinSizeRel)
        string(TOUPPER ${config} CONFIG)
        set(CMAKE_${kind}_OUTPUT_DIRECTORY_${CONFIG} ${CMAKE_BINARY_DIR}/bin)
    endforeach()
endforeach()

# libbpg decoder by Fabrice Bellard (libbpg.c: MIT; HEVC decoder from FFmpeg: LGPL 2.1+), pinned release.
include(FetchContent)
FetchContent_Declare(libbpg
    URL https://bellard.org/bpg/libbpg-0.9.8.tar.gz
    URL_HASH SHA256=c0788e23bdf1a7d36cb4424ccb2fae4c7789ac94949563c4ad0e2569d3bf0095)
FetchContent_GetProperties(libbpg)
if(NOT libbpg_POPULATED)
    FetchContent_Populate(libbpg)
endif()

# Same object list as libbpg's Makefile (LIBBPG_OBJS): decoder only, no encoder (x265 is GPL).
add_library(bpg_decoder STATIC
    ${libbpg_SOURCE_DIR}/libbpg.c
    ${libbpg_SOURCE_DIR}/libavcodec/hevc_cabac.c
    ${libbpg_SOURCE_DIR}/libavcodec/hevc_filter.c
    ${libbpg_SOURCE_DIR}/libavcodec/hevc.c
    ${libbpg_SOURCE_DIR}/libavcodec/hevcpred.c
    ${libbpg_SOURCE_DIR}/libavcodec/hevc_refs.c
    ${libbpg_SOURCE_DIR}/libavcodec/hevcdsp.c
    ${libbpg_SOURCE_DIR}/libavcodec/hevc_mvs.c
    ${libbpg_SOURCE_DIR}/libavcodec/hevc_ps.c
    ${libbpg_SOURCE_DIR}/libavcodec/hevc_sei.c
    ${libbpg_SOURCE_DIR}/libavcodec/utils.c
    ${libbpg_SOURCE_DIR}/libavcodec/cabac.c
    ${libbpg_SOURCE_DIR}/libavcodec/golomb.c
    ${libbpg_SOURCE_DIR}/libavcodec/videodsp.c
    ${libbpg_SOURCE_DIR}/libavutil/mem.c
    ${libbpg_SOURCE_DIR}/libavutil/buffer.c
    ${libbpg_SOURCE_DIR}/libavutil/log2_tab.c
    ${libbpg_SOURCE_DIR}/libavutil/frame.c
    ${libbpg_SOURCE_DIR}/libavutil/pixdesc.c
    ${libbpg_SOURCE_DIR}/libavutil/md5.c
    src/core/libbpg_compat.c)
target_include_directories(bpg_decoder PUBLIC ${libbpg_SOURCE_DIR})
target_compile_definitions(bpg_decoder PRIVATE HAVE_AV_CONFIG_H USE_VAR_BIT_DEPTH USE_PRED CONFIG_BPG_VERSION="0.9.8")
# libbpg's config.h was generated for GCC (GCC atomics, inline asm keyword): clang-cl, shipped with
# Visual Studio, understands it while producing MSVC-compatible objects.
set_target_properties(bpg_decoder PROPERTIES VS_PLATFORM_TOOLSET ClangCL)
target_compile_options(bpg_decoder PRIVATE -w)  # third-party code, built as is
# FFmpeg's intmath.h calls _BitScanReverse/_BitScanForward when _MSC_VER is set, without including intrin.h.
target_compile_options(bpg_decoder PRIVATE /FIintrin.h)

function(bpg_warnings target)
    target_compile_definitions(${target} PRIVATE UNICODE _UNICODE NOMINMAX _CRT_SECURE_NO_WARNINGS)
    if(MSVC)
        target_compile_options(${target} PRIVATE /W4 /utf-8)
    endif()
endfunction()

# BPG reader shared by every component.
add_library(bpg_core STATIC src/core/bpg.cpp src/core/bpg.h)
target_include_directories(bpg_core PUBLIC src/core)
target_link_libraries(bpg_core PUBLIC bpg_decoder)
bpg_warnings(bpg_core)

# Windows Imaging Component codec: Explorer thumbnails, preview pane, photo viewers.
add_library(BpgWic SHARED src/wic/bpgwic.cpp src/wic/bpgwic.def)
target_link_libraries(BpgWic PRIVATE bpg_core windowscodecs ole32 advapi32 shell32)
bpg_warnings(BpgWic)

# Command-line tools.
add_executable(bpg2png tools/bpg2png.cpp)
target_link_libraries(bpg2png PRIVATE bpg_core)
bpg_warnings(bpg2png)

add_executable(wictest tools/wictest.cpp)
target_link_libraries(wictest PRIVATE windowscodecs ole32)
bpg_warnings(wictest)

# Release package (cmake --install build --prefix dist).
install(TARGETS BpgWic bpg2png wictest RUNTIME DESTINATION . LIBRARY DESTINATION .)
install(FILES scripts/install.bat scripts/uninstall.bat scripts/clear-thumbnails.bat README.md LICENSE
        DESTINATION .)
# libbpg is linked statically: its licenses ship with the binaries (see THIRD_PARTY_NOTICES.md).
install(FILES third_party/LICENSE-LGPL-2.1.txt THIRD_PARTY_NOTICES.md DESTINATION .)
