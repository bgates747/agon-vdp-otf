#ifndef ON_THE_FLY
#define ON_THE_FLY

#include "src/di_manager.h"
#include <memory>
#include <fabgl.h>
#include "agon.h"
#include "agon_fonts.h"

#define OTF_MANAGER_PRIORITY    (configMAX_PRIORITIES - 1) // Task priority for manager for OTF (800x600x64) mode

extern std::unique_ptr<fabgl::Canvas>           	canvas;
extern std::unique_ptr<fabgl::VGABaseController>	_VGAController;
extern uint8_t                                      videoMode;
extern uint8_t                                      _VGAColourDepth;

extern void print(char const * text);
extern int8_t change_mode(uint8_t mode);

DiManager* di_manager; // Used for OTF 800x600x64 mode

void otf(void * pvParameters) {
	debug_log("OTF task running\r\n");
	di_manager = new DiManager();
	di_manager->create_root();

	auto text_area = di_manager->create_text_area(1, ROOT_PRIMITIVE_ID, PRIM_FLAGS_DEFAULT, 0, 0, 100, 75, fabgl::FONT_AGON_DATA);
	text_area->define_character_range(0x20, 0x7E, PIXEL_ALPHA_100_MASK|0x05, PIXEL_ALPHA_100_MASK|0x00);
	text_area->clear_screen();
	di_manager->generate_code_for_primitive(1);
	debug_log("Running OTF manager...\r\n");
	di_manager->run();
}

int8_t use_otf_mode(int8_t mode) {
	if (_VGAController) {
		_VGAController->end();
		_VGAController.reset();
	}

    uint8_t oldMode = videoMode;
	videoMode = mode;

	TaskHandle_t xHandle = NULL;
	xTaskCreatePinnedToCore(otf, "OTF-MODE", 8192, NULL,
							OTF_MANAGER_PRIORITY, &xHandle, 1); // Core #1
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
