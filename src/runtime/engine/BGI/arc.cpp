#include <string_view>

#include <base/range.hpp>
#include "magic.hpp"
#include "arc.hpp"

NAMESPACE_BEGIN(BGI)

bool File::is_arc(const Header* header) noexcept
{
    auto const magic = std::string_view{reinterpret_cast<const char*>(header->magic), std::size(header->magic)};
    return magic == magic::arc;
}

File File::read(std::string_view filename)
{
    fast_io::native_file_loader file;
    try
    {
        file = fast_io::native_file_loader{filename};
    }
    catch (fast_io::error& e)
    {
        perrln(e);
        return {};
    }

    
    if (file.size() < sizeof(Header))
        return {};

    auto const header = reinterpret_cast<const Header*>(file.data());
    if (!File::is_arc(header))
        return {};

    File arc;
    arc.entry_size = header->entry_size;
    arc.entry_names = Owned<std::string_view[]>::make_uninitialize(header->entry_size);

    auto total_chunk_size = usize{};
    auto ptr = file.data() + sizeof(Header);
    for (auto i : range(header->entry_size))
    {
        auto const entry = reinterpret_cast<const Entry*>(ptr);
        auto const name = reinterpret_cast<const char*>(entry->name);
        arc.entry_names[i] = {name, std::strlen(name)};

        total_chunk_size += entry->size;
        ptr += sizeof(Entry);
    }

    auto const last = reinterpret_cast<const Entry*>(ptr - sizeof(Entry));
    auto const correct_file_size = sizeof(Header) + header->entry_size * sizeof(Entry) + total_chunk_size;
    if (correct_file_size != file.size() || total_chunk_size != (last->offset + last->size))
        return {};

    arc.data = std::move(file);
    return arc;
}

Data File::find(usize entry_id) const noexcept
{
    if (entry_id >= entry_size) [[unlikely]]
        return {};

    auto ptr = data.data() + sizeof(Header);
    auto const entry = reinterpret_cast<Entry*>(ptr) + entry_id;

    ptr += sizeof(Entry) * entry_size;
    return {entry->size, reinterpret_cast<Image_1*>(ptr + entry->offset)};
}

NAMESPACE_END(BGI)
