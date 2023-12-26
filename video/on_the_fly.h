#ifndef ON_THE_FLY
#define ON_THE_FLY

#include "src/di_manager.h"
#include <memory>
#include <fabgl.h>
#include "agon.h"
#include "agon_fonts.h"
#include "dispdrivers/vgabasecontroller.h"
#include "src/di_timing.h"

#define OTF_MANAGER_PRIORITY    (configMAX_PRIORITIES - 1) // Task priority for manager for OTF (800x600x64) mode

extern std::unique_ptr<fabgl::Canvas>           	canvas;
extern std::unique_ptr<fabgl::VGABaseController>	_VGAController;
extern uint8_t                                      videoMode;
extern uint8_t                                      _VGAColourDepth;

extern void print(char const * text);
extern int8_t change_mode(uint8_t mode);

DiManager* di_manager; // Used for OTF 800x600x64 mode

void otf(void * options) {
	debug_log("OTF task running\r\n");
	di_manager = new DiManager();
	di_manager->create_root();

	auto opts = (uint32_t) options;
	if (opts == 1) {
		// Create a solid black rectangle as the screen background.
		OtfCmd_41_Create_primitive_Solid_Rectangle cmd;
		cmd.m_color = PIXEL_ALPHA_100_MASK|0x00; // 100% black
		cmd.m_flags = PRIM_FLAGS_DEFAULT;
		cmd.m_w = otf_video_params.m_active_pixels;
		cmd.m_h = otf_video_params.m_active_lines;
		cmd.m_id = 1;
		cmd.m_pid = 0;
		cmd.m_x = 0;
		cmd.m_y = 0;
		auto rect = di_manager->create_solid_rectangle(&cmd);
		di_manager->generate_code_for_primitive(cmd.m_id);
	} else if (opts == 2) {
		// Create a full screen text area as the screen background.
		auto columns = otf_video_params.m_active_pixels / 8;
		auto rows = otf_video_params.m_active_lines / 8;
		auto text_area = di_manager->create_text_area(1, ROOT_PRIMITIVE_ID, PRIM_FLAGS_DEFAULT,
			0, 0, columns, rows, fabgl::FONT_AGON_DATA);
		text_area->define_character_range(0x20, 0x7E, PIXEL_ALPHA_100_MASK|0x05, PIXEL_ALPHA_100_MASK|0x00);
		text_area->clear_screen();
		di_manager->generate_code_for_primitive(1);
	}

	debug_log("Running OTF manager...\r\n");
	di_manager->run();
}

int8_t use_otf_mode(int8_t mode) {
	if (_VGAController) {
		_VGAController->end();
		_VGAController.reset();
	}

	// Modes: 30..39 - change mode, but create no primitives
	// Modes: 40..49 - change mode, create full screen black rectangle
	// Modes: 50..59 - change mode, create full screen text area

    auto options = (mode - 30) / 10;
	auto resolution = (mode % 10);

	// Modeline for 800x600@60Hz resolution, positive syncs
	//                                                 800+40+128+88    600+1+4+24
	#define SVGA_800x600_60Hz_Pos "\"800x600@60Hz\" 40 800 840 968 1056 600 601 605 628 +HSync +VSync"

	// Modeline for 684x384@60Hz resolution, opposite syncs
	//                                                 1368+72+144+216 1800   768+1+3+23 795
	//                                                  684+36+72+108  900    384+1+2+11 398
	//#define SVGA_684x384_60Hz "\"684x384@60Hz\" 85.5 684 720 792 900 384 385 387 398 -HSync +VSync DoubleScan"
	#define SVGA_684x384_60Hz "\"1366x768@60Hz\" 85.5 1366 1438 1582 1798 768 769 772 795 -HSync +VSync"

	const char* mode_line = NULL;
	switch (resolution) {
		case 0: mode_line = SVGA_800x600_60Hz_Pos; break;
		case 1: mode_line = SVGA_800x600_60Hz; break;
		case 2: mode_line = SVGA_684x384_60Hz; break; // quarter of 1368x768
		case 3: mode_line = QSVGA_640x512_60Hz; break; // quarter of 1280x1024
		case 4: mode_line = VGA_640x480_60Hz; break;
		case 5: mode_line = VGA_640x240_60Hz; break;
		case 6: mode_line = VGA_512x384_60Hz; break; // quarter of 1024x768
		case 7: mode_line = QVGA_320x240_60Hz; break; // quarter of 640x480
		case 8: mode_line = VGA_320x200_75Hz; break;
		case 9: mode_line = VGA_320x200_70Hz; break;
	}

	if (!mode_line) {
		return -1; // invalid mode
	}

	fabgl::VGATimings timings;
	if (!fabgl::VGABaseController::convertModelineToTimings(mode_line, &timings)) {
		return -1; // invalid mode
	}

	otf_video_params.m_scan_count = timings.scanCount;
	otf_video_params.m_active_lines = timings.VVisibleArea;
    otf_video_params.m_vfp_lines = timings.VFrontPorch;
    otf_video_params.m_vs_lines = timings.VSyncPulse;
    otf_video_params.m_vbp_lines = timings.VBackPorch;
    otf_video_params.m_hfp_pixels = timings.HFrontPorch;
    otf_video_params.m_hs_pixels = timings.HSyncPulse;
    otf_video_params.m_active_pixels = timings.HVisibleArea;
    otf_video_params.m_hbp_pixels = timings.HBackPorch;
    otf_video_params.m_active_buffers_written = timings.VVisibleArea / NUM_LINES_PER_BUFFER;
    otf_video_params.m_vs_buffers_written = timings.VSyncPulse / NUM_LINES_PER_BUFFER;
    otf_video_params.m_dma_clock_freq = timings.frequency;
    otf_video_params.m_dma_active_lines = NUM_ACTIVE_BUFFERS*NUM_LINES_PER_BUFFER;
    otf_video_params.m_dma_total_lines = timings.VVisibleArea + timings.VFrontPorch + timings.VSyncPulse + timings.VBackPorch;
    otf_video_params.m_dma_total_descr =
		otf_video_params.m_active_buffers_written +
		timings.VFrontPorch +
		timings.VSyncPulse +
		timings.VBackPorch;
    otf_video_params.m_hs_on = (timings.HSyncLogic == '+' ? 1 : 0) << VGA_HSYNC_BIT;
    otf_video_params.m_hs_off = (timings.HSyncLogic == '+' ? 0 : 1) << VGA_HSYNC_BIT;
    otf_video_params.m_vs_on = (timings.VSyncLogic == '+' ? 1 : 0) << VGA_VSYNC_BIT;
    otf_video_params.m_vs_off = (timings.VSyncLogic == '+' ? 0 : 1) << VGA_VSYNC_BIT;
    otf_video_params.m_syncs_on = otf_video_params.m_hs_on | otf_video_params.m_vs_on;
    otf_video_params.m_syncs_off = otf_video_params.m_hs_off | otf_video_params.m_vs_off;
    otf_video_params.m_syncs_off_x4 =
		(otf_video_params.m_syncs_off << 24) | (otf_video_params.m_syncs_off << 16) |
		(otf_video_params.m_syncs_off << 8) | otf_video_params.m_syncs_off;

	otf_video_params.dump();

    uint8_t oldMode = videoMode;
	videoMode = mode;

	TaskHandle_t xHandle = NULL;
	xTaskCreatePinnedToCore(otf, "OTF-MODE", 4096, (void*) options,
							OTF_MANAGER_PRIORITY, &xHandle, 1); // Core #1
	while(true);
	return 0; // success
}

bool is_otf_mode() {
	return (di_manager != 0);
}

void otf_print(const char* text) {
	while (*text) {
		di_manager->store_character(*text++);
	}
}

#endif
