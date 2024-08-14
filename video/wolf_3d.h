#ifndef WOLF_3D_H
#define WOLF_3D_H

#include <stdint.h>
#include <string.h>
#include <agon.h>
#include <map>
#include "esp_heap_caps.h"
#include "sprites.h"
#include "vdu_stream_processor.h"

namespace w3d {

    extern "C" {

        #include "wolf/render/mesh.h"
        #include "wolf/render/object.h"
        #include "wolf/render/pixel.h"
        #include "wolf/render/renderer.h"
        #include "wolf/render/scene.h"
        #include "wolf/render/backend.h"
        #include "wolf/render/depth.h"

        #include "wolf/fixed.h"
        #include "wolf/map.h"
        #include "wolf/camera.h"
        #include "wolf/render.h"

    } // extern "C"

} // namespace w3d

#define WOLF_3D_CONTROL_TAG    0x43443350 // "W3DC"

#define PI2                    6.283185307179586476925286766559f

// Convert from integer to 8.8 fixed-point
#define INT_TO_FIXED8_8(x) ((fixed8_8)((x) << 8))
// Convert from 8.8 fixed-point to integer (truncating the fractional part)
#define FIXED8_8_TO_INT(x) ((int16_t)((x) >> 8))
// Convert from floating-point to 8.8 fixed-point
#define FLOAT_TO_FIXED8_8(x) ((fixed8_8)((x) * 256.0f))
// Convert from 8.8 fixed-point to floating-point
#define FIXED8_8_TO_FLOAT(x) ((float)(x) / 256.0f)
// Add two 8.8 fixed-point numbers
#define FIXED8_8_ADD(x, y) ((x) + (y))
// Subtract two 8.8 fixed-point numbers
#define FIXED8_8_SUB(x, y) ((x) - (y))
// Multiply two 8.8 fixed-point numbers
#define FIXED8_8_MUL(x, y) ((fixed8_8)(((int32_t)(x) * (y)) >> 8))
// Divide two 8.8 fixed-point numbers
#define FIXED8_8_DIV(x, y) ((fixed8_8)(((int32_t)(x) << 8) / (y)))

class VDUStreamProcessor;typedef struct tag_Transformable {
    w3d::Vec3f      m_scale;
    w3d::Vec3f      m_rotation;
    w3d::Vec3f      m_translation;
    w3d::Mat4       m_transform;
    bool            m_modified;

    w3d::Vec3f      m_rotation_loc;
    w3d::Vec3f      m_translation_loc;

    bool            m_modified_loc;
    bool            m_is_camera;

    void initialize_scale() {
        m_scale.x = 1.0f;
        m_scale.y = 1.0f;
        m_scale.z = 1.0f;
        m_modified = true;
    }

    void initialize() {
        memset(this, 0, sizeof(struct tag_Transformable));
        initialize_scale();
    }

    // Helper function to get the forward direction of the transformable
    w3d::Vec3f get_forward_direction() const {
        w3d::Mat4 rotation_matrix = w3d::mat4Identity();
        if (m_rotation.x) {
            auto t = w3d::mat4RotateX(m_rotation.x);
            rotation_matrix = mat4MultiplyM(&rotation_matrix, &t);
        }
        if (m_rotation.y) {
            auto t = w3d::mat4RotateY(m_rotation.y);
            rotation_matrix = mat4MultiplyM(&rotation_matrix, &t);
        }
        if (m_rotation.z) {
            auto t = w3d::mat4RotateZ(m_rotation.z);
            rotation_matrix = mat4MultiplyM(&rotation_matrix, &t);
        }

        // Forward direction is typically -Z in a right-handed coordinate system
        w3d::Vec3f forward = {0.0f, 0.0f, -1.0f};
        return w3d::mat4MultiplyVec3(&forward, &rotation_matrix);
    }

    void compute_transformation_matrix() {
        m_transform = w3d::mat4Scale(m_scale);
        if (m_rotation.x) {
            auto t = w3d::mat4RotateX(m_rotation.x);
            m_transform = mat4MultiplyM(&m_transform, &t);
        }
        if (m_rotation.y) {
            auto t = w3d::mat4RotateY(m_rotation.y);
            m_transform = mat4MultiplyM(&m_transform, &t);
        }
        if (m_rotation.z) {
            auto t = w3d::mat4RotateZ(m_rotation.z);
            m_transform = mat4MultiplyM(&m_transform, &t);
        }
        if (m_translation.x || m_translation.y || m_translation.z) {
            auto t = w3d::mat4Translate(m_translation);
            m_transform = mat4MultiplyM(&m_transform, &t);
        }
        m_modified = false;
    }

    void compute_transformation_matrix_local() {
        // Initialize the local transformation matrix
        w3d::Mat4 m_transform_loc = w3d::mat4Scale(m_scale);

        if (m_rotation_loc.x) {
            auto t = w3d::mat4RotateX(m_rotation_loc.x);
            m_transform_loc = mat4MultiplyM(&m_transform_loc, &t);
        }
        if (m_rotation_loc.y) {
            auto t = w3d::mat4RotateY(m_rotation_loc.y);
            m_transform_loc = mat4MultiplyM(&m_transform_loc, &t);
        }
        if (m_rotation_loc.z) {
            auto t = w3d::mat4RotateZ(m_rotation_loc.z);
            m_transform_loc = mat4MultiplyM(&m_transform_loc, &t);
        }
        if (m_translation_loc.x || m_translation_loc.y || m_translation_loc.z) {
            auto t = w3d::mat4Translate(m_translation_loc);
            m_transform_loc = mat4MultiplyM(&m_transform_loc, &t);
        }
        // Apply the local transformation matrix to the initial transform
        m_transform = mat4MultiplyM(&m_transform_loc, &m_transform);

        // Clear local transformation values
        m_rotation_loc = {0.0f, 0.0f, 0.0f};
        m_translation_loc = {0.0f, 0.0f, 0.0f};

        m_modified_loc = false;
    }

    void update_transformation() {
        if (m_modified) {
            compute_transformation_matrix();
        }
    }

    void dump() {
        for (int i = 0; i < 16; i++) {
            debug_log("        [%i] %f\n", i, m_transform.elements[i]);
        }
        debug_log("Scale:       %f %f %f\n", m_scale.x, m_scale.y, m_scale.z);
        debug_log("Rotation:    %f %f %f\n", m_rotation.x * (180.0 / M_PI), m_rotation.y * (180.0 / M_PI), m_rotation.z * (180.0 / M_PI));
        debug_log("Translation: %f %f %f\n", m_translation.x, m_translation.y, m_translation.z);
    }
} Transformable;

typedef struct tag_TexObject : public Transformable {
    w3d::Object     m_object;
    w3d::Texture    m_texture;
    w3d::Material   m_material;
    uint16_t        m_oid;

    void bind() {
        m_object.material = &m_material;
        m_material.texture = &m_texture;
    }

    void initialize() {
        Transformable::initialize();
        bind();
    }

    void update_transformation_matrix() {
        compute_transformation_matrix();
        m_object.transform = m_transform;
    }

    void update_transformation_matrix_loc() {
        compute_transformation_matrix_local();
        m_object.transform = m_transform;
    }

    void dump() {
        Transformable::dump();
        debug_log("TObject: %p %u\n", this, m_oid);
        debug_log("Object: %p %p %p %p\n", &m_object, m_object.material, m_object.mesh,
                    m_object.transform.elements);
        debug_log("Texture: %p %u %u %p\n", &m_texture, m_texture.size.x, m_texture.size.y, m_texture.frameBuffer);
        debug_log("Material: %p %p %u %u %p\n", &m_material, m_material.texture, m_material.texture->size.x,
                    m_material.texture->size.y, m_material.texture->frameBuffer);
    }
} TexObject;

struct Wolf3dControl;

extern "C" {

    void static_init(w3d::Renderer* ren, w3d::BackEnd* backEnd, w3d::Vec4i _rect);

    void static_before_render(w3d::Renderer* ren, w3d::BackEnd* backEnd);

    void static_after_render(w3d::Renderer* ren, w3d::BackEnd* backEnd);

    w3d::Pixel* static_get_frame_buffer(w3d::Renderer* ren, w3d::BackEnd* backEnd);

    w3d::PingoDepth* static_get_zeta_buffer(w3d::Renderer* ren, w3d::BackEnd* backEnd);

} // extern "C"

typedef struct Wolf3dControl {
    uint32_t            m_tag;              // Used to verify the existence of this structure
    uint32_t            m_size;             // Used to verify the existence of this structure
    VDUStreamProcessor* m_proc;             // Used by subcommands to obtain more data
    w3d::BackEnd        m_backend;          // Used by the renderer
    w3d::Pixel*         m_frame;            // Frame buffer for rendered pixels
    w3d::PingoDepth*    m_zeta;             // Zeta buffer for depth information
    uint16_t            m_width;            // Width of final render in pixels
    uint16_t            m_height;           // Height of final render in pixels
    Transformable       m_camera;           // Camera transformation settings
    Transformable       m_scene;            // Scene transformation settings
    std::map<uint16_t, w3d::Mesh>* m_meshes;    // Map of meshes for use by objects
    std::map<uint16_t, TexObject>* m_objects;   // Map of textured objects that use meshes and have transforms
    uint8_t             m_dither_type;      // Dithering type and options to be applied to rendered bitmap
    // new wolf3d members
    std::map<uint16_t, w3d::Map>*  m_maps;   // Map of maps
    w3d::Camera         m_wolf_camera;      // Camera for wolf3d

    void show_free_ram() {
        debug_log("Free PSRAM: %u\n", heap_caps_get_free_size(MALLOC_CAP_SPIRAM));
    }

    // VDU 23, 0, &A0, sid; &49, 0, 1 :  Initialize Control Structure
    void initialize(VDUStreamProcessor& processor, uint16_t width, uint16_t height) {
        debug_log("initialize: wolf creating control structure for %ux%u scene\n", width, height);
        memset(this, 0, sizeof(Wolf3dControl));
        m_tag = WOLF_3D_CONTROL_TAG;
        m_size = sizeof(Wolf3dControl);
        m_width = width;
        m_height = height;
        m_camera.initialize_scale();
        m_scene.initialize_scale();

        auto frame_size = (uint32_t) width * (uint32_t) height;

        auto size = sizeof(w3d::Pixel) * frame_size;
        m_frame = (w3d::Pixel*) heap_caps_malloc(size, MALLOC_CAP_SPIRAM);
        if (!m_frame) {
            debug_log("initialize: failed to allocate %u bytes for frame\n", size);
            show_free_ram();
        }

        size = sizeof(w3d::PingoDepth) * frame_size;
        m_zeta = (w3d::PingoDepth*) heap_caps_malloc(size, MALLOC_CAP_SPIRAM);
        if (!m_zeta) {
            debug_log("initialize: failed to allocate %u bytes for zeta\n", size);
            show_free_ram();
        }

        m_backend.init = &static_init;
        m_backend.beforeRender = &static_before_render;
        m_backend.afterRender = &static_after_render;
        m_backend.getFrameBuffer = &static_get_frame_buffer;
        m_backend.getZetaBuffer = &static_get_zeta_buffer;
        m_backend.drawPixel = NULL;
        m_backend.clientCustomData = (void*) this;

        m_meshes = new std::map<uint16_t, w3d::Mesh>;
        m_objects = new std::map<uint16_t, TexObject>;
        m_maps = new std::map<uint16_t, w3d::Map>;
    }

    // VDU 23, 0, &A0, sid; &49, 0, 0 :  Deinitialize Control Structure
    void deinitialize(VDUStreamProcessor& processor) {
    }

    bool validate() {
        return (m_tag == WOLF_3D_CONTROL_TAG &&
                m_size == sizeof(Wolf3dControl));
    }

    void handle_subcommand(VDUStreamProcessor& processor, uint8_t subcmd) {
        debug_log("W3D: handle_subcommand(%hu)\n", subcmd);
        m_proc = &processor;
        switch (subcmd) {
            case 128: wolf_maps(); break; // wolf3d map commands
            case 129: wolf_cam(); break; // wolf3d camera commands
            case 130: wolf_render(); break; // wolf3d render commands
        }
    }

    w3d::Map* establish_map(uint16_t map_id) {
        // printf("establish_map(%u)\n", map_id);
        auto map_iter = m_maps->find(map_id);
        // printf("establish_map(%u) map_iter=%p\n", map_id, map_iter);
        if (map_iter == m_maps->end()) {
            // printf("establish_map(%u) not found\n", map_id);
            w3d::Map map;
            memset(&map, 0, sizeof(map));
            (*m_maps).insert(std::pair<uint16_t, w3d::Map>(map_id, map));
            // printf("establish_map(%u) inserted\n", map_id);
            return &m_maps->find(map_id)->second;
        } else {
            // printf("establish_map(%u) found\n", map_id);
            return &map_iter->second;
        }
    }

    w3d::Map* get_map() {
        // printf("get_map()\n");
        auto map_id = m_proc->readWord_t();
        // printf("get_map() map_id=%u\n", map_id);
        if (map_id >= 0) {
            return establish_map(map_id);
        }
        return NULL;
    }

    void wolf_maps() {
        uint8_t wolf_cmd = m_proc->readByte_t();
        switch (wolf_cmd) {
            case 0: wolf_map_load_cells(); break;
            case 1: wolf_map_load_tex_lut(); break;
        }
    }

    void wolf_cam() {
        uint8_t wolf_cmd = m_proc->readByte_t();
        switch (wolf_cmd) {
            case 0: wolf_cam_create(); break;
        }
    }

    void wolf_render() {
        uint8_t wolf_cmd = m_proc->readByte_t();
        switch (wolf_cmd) {
            case 0: wolf_render_update_zbuffer(); break;
        }
    }

    // VDU 23, 0, &A0, sid; &49, 130, 0, map_id; : Update Wolf3D ZBuffer
    void wolf_render_update_zbuffer() {
        // printf("wolf_render_update_zbuffer\n");
        auto map = get_map();
        if (map) {
            // printf("map=%p\n", map);
            w3d::updatePanelsAndZBuffer(map, m_wolf_camera.zbuffer, &m_wolf_camera);
            // printf("ZBuffer updated successfully\n");
        }
    }

    // VDU 23, 0, &A0, sid; &49, 129, 0, x; y; theta; fov; screen_width; screen_height; screen_dist; :  Create Wolf3D Camera
    void wolf_cam_create() {
        // printf("wolf_cam_create\n");

        // Read and convert camera properties inline
        m_wolf_camera.x = FIXED8_8_TO_FLOAT(m_proc->readWord_t());
        m_wolf_camera.y = FIXED8_8_TO_FLOAT(m_proc->readWord_t());
        m_wolf_camera.theta = FIXED8_8_TO_FLOAT(m_proc->readWord_t()) * (M_PI / 128.0); // Convert to radians
        m_wolf_camera.fov = FIXED8_8_TO_FLOAT(m_proc->readWord_t()) * (M_PI / 128.0); // Convert to radians
        m_wolf_camera.screen_width = m_proc->readWord_t();
        m_wolf_camera.screen_height = m_proc->readWord_t();
        m_wolf_camera.screen_dist = FIXED8_8_TO_FLOAT(m_proc->readWord_t());

        // Initialize the ZBuffer
        // printf("Creating ZBuffer\n");
        m_wolf_camera.zbuffer = w3d::create_zbuffer(&m_wolf_camera);
        if (!m_wolf_camera.zbuffer) {
            // printf("wolf_cam_create: failed to initialize ZBuffer\n");
            show_free_ram();
            return;
        }
        // printf("ZBuffer created successfully. ZBuffer pointer: %p\n", m_wolf_camera.zbuffer);

        // Convert theta and fov to degrees for debugging output
        float theta_degrees = m_wolf_camera.theta * (180.0f / M_PI);
        float fov_degrees = m_wolf_camera.fov * (180.0f / M_PI);

        // // Print camera properties for debugging
        // printf("Camera: x: %f, y: %f, theta: %f (degrees), fov: %f (degrees), screen_width: %d, screen_height: %d, screen_dist: %f\n",
        //     m_wolf_camera.x, 
        //     m_wolf_camera.y, 
        //     theta_degrees,
        //     fov_degrees,
        //     m_wolf_camera.screen_width, 
        //     m_wolf_camera.screen_height, 
        //     m_wolf_camera.screen_dist);
    }


    // VDU 23, 0, &A0, sid; &49, 128, 1, map_id; num_panels; <texture_id; width; height;> :  Load Wolf3D Map Texture Panel Lookup Table
    void wolf_map_load_tex_lut() {
        printf("wolf_map_load_tex_lut: Starting\n");
        auto map = get_map();
        if (map) {
            auto num_panels = m_proc->readWord_t();
            if (num_panels > 0) {
                map->tex_lut = (w3d::TexPanelLut*) heap_caps_malloc(sizeof(w3d::TexPanelLut), MALLOC_CAP_SPIRAM);
                if (!map->tex_lut) {
                    printf("wolf_map_load_tex_lut: failed to allocate %u bytes for TexPanelLut\n", sizeof(w3d::TexPanelLut));
                    show_free_ram();
                    return;
                }

                map->tex_lut->panels = (w3d::TexPanel**) heap_caps_malloc(num_panels * sizeof(w3d::TexPanel*), MALLOC_CAP_SPIRAM);
                if (!map->tex_lut->panels) {
                    printf("wolf_map_load_tex_lut: failed to allocate %u bytes for TexPanel* array\n", num_panels * sizeof(w3d::TexPanel*));
                    show_free_ram();
                    return;
                }
                map->tex_lut->num_panels = num_panels;

                for (uint8_t i = 0; i < num_panels; i++) {
                    auto img_idx = i;
                    auto texture_id = m_proc->readWord_t();
                    auto width = m_proc->readWord_t();
                    auto height = m_proc->readWord_t();

                    w3d::TexPanel* panel = (w3d::TexPanel*) heap_caps_malloc(sizeof(w3d::TexPanel), MALLOC_CAP_SPIRAM);
                    if (!panel) {
                        printf("wolf_map_load_tex_lut: failed to allocate %u bytes for TexPanel\n", sizeof(w3d::TexPanel));
                        show_free_ram();
                        return;
                    }

                    panel->img_idx = img_idx;
                    panel->texture_id = texture_id;
                    panel->width = width;
                    panel->height = height;
                    map->tex_lut->panels[i] = panel;
                    // printf("panel %u: img_idx=%u, texture_id=%u, width=%u, height=%u\n", i, panel->img_idx, panel->texture_id, panel->width, panel->height);
                }
                // Assign the texture panel lookup to the map before going to the next step
                printf("wolf_map_load_tex_lut: Texture map uploaded successfully\n");
                w3d::initializePanels(map, map->tex_lut);
            }
        }
    }

    // VDU 23, 0, &A0, sid; &49, 128, 0, map_id; width; height; <cells> :  Load Wolf3D Map Cells
    void wolf_map_load_cells() {
        auto map = get_map();
        if (map) {
            auto map_width = m_proc->readWord_t();
            auto map_height = m_proc->readWord_t();
            auto map_size = map_width * map_height;
            map->width = map_width;
            map->height = map_height;
            if (map_size > 0) {
                auto size = map_size*sizeof(w3d::Cell);
                w3d::Cell* cells = (w3d::Cell*) heap_caps_malloc(size, MALLOC_CAP_SPIRAM);
                map->cells = cells;
                if (!map->cells) {
                    printf("wolf_map_load_cells: failed to allocate %u bytes for map cells\n", size);
                    show_free_ram();
                }
                for (uint32_t i = 0; i < map_size; i++) {
                    // mind the endianess of the first three bytes
                    uint8_t map_type_status = m_proc->readByte_t();
                    uint8_t img_idx = m_proc->readByte_t();
                    uint8_t obj_id = m_proc->readByte_t();
                    uint8_t sprite_id = m_proc->readByte_t();
                    if (map->cells) {
                        // mind the endianess of the first three bytes
                        map->cells[i].map_type_status = map_type_status;
                        map->cells[i].img_idx = img_idx;
                        map->cells[i].obj_id = obj_id;
                        map->cells[i].sprite_id = sprite_id;
                        // printf("%u %u %u %u\n", obj_id, img_idx, map_type_status, sprite_id);
                    }
                }

                // Debug information block
                uint32_t sprite_count = 0;
                int start_cell_x = -1;
                int start_cell_y = -1;

                for (uint32_t i = 0; i < map_size; i++) {
                    if (map->cells[i].sprite_id != 255) {
                        sprite_count++;
                    }
                    if (map->cells[i].map_type_status & CELL_IS_START) {
                        start_cell_x = i % map_width;
                        start_cell_y = i / map_width;
                    }
                }
                // Print summary information about the map
                printf("Map loaded successfully:\n");
                printf("Width: %u, Height: %u, Total Cells: %u\n", map_width, map_height, map_size);
                printf("Memory allocated: %u bytes\n", size);
                printf("Number of sprites: %u\n", sprite_count);
                if (start_cell_x != -1 && start_cell_y != -1) {
                    printf("Starting cell coordinates: (%d, %d)\n", start_cell_x, start_cell_y);
                } else {
                    printf("Starting cell not found\n");
                }
                // End of debug information block
            }
        }
    }

    // VDU 23, 0, &A0, sid; &49, 38, bmid; :  Render To Bitmap
    void render_to_bitmap() {
        auto bmid = m_proc->readWord_t();
        if (bmid < 0) {
            return;
        }

        w3d::Pixel* dst_pix = NULL;
        auto old_bitmap = getBitmap(bmid);
        if (old_bitmap) {
            auto bitmap = old_bitmap.get();
            if (bitmap && bitmap->width == m_width && bitmap->height == m_height) {
                dst_pix = (w3d::Pixel*) bitmap->data;
            }
        }

        if (!dst_pix) {
            debug_log("render_to_bitmap: output bitmap %u not found or invalid\n", bmid);
            return;
        }

        auto start = millis();
        auto size = w3d::Vec2i{(w3d::I_TYPE)m_width, (w3d::I_TYPE)m_height};
        w3d::Renderer renderer;
        rendererInit(&renderer, size, &m_backend );
        rendererSetCamera(&renderer,(w3d::Vec4i){0,0,size.x,size.y});

        w3d::Scene scene;
        sceneInit(&scene);
        w3d::rendererSetScene(&renderer, &scene);

        for (auto object = m_objects->begin(); object != m_objects->end(); object++) {
            object->second.bind();
            if (object->second.m_modified) {
                object->second.m_is_camera = false;
                object->second.update_transformation_matrix();
                //object->second.dump();
            }
            if (object->second.m_modified_loc) {
                object->second.m_is_camera = false;
                object->second.update_transformation_matrix_loc();
                //object->second.dump();
            }
            sceneAddRenderable(&scene, w3d::object_as_renderable(&object->second.m_object));
        }

        // Set the projection matrix
        renderer.camera_projection =
            w3d::mat4Perspective( 1, 2500.0, (w3d::F_TYPE)size.x / (w3d::F_TYPE)size.y, 0.5);

        if (m_camera.m_modified) {
            m_camera.m_is_camera = true;
            m_camera.compute_transformation_matrix();
        }
        if (m_camera.m_modified_loc) {
            m_camera.m_is_camera = true;
            m_camera.compute_transformation_matrix_local();
        }
        //debug_log("Camera:\n");
        // m_camera.dump();
        renderer.camera_view = m_camera.m_transform;

        if (m_scene.m_modified) {
            m_scene.compute_transformation_matrix();
        }
        scene.transform = m_scene.m_transform;

        //debug_log("Frame data:  %02hX %02hX %02hX %02hX\n", m_frame->r, m_frame->g, m_frame->b, m_frame->a);
        //debug_log("Destination: %02hX %02hX %02hX %02hX\n", dst_pix->r, dst_pix->g, dst_pix->b, dst_pix->a);

        renderer.clear = 1; // 0 = don't clear, non-0 = clear before rendering

        rendererRender(&renderer);

        memcpy(dst_pix, m_frame, sizeof(w3d::Pixel) * m_width * m_height);

        auto stop = millis();
        auto diff = stop - start;
        float fps = 1000.0 / diff;
        // printf("Render to %ux%u took %u ms (%.2f FPS)\n", m_width, m_height, diff, fps);
        //debug_log("Frame data:  %02hX %02hX %02hX %02hX\n", m_frame->r, m_frame->g, m_frame->b, m_frame->a);
        //debug_log("Final data:  %02hX %02hX %02hX %02hX\n", dst_pix->r, dst_pix->g, dst_pix->b, dst_pix->a);
    }

} Wolf3dControl;

extern "C" {

    void static_init(w3d::Renderer* ren, w3d::BackEnd* backEnd, w3d::Vec4i _rect) {
        //rect = _rect;
    }

    void static_before_render(w3d::Renderer* ren, w3d::BackEnd* backEnd) {
    }

    void static_after_render(w3d::Renderer* ren, w3d::BackEnd* backEnd) {
    }

    w3d::Pixel* static_get_frame_buffer(w3d::Renderer* ren, w3d::BackEnd* backEnd) {
        auto p_this = (struct Wolf3dControl*) backEnd->clientCustomData;
        return p_this->m_frame;
    }

    w3d::PingoDepth* static_get_zeta_buffer(w3d::Renderer* ren, w3d::BackEnd* backEnd) {
        auto p_this = (struct Wolf3dControl*) backEnd->clientCustomData;
        return p_this->m_zeta;
    }

#if DEBUG
    void show_pixel(float x, float y, uint8_t a, uint8_t b, uint8_t g, uint8_t r) {
        debug_log("%f %f %02hX %02hX %02hX %02hX\n", x, y, a, b, g, r);
    }
#endif

} // extern "C"

#endif // WOLF_3D_H