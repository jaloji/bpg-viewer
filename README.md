# bpg-viewer

Windows Explorer thumbnails for [BPG](https://bellard.org/bpg/) images.

Windows has no BPG decoder. bpg-viewer adds a WIC codec built on libbpg, the reference decoder:

- Thumbnails, preview pane and photo viewers.
- All BPG variants libbpg reads (chroma formats, bit depths, alpha, color spaces). Animations show their first frame.

## Install

1. Download a release (or build it) and extract it.
2. Run `install.bat` as administrator.
3. If old icons remain, run `clear-thumbnails.bat`.

Uninstall with `uninstall.bat`. If Microsoft Defender ASR rules block the DLL, allow `C:\Program Files\BpgViewer\BpgWic.dll` with `Add-MpPreference -AttackSurfaceReductionOnlyExclusions`.

## Build

Needs CMake 3.28+ and Visual Studio 2022 with the C++ workload and "C++ Clang tools for Windows" (libbpg's FFmpeg code is compiled with clang-cl). Run `build.bat`: the package is created in `dist\`. libbpg 0.9.8 is downloaded and checked against its SHA-256.

## License

MIT for this project. The bundled libbpg decoder is MIT and LGPL 2.1+, see [THIRD_PARTY_NOTICES.md](THIRD_PARTY_NOTICES.md).
