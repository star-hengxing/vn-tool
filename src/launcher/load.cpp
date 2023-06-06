#include <iostream>
#include <string>
#include <string_view>

#include <fast_io.h>
#include <toml++/toml.h>

#include <base/range.hpp>
#include "tool.hpp"

fixed_buffer<Program> read(const std::string_view filename) noexcept
{
    try
    {
        auto const file = fast_io::native_file_loader{filename};
        toml::table toml = toml::parse({file.data(), file.size()});
        toml::array* arr = toml["programs"].as_array();
        if (arr)
        {
            auto const size = arr->size();
            auto programs = fixed_buffer<Program>{Owned<Program[]>::make_uninitialize(size), size};

            auto const data = programs.data.get();
            for (auto i : range(size))
            {
                auto& cur = data[i];
                auto const node = toml["programs"][i];
                std::optional<std::string_view> path = node["path"].value<std::string_view>();

                cur.path = string2wstring(path.value());
                cur.start_count = node["start_count"].value<usize>().value();
                cur.run_times = node["run_times"].value<usize>().value();
            }

            return programs;
        }
    }
    catch (fast_io::error& err)
    {
        perrln(err);
    }
    catch (const toml::parse_error& err)
    {
        std::cerr << "Parsing failed:\n" << err << '\n';
        // auto const [column, line] = err.source().begin;
        // perr(fast_io::concat(
        //     "Error parsing file '", *err.source().path,
        //     "':\n", err.description(),
        //     "\n  [", column, ':', line, "]\n"));
    }
    catch ([[maybe_unused]] std::bad_optional_access& err)
    {
        perr("Invalid toml file\n");
    }

    return {nullptr, 0};
}

void save(const std::string_view filename, unsafe::buffer_view<Program> programs) noexcept
{}
