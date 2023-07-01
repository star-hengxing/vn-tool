#pragma once

#include <string>

#include <base/unsafe/buffer_view.hpp>
#include <base/Owned.hpp>
#include "Program.hpp"

template <typename T>
struct fixed_buffer
{
    Owned<T[]> data;
    usize size;
};

std::wstring u8string2wstring(const std::u8string_view str) noexcept;
std::u8string wstring2u8string(const std::wstring_view str) noexcept;

void handle(unsafe::buffer_view<Program> programs) noexcept;

fixed_buffer<Program> read(const std::string_view filename) noexcept;

void write(const std::string_view filename, const unsafe::buffer_view<Program> programs) noexcept;
