#include <string>
#include <string_view>

#include <Windows.h>

#include "string.hpp"

std::wstring u8string_to_wstring(const std::u8string_view str) noexcept
{
    if (str.empty())
        return {};

    auto const size = ::MultiByteToWideChar(
        CP_UTF8, 0, reinterpret_cast<LPCCH>(str.data()),
        static_cast<int>(str.size()), nullptr, 0);
    if (size == 0)
    {
        return {};
    }

    auto ret = std::wstring(size, 0);
    if (::MultiByteToWideChar(
        CP_UTF8, 0, reinterpret_cast<LPCCH>(str.data()),
        static_cast<int>(str.size()), ret.data(), size) == 0)
    {
        return {};
    }
    return ret;
}

std::u8string wstring_to_u8string(const std::wstring_view str) noexcept
{
    if (str.empty())
        return {};

    auto const size = ::WideCharToMultiByte(CP_UTF8, 0, str.data(), static_cast<int>(str.size()), nullptr, 0, nullptr, nullptr);
    if (size == 0)
    {
        return {};
    }

    auto ret = std::u8string(size, 0);
    if (::WideCharToMultiByte(
        CP_UTF8, 0, str.data(), static_cast<int>(str.size()),
        reinterpret_cast<LPSTR>(ret.data()), size, nullptr, nullptr) == 0)
    {
        return {};
    }
    return ret;
}
