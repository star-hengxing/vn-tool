target("format")
    add_rules("module.component")
    add_files("format/**.cpp")

    add_deps("resource")
    add_packages("fast_io")

target("platform")
    add_rules("module.component")
    add_files("platform/*.cpp")

    if is_plat("windows") then
        add_defines("WIN32_LEAN_AND_MEAN", {public = true})
        add_syslinks("user32")
    end

    add_packages("fast_io", {public = true})

target("resource")
    add_rules("module.component")
    add_files("resource/*.cpp")

    add_packages("fast_io", "libpng")
