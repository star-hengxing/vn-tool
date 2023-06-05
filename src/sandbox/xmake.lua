target("sandbox")
    add_rules("module.program")
    add_files("*.cpp")

    add_deps("format")

    before_run(function (target)
        os.execv("xmake build " .. target:name())
    end)
