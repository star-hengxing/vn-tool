#pragma once

#include <string_view>

#include <base/basic_type.hpp>

// only support rgba 8-bit.
[[nodiscard]]
bool write_png(const std::string_view filename, const u8* data, u32 width, u32 height) noexcept;
