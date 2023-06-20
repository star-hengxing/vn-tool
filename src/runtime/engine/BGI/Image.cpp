#include <base/unsafe/buffer_view.hpp>
#include <base/Owned.hpp>
#include <base/range.hpp>
#include "magic.hpp"
#include "arc.hpp"

NAMESPACE_BEGIN()

ALWAYS_INLINE
u32 update_key(u32& key) noexcept
{
    auto const a = (key & 0xffff) * 20021;
    auto const b = ((key >> 16) & 0xffff) * 20021;
    auto const c = (key * 346 + b + (a >> 16)) & 0xffff;
    key = (c << 16) + (a & 0xffff) + 1;
    return c;
}

Owned<u8[]> dc_decode(const BGI::Image_1* image) noexcept
{
    auto sum_check = u8{};
    auto xor_check = u8{};
    auto key = image->key;

    auto const size = image->encode_size;
    auto const src = reinterpret_cast<const u8*>(image + 1);

    auto dst = Owned<u8[]>::make_uninitialize(size);
    for (auto i : range(size))
    {
        auto const data = src[i] - static_cast<u8>(update_key(key));
        sum_check += data;
        xor_check ^= data;
        dst[i] = data;
    }

    if (image->sum_check != sum_check || image->xor_check != xor_check)
        return {};

    return dst;
}

bool init_weight_table(u32 (&table)[256], const u8* buffer, usize encode_size) noexcept
{
    auto index = usize{};
    for (auto& i : table)
    {
        u8 code;
        auto weight = u32{};
        auto code_bit_size = u32{};
        do
        {
            if (index >= encode_size || code_bit_size >= 32)
                return false;

            code = buffer[index];
            index += 1;
            weight |= (code & 0x7f) << code_bit_size;
            code_bit_size += 7;
        } while ((code & 0x80) != 0);

        i = weight;
    }
    return true;
}

NAMESPACE_BEGIN(Huffman)

struct Node
{
    bool is_valid;
    bool is_parent;
    u32 weight;
    u32 left;
    u32 right;
};

struct Tree
{
public:
    Node nodes[512];
    Node* root = nullptr;

public:
    static Tree create(u32 (&table)[256]) noexcept;

    Owned<u8[]> decompress(u32 size, unsafe::buffer_view<const u8> data) noexcept;
};

Tree Tree::create(u32 (&table)[256]) noexcept
{
    Tree tree;
    Node* nodes = tree.nodes;

    auto root_node_weight = u32{};
    auto const table_size = std::size(table);
    // init
    for (auto i : range(table_size))
    {
        nodes[i].is_valid = (table[i] != 0);
        nodes[i].is_parent = false;
        nodes[i].weight = table[i];

        root_node_weight += table[i];
    }

    auto index = table_size;
    Node* parent_node = &nodes[index];
    while (true)
    {
        isize child_node_index[2]{-1, -1};
        for (auto& i : child_node_index)
        {
            u32 min_weight = 1 << (sizeof(u32) * 8 - 1);

            for (auto j : range(index))
            {
                if (nodes[j].is_valid && nodes[j].weight < min_weight)
                {
                    min_weight = nodes[j].weight;
                    i = j;
                }
            }

            if (i == -1)
            {
                continue;
            }

            nodes[i].is_valid = false;
        }

        parent_node->is_valid = true;
        parent_node->is_parent = true;
        parent_node->left = static_cast<u32>(child_node_index[0]);
        parent_node->right = static_cast<u32>(child_node_index[1]);
        parent_node->weight = nodes[parent_node->left].weight + nodes[parent_node->right].weight;

        if (parent_node->weight >= root_node_weight)
            break;
        // next node
        index += 1;
        parent_node = &nodes[index];
    }

    tree.root = parent_node;
    return tree;
}

struct bit_stream
{
public:
    const u8* data;
    const usize capacity;

    usize size = 0;
    u8 value = 0;
    u8 bits = 0;

public:
    // @return 1, 0, -1(error)
    i8 next() noexcept
    {
        if (bits == 0)
        {
            if (size >= capacity)
                return -1;

            value = data[size];
            size += 1;
            bits = 8;
        }

        auto const result = (value & 0b10000000) ? 1 : 0;
        value <<= 1;
        bits -= 1;
        return result;
    }
};

Owned<u8[]> Tree::decompress(u32 size, unsafe::buffer_view<const u8> data) noexcept
{
    auto stream = bit_stream{data.data(), data.size()};
    auto buffer = Owned<u8[]>::make_uninitialize(size);
    for (auto& i : unsafe::buffer_view{buffer.get(), size})
    {
        auto index = static_cast<usize>(root - &nodes[0]);
        do
        {
            auto const bit = stream.next();
            if (bit == -1) [[unlikely]]
                return buffer;

            index = (bit == 0) ? nodes[index].left : nodes[index].right;
        } while (nodes[index].is_parent);
        i = static_cast<u8>(index);
    }
    return buffer;
}

NAMESPACE_END(Huffman)

u32 zero_decompress(
    u8* uncompressed, u32 uncompressed_size,
    const u8* compressed, u32 compressed_size) noexcept
{
    auto index = u32{};
    auto decompressed_size = u32{};
    auto is_zero_block = false;

    while (true)
    {
        u8 code;
        auto bits = u32{};
        auto block_size = u32{};

        do
        {
            if (index >= compressed_size)
                goto end;

            code = compressed[index];
            index += 1;

            block_size |= (code & 0x7f) << bits;
            bits += 7;
        } while (code & 0b10000000);

        if (decompressed_size + block_size > uncompressed_size)
            break;
        if (!is_zero_block && (index + block_size > compressed_size))
            break;

        if (!is_zero_block)
        {
            std::memcpy(&uncompressed[decompressed_size], &compressed[index], block_size);
            index += block_size;
            is_zero_block = true;
        }
        else
        {
            std::memset(&uncompressed[decompressed_size], 0, block_size);
            is_zero_block = false;
        }
        decompressed_size += block_size;
    }
end:
    return decompressed_size;
}

void defilting_bgr(u8* buffer, u32 width, u32 height) noexcept
{
    auto dst = buffer;
    auto src = buffer + width * height;
    // bgr -> rgba
    dst[0] = src[2];
    dst[1] = src[1];
    dst[2] = src[0];
    dst[3] = 255;
    // y = 0
    for ([[maybe_unused]] auto i : range<u32>(1, width))
    {
        src += 3;

        dst[4] = src[2] + dst[0];
        dst[5] = src[1] + dst[1];
        dst[6] = src[0] + dst[2];
        dst[7] = dst[3];

        dst += 4;
    }

    for ([[maybe_unused]] auto y : range<u32>(1, height))
    {
        dst += 4;
        src += 3;
        auto last_row = dst - width * 4;
        // x = 0
        dst[0] = src[2] + last_row[0];
        dst[1] = src[1] + last_row[1];
        dst[2] = src[0] + last_row[2];
        dst[3] = last_row[3];

        for ([[maybe_unused]] auto x : range<u32>(1, width))
        {
            src += 3;
            last_row += 4;
            // left + top pixel
            dst[4] = src[2] + (dst[0] + last_row[0]) / 2;
            dst[5] = src[1] + (dst[1] + last_row[1]) / 2;
            dst[6] = src[0] + (dst[2] + last_row[2]) / 2;
            dst[7] = dst[3];

            dst += 4;
        }
    }
}

void defilting_bgra(u8* buffer, u32 width, u32 height) noexcept
{
    using std::swap;
    // bgra -> rgba
    swap(buffer[0], buffer[2]);
    // y = 0
    for ([[maybe_unused]] auto i : range<u32>(1, width))
    {
        swap(buffer[4], buffer[6]);

        buffer[4] += buffer[0];
        buffer[5] += buffer[1];
        buffer[6] += buffer[2];
        buffer[7] += buffer[3];

        buffer += 4;
    }

    for ([[maybe_unused]] auto y : range<u32>(1, height))
    {
        buffer += 4;
        auto last_row = buffer - width * 4;
        swap(buffer[0], buffer[2]);

        buffer[0] += last_row[0];
        buffer[1] += last_row[1];
        buffer[2] += last_row[2];
        buffer[3] += last_row[3];

        for ([[maybe_unused]] auto x : range<u32>(1, width))
        {
            swap(buffer[4], buffer[6]);
            last_row += 4;

            buffer[4] += (buffer[0] + last_row[0]) / 2;
            buffer[5] += (buffer[1] + last_row[1]) / 2;
            buffer[6] += (buffer[2] + last_row[2]) / 2;
            buffer[7] += (buffer[3] + last_row[3]) / 2;

            buffer += 4;
        }
    }
}

NAMESPACE_END()

NAMESPACE_BEGIN(BGI)

bool Image_1::is_valid(const Image_1& image_1) noexcept
{
    // end with '\0'
    auto const magic = std::string_view{reinterpret_cast<const char*>(image_1.magic), magic::image_1.size()};
    return magic == magic::image_1;
}

Owned<u8> File::read(const Image_1& image_1, usize size) const noexcept
{
    if (!Image_1::is_valid(image_1))
        return {};

    auto const image = std::addressof(image_1);
    if (image->version == 1)
    {
        if (size < sizeof(Image_1) + image->encode_size)
            return {};

        auto const buffer = dc_decode(image);
        if (!buffer)
            return {};

        u32 leaf_nodes_weight[256];
        if (!init_weight_table(leaf_nodes_weight, buffer.get(), image->encode_size))
            return {};

        auto tree = Huffman::Tree::create(leaf_nodes_weight);
        if (!tree.root)
            return {};

        auto const compress_data = reinterpret_cast<const u8*>(image + 1) + image->encode_size;
        auto const compress_data_size = size - sizeof(Image_1) - image->encode_size;
        auto const zero_buffer = tree.decompress(image->zero_count, {compress_data, compress_data_size});

        auto const tmp = static_cast<u32>(image->width) * image->height;
        auto const channels = image->color_depth / 8;
        // always rgba
        auto const image_size = tmp * 4;

        auto const is_4_channel = (channels == 4);
        auto image_buffer = Owned<u8[]>::make_uninitialize(image_size);
        auto const decompressed_size = zero_decompress(
            image_buffer.get() + (is_4_channel ? 0 : tmp),
            image_size - (is_4_channel ? 0 : tmp),
            zero_buffer.get(), image->zero_count);

        if (decompressed_size != tmp * channels)
            return {};

        if (is_4_channel)
        {
            defilting_bgra(image_buffer.get(), image->width, image->height);
        }
        else
        {
            defilting_bgr(image_buffer.get(), image->width, image->height);
        }
        return image_buffer;
    }
    else if (image->version == 2)
    {
        perr("unimplemented!\n");
    }
    else
    {
        perr("unknown version!\n");
    }
    return {};
}

NAMESPACE_END(BGI)
