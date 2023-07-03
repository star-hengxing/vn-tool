includes("engine")

target("platform")
    add_rules("module.component")
    add_files("platform/os.cpp")
    add_headerfiles("platform/os.hpp")

    if is_plat("windows") then
        add_files("platform/string.cpp")
        add_headerfiles("platform/string.hpp")

        add_files("platform/process_manager.cpp")
        add_headerfiles("platform/process_manager.hpp")

        add_defines("WIN32_LEAN_AND_MEAN", {public = true})
        add_syslinks("user32")
    end

    add_packages("fast_io", {public = true})

target("resource")
    add_rules("module.component")
    add_files("resource/*.cpp")
    add_headerfiles("resource/*.hpp")

    add_packages("fast_io", "libpng")
