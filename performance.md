# GnuChan Engine Performance Optimization Guide (Desktop Only)

## 1. Renderer (GLES3)

### 1.1 Framebuffer Allocation
- `rendering/quality/intended_usage/framebuffer_allocation` — 2 (Desktop default).

### 1.2 Shader Compilation Mode
- `rendering/gles3/shaders/shader_compilation_mode` — 0: Synchronous, 1: Asynchronous, 2: Async + Cache.
- **Recommendation:** Use 2 for release builds to avoid hitches during first shader compile.
- `rendering/gles3/shaders/max_simultaneous_compiles` — 2 default. Limits concurrent shader compiles to reduce CPU spikes.
- `rendering/gles3/shaders/shader_cache_size_mb` — 512 MB default. Larger cache = fewer recompiles.
- `rendering/gles3/shaders/log_active_async_compiles_count` — Enable only for debugging async compile issues.

### 1.3 Shadow Atlas
- `rendering/quality/shadow_atlas/size` — 4096 default.
- `rendering/quality/shadow_atlas/quadrant_*_subdiv` — 1..4. Lower subdivision = fewer draws per shadow, but coarser shadows.
- `rendering/quality/directional_shadow/size` — 4096 default.
- `rendering/quality/shadows/filter_mode` — 0: Disabled, 1: PCF5, 2: PCF13. PCF5 is cheaper.

### 1.4 Reflections
- `rendering/quality/reflections/texture_array_reflections` — true default. Texture arrays reduce state changes but use more vram.
- `rendering/quality/reflections/high_quality_ggx` — true default.
- `rendering/quality/reflections/irradiance_max_size` — 128 default.

### 1.5 Shading
- `rendering/quality/shading/force_vertex_shading` — false default. Per-pixel shading enabled.
- `rendering/quality/shading/force_lambert_over_burley` — false default.
- `rendering/quality/shading/force_blinn_over_ggx` — false default.
- `rendering/quality/shading/use_physical_light_attenuation` — false default. Standard attenuation is cheaper.

### 1.6 Depth Prepass
- `rendering/quality/depth_prepass/enable` — true default. Reduces fragment shader invocations by culling occluded fragments.
- `rendering/quality/depth_prepass/disable_for_vendors` — "PowerVR,Mali,Adreno,Apple". TBDR gpus don't benefit from depth prepass.

### 1.7 Skinning
- `rendering/quality/skinning/software_skinning_fallback` — true. Falls back when hardware bone limit is exceeded.
- `rendering/quality/skinning/force_software_skinning` — false. Force only for debugging.

### 1.8 Anisotropic Filtering
- `rendering/quality/filters/anisotropic_filter_level` — 4 default. Range 1-16. Lower for perf.
- `rendering/quality/filters/use_nearest_mipmap_filter` — false default. Enable for pixel-art style or minor perf gain.

### 1.9 Occlusion Culling
- `rendering/misc/occlusion_culling/max_active_spheres` — 8 default. Lower to reduce cpu overhead.
- `rendering/misc/occlusion_culling/max_active_polygons` — 8 default. Lower = cheaper but less accurate.
- Enable via `VS::set_use_occlusion_culling(true)`.

### 1.10 Spatial Partitioning
- `rendering/quality/spatial_partitioning/render_tree_balance` — 0.0 default. Higher values balance the bvh tree more at insertion cost.

## 2. Batching (Canvas / 2D)

### 2.1 General
- `rendering/batching/options/use_batching` — true. Combines draw calls for canvasitems sharing same material/texture.
- `rendering/batching/options/use_batching_in_editor` — true default.
- `rendering/batching/options/single_rect_fallback` — false default.
- `rendering/batching/options/use_multirect` — true. Batching uses multiple rects per batch.

### 2.2 Batch Parameters
- `rendering/batching/parameters/max_join_item_commands` — 16 default. Max items joined into a single batch.
- `rendering/batching/parameters/colored_vertex_format_threshold` — 0.25 default.
- `rendering/batching/parameters/batch_buffer_size` — 16384 default.
- `rendering/batching/parameters/item_reordering_lookahead` — 4. Higher = better ordering but more cpu.

### 2.3 Light Batching
- `rendering/batching/lights/scissor_area_threshold` — 1.0 default.
- `rendering/batching/lights/max_join_items` — 32. Max items batched together for light rendering.

### 2.4 Debug
- `rendering/batching/debug/flash_batching` — false. Flashes batched items for visual debugging.
- `rendering/batching/debug/diagnose_frame` — false. Logs batch diagnostics for the frame.

### 2.5 Precision
- `rendering/batching/precision/uv_contract` — false. Enable to contract uvs to prevent bleeding.
- `rendering/batching/precision/uv_contract_amount` — 100.

## 3. 2D Renderer

### 3.1 Software Skinning
- `rendering/2d/options/use_software_skinning` — true. Reduces draw calls for skeletal 2d animations.

### 3.2 Ninepatch Mode
- `rendering/2d/options/ninepatch_mode` — 1: Fixed (default), 2: Scaling. Fixed mode has fewer triangles.

### 3.3 GPU Pixel Snap
- `rendering/2d/snapping/use_gpu_pixel_snap` — false default. Enable for pixel-perfect 2d at low res.

### 3.4 NVIDIA Workaround
- `rendering/2d/options/use_nvidia_rect_flicker_workaround` — false. Enable only if seeing flickering rectangles on nvidia gpus.

### 3.5 GL State Batching
- `rendering/2d/opengl/batching_send_null` — 0 (Auto).
- `rendering/2d/opengl/batching_stream` — 0 (Auto).
- `rendering/2d/opengl/legacy_orphan_buffers` — 0 (Auto).
- `rendering/2d/opengl/legacy_stream` — 0 (Auto).

## 4. VRAM / Texture Compression

### 4.1 Import Formats
- `rendering/vram_compression/import_bptc` — false default. BC7/BPTC highest quality, largest size.
- `rendering/vram_compression/import_s3tc` — true default. BC1-5, DXT. Standard desktop compression.
- `rendering/vram_compression/import_etc2` — true default.

### 4.2 Lossless Compression
- `rendering/misc/lossless_compression/force_png` — false. Use webp lossless for smaller textures.
- `rendering/misc/lossless_compression/webp_compression_level` — 2 (range 0-9). Higher = smaller but slower.

## 5. Mesh / Geometry

### 5.1 Split Stream
- `rendering/misc/mesh_storage/split_stream` — false default. Splits vertex position stream from attributes for better gpu cache utilization. Enable for large meshes.

### 5.2 Vertex Cache Optimization
- Automatically applied when `ARRAY_FLAG_USE_VERTEX_CACHE_OPTIMIZATION` is set. Reorders indices to maximize post-transform cache hits.

## 6. Threading

### 6.1 Render Thread
- `rendering/threads/thread_model` — 0: Unsafe, 1: Safe, 2: Separate. Safe is recommended. Separate provides max parallelism but requires gpu driver support.

### 6.2 BVH Thread Safety
- `rendering/threads/thread_safe_bvh` — false default. Enable for threaded bvh update.

### 6.3 RID Pool
- `memory/limits/multithreaded_server/rid_pool_prealloc` — 60 default.

## 7. Viewport / Render Target

### 7.1 Viewport Usage
- Use `VIEWPORT_USAGE_2D_NO_SAMPLING` for ui viewports. Skips 3d and msaa, saves significant gpu time.
- Use `VIEWPORT_USAGE_3D_NO_EFFECTS` for shadows/reflection renders. Skips post-processing.

### 7.2 MSAA
- `VIEWPORT_MSAA_DISABLED` for performance. 2x/4x for quality. EXT variants use lower-quality resolve.

### 7.3 HDR
- Disable hdr (`viewport_set_hdr(false)`) if not using hdr rendering. Saves 2x framebuffer memory.

### 7.4 Shadow Atlas
- Use `viewport_set_shadow_atlas_quadrant_subdivision` to control shadow resolution per direction. Fewer subdivisions = fewer shadow draws.

## 8. Portal Rendering (Occlusion)

### 8.1 Settings
- `rendering/portals/pvs/use_simple_pvs` — false. Simple pvs is faster but less accurate.
- `rendering/portals/gameplay/use_signals` — true. Signal-based activation for dynamic objects.
- `rendering/portals/optimize/remove_danglers` — true. Removes unreachable rooms.
- `rendering/portals/debug/logging` — true in editor, disable in release.

## 9. Asset Optimization

### 9.1 Textures
- Always enable mipmaps for 3d textures (reduces moire and improves cache coherency).
- Use power-of-two (POT) textures when possible for better gpu memory alignment.
- Compress all textures to vram format (S3TC/BPTC) for production builds.
- Use `size_limit` import option to cap texture resolution.
- Set `flags/filter=false` for pixel art textures.

### 9.2 Meshes
- Use as few materials per mesh as possible — each material = one draw call.
- Use `compress/mode=Video RAM` for imported textures.
- Enable `rendering/misc/mesh_storage/split_stream` for meshes >10k vertices.

### 9.3 Particles
- Lower `amount` and `lifetime`. Fewer alive particles = less gpu time.
- Use `fixed_fps` to reduce update frequency.
- Set `explosiveness_ratio` high for burst-style particles to reduce steady-state cost.

### 9.4 Shaders
- Minimize texture reads. Each additional `texture()` call in fragment shader adds cost.
- Avoid `discard` in fragment shaders (disables early-Z optimization).
- Prefer vertex shading over per-pixel when quality difference is negligible.

### 9.5 Materials
- `material_set_render_priority` can be used to control rendering order.
- `material_set_next_pass` for multi-pass materials — each pass is a full redraw.

## 10. CPU / General

### 10.1 Low Processor Mode
- `interface/editor/low_processor_mode_sleep_usec` — Controls sleep time in editor idle.
- `OS::set_low_processor_usage_mode(true)` for game builds (enables frame pacing).
- `OS::set_low_processor_usage_mode_sleep_usec()` — Fine-grained control.

### 10.2 VSync & Frame Pacing
- `display/window/vsync/use_vsync` — true default. Reduces screen tearing and power usage.
- `--fixed-fps <fps>` — Disables real-time sync, useful for benchmarks.
- `--disable-render-loop` — Only render when explicitly called from script, saves cpu when nothing changes.

### 10.3 Physics
- `physics/common/enable_pause_aware_picking` — true default. Process physics only when game is active.
- `physics/3d/godot_physics/use_bvh` — true default. BVH-based broadphase is O(n log n) vs O(n2) for brute force.

### 10.4 Navigation
- Disable navigation servers (`NavigationServer::set_active(false)`) when not used.
- Similarly disable `PhysicsServer` and `Physics2DServer` when not needed (editor does this).

### 10.5 Scripting
- GDScript: Compile scripts for export (`script_export_mode=1`). Reduces parse time on startup.
- Avoid creating/destroying objects every frame — use object pooling.
- Use `set_process(false)` for nodes that don't need per-frame updates.

## 11. Profiling & Debugging

### 11.1 Render Info
- `viewport_get_render_info()` returns: objects, vertices, material/shader/surface changes per frame, draw calls.
- Use `VS::INFO_*` constants to query specific counters.

### 11.2 Debug Draw
- `viewport_set_debug_draw(VIEWPORT_DEBUG_DRAW_WIREFRAME)` — Wireframe for overdraw analysis.
- `viewport_set_debug_draw(VIEWPORT_DEBUG_DRAW_OVERDRAW)` — Overdraw heatmap.
- `scenario_set_debug(SCENARIO_DEBUG_*)` — Scene-level debug modes.

### 11.3 Benchmark
- `--benchmark` CLI flag to print benchmark results.
- `--benchmark-file <path>` to save JSON benchmark output.

### 11.4 Performance Monitor
- Access via `Performance` singleton: `Performance.get_monitor(Performance.*)` for FPS, draw calls, memory, etc.
- `--print-fps` CLI flag for stdout FPS logging.

## 12. Quick Win Checklist

- [ ] Set `rendering/vram_compression/import_s3tc=true` and compress all textures.
- [ ] Enable `rendering/batching/options/use_batching=true`.
- [ ] Set `rendering/gles3/shaders/shader_compilation_mode=2` (async + cache) for release.
- [ ] Enable `rendering/quality/depth_prepass/enable=true`.
- [ ] Use `VIEWPORT_USAGE_2D_NO_SAMPLING` for UI viewports.
- [ ] Disable hiDPI if not needed: `display/window/dpi/allow_hidpi=false`.
- [ ] Pre-compile shaders at load time where possible.
- [ ] Profile with `--print-fps` and `viewport_get_render_info()` to identify bottlenecks.
- [ ] Set `rendering/quality/skinning/software_skinning_fallback=true`.
- [ ] Enable portal/occlusion culling for interior scenes.
