#pragma once

#include <string_view>

#include <Windows.h>

#include <base/base.hpp>

struct process_manager
{
public:
    DWORD id_list[1024]{};
    usize id_size = 0;

    TCHAR full_path[MAX_PATH];

public:
    void get_process_list() noexcept;

    bool contain(usize target_id) const noexcept;

    static DWORD get_current_process() noexcept;

    std::wstring_view get_process_filename(usize target_id) noexcept;
};
