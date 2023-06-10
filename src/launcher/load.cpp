#include <iostream>
#include <fstream>
#include <filesystem>
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
        toml::array* arr = toml.get_as<toml::array>("programs");
        if (arr)
        {
            auto const size = arr->size();
            auto programs = fixed_buffer<Program>{Owned<Program[]>::make_uninitialize(size), size};

            auto const data = programs.data.get();
            for (auto i : range(size))
            {
                auto& cur = data[i];
                auto const node = toml["programs"][i];
                std::optional<std::string_view> path_view = node["path"].value<std::string_view>();

                auto const path = std::filesystem::path{path_view.value()}.make_preferred();
                cur.path = ::string2wstring(path.string());
                cur.is_valid_path = std::filesystem::exists(path);
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

void write(const std::string_view filename, const unsafe::buffer_view<Program> programs) noexcept
{
    if (programs.empty())
        return;

    auto toml = toml::table{{"programs", toml::array{}}};
    auto vec = toml.find("programs")->second.as_array();

    vec->reserve(programs.size());
    for (auto proc : programs)
    {
        toml::table cur;
        cur.emplace("path", ::wstring2string(proc.path));
        // cur.emplace("is_valid_path", proc.is_valid_path);
        cur.emplace("start_count", static_cast<isize>(proc.start_count));
        auto run_times = static_cast<isize>(proc.run_times);
        if (proc.elapsed != 0)
        {
            run_times += proc.elapsed;
        }
        cur.emplace("run_times", run_times);

        vec->push_back(std::move(cur));
    }

    auto out = std::ofstream{filename.data()};
    out << toml;
}
