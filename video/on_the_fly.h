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
	debug_log("@%i\n",__LINE__);
	di_manager->create_root();
	debug_log("@%i\n",__LINE__);

	auto terminal = di_manager->create_terminal(1, ROOT_PRIMITIVE_ID, PRIM_FLAGS_DEFAULT, 0, 0, 100, 75, fabgl::FONT_AGON_DATA);
	debug_log("@%i\n",__LINE__);
	terminal->define_character_range(0x20, 0x7E, PIXEL_ALPHA_100_MASK|0x05, PIXEL_ALPHA_100_MASK|0x00);
	debug_log("@%i\n",__LINE__);
	terminal->clear_screen();
	debug_log("@%i\n",__LINE__);
	di_manager->generate_code_for_primitive(1);
	debug_log("@%i\n",__LINE__);
	print("\r\n\r\n\r\n\r\n\r\n");
	print("** Test of OTF 800x600x64 mode **\r\n");
	debug_log("@%i\n",__LINE__);
	debug_log("Running OTF manager...\r\n");
	debug_log("@%i\n",__LINE__);
	di_manager->run();
}

int8_t use_otf_mode(int8_t mode) {
	debug_log("@%i\n",__LINE__);
	if (_VGAController) {
	debug_log("@%i\n",__LINE__);
		_VGAController->end();
	debug_log("@%i\n",__LINE__);
		_VGAController.reset();
	debug_log("@%i\n",__LINE__);
	}
	debug_log("@%i\n",__LINE__);

    uint8_t oldMode = videoMode;
	debug_log("@%i\n",__LINE__);
	videoMode = mode;
	debug_log("@%i\n",__LINE__);

	TaskHandle_t xHandle = NULL;
	xTaskCreatePinnedToCore(otf, "OTF-MODE", 8192, NULL,
							OTF_MANAGER_PRIORITY, &xHandle, 1); // Core #1
	debug_log("@%i\n",__LINE__);
	return 0; // success

    //return change_mode(oldMode);
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
