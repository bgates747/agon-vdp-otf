//
// Title:			Agon Video BIOS
// Author:			Dean Belfield
// Contributors:	Jeroen Venema (Sprite Code, VGA Mode Switching)
//					Damien Guard (Fonts)
//					Igor Chaves Cananea (vdp-gl maintenance)
//					Steve Sims (Audio enhancements, refactoring, bug fixes)
// Created:			22/03/2022
// Last Updated:	12/09/2023
//
// Modinfo:
// 11/07/2022:		Baud rate tweaked for Agon Light, HW Flow Control temporarily commented out
// 26/07/2022:		Added VDU 29 support
// 03/08/2022:		Set codepage 1252, fixed keyboard mappings for AGON, added cursorTab, VDP serial protocol
// 06/08/2022:		Added a custom font, fixed UART initialisation, flow control
// 10/08/2022:		Improved keyboard mappings, added sprites, audio, new font
// 05/09/2022:		Moved the audio class to agon_audio.h, added function prototypes in agon.h
// 02/10/2022:		Version 1.00: Tweaked the sprite code, changed bootup title to Quark
// 04/10/2022:		Version 1.01: Can now change keyboard layout, origin and sprites reset on mode change, available modes tweaked
// 23/10/2022:		Version 1.02: Bug fixes, cursor visibility and scrolling
// 15/02/2023:		Version 1.03: Improved mode, colour handling and international support
// 04/03/2023:					+ Added logical screen resolution, sendScreenPixel now sends palette index as well as RGB values
// 09/03/2023:					+ Keyboard now sends virtual key data, improved VDU 19 to handle COLOUR l,p as well as COLOUR l,r,g,b
// 15/03/2023:					+ Added terminal support for CP/M, RTC support for MOS
// 21/03/2023:				RC2 + Added keyboard repeat delay and rate, logical coords now selectable
// 22/03/2023:					+ VDP control codes now indexed from 0x80, added paged mode (VDU 14/VDU 15)
// 23/03/2023:					+ Added VDP_GP
// 26/03/2023:				RC3 + Potential fixes for FabGL being overwhelmed by faster comms
// 27/03/2023:					+ Fix for sprite system crash
// 29/03/2023:					+ Typo in boot screen fixed
// 01/04/2023:					+ Added resetPalette to MODE, timeouts to VDU commands
// 08/04/2023:				RC4 + Removed delay in readbyte_t, fixed VDP_SCRCHAR, VDP_SCRPIXEL
// 12/04/2023:					+ Fixed bug in play_note
// 13/04/2023:					+ Fixed bootup fail with no keyboard
// 17/04/2023:				RC5 + Moved wait_completion in vdu so that it only executes after graphical operations
// 18/04/2023:					+ Minor tweaks to wait completion logic
// 12/05/2023:		Version 1.04: Now uses vdp-gl instead of FabGL, implemented GCOL mode, sendModeInformation now sends video mode
// 19/05/2023:					+ Added VDU 4/5 support
// 25/05/2023:					+ Added VDU 24, VDU 26 and VDU 28, fixed inverted text colour settings
// 30/05/2023:					+ Added VDU 23,16 (cursor movement control)
// 28/06/2023:					+ Improved get_screen_char, fixed vdu_textViewport, cursorHome, changed modeline for Mode 2
// 30/06/2023:					+ Fixed vdu_sys_sprites to correctly discard serial input if bitmap allocation fails
// 13/08/2023:				RC2	+ New video modes, mode change resets page mode
// 05/09/2023:					+ New audio enhancements, improved mode change code
// 12/09/2023:					+ Refactored
// 17/09/2023:					+ Added ZDI mode

#include <WiFi.h>
#include <HardwareSerial.h>
#include <fabgl.h>

#define	DEBUG			1						// Serial Debug Mode: 1 = enable
#define SERIALBAUDRATE	115200

HardwareSerial	DBGSerial(0);

bool			terminalMode = false;			// Terminal mode (for CP/M)
bool			consoleMode = false;			// Serial console mode (0 = off, 1 = console enabled)

#include "agon.h"								// Configuration file
#include "version.h"							// Version information
#include "agon_ps2.h"							// Keyboard support
#include "agon_audio.h"							// Audio support
#include "agon_ttxt.h"
#include "graphics.h"							// Graphics support
#include "cursor.h"								// Cursor support
#include "vdp_protocol.h"						// VDP Protocol
#include "vdu_stream_processor.h"
#include "hexload.h"
#include "on_the_fly.h"							// Access to On-the-Fly (OTF mode)

fabgl::Terminal			Terminal;				// Used for CP/M mode
VDUStreamProcessor *	processor;				// VDU Stream Processor

#include "zdi.h"								// ZDI debugging console

void setup() {
	disableCore0WDT(); delay(200);				// Disable the watchdog timers
	disableCore1WDT(); delay(200);
	DBGSerial.begin(SERIALBAUDRATE, SERIAL_8N1, 3, 1);
	setupVDPProtocol();
	processor = new VDUStreamProcessor(&VDPSerial);
	processor->wait_eZ80();
	setupKeyboardAndMouse();
	init_audio();
	copy_font();
	set_mode(0x47);
	processor->sendModeInformation();
	boot_screen();
}

// The main loop
//
void loop() {
	bool drawCursor = false;
	auto cursorTime = millis();

	while (true) {
		if (terminalMode) {
			do_keyboard_terminal();
			continue;
		}
		if (millis() - cursorTime > CURSOR_PHASE) {
			cursorTime = millis();
			drawCursor = !drawCursor;
			if (ttxtMode) {
				ttxt_instance.flash(drawCursor);
			}
			do_cursor();
		}
		do_keyboard();
		do_mouse();

		if (processor->byteAvailable()) {
			if (drawCursor) {
				drawCursor = false;
				do_cursor();
			}
			processor->processNext();
		}
	}
}

// Determine whether stream data is available
bool stream_byte_available() {
	return processor->byteAvailable();
}

// Get a byte from the incoming stream
uint8_t stream_read_byte() {
	return processor->readByte();
}

// Send the keyboard state
void sendKeyboardState() {
	processor->sendKeyboardState();
}

// Handle the keyboard: BBC VDU Mode
//
void do_keyboard() {
	uint8_t keycode;
	uint8_t modifiers;
	uint8_t vk;
	uint8_t down;
	if (getKeyboardKey(&keycode, &modifiers, &vk, &down)) {
		// Handle some control keys
		//
		switch (keycode) {
			case 14: setPagedMode(true); break;
			case 15: setPagedMode(false); break;
		}
		// Create and send the packet back to MOS
		//
		uint8_t packet[] = {
			keycode,
			modifiers,
			vk,
			down,
		};
		processor->send_packet(PACKET_KEYCODE, sizeof packet, packet);
	}
}

// Handle the keyboard: CP/M Terminal Mode
// 
void do_keyboard_terminal() {
	uint8_t ascii;
	if (getKeyboardKey(&ascii)) {
		// send raw byte straight to z80
		processor->writeByte(ascii);
	}

	// Write anything read from z80 to the screen
	//
	while (processor->byteAvailable()) {
		Terminal.write(processor->readByte());
	}
}

// Handle the mouse
//
void do_mouse() {
	// get mouse delta, if the mouse is active
	MouseDelta delta;
	if (mouseMoved(&delta)) {
		auto mouse = getMouse();
		auto mStatus = mouse->status();
		// update mouse cursor position if it's active
		setMouseCursorPos(mStatus.X, mStatus.Y);
		processor->sendMouseData(&delta);
	}
}

// The boot screen
//
void boot_screen() {
	printFmt("Agon %s VDP Version %d.%d.%d", VERSION_VARIANT, VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH);
	#if VERSION_CANDIDATE > 0
		printFmt(" %s%d", VERSION_TYPE, VERSION_CANDIDATE);
	#endif
	// Show build if defined (intended to be auto-generated string from build script from git commit hash)
	#ifdef VERSION_BUILD
		printFmt(" Build %s", VERSION_BUILD);
	#endif
	printFmt("\n\r");
}

// Debug printf to PC
//
void debug_log(const char *format, ...) {
	#if DEBUG == 1
	va_list ap;
	va_start(ap, format);
	auto size = vsnprintf(nullptr, 0, format, ap) + 1;
	if (size > 0) {
		va_end(ap);
		va_start(ap, format);
		char buf[size + 1];
		vsnprintf(buf, size, format, ap);
		DBGSerial.print(buf);
	}
	va_end(ap);
	#endif
}

// Set console mode
// Parameters:
// - mode: 0 = off, 1 = on
//
void setConsoleMode(bool mode) {
	consoleMode = mode;
}

// Switch to terminal mode
//
void switchTerminalMode() {
	cls(true);
	canvas.reset();
	Terminal.begin(_VGAController.get());	
	Terminal.connectSerialPort(VDPSerial);
	Terminal.enableCursor(true);
	terminalMode = true;
}

void print(char const * text) {
	if (is_otf_mode()) {
		otf_print(text);
		return;
	}
	for (auto i = 0; i < strlen(text); i++) {
		processor->vdu(text[i]);
	}
}

void printFmt(const char *format, ...) {
	va_list ap;
	va_start(ap, format);
	int size = vsnprintf(nullptr, 0, format, ap) + 1;
	if (size > 0) {
		va_end(ap);
		va_start(ap, format);
		char buf[size + 1];
		vsnprintf(buf, size, format, ap);
		print(buf);
	}
	va_end(ap);
}
