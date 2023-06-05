#pragma once

#include <base/base.hpp>

NAMESPACE_BEGIN(arc::header)

struct Image
{
    u8 magic[16];
    u16 width;
    u16 height;
    u32 color_depth;
    u32 reserved0[2];

    u32 zero_comprlen;
    u32 key;
    u32 encode_length;
    u8 sum_check;
    u8 xor_check;
    u16 reserved1;
};

static_assert(sizeof(Image) == 48);

bool is_image(const Image* header) noexcept;

NAMESPACE_END(arc::header)
