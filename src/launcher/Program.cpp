#include <iostream>
#include <fstream>
#include <string>
#include <string_view>
#include <filesystem>

#include <fast_io.h>
#include <toml++/toml.h>

#include <base/range.hpp>
#include <platform/string.hpp>
#include "Monitor.hpp"

namespace fs = std::filesystem;

bool Monitor::read_programs(const std::string_view filename) noexcept
{
    try
    {
        auto const file = fast_io::native_file_loader{filename};
        toml::table toml = toml::parse({file.data(), file.size()});
        toml::array* arr = toml.get_as<toml::array>("programs");

        auto const size = arr->size();
        if (arr && size != 0)
        {
            programs.resize(size);

            for (auto i : range(size))
            {
                auto& cur = programs[i];
                auto const node = toml["programs"][i];
                std::optional path_view = node["path"].value<std::u8string_view>();

                auto const path = fs::path{path_view.value()}.make_preferred();
                cur.path = ::u8string_to_wstring(path.u8string());
                cur.is_valid_path = fs::exists(path);
                cur.start_count = node["start_count"].value<usize>().value();
                cur.run_times = node["run_times"].value<usize>().value();
            }

            this->config_filename = filename;
            return true;
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

    return false;
}

void Monitor::write_programs(const std::string_view filename) const noexcept
{
    if (programs.empty())
        return;

    auto toml = toml::table{{"programs", toml::array{}}};
    auto vec = toml.find("programs")->second.as_array();

    vec->reserve(programs.size());
    for (auto proc : programs)
    {
        toml::table cur;
        cur.emplace("path", ::wstring_to_u8string(proc.path));
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

    std::ofstream out;
    if (!filename.empty())
    {
        out.open(filename.data());
        out << toml;
        return;
    }

    if (!config_filename.empty())
    {
        out.open(config_filename.c_str());
        out << toml;
    }
}
