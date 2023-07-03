#pragma once

#include <string>
#include <string_view>

std::wstring u8string_to_wstring(const std::u8string_view str) noexcept;
std::u8string wstring_to_u8string(const std::wstring_view str) noexcept;
