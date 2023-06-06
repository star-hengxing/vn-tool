#pragma once

#include <string>

#include <base/base.hpp>

struct Program
{
public:
    std::wstring path;
    usize start_count = 0;
    usize run_times = 0;

    // runtime
    // process id
    usize pid = 0;
    // unit: second
    usize elapsed = 0;

public:
    bool is_run() const noexcept
    {
        return pid != 0;
    }
};
