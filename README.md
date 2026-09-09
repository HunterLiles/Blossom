# Blossom Vulkan

A small data-oriented Vulkan sandbox using GLFW, Dear ImGui, and HLSL shaders.

```sh
./run.sh
```

Controls: click Scene or Game, then `Tab` to capture input. Use `WASD`, `Space`, `Ctrl`, and the mouse. `Escape` opens the exit prompt.

Rebuild shader artifacts with `cmake --build build --target shaders` while the application is running. Blossom detects the changed SPIR-V files and reloads the graphics pipelines; use `F5` or the **Reload shaders** button to retry manually. Invalid replacements leave the current pipelines active.

Use the **Projects** panel to create or open a self-contained game project. Blossom stores projects in `projects/<name>/` with `src/`, `assets/`, and a `blossom.project` descriptor; the file browser shows only the active project.
