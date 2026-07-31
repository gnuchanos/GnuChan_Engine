# OpenGL 4.3 Driver Implementation

- [ ] Create `drivers/gl43/` directory structure with all rasterizer files
- [ ] Create rasterizer_gl43.h / rasterizer_gl43.cpp (main rasterizer class)
- [ ] Create shader_gl43.h / shader_gl43.cpp (shader management)
- [ ] Create rasterizer_storage_gl43.h / rasterizer_storage_gl43.cpp (textures, meshes, etc.)
- [ ] Create rasterizer_scene_gl43.h / rasterizer_scene_gl43.cpp (scene rendering)
- [ ] Create rasterizer_canvas_gl43.h / rasterizer_canvas_gl43.cpp & base (2D canvas)
- [ ] Create shader_compiler_gl43.h / shader_compiler_gl43.cpp
- [ ] Create SCsub build files
- [ ] Update `drivers/SCsub` to include gl43
- [ ] Update `register_driver_types.cpp` to register GL43 driver
- [ ] Update `main.cpp` to add "GL43" as a video driver option
- [ ] Verify with `scons` build
