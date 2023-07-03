target("launcher.gui")
    add_rules("module.component")
    add_rules("qt.moc") -- preload for header files

    add_files("gui/*.cpp", "gui/*.hpp")
    add_includedirs(path.join(os.scriptdir()))
    add_headerfiles("gui/*.hpp")

    add_frameworks("QtGui", "QtWidgets")

    on_load(function (target)
        local kind = target:kind()
        if kind then
            local rule = import("core.project.rule").rule("qt." .. kind)
            target:rule_add(rule)
        end
    end)

target("launcher")
    add_rules("module.program")
    add_rules("qt.widgetapp")

    add_files("*.cpp")
    add_headerfiles("*.hpp")

    if is_mode("debug") then
        -- print to console
        add_ldflags("/subsystem:console")
    end

    add_deps("platform", "launcher.gui")
    add_packages("toml++")
