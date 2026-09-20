set_project("blossom")
set_version("0.1")
set_languages("cxx17")
add_rules("plugin.compile_commands.autoupdate", {outputdir = "build"})

target("blossom")
    set_kind("binary")

    add_files(
        "src/main.cpp",
        "src/gl.c",
        "external/imgui/imgui.cpp",
        "external/imgui/imgui_draw.cpp",
        "external/imgui/imgui_tables.cpp",
        "external/imgui/imgui_widgets.cpp",
        "external/imgui/backends/imgui_impl_opengl3.cpp",
        "external/imgui/backends/imgui_impl_glfw.cpp"
    )

    add_includedirs(
        "external/imgui",
        "external/imgui/backends",
        "backend",
        "external",
        "resources",
        "backend/shaders"
    )

    add_syslinks("glfw", "GL")
