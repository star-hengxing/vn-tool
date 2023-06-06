#include <string_view>

#include <platform/os.hpp>
#include "tool.hpp"

int main(int argc, char** argv)
{
    std::string_view config_file = "vn-tool.toml";
    if (argc >= 2)
    {
        config_file = argv[1];
    }

    fixed_buffer<Program> programs = ::read(config_file);
    auto const size = programs.size;
    if (size == 0)
    {
        return -1;
    }

    auto const view = unsafe::buffer_view<Program>{programs.data.get(), size};
    while (true)
    {
        handle(view);
        os::sleep(1000);
    }
}
