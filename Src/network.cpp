/*
 * network.c
 *
 *  Created on: Apr 7, 2018
 *      Author: kisada
 */


extern "C" {
	#include "mac_eeprom.h"
	#include "dhcp.h"
	#include "wizchip_conf.h"
	#include "socket.h"
	#include "w5500.h"
	#include "main.h"
	#include "main2.h"
	#include "window.h"
	#include "stm32f0xx_hal.h"
};


#include "network.hpp"
#include "main2.hpp"
#include "window.hpp"




wiz_NetInfo netInfo = {
		.mac   = { 0 },       // Mac address will be set later from EEPROM
		.ip    = { 0 },
		.sn    = { 0 },
		.gw    = { 0 },
		.dns   = { 0 },
		.dhcp  = NETINFO_DHCP //Using DHCP
};

const size_t DATA_BUF_SIZE = 2048;
uint8_t gDATABUF[DATA_BUF_SIZE];

void ip_assign(){
	getIPfromDHCP(netInfo.ip);
	getGWfromDHCP(netInfo.gw);
	getSNfromDHCP(netInfo.sn);
	getDNSfromDHCP(netInfo.dns);
	wizchip_setnetinfo(&netInfo);
	HAL_GPIO_WritePin(LED_DHCP_GPIO_Port, LED_DHCP_Pin, GPIO_PIN_SET);
}

void ip_conflict(){
	HAL_GPIO_WritePin(LED_DHCP_GPIO_Port, LED_DHCP_Pin, GPIO_PIN_RESET);
	while(1);
}

void reset_w5500(){
	HAL_GPIO_WritePin(W5500_RESET_GPIO_Port, W5500_RESET_Pin, GPIO_PIN_RESET);
	HAL_Delay(1); //min reset cycle 500 us
	HAL_GPIO_WritePin(W5500_RESET_GPIO_Port, W5500_RESET_Pin, GPIO_PIN_SET);
	HAL_Delay(2); //PLL lock 1 ms max (refer datasheet)
}


//DHCP 1s timer located in stm32f0xx_it.c


namespace net{

void cs_sel() {
	HAL_GPIO_WritePin(SPI1_NSS_GPIO_Port, SPI1_NSS_Pin, GPIO_PIN_RESET); //CS LOW
}

void cs_desel() {
	HAL_GPIO_WritePin(SPI1_NSS_GPIO_Port, SPI1_NSS_Pin, GPIO_PIN_SET); //CS HIGH
}

uint8_t spi_rb(void) {
	uint8_t rbuf;
	HAL_SPI_Receive(&hspi1, &rbuf, 1, 0xFFFFFFFF);
	return rbuf;
}

void spi_wb(uint8_t b) {
	HAL_SPI_Transmit(&hspi1, &b, 1, 0xFFFFFFFF);
}

/*********************************
 *  network Class function defs
 ********************************/

network::network(){
}

void network::init(){
	uint8_t memsize[2][8] = { { 2, 2, 2, 2, 2, 2, 2, 2 }, { 2, 2, 2, 2, 2, 2, 2, 2 } };

	reset_w5500();

	reg_wizchip_cs_cbfunc(cs_sel, cs_desel);
	reg_wizchip_spi_cbfunc(spi_rb, spi_wb);

	ctlwizchip(CW_INIT_WIZCHIP, (void*) memsize);

	getMAC(netInfo.mac);
	wizchip_setnetinfo(&netInfo);
	wizchip_getnetinfo(&netInfo);

	DHCP_init(6, gDATABUF);
	reg_dhcp_cbfunc(ip_assign, ip_assign, ip_conflict);

	socket(1, Sn_MR_UDP, 2000, 0x00);
	socket(2, Sn_MR_UDP, 3000, 0x00);
}

void network::step_network(){
	size_t size = getSn_RX_RSR(1);

	if(size){
		HAL_GPIO_TogglePin(LED_COMM_GPIO_Port, LED_COMM_Pin);

		//todo treat big and small datagrams

		char buff[5];

		uint8_t svr_addr[6];
		uint16_t len;
		uint16_t  svr_port;
		len = recvfrom(1, (uint8_t *)buff, size, svr_addr, &svr_port);

		if(buff[0]!='S' || buff[1]!='E' || buff[2]!='M')
			return;

		switch(buff[3]){
		case use_external_anim:
			main_state=external_anim;
			break;
		case use_internal_anim:
			main_state=internal_anim;
			break;
		case blank:
			for(size_t j=0; j<windows::num_of_pixels; j++){
				windows::right_window.blank();
				windows::left_window.blank();
			}

			break;
		default:
			break;
		}
	}

	size= getSn_RX_RSR(2);
	if(size){
		HAL_GPIO_TogglePin(LED_COMM_GPIO_Port, LED_COMM_Pin);
		//todo treat big and small datagrams

				char buff[10];

				uint8_t svr_addr[6];
				uint16_t  svr_port;
				uint16_t len;
				len = recvfrom(2, (uint8_t *)buff, size, svr_addr, &svr_port);

				//todo check indexes, datagram size

				size_t window = buff[0];
				size_t pixel_num = buff[1];
				uint8_t red = buff[2];
				uint8_t green = buff[3];
				uint8_t blue = buff[4];

				if(window == 0) //right window
					windows::right_window.pixels[pixel_num].set(red, green, blue);
				else
					windows::left_window.pixels[pixel_num].set(red, green, blue);

	}

	//DHCP_run();
	return;

	//do DHCP task
	switch(DHCP_run()){
	case DHCP_IP_ASSIGN:
	case DHCP_IP_CHANGED:
		HAL_GPIO_WritePin(LED_DHCP_GPIO_Port, LED_DHCP_Pin, GPIO_PIN_SET);
		break;
	case DHCP_IP_LEASED:
		//HAL_GPIO_WritePin(LED_DHCP_GPIO_Port, LED_DHCP_Pin, GPIO_PIN_RESET);
		break;
	case DHCP_FAILED:
		HAL_GPIO_WritePin(LED_COMM_GPIO_Port, LED_COMM_Pin, GPIO_PIN_RESET);
		break;
	default:
		break;
	}
}


/*****************************
 *
 *    Static instances
 *
 ****************************/
	network inetwork;
};

