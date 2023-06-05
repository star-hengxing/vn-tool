#include <string>
#include <string_view>

#include <fast_io.h>

#include <base/range.hpp>
#include <base/Owned.hpp>
#include <format/magic.hpp>
#include "arc.hpp"

NAMESPACE_BEGIN(arc)

bool is_arc(const Header* header) noexcept
{
    auto const magic = std::string_view{reinterpret_cast<const char*>(header->magic), std::size(header->magic)};
    return magic == magic::arc;
}

void dump(const std::string_view file) noexcept
{
    auto header = reinterpret_cast<const Header*>(file.data());
    if (!is_arc(header)) [[unlikely]]
        return;

    perrln(fast_io::concat("entry_size: ", header->entry_size));
    if (file.size() < sizeof(Header) + sizeof(header::Image) * header->entry_size) [[unlikely]]
    {
        perr("invalid arc file\n");
        return;
    }

    auto total_chunk_size = usize{};
    auto ptr = file.data() + sizeof(Header);
    // cache offset and size
    auto entries = Owned<u32[]>::make_uninitialize(header->entry_size * 2);
    for (auto i : range(header->entry_size))
    {
        auto const entry = reinterpret_cast<const Entry*>(ptr);
        entries[i * 2] = entry->offset;
        entries[i * 2 + 1] = entry->size;
        total_chunk_size += entry->size;
        // auto const name = std::string_view{reinterpret_cast<const char*>(entry->name)};
        // perrln(fast_io::concat(
        //     "name: ", name, " ",
        //     "offset: ", entry->offset, " ",
        //     "size: ", entry->size));
        ptr += sizeof(Entry);
    }

    auto const last = reinterpret_cast<const Entry*>(ptr - sizeof(Entry));
    auto const correct_file_size = sizeof(Header) + header->entry_size * sizeof(Entry) + total_chunk_size;
    if (correct_file_size != file.size() || total_chunk_size != (last->offset + last->size)) [[unlikely]]
    {
        perr("invalid arc entry header\n");
        return;
    }

    auto const end = file.data() + file.size();
    for (auto i : range(header->entry_size))
    {
        auto const current = ptr + entries[i * 2];
        auto const image = reinterpret_cast<const header::Image*>(current);
        if (current >= end || !header::is_image(image)) [[unlikely]]
        {
            perrln("invalid arc image entry: ", i);
            return;
        }

        parse_image(image, entries[i * 2 + 1]);
    }

    perrln("success");
}

void read(std::string_view filename) noexcept
{
    try
    {
        auto const file = fast_io::native_file_loader{filename};
        if (file.size() >= sizeof(Header))
        {
            dump({file.data(), file.size()});
        }
    }
    catch (fast_io::error& e)
    {
        perrln(e);
    }
}

NAMESPACE_END(arc)
