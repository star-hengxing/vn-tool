#pragma once

#include <string_view>

#include <fast_io.h>

#include <base/base.hpp>
#include <base/Owned.hpp>
#include "Image.hpp"

NAMESPACE_BEGIN(BGI)

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

struct Data
{
public:
    u32 size = 0;
    const Image_1* metadata = nullptr;

public:
    bool empty() const noexcept
    {
        return size == 0 || metadata == nullptr;
    }
};

struct File
{
public:
    fast_io::native_file_loader data;

    u32 entry_size = 0;
    Owned<std::string_view[]> entry_names;

public:
    static File read(const std::string_view filename);

    static bool is_arc(const Header& header) noexcept;

    Data find(usize entry_id) const noexcept;

    Owned<u8> read(const Image_1& image, usize size) const noexcept;
};

NAMESPACE_END(BGI)
