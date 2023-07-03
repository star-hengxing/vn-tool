#include <filesystem>

#include <base/unsafe/buffer_view.hpp>
#include <platform/os.hpp>
#include "Monitor.hpp"

namespace fs = std::filesystem;

bool Monitor::handle() noexcept
{
    manager.get_process_list();
    if (manager.id_size == 0)
        return false;

    for (auto& proc : programs)
    {
        if (proc.is_empty()) [[unlikely]]
            continue;

        if (proc.is_run())
        {
            if (manager.contain(proc.pid))
            {
                proc.elapsed += this->interval;
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
            auto const target = fs::path{proc.path};
            for (auto const pid : unsafe::buffer_view{manager.id_list, manager.id_size})
            {
                if (pid == 0)
                {
                    continue;
                }

                auto const src = manager.get_process_filename(pid);
                if (!src.empty() && fs::equivalent(src, target))
                {
                    proc.pid = pid;
                    proc.start_count += 1;
                    break;
                }
            }
        }
    }
    return true;
}

void Monitor::run() noexcept
{
    u32 seconds = 0;
    while (true)
    {
        this->handle();
        if (seconds == save_time)
        {
            seconds = 0;
            this->write_programs();
        }
        else
        {
            seconds += 1;
        }

        os::sleep(sleep_time);
    }
}
