-- dev

-- file io
add_requires("fast_io")
-- image codec
add_requires("libpng")

if is_plat("windows") and is_mode("release") then
    add_requires("vc-ltl5")
end
