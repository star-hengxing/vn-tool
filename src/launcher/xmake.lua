target("launcher")
    add_rules("module.program")
    add_files("*.cpp")
    add_headerfiles("*.hpp")

    add_deps("platform")
    add_packages("toml++")
