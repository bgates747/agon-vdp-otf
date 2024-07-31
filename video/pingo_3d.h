#define PINGO_3D_CONTROL_TAG    0x43443350 // "P3DC"

#include <stdint.h>
#include <string.h>
#include <agon.h>
#include <map>
#include "esp_heap_caps.h"
#include "sprites.h"
#include "vdu_stream_processor.h"
// Math headers
#include "pingo/math/mat3.h"
#include "pingo/math/mat4.h"
#include "pingo/math/types.h"
#include "pingo/math/vec2.h"
#include "pingo/math/vec3.h"
#include "pingo/math/vec4.h"
// Render headers
#include "pingo/render/backend.h"
#include "pingo/render/depth.h"
#include "pingo/render/material.h"
#include "pingo/render/mesh.h"
#include "pingo/render/object.h"
#include "pingo/render/pixel.h"
#include "pingo/render/rasterizer.h"
#include "pingo/render/renderable.h"
#include "pingo/render/renderer.h"
#include "pingo/render/scene.h"
#include "pingo/render/sprite.h"
#include "pingo/render/texture.h"

class VDUStreamProcessor;

typedef struct Transformable {
    Vec3f      m_scale;
    Vec3f      m_rotation;
    Vec3f      m_translation;
    Mat4       m_transform;
    bool       m_modified;

    void initialize_scale() {
        m_scale.x = 1.0f;
        m_scale.y = 1.0f;
        m_scale.z = 1.0f;
        m_modified = true;
    }

    void initialize() {
        memset(this, 0, sizeof(Transformable));
        initialize_scale();
    }

    void compute_transformation_matrix() {
        m_transform = mat4Scale(m_scale);
        if (m_rotation.x) {
            Mat4 t = mat4RotateX(m_rotation.x);
            m_transform = mat4MultiplyM(&m_transform, &t);
        }
        if (m_rotation.y) {
            Mat4 t = mat4RotateY(m_rotation.y);
            m_transform = mat4MultiplyM(&m_transform, &t);
        }
        if (m_rotation.z) {
            Mat4 t = mat4RotateZ(m_rotation.z);
            m_transform = mat4MultiplyM(&m_transform, &t);
        }
        if (m_translation.x || m_translation.y || m_translation.z) {
            Mat4 t = mat4Translate(m_translation);
            m_transform = mat4MultiplyM(&m_transform, &t);
        }
        m_modified = false;
    }
} Transformable;

typedef struct TexObject : public Transformable {
    uint16_t   m_oid;
    Object     m_object;
    Material   *m_material;
    Mesh       *m_mesh;

    void initialize() {
        Transformable::initialize();
    }
} TexObject;

typedef struct Pingo3dControl {
    uint32_t            m_tag;              // Used to verify the existence of this structure
    uint32_t            m_size;             // Used to verify the existence of this structure
    VDUStreamProcessor* m_proc;             // Used by subcommands to obtain more data
    BackEnd             m_backend;          // Used by the renderer
    Pixel*              m_frame;            // Frame buffer for rendered pixels
    PingoDepth*         m_zeta;             // Zeta buffer for depth information
    uint16_t            m_width;            // Width of final render in pixels
    uint16_t            m_height;           // Height of final render in pixels
    Transformable       m_camera;           // Camera transformation settings
    Transformable       m_scene;            // Scene transformation settings
    std::map<uint16_t, Normals>* m_normals; // Map of normals for use by meshes
    std::map<uint16_t, UVs>* m_uvs;         // Map of UVs for use by meshes
    std::map<uint16_t, Vertices>* m_vertices; // Map of vertices for use by meshes
    std::map<uint16_t, Mesh>* m_meshes;     // Map of meshes for use by objects
    std::map<uint16_t, TexObject>* m_objects; // Map of textured objects that use meshes and have transforms

    void show_free_ram() {
        debug_log("Free PSRAM: %u\n", heap_caps_get_free_size(MALLOC_CAP_SPIRAM));
    }


    // VDU 23, 0, &A0, sid; &49, 0, 1 :  Initialize Control Structure
    void initialize(VDUStreamProcessor& processor, uint16_t width, uint16_t height) {
        debug_log("initialize: pingo creating control structure for %ux%u scene\n", width, height);
        memset(this, 0, sizeof(Pingo3dControl));
        m_tag = PINGO_3D_CONTROL_TAG;
        m_size = sizeof(Pingo3dControl);
        m_width = width;
        m_height = height;
        m_camera.initialize_scale();
        m_scene.initialize_scale();

        auto frame_size = (uint32_t) width * (uint32_t) height;

        auto size = sizeof(Pixel) * frame_size;
        m_frame = (Pixel*) heap_caps_malloc(size, MALLOC_CAP_SPIRAM);
        if (!m_frame) {
            debug_log("initialize: failed to allocate %u bytes for frame\n", size);
            show_free_ram();
        }

        size = sizeof(PingoDepth) * frame_size;
        m_zeta = (PingoDepth*) heap_caps_malloc(size, MALLOC_CAP_SPIRAM);
        if (!m_zeta) {
            debug_log("initialize: failed to allocate %u bytes for zeta\n", size);
            show_free_ram();
        }

        m_backend.drawPixel = NULL;
        m_backend.clientCustomData = (void*) this;

        m_vertices = new std::map<uint16_t, Vertices>;
        m_uvs = new std::map<uint16_t, UVs>;
        m_normals = new std::map<uint16_t, Normals>;
        m_meshes = new std::map<uint16_t, Mesh>;
        m_objects = new std::map<uint16_t, TexObject>;
    }

    // VDU 23, 0, &A0, sid; &49, 0, 0 :  Deinitialize Control Structure
    void deinitialize(VDUStreamProcessor& processor) {
    }

    bool validate() {
        return (m_tag == PINGO_3D_CONTROL_TAG &&
                m_size == sizeof(Pingo3dControl));
    }

    void handle_subcommand(VDUStreamProcessor& processor, uint8_t subcmd) {
        // //debug_log("P3D: handle_subcommand(%hu)\n", subcmd);
        // m_proc = &processor;
        // switch (subcmd) {
        //     case 1: define_mesh_vertices(); break;
        //     case 2: set_mesh_vertex_indexes(); break;
        //     case 3: define_mesh_texture_coordinates(); break;
        //     case 4: set_texture_coordinate_indexes(); break;
        //     case 5: create_object(); break;
        //     case 40: define_object_texture_coordinates(); break;
        //     case 6: set_object_x_scale_factor(); break;
        //     case 7: set_object_y_scale_factor(); break;
        //     case 8: set_object_z_scale_factor(); break;
        //     case 9: set_object_xyz_scale_factors(); break;
        //     case 10: set_object_x_rotation_angle(); break;
        //     case 11: set_object_y_rotation_angle(); break;
        //     case 12: set_object_z_rotation_angle(); break;
        //     case 13: set_object_xyz_rotation_angles(); break;
        //     case 14: set_object_x_translation_distance(); break;
        //     case 15: set_object_y_translation_distance(); break;
        //     case 16: set_object_z_translation_distance(); break;
        //     case 17: set_object_xyz_translation_distances(); break;
        //     case 18: set_camera_x_rotation_angle(); break;
        //     case 19: set_camera_y_rotation_angle(); break;
        //     case 20: set_camera_z_rotation_angle(); break;
        //     case 21: set_camera_xyz_rotation_angles(); break;
        //     case 22: set_camera_x_translation_distance(); break;
        //     case 23: set_camera_y_translation_distance(); break;
        //     case 24: set_camera_z_translation_distance(); break;
        //     case 25: set_camera_xyz_translation_distances(); break;
        //     case 26: set_scene_x_scale_factor(); break;
        //     case 27: set_scene_y_scale_factor(); break;
        //     case 28: set_scene_z_scale_factor(); break;
        //     case 29: set_scene_xyz_scale_factors(); break;
        //     case 30: set_scene_x_rotation_angle(); break;
        //     case 31: set_scene_y_rotation_angle(); break;
        //     case 32: set_scene_z_rotation_angle(); break;
        //     case 33: set_scene_xyz_rotation_angles(); break;
        //     case 34: set_scene_x_translation_distance(); break;
        //     case 35: set_scene_y_translation_distance(); break;
        //     case 36: set_scene_z_translation_distance(); break;
        //     case 37: set_scene_xyz_translation_distances(); break;
        //     case 38: render_to_bitmap(); break;
        // }
    }

} Pingo3dControl;

