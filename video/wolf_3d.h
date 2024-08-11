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

    #include "wolf/camera.h"
    #include "wolf/fixed.h"
    #include "wolf/map.h"

    extern "C" {

    } // extern "C"

} // namespace w3d

#define WOLF_3D_CONTROL_TAG    0x43336457 // "w3dC"

class VDUStreamProcessor;

typedef struct Wolf3dControl {
    uint32_t            m_tag;              // Used to verify the existence of this structure
    uint32_t            m_size;             // Used to verify the existence of this structure
    VDUStreamProcessor* m_proc;             // Used by subcommands to obtain more data

    void show_free_ram() {
        debug_log("Free PSRAM: %u\n", heap_caps_get_free_size(MALLOC_CAP_SPIRAM));
    }

    // VDU 23,0, &A0, sid; &50, 0, width; height; : Create Control Structure
    void initialize(VDUStreamProcessor& processor, uint16_t width, uint16_t height) {
        printf("initialize: wolf creating control structure for %ux%u scene\n", width, height);
        memset(this, 0, sizeof(Wolf3dControl));
        m_tag = WOLF_3D_CONTROL_TAG;
        m_size = sizeof(Wolf3dControl);

    }

    // VDU 23, 0, &A0, sid; &50, 0, 255 : Deinitialize Control Structure
    void deinitialize(VDUStreamProcessor& processor) {
    }

    bool validate() {
        return (m_tag == WOLF_3D_CONTROL_TAG &&
                m_size == sizeof(Wolf3dControl));
    }

    void handle_subcommand(VDUStreamProcessor& processor, uint8_t subcmd) {
        //debug_log("w3d: handle_subcommand(%hu)\n", subcmd);
        m_proc = &processor;
        // switch (subcmd) {
        //     case 1: do_something(); break;
        // }
    }


} Wolf3dControl;

#endif // WOLF_3D_H