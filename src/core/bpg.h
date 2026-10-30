// BPG (Better Portable Graphics) reader built on Fabrice Bellard's libbpg decoder.
#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

namespace bpg {

struct Info {
    uint32_t width = 0;
    uint32_t height = 0;
    uint8_t bitDepth = 0;
    bool hasAlpha = false;
    bool animated = false;
    std::string colorFormat;  // "4:2:0", "4:4:4", "gray"...
};

struct Image {
    uint32_t width = 0;
    uint32_t height = 0;
    std::vector<uint8_t> bgra;  // top-down, straight alpha, stride = width * 4
};

// Reads the header only. Returns false when the data is not a BPG image.
bool probe(const uint8_t* data, size_t size, Info& info);

// Decodes the image (the first frame of an animation) to 8-bit BGRA.
bool decode(const uint8_t* data, size_t size, Image& out, std::string* error = nullptr);

}  // namespace bpg
