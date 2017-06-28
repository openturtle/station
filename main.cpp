/*
 * station.cpp
 *
 * Created: 6/26/2017 2:47:13 PM
 * Author : @ibrohimislam
 */

#include <avr/io.h>
#include "lib/ldpc.h"
#include "lib/ST7540.h"

extern "C" {
	#include "pca.h"
	#include "bus.h"
	#include "serial.h"
	#include "spi_common.h"
	#include "spi_hw_poll.h"
}

uint8_t ST7540_REG[3] = {0x05, 0x24, 0x27};
	
// 0000 0101 0010 0100 0010 0111
	
// 0 0 0 0 0 1 0 10 0 10 0 10 00 0 1 00 111
// 3 2 1 0 9 8 7 65 4 32 1 09 87 6 5 43 210
//       2               X 1              0
	
typedef struct {
	uint8_t dummy[2];
	uint8_t header[4];
	uint8_t body[26];
} Packet;

typedef union {
	Packet data;
	uint8_t byte[32];
} PacketToSend;

typedef struct {
	uint8_t* dummy;
	uint8_t* header;
	uint8_t* body;
} PacketPointer;

int main(void) {
	
	struct bus_t spi_bus = initialize();
	
	serial_init(E_BAUD_9600);
	serial_install_interrupts(E_FLAGS_SERIAL_RX_INTERRUPT);
	serial_flush();

	serial_install_stdio();
	
	PacketToSend packetToSend;
	
	PacketPointer packet;
	packet.dummy = packetToSend.data.dummy;
	packet.header = packetToSend.data.header;
	packet.body = packetToSend.data.body;
	
	packet.dummy[0] = 0x00;
	packet.dummy[1] = 0x00;
	
	packet.header[0] = 0xFF;
	packet.header[1] = 0xFF;
	packet.header[2] = 0xFF;
	packet.header[3] = 0x00;
	
	uint8_t nextIndex = 0;
	
	while (1) {
		
		uint8_t c;
		
		if (serial_getc(&c)) {
			
			packet.body[nextIndex] = c;
			nextIndex++;
			
			if (nextIndex == 20) {
				
				LDPC_Encode(packet.body);
				nextIndex = 0;
				
				tx_mode(); _delay_ms(10);
				spi_bus.f_send(spi_bus.priv, packetToSend.byte, 32, 1);
				rx_mode(); _delay_ms(10);
			}
			
		}
		
    }
}

