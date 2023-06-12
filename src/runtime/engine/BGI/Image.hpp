#pragma once

#include <base/base.hpp>

NAMESPACE_BEGIN(BGI)

// Compressed image, rgb(jpeg) + alpha
struct Image_1
{
public:
    u8 magic[16];
    u16 width;
    u16 height;
    u32 color_depth;

    u32 reserved0[2];

    u32 zero_count;
    u32 key;
    u32 encode_size;
    u8 sum_check;
    u8 xor_check;

    u16 version;

public:
    static bool is_valid(const Image_1* image) noexcept;
};

static_assert(sizeof(Image_1) == 48);

NAMESPACE_END(BGI)
