# Blossom Vulkan

A small data-oriented Vulkan sandbox using GLFW, Dear ImGui, and HLSL shaders.

```sh
./run.sh
```

Controls: click Scene or Game, then `Tab` to capture input. Use `WASD`, `Space`, `Ctrl`, and the mouse. `Escape` opens the exit prompt.

Rebuild shader artifacts with `cmake --build build --target shaders` while the application is running. Blossom detects the changed SPIR-V files and reloads the graphics pipelines; use `F5` or the **Reload shaders** button to retry manually. Invalid replacements leave the current pipelines active.

Use the **Projects** panel to create or open a self-contained game project. Blossom stores projects in `projects/<name>/` with `src/`, `assets/`, and a `blossom.project` descriptor. In the active project, the panel can create files and folders, move or delete a selected file, and open it in embedded Neovim. All file operations are contained within that project; deletion requires confirmation.

Double-click a project file to start `nvim --embed` and open it in an ImGui popup without leaving Blossom. Click inside its grid before typing. Its isolated [Lua configuration](editor/nvim/init.lua) provides C/C++ syntax highlighting and starts `clangd` when it is available; use `Ctrl-Space` for built-in LSP completion. This first embedded-editor experiment supports the single Neovim line grid, text entry, cursor keys, Enter, Backspace, and Escape.

Within embedded Neovim, `:w` saves, `:qw` saves and closes the editor window, and `:q` discards changes and closes it.

Create a `.cpp` script (for example `src/walk.cpp`) and drag it from the project browser to the Cube script target, or select it and use **Attach selected**. This records a script attachment on the cube, which is the engine's object-to-script hook point. Script compilation/reload is intentionally not yet configured, so native C++ script behavior requires a subsequent project build/runtime integration step.
