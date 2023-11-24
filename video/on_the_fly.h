#ifndef ON_THE_FLY
#define ON_THE_FLY

#include "src/di_manager.h"
#include <memory>
#include <fabgl.h>
#include "agon.h"
#include "agon_fonts.h"

extern std::unique_ptr<fabgl::Canvas>           	canvas;
extern std::unique_ptr<fabgl::VGABaseController>	_VGAController;
extern uint8_t                                      videoMode;
extern uint8_t                                      _VGAColourDepth;

extern void print(char const * text);
extern int8_t change_mode(uint8_t mode);

DiManager* di_manager; // Used for OTF 800x600x64 mode

int8_t use_otf_mode(int8_t mode) {
	if (_VGAController) {
		_VGAController->end();
		_VGAController.reset();
	}

    uint8_t oldMode = videoMode;
	di_manager = new DiManager();
	di_manager->create_root();

	auto terminal = di_manager->create_terminal(1, ROOT_PRIMITIVE_ID, PRIM_FLAGS_DEFAULT, 0, 0, 100, 75, fabgl::FONT_AGON_DATA);
	terminal->define_character_range(0x20, 0x7E, PIXEL_ALPHA_100_MASK|0x05, PIXEL_ALPHA_100_MASK|0x00);
	terminal->clear_screen();
	di_manager->generate_code_for_primitive(1);
	print("** Test of OTF 800x600x64 mode **\r\n");
	debug_log("Running OTF manager...\r\n");
	di_manager->run();

    return change_mode(oldMode);
}

#endif
