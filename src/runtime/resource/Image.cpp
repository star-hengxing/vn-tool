#include <png.h>
#include <fast_io.h>

#include <base/Owned.hpp>
#include "Image.hpp"

bool write_png(const std::string_view filename, const u8* data, u32 width, u32 height) noexcept
{
    auto write_image = png_image
    {
        .version = PNG_IMAGE_VERSION,
        .width   = width,
        .height  = height,
        .format  = PNG_FORMAT_RGBA,
    };

    auto size = usize{};
    if (png_image_write_to_memory(&write_image, nullptr, &size, 0, data, 0, nullptr) == 0)
        return false;
    
    auto buffer = Owned<char[]>::make(size);
    if (png_image_write_to_memory(&write_image, buffer.get(), &size, 0, data, 0, nullptr) == 0)
        return false;

    auto file = fast_io::native_file{filename, fast_io::open_mode::out};
    print(file, std::string_view{buffer.get(), size});
    return true;
}
