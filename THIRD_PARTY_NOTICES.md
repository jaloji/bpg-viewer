# Third-party notices

`BpgWic.dll` and `bpg2png.exe` contain the BPG decoder of **libbpg 0.9.8** by Fabrice Bellard, linked statically.

- Source: <https://bellard.org/bpg/libbpg-0.9.8.tar.gz>
  (SHA-256 `c0788e23bdf1a7d36cb4424ccb2fae4c7789ac94949563c4ad0e2569d3bf0095`), downloaded unmodified by `CMakeLists.txt`.
- `libbpg.c`: Copyright (c) 2014 Fabrice Bellard, MIT license (see the file header).
- `libavcodec/` and `libavutil/` (HEVC decoder from FFmpeg): GNU Lesser General Public License version 2.1 or later,
  see `LICENSE-LGPL-2.1.txt`.

Only the decoder is used. The libbpg encoder, which depends on GPL-licensed x265, is not built.

## Relinking with another libbpg (LGPL section 6)

This project is free software and its complete build is reproducible: change the `URL` / `URL_HASH` of `libbpg` in
`CMakeLists.txt` (or point `FETCHCONTENT_SOURCE_DIR_LIBBPG` to a modified copy) and run `build.bat` to produce the
binaries with any compatible version of the library.

`src/core/libbpg_compat.c` (this project, MIT) only supplies two C runtime forwarders that FFmpeg expects when built
with MSVC-compatible compilers.
