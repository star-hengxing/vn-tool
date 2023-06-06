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

std::wstring string2wstring(std::string_view str) noexcept;

void handle(unsafe::buffer_view<Program> programs) noexcept;

fixed_buffer<Program> read(const std::string_view filename) noexcept;

void save(const std::string_view filename, unsafe::buffer_view<Program> programs) noexcept;
