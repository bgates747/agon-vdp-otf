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

	//auto text_area = di_manager->create_text_area(1, ROOT_PRIMITIVE_ID, PRIM_FLAGS_DEFAULT, 0, 0, 100, 75, fabgl::FONT_AGON_DATA);
	//text_area->define_character_range(0x20, 0x7E, PIXEL_ALPHA_100_MASK|0x05, PIXEL_ALPHA_100_MASK|0x00);
	//text_area->clear_screen();
	//di_manager->generate_code_for_primitive(1);
	
	OtfCmd_41_Create_primitive_Solid_Rectangle cmd;
	cmd.m_color = 0xC0;
	cmd.m_flags = PRIM_FLAGS_DEFAULT;
	cmd.m_w = 800;
	cmd.m_h = 600;
	cmd.m_id = 3;
	cmd.m_pid = 0;
	cmd.m_x = 0;
	cmd.m_y = 0;
	auto rect = di_manager->create_solid_rectangle(&cmd);
	di_manager->generate_code_for_primitive(cmd.m_id);

debug_log("@%i\n",__LINE__);
	OtfCmd_140_Create_primitive_Group gcmd;
	gcmd.m_flags = PRIM_FLAGS_DEFAULT;
	gcmd.m_w = 40;
	gcmd.m_h = 30;
	gcmd.m_id = 4;
	gcmd.m_pid = 0;
	gcmd.m_x = 400-20;
	gcmd.m_y = 300-15;
debug_log("@%i\n",__LINE__);
	di_manager->create_primitive_group(&gcmd);
debug_log("@%i\n",__LINE__);
	di_manager->generate_code_for_primitive(gcmd.m_id);
debug_log("@%i\n",__LINE__);

	cmd.m_color = 0xD0;
	cmd.m_flags = PRIM_FLAGS_DEFAULT;
	cmd.m_w = 80;
	cmd.m_h = 60;
	cmd.m_id = 5;
	cmd.m_pid = gcmd.m_id;
	cmd.m_x = 40/2-40;
	cmd.m_y = 30/2-30;
	rect = di_manager->create_solid_rectangle(&cmd);
	di_manager->generate_code_for_primitive(cmd.m_id);

	auto dot = di_manager->create_point(10, 0, PRIM_FLAGS_DEFAULT, 400, 300, 0xFF);
	di_manager->generate_code_for_primitive(dot->get_id());

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
