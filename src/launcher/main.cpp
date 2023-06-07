#include <csignal>
#include <string_view>

#include <platform/os.hpp>
#include "tool.hpp"

static fixed_buffer<Program> programs;
static std::string_view config_file = "vn-tool.toml";
static auto constexpr SLEEP_TIME = 1000;

void sigint_callback(int signum) noexcept
{
    ::write(config_file, {programs.data.get(), programs.size});
    std::exit(signum);
}

int main(int argc, char** argv)
{
    signal(SIGINT, sigint_callback);
    signal(SIGTERM, sigint_callback);

    if (argc >= 2)
    {
        config_file = argv[1];
    }

    programs = ::read(config_file);
    if (programs.size == 0)
    {
        return -1;
    }

    while (true)
    {
        ::handle({programs.data.get(), programs.size});
        os::sleep(SLEEP_TIME);
    }
}
