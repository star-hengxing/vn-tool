set_project("vn-tool")

set_version("0.0.1")

set_xmakever("2.7.9")

set_allowedplats("windows")

set_warnings("all")
set_languages("c++20")

add_rules("mode.debug", "mode.release")

if is_mode("debug") then
    set_policy("build.warning", true)
    add_requireconfs("*", {configs = {shared = true}})
end

if is_plat("windows") then
    if is_mode("debug") then
        set_runtimes("MDd")
    else
        set_runtimes("MT")
    end
end
-- support utf-8 on msvc
if is_host("windows") then
    add_defines("UNICODE", "_UNICODE")
    add_cxflags("/utf-8", {tools = "cl"})
end

includes("src")
