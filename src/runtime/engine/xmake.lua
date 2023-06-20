target("BGI")
    add_rules("module.component")
    add_files("BGI/*.cpp")
    add_headerfiles("BGI/*.hpp")

    add_packages("fast_io", {public = true})
