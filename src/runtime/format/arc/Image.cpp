#include <fast_io.h>

#include <base/Owned.hpp>
#include <resource/Image.hpp>
#include <format/magic.hpp>
#include "Image.hpp"

NAMESPACE_BEGIN()

struct bits
{
    unsigned long curbits;
    unsigned long curbyte;
    unsigned char cache;
    unsigned char* stream;
    unsigned long stream_length;
};

void bits_init(struct bits* bits, unsigned char* stream, unsigned long stream_length)
{
    memset(bits, 0, sizeof(*bits));
    bits->stream = stream;
    bits->stream_length = stream_length;
}

int bits_get_high(struct bits* bits, unsigned int req_bits, unsigned int* retval)
{
    unsigned int bits_value = 0;

    while (req_bits > 0)
    {
        unsigned int req;

        if (!bits->curbits)
        {
            if (bits->curbyte >= bits->stream_length)
                return -1;
            bits->cache = bits->stream[bits->curbyte++];
            bits->curbits = 8;
        }

        if (bits->curbits < req_bits)
            req = bits->curbits;
        else
            req = req_bits;

        bits_value <<= req;
        bits_value |= bits->cache >> (bits->curbits - req);
        bits->cache &= (1 << (bits->curbits - req)) - 1;
        req_bits -= req;
        bits->curbits -= req;
    }
    *retval = bits_value;
    return 0;
}

int bit_get_high(struct bits* bits, void* retval)
{
    return bits_get_high(bits, 1, (unsigned int*)retval);
}

typedef struct
{
    unsigned int valid;             /* 是否有效的标记 */
    unsigned int weight;            /* 权值 */
    unsigned int is_parent;         /* 是否是父节点 */
    unsigned int parent_index;      /* 父节点索引 */
    unsigned int left_child_index;  /* 左子节点索引 */
    unsigned int right_child_index; /* 右子节点索引 */
} bg_huffman_node;

static unsigned int bg_huffman_decompress(bg_huffman_node* huffman_nodes,
                                          unsigned int root_node_index,
                                          unsigned char* uncompr, unsigned int uncomprlen,
                                          unsigned char* compr, unsigned int comprlen)
{
    struct bits bits;

    bits_init(&bits, compr, comprlen);
    unsigned int act_uncomprlen;
    for (act_uncomprlen = 0; act_uncomprlen < uncomprlen; act_uncomprlen++)
    {
        unsigned char child;
        unsigned int node_index;

        node_index = root_node_index;
        do {
            if (bit_get_high(&bits, &child))
                goto out;

            if (!child)
                node_index = huffman_nodes[node_index].left_child_index;
            else
                node_index = huffman_nodes[node_index].right_child_index;
        } while (huffman_nodes[node_index].is_parent);

        uncompr[act_uncomprlen] = node_index;
    }
out:
    return act_uncomprlen;
}

static unsigned int bg_create_huffman_tree(bg_huffman_node* nodes, u32* leaf_nodes_weight)
{
    unsigned int parent_node_index = 256; /* 父节点从nodes[]的256处开始 */
    bg_huffman_node* parent_node = &nodes[parent_node_index];
    unsigned int root_node_weight = 0; /* 根节点权值 */
    unsigned int i;

    /* 初始化叶节点 */
    for (i = 0; i < 256; i++)
    {
        nodes[i].valid = !!leaf_nodes_weight[i];
        nodes[i].weight = leaf_nodes_weight[i];
        nodes[i].is_parent = 0;
        root_node_weight += nodes[i].weight;
    }

    while (1)
    {
        unsigned int child_node_index[2];

        /* 创建左右子节点 */
        for (i = 0; i < 2; i++)
        {
            unsigned int min_weight;

            min_weight = -1;
            child_node_index[i] = -1;
            /* 遍历nodes[], 找到weight最小的2个节点作为子节点 */
            for (unsigned int n = 0; n < parent_node_index; n++)
            {
                if (nodes[n].valid)
                {
                    if (nodes[n].weight < min_weight)
                    {
                        min_weight = nodes[n].weight;
                        child_node_index[i] = n;
                    }
                }
            }
            /* 被找到的子节点标记为无效，以便不参与接下来的查找 */
            nodes[child_node_index[i]].valid = 0;
            nodes[child_node_index[i]].parent_index = parent_node_index;
        }
        /* 创建父节点 */
        parent_node->valid = 1;
        parent_node->is_parent = 1;
        parent_node->left_child_index = child_node_index[0];
        parent_node->right_child_index = child_node_index[1];
        parent_node->weight = nodes[parent_node->left_child_index].weight
                              + nodes[parent_node->right_child_index].weight;
        if (parent_node->weight == root_node_weight)
            break;
        parent_node = &nodes[++parent_node_index];
    }

    return parent_node_index;
}

static unsigned int zero_decompress(unsigned char* uncompr, unsigned int uncomprlen,
                                    unsigned char* compr, unsigned int comprlen)
{
    unsigned int act_uncomprlen = 0;
    int dec_zero = 0;
    unsigned int curbyte = 0;

    while (1)
    {
        unsigned int bits = 0;
        unsigned int copy_bytes = 0;
        unsigned char code;

        do {
            if (curbyte >= comprlen)
                goto out;

            code = compr[curbyte++];
            copy_bytes |= (code & 0x7f) << bits;
            bits += 7;
        } while (code & 0x80);

        if (act_uncomprlen + copy_bytes > uncomprlen)
            break;
        if (!dec_zero && (curbyte + copy_bytes > comprlen))
            break;

        if (!dec_zero)
        {
            memcpy(&uncompr[act_uncomprlen], &compr[curbyte], copy_bytes);
            curbyte += copy_bytes;
            dec_zero = 1;
        }
        else
        {
            memset(&uncompr[act_uncomprlen], 0, copy_bytes);
            dec_zero = 0;
        }
        act_uncomprlen += copy_bytes;
    }
out:
    return act_uncomprlen;
}

static void bg_average_defilting(unsigned char* dib_buf, unsigned int width,
                                 unsigned int height, unsigned int bpp)
{
    unsigned int line_len = width * bpp;

    for (unsigned int y = 0; y < height; y++)
    {
        for (unsigned int x = 0; x < width; x++)
        {
            for (unsigned int p = 0; p < bpp; p++)
            {
                unsigned int a, b;
                unsigned int avg;

                b = y ? dib_buf[(y - 1) * line_len + x * bpp + p] : -1;
                a = x ? dib_buf[y * line_len + (x - 1) * bpp + p] : -1;
                avg = 0;

                if (a != -1)
                    avg += a;
                if (b != -1)
                    avg += b;
                if (a != -1 && b != -1)
                    avg /= 2;

                dib_buf[y * line_len + x * bpp + p] += avg;
            }
        }
    }
}

static void decode_bg(unsigned char* enc_buf, unsigned int enc_buf_len,
                      unsigned long key, unsigned char* ret_sum, unsigned char* ret_xor)
{
    unsigned char sum = 0;
    unsigned char _xor = 0;

    unsigned long a = 0, b = 0, c = 0;
    for (unsigned long i = 0; i < enc_buf_len; i++)
    {
        a = (key & 0xffff) * 20021;
        b = ((key >> 16) & 0xffff) * 20021;
        c = key * 346 + b;
        c += (a >> 16) & 0xffff;
        key = (c << 16) + (a & 0xffff) + 1;
        // key每次都会被更新
        (*enc_buf) -= (char)c; // 原始数据 - 结果低8位
        enc_buf++;
    }

    *ret_sum = sum;
    *ret_xor = _xor;
}

static unsigned int bg_decompress(const arc::header::Image* bg_header, unsigned int bg_len,
                                  unsigned char* image_buf, unsigned int image_size)
{
    unsigned int act_uncomprlen = 0;
    unsigned int i;
    unsigned char* enc_buf = (unsigned char*)(bg_header + 1);

    /* 解密叶节点权值 */
    unsigned char sum;
    unsigned char _xor;
    decode_bg(enc_buf, bg_header->encode_length, bg_header->key, &sum, &_xor);

    /*
    if (sum != bg_header->sum_check || _xor != bg_header->xor_check)
        return 0;
    */

    /* 初始化叶节点权值 */
    u32 leaf_nodes_weight[256];
    unsigned int curbyte = 0;
    for (i = 0; i < 256; i++)
    {
        unsigned int bits = 0;
        u32 weight = 0;
        unsigned char code;

        do {
            if (curbyte >= bg_header->encode_length)
                return 0;
            code = enc_buf[curbyte++];
            weight |= (code & 0x7f) << bits;
            bits += 7;
        } while (code & 0x80);
        leaf_nodes_weight[i] = weight;
    }

    bg_huffman_node nodes[511];
    unsigned int root_node_index = bg_create_huffman_tree(nodes, leaf_nodes_weight);
    unsigned char* zero_compr = (unsigned char*)malloc(bg_header->zero_comprlen);
    if (!zero_compr)
        return 0;

    unsigned char* compr = enc_buf + bg_header->encode_length;
    unsigned int comprlen = bg_len - sizeof(arc::header::Image) - bg_header->encode_length;
    act_uncomprlen = bg_huffman_decompress(nodes, root_node_index,
                                           zero_compr, bg_header->zero_comprlen, compr, comprlen);
    if (act_uncomprlen != bg_header->zero_comprlen)
    {
        free(zero_compr);
        return 0;
    }

    act_uncomprlen = zero_decompress(image_buf, image_size, zero_compr, bg_header->zero_comprlen);
    free(zero_compr);

    bg_average_defilting(image_buf, bg_header->width, bg_header->height, bg_header->color_depth / 8);

    return act_uncomprlen;
}

NAMESPACE_END()

NAMESPACE_BEGIN(arc)

NAMESPACE_BEGIN(header)

bool is_image(const Image* header) noexcept
{
    // end with '\0'
    auto const magic = std::string_view{reinterpret_cast<const char*>(header->magic), magic::arc_image.size()};
    return magic == magic::arc_image;
}

NAMESPACE_END(header)

void parse_image(const header::Image* header, usize size) noexcept
{
    auto image_size = header->width * header->height * header->color_depth / 8;
    auto buffer = Owned<u8[]>::make_uninitialize(image_size);
    auto acture_size = bg_decompress(header, size, buffer.get(), image_size);
    perrln(acture_size, " ", image_size);
    if (acture_size == image_size)
    {

    }
    auto img = Owned<u8[]>::make_uninitialize(image_size + header->width * header->height);
    for (auto i = usize{}, j = usize{}; j < image_size; i += 4, j += 3)
    {
        img.get()[i] = buffer.get()[j + 2];
        img.get()[i + 1] = buffer.get()[j + 1];
        img.get()[i + 2] = buffer.get()[j];
        img.get()[i + 3] = 255;
    }
    ::write_png("test.png", img.get(), header->width, header->height);
    ::exit(0);
}

NAMESPACE_END(arc)
