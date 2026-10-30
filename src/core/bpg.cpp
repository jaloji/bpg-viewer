#include "bpg.h"

#include <climits>
#include <memory>

extern "C" {
#include "libbpg.h"
}

namespace bpg {

namespace {

bool fail(std::string* error, const char* message) {
    if (error) *error = message;
    return false;
}

struct DecoderDeleter {
    void operator()(BPGDecoderContext* ctx) const { bpg_decoder_close(ctx); }
};

const char* formatName(uint8_t format) {
    switch (format) {
        case BPG_FORMAT_GRAY: return "gray";
        case BPG_FORMAT_420:
        case BPG_FORMAT_420_VIDEO: return "4:2:0";
        case BPG_FORMAT_422:
        case BPG_FORMAT_422_VIDEO: return "4:2:2";
        case BPG_FORMAT_444: return "4:4:4";
        default: return "?";
    }
}

}  // namespace

bool probe(const uint8_t* data, size_t size, Info& info) {
    if (size < 4 || size > INT_MAX) return false;
    BPGImageInfo bi = {};
    if (bpg_decoder_get_info_from_buf(&bi, nullptr, data, int(size)) < 0) return false;
    if (bi.width == 0 || bi.height == 0) return false;
    info.width = bi.width;
    info.height = bi.height;
    info.bitDepth = bi.bit_depth;
    info.hasAlpha = bi.has_alpha != 0;
    info.animated = bi.has_animation != 0;
    info.colorFormat = formatName(bi.format);
    return true;
}

bool decode(const uint8_t* data, size_t size, Image& out, std::string* error) {
    if (size > INT_MAX) return fail(error, "file too large");
    std::unique_ptr<BPGDecoderContext, DecoderDeleter> ctx(bpg_decoder_open());
    if (!ctx) return fail(error, "out of memory");
    if (bpg_decoder_decode(ctx.get(), data, int(size)) < 0) return fail(error, "not a BPG image or corrupt data");

    BPGImageInfo bi = {};
    if (bpg_decoder_get_info(ctx.get(), &bi) < 0) return fail(error, "cannot read image info");
    // RGBA32 output is 8-bit with straight alpha (libbpg undoes premultiplication); CMYK is converted to RGB.
    if (bpg_decoder_start(ctx.get(), BPG_OUTPUT_FORMAT_RGBA32) < 0) return fail(error, "cannot start decoding");

    out.width = bi.width;
    out.height = bi.height;
    out.bgra.resize(size_t(bi.width) * bi.height * 4);
    for (uint32_t y = 0; y < bi.height; ++y) {
        uint8_t* row = &out.bgra[size_t(y) * bi.width * 4];
        if (bpg_decoder_get_line(ctx.get(), row) < 0) return fail(error, "corrupt image data");
        for (uint32_t x = 0; x < bi.width; ++x) std::swap(row[x * 4], row[x * 4 + 2]);  // RGBA -> BGRA
    }
    return true;
}

}  // namespace bpg
