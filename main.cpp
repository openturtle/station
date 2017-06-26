/*
 * station.cpp
 *
 * Created: 6/26/2017 2:47:13 PM
 * Author : @ibrohimislam
 */

#define F_CPU 16000000UL

#include <avr/io.h>
#include "lib/ldpc.h"

extern "C" {
	#include "pca.h"
	#include "bus.h"
	#include "serial.h"
	#include "spi_common.h"
	#include "spi_hw_poll.h"
}

#define DDR_BU_THERM    DDRC
#define DDR_RXTX        DDRC
#define DDR_REG_DATA    DDRC
#define DDR_CD_PD       DDRC

#define PORT_BU_THERM   PORTC
#define PORT_RXTX       PORTC
#define PORT_REG_DATA   PORTC
#define PORT_CD_PD      PORTC

#define PIN_BU_THERM    PINC
#define PIN_CD_PD       PINC

#define BU_THERM        PC0
#define RXTX            PC1
#define REG_DATA        PC4
#define CD_PD           PC5

#define reg_acc()       PORT_REG_DATA   |=  (1<<REG_DATA);
#define data_acc()      PORT_REG_DATA   &=  ~(1<<REG_DATA);

#define tx_mode()       PORT_RXTX   &=  ~(1<<RXTX);
#define rx_mode()       PORT_RXTX   |=  (1<<RXTX);

// SPI
#define PORT_SPI    PORTB
#define PIN_SPI     PINB
#define DDR_SS      DDRB
#define PORT_SS     PORTB
#define ss_clear()      PORT_SS &=  ~(1<<SS_PIN);
#define ss_set()        PORT_SS |=  (1<<SS_PIN);

// LED
#define SS_PIN      PB1
#define DDR_LED     DDRD
#define PORT_LED    PORTD
#define LED1        PD6
#define LED2        PD7

typedef struct {
	uint8_t dummy[2];
	uint8_t header[4];
	uint8_t body[26];
} packet;

typedef union {
	packet data;
	uint8_t byte[32];
} packet_to_send;

packet c = {
	{0x00, 0x00},
	{0xFF, 0xFF, 0xFF, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}
};

int main(void) {
	
	serial_init(E_BAUD_9600);
	serial_install_interrupts(E_FLAGS_SERIAL_RX_INTERRUPT);
	serial_flush();
	serial_install_stdio();
	
	while (1) {
		for (int i = 0; i < 20; i++) {
			scanf("%c", &(c.body[i]));
		}
		
		LDPC_Encode(c.body); 
		
		for (int i = 0; i < 25; i++) {
			printf("0x%02x ", c.body[i]);
		}
		printf("0x%02x\n", c.body[25]);
    }
}

