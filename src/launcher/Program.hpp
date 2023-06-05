#pragma once

#include <base/base.hpp>

struct fixed_string
{
public:
    static auto constexpr MAX_SIZE = 1024;

    u8 path[MAX_SIZE];
    u16 size = 0;
};

struct Program
{
public:
    fixed_string path;
    usize start_count = 0;
    usize run_times = 0;

    // runtime
    bool is_run = false;
    usize pid = 0;
    // unit: second
    usize elapsed = 0;

public:
    bool has_id() const noexcept
    {
        return pid != 0;
    }
};
