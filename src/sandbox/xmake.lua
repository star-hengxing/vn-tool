target("sandbox")
    add_rules("module.program")
    add_files("main.cpp")

    before_run(function (target)
        os.execv("xmake build " .. target:name())
    end)
