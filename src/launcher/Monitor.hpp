#pragma once

#include <string>
#include <vector>

#include <platform/process_manager.hpp>
#include "Program.hpp"

struct Monitor
{
public:
    std::string config_filename;
    // interval time for monitoring a process
    // default 1 second
    u32 interval = 1;

    u32 sleep_time = 1000; // ms
    u32 save_time = 60; // s

    process_manager manager;
    std::vector<Program> programs;

public:
    bool handle() noexcept;

    [[nodiscard]]
    bool read_programs(const std::string_view filename) noexcept;

    void write_programs(const std::string_view filename) const noexcept;

    void run() noexcept;
};
