#include <csignal>
#include <string_view>

#include <platform/os.hpp>
#include "tool.hpp"

static fixed_buffer<Program> programs;
static std::string_view config_file = "vn-tool.toml";
// ms
static auto constexpr SLEEP_TIME = 1000;
// s
static auto constexpr SAVE_TIME = 60;

void sigint_callback(int signum) noexcept
{
    ::write(config_file, {programs.data.get(), programs.size});
    std::exit(signum);
}

int main(int argc, char** argv)
{
    std::signal(SIGINT, sigint_callback);
    std::signal(SIGTERM, sigint_callback);

    if (argc >= 2)
    {
        config_file = argv[1];
    }

    programs = ::read(config_file);
    if (programs.size == 0)
    {
        return -1;
    }

    u8 seconds = 0;
    auto view = unsafe::buffer_view{programs.data.get(), programs.size};
    while (true)
    {
        ::handle(view);
        if (seconds == SAVE_TIME)
        {
            seconds = 0;
            ::write(config_file, view);
        }
        else
        {
            seconds += 1;
        }

        os::sleep(SLEEP_TIME);
    }
}
