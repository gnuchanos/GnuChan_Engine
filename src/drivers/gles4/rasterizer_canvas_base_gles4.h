/**************************************************************************/
/*  rasterizer_canvas_base_gles4.h                                        */
/**************************************************************************/
/*                         This file is part of:                          */
/*                             GNUCHAN ENGINE                             */
/*                    https://github.com/gnuchanos/GnuChan_Engine         */
/**************************************************************************/
/* Copyright (C) 2025 GnuChan Engine contributors (see AUTHORS.md).      */
/*                                                                        */
/* This program is free software: you can redistribute it and/or modify   */
/* it under the terms of the GNU General Public License as published by   */
/* the Free Software Foundation, either version 3 of the License, or      */
/* (at your option) any later version.                                    */
/*                                                                        */
/* This program is distributed in the hope that it will be useful,        */
/* but WITHOUT ANY WARRANTY; without even the implied warranty of         */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the           */
/* GNU General Public License for more details.                           */
/*                                                                        */
/* You should have received a copy of the GNU General Public License      */
/* along with this program. If not, see <https://www.gnu.org/licenses/>.  */
/**************************************************************************/

#ifndef RASTERIZER_CANVAS_BASE_GLES4_H
#define RASTERIZER_CANVAS_BASE_GLES4_H

#include "rasterizer_storage_gles4.h"
#include "servers/visual/rasterizer.h"

#include "shaders/canvas_shadow.glsl.gen.h"
#include "shaders/lens_distorted.glsl.gen.h"

class RasterizerSceneGLES4;

class RasterizerCanvasBaseGLES4 : public RasterizerCanvas {
public:
	struct CanvasItemUBO {
		float projection_matrix[16];
		float time;
		uint8_t padding[12];
	};

	RasterizerSceneGLES4 *scene_render;

	struct Data {
		enum { NUM_QUAD_ARRAY_VARIATIONS = 8 };

		GLuint canvas_quad_vertices;
		GLuint canvas_quad_array;

		GLuint polygon_buffer;
		GLuint polygon_buffer_quad_arrays[NUM_QUAD_ARRAY_VARIATIONS];
		GLuint polygon_buffer_pointer_array;
		GLuint polygon_index_buffer;

		GLuint particle_quad_vertices;
		GLuint particle_quad_array;

		uint32_t polygon_buffer_size;
		uint32_t polygon_index_buffer_size;

	} data;

	struct State {
		CanvasItemUBO canvas_item_ubo_data;
		GLuint canvas_item_ubo;
		bool canvas_texscreen_used;
		CanvasShaderGLES4 canvas_shader;
		CanvasShadowShaderGLES4 canvas_shadow_shader;
		LensDistortedShaderGLES4 lens_shader;

		bool using_texture_rect;
		bool using_ninepatch;

		bool using_light_angle;
		bool using_modulate;
		bool using_large_vertex;

		RID current_tex;
		RID current_normal;
		RasterizerStorageGLES4::Texture *current_tex_ptr;

		Transform vp;

		Color canvas_item_modulate;
		Transform2D extra_matrix;
		Transform2D final_transform;
		bool using_skeleton;
		Transform2D skeleton_transform;
		Transform2D skeleton_transform_inverse;

	} state;

	RasterizerStorageGLES4 *storage;

	// allow user to choose api usage
	GLenum _buffer_upload_usage_flag;

	struct LightInternal : public RID_Data {
		struct UBOData {
			float light_matrix[16];
			float local_matrix[16];
			float shadow_matrix[16];
			float color[4];
			float shadow_color[4];
			float light_pos[2];
			float shadowpixel_size;
			float shadow_gradient;
			float light_height;
			float light_outside_alpha;
			float shadow_distance_mult;
			uint8_t padding[4];
		} ubo_data;

		GLuint ubo;
	};

	RID_Owner<LightInternal> light_internal_owner;

	virtual RID light_internal_create();
	virtual void light_internal_update(RID p_rid, Light *p_light);
	virtual void light_internal_free(RID p_rid);

	virtual void canvas_begin();
	virtual void canvas_end();

	void _set_texture_rect_mode(bool p_enable, bool p_ninepatch = false, bool p_light_angle = false, bool p_modulate = false, bool p_large_vertex = false);
	RasterizerStorageGLES4::Texture *_bind_canvas_texture(const RID &p_texture, const RID &p_normal_map, bool p_force = false);

	void _draw_gui_primitive(int p_points, const Vector2 *p_vertices, const Color *p_colors, const Vector2 *p_uvs, const float *p_light_angles = nullptr);
	void _draw_polygon(const int *p_indices, int p_index_count, int p_vertex_count, const Vector2 *p_vertices, const Vector2 *p_uvs, const Color *p_colors, bool p_singlecolor, const int *p_bones, const float *p_weights);
	void _draw_generic(GLuint p_primitive, int p_vertex_count, const Vector2 *p_vertices, const Vector2 *p_uvs, const Color *p_colors, bool p_singlecolor);
	void _draw_generic_indices(GLuint p_primitive, const int *p_indices, int p_index_count, int p_vertex_count, const Vector2 *p_vertices, const Vector2 *p_uvs, const Color *p_colors, bool p_singlecolor);

	void _copy_texscreen(const Rect2 &p_rect);

	virtual void canvas_debug_viewport_shadows(Light *p_lights_with_shadow);

	virtual void canvas_light_shadow_buffer_update(RID p_buffer, const Transform2D &p_light_xform, int p_light_mask, float p_near, float p_far, LightOccluderInstance *p_occluders, CameraMatrix *p_xform_cache);

	virtual void reset_canvas();

	void draw_generic_textured_rect(const Rect2 &p_rect, const Rect2 &p_src);
	void draw_lens_distortion_rect(const Rect2 &p_rect, float p_k1, float p_k2, const Vector2 &p_eye_center, float p_oversample);
	void render_rect_nvidia_workaround(const Item::CommandRect *p_rect, const RasterizerStorageGLES4::Texture *p_texture);

	void initialize();
	void finalize();

	virtual void draw_window_margins(int *black_margin, RID *black_image);

	RasterizerCanvasBaseGLES4();
};

#endif // RASTERIZER_CANVAS_BASE_GLES4_H
