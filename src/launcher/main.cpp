#include <csignal>
#include <string_view>

#include "Monitor.hpp"

inline Monitor monitor;

void sigint_callback(int signum) noexcept
{
    monitor.write_programs();
    std::exit(signum);
}

int main(int argc, char** argv)
{
    auto config_filename = std::string_view{"vn-tool.toml"};

    std::signal(SIGINT, sigint_callback);
    std::signal(SIGTERM, sigint_callback);

    if (argc >= 2)
    {
        config_filename = argv[1];
    }

    if (!monitor.read_programs(config_filename))
        return -1;

    monitor.run();
}
