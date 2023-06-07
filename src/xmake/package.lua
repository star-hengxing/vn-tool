-- dev

add_requires("fast_io")
add_requires("libpng")
add_requires("toml++", {configs = {header_only = true}})

if is_plat("windows") and is_mode("release") then
    add_requires("vc-ltl5")
end
