#pragma once

#include <string_view>

#include <base/base.hpp>
#include "Image.hpp"

// v2
NAMESPACE_BEGIN(arc)

struct Header
{
    u8 magic[12];
    u32 entry_size;
};

static_assert(sizeof(Header) == 16);

struct Entry
{
    u8 name[96];
    u32 offset;
    u32 size;
    u8 reserved[24];
};

static_assert(sizeof(Entry) == 128);

bool is_arc(const Header* header) noexcept;

void dump(const std::string_view file) noexcept;

void read(std::string_view filename) noexcept;

void parse_image(const header::Image* header, usize size) noexcept;

NAMESPACE_END(arc)
