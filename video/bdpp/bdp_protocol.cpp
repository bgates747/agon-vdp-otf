//
// Title:			Agon Video BIOS
// Author:			Dean Belfield
// Subtitle:		Bidirectional Packet Protocol
// Author:			Curtis Whitley
// Contributors:	
// Created:			29/01/2024
// Last Updated:	29/01/2024
//

#include "bdp_protocol.h"
#include <HardwareSerial.h>
#include <string.h>
#include <queue>
#include "soc/uhci_struct.h"
#include "soc/uart_struct.h"
#include "soc/uart_periph.h"
#include "driver/uart.h"
#include "uhci_driver.h"
#include "uhci_hal.h"

#define UHCI_NUM  UHCI_NUM_0
#define UART_NUM  UART_NUM_2
#define DEBUG_BDPP 1

extern void debug_log(const char* fmt, ...);

bool bdpp_initialized; // Whether the driver has been initialized
std::queue<Packet*> bdpp_tx_queue; // Transmit (TX) packet queue
std::queue<UhciPacket*> bdpp_rx_queue[BDPP_MAX_STREAMS]; // Receive (RX) packet queue

//--------------------------------------------------

// Get whether the driver has been initialized.
bool bdpp_is_initialized() {
	return bdpp_initialized;
}

// Initialize the BDPP driver.
//
void bdpp_initialize_driver() {
#if DEBUG_BDPP
	debug_log("Activating BDPP.\n");
#endif
//bdpp_initialized = true; return;
	// Initialize the UART2/UHCI hardware.
	//while (Serial2.available()) {
	//	debug_log("Toss: %02hX\n", Serial2.read());
	//}
	Serial2.end(); // stop existing communication

    uart_config_t uart_config = {
        .baud_rate = 1152000,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_RTS,
        .rx_flow_ctrl_thresh = 120,
		.source_clk = UART_SCLK_APB
    };

    uhci_driver_install(UHCI_NUM, 0);
    uhci_attach_uart_port(UHCI_NUM, UART_NUM, &uart_config);
	uart_dma_read();
	bdpp_initialized = true;
}

// Queue a packet for transmission to the EZ80.
// The packet is expected to be full (to contain all data that
// VDP wants to place into it) when this function is called.
void bdpp_queue_tx_packet(Packet* packet) {
	auto uhci_packet = packet->get_uhci_packet();
	auto act_size = packet->get_uhci_packet()->get_actual_data_size();
	debug_log("Queue TX pkt: %02hX %02hX %02hX %02hX %02hX (%hu):",
		uhci_packet->get_flags(),
		uhci_packet->get_packet_index(),
		uhci_packet->get_stream_index(),
		act_size & 0xFF, act_size >> 8, act_size);
	auto data = uhci_packet->get_data();
	for (uint16_t i = 0; i < act_size; i++) {
		debug_log(" %02hX", data[i]);
	}
	debug_log("\n");

	auto old_int = uhci_disable_interrupts();
	uhci_packet->set_flags(BDPP_PKT_FLAG_READY);
	bdpp_tx_queue.push(packet);
	uhci_enable_interrupts(old_int);
	uart_dma_start_transmitter();
}

// Check for a received packet being available.
bool bdpp_rx_packet_available(uint8_t stream_index) {
	auto old_int = uhci_disable_interrupts();
	bool available = !bdpp_rx_queue[stream_index].empty();
	uhci_enable_interrupts(old_int);
	return available;
}

// Get a received packet.
UhciPacket* bdpp_get_rx_packet(uint8_t stream_index) {
	debug_log("@%i\n",__LINE__);
	UhciPacket* packet = NULL;
	auto old_int = uhci_disable_interrupts();
	auto queue = &bdpp_rx_queue[stream_index];
	if (!queue->empty()) {
		packet = queue->front();
		queue->pop();
	}
	uhci_enable_interrupts(old_int);
	debug_log("@%i\n",__LINE__);
	return packet;
}
