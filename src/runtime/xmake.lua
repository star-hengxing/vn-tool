includes("engine")

target("platform")
    add_rules("module.component")
    add_files("platform/*.cpp")
    add_headerfiles("platform/*.hpp")

    if is_plat("windows") then
        add_defines("WIN32_LEAN_AND_MEAN", {public = true})
        add_syslinks("user32")
    end

    add_packages("fast_io", {public = true})

target("resource")
    add_rules("module.component")
    add_files("resource/*.cpp")
    add_headerfiles("resource/*.hpp")

    add_packages("fast_io", "libpng")
