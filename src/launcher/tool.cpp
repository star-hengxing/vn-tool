#include <string>
#include <string_view>
#include <filesystem>

#include <Windows.h>

#include <platform/process_manager.hpp>
#include "tool.hpp"

static process_manager process_m;

void handle(unsafe::buffer_view<Program> programs) noexcept
{
    process_m.get_process_list();
    if (process_m.id_size == 0)
    {
        std::exit(-2);
    }

    for (auto& proc : programs)
    {
        if (proc.is_empty()) [[unlikely]]
            continue;

        if (proc.is_run())
        {
            if (process_m.contain(proc.pid))
            {
                // +1s
                proc.elapsed += 1;
            }
            else
            {
                proc.run_times += proc.elapsed;
                proc.elapsed = 0;
                proc.pid = 0;
            }
        }
        else
        {
            auto const target = std::filesystem::path{proc.path};
            auto const list = unsafe::buffer_view{process_m.id_list, process_m.id_size};
            for (auto const pid : list)
            {
                if (pid == 0)
                {
                    continue;
                }

                auto const src = process_m.get_process_filename(pid);
                if (!src.empty() && std::filesystem::equivalent(src, target))
                {
                    proc.pid = pid;
                    proc.start_count += 1;
                    break;
                }
            }
        }
    }
}

std::wstring string2wstring(const std::string_view str) noexcept
{
    if (str.empty())
        return {};

    auto const size = ::MultiByteToWideChar(CP_ACP, 0, str.data(), static_cast<int>(str.size()), nullptr, 0);
    if (size == 0)
    {
        return {};
    }

    auto ret = std::wstring(size, 0);
    if (::MultiByteToWideChar(CP_ACP, 0, str.data(), static_cast<int>(str.size()), ret.data(), size) == 0)
    {
        return {};
    }
    return ret;
}

std::string wstring2string(const std::wstring_view str) noexcept
{
    if (str.empty())
        return {};

    auto const size = ::WideCharToMultiByte(CP_ACP, 0, str.data(), static_cast<int>(str.size()), nullptr, 0, nullptr, nullptr);
    if (size == 0)
    {
        return {};
    }

    auto ret = std::string(size, 0);
    if (::WideCharToMultiByte(CP_ACP, 0, str.data(), static_cast<int>(str.size()), ret.data(), size, nullptr, nullptr) == 0)
    {
        return {};
    }
    return ret;
}