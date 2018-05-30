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
	#include "stm32f0xx_hal.h"
	#include "dhcp_buffer.h"
	#include "stm32f0xx_ll_gpio.h"
};

#include "network.hpp"
#include "main2.hpp"
#include "window.hpp"
#include <cstdio>
#include "version.hpp"

//------------------------------------------------------------------------

wiz_NetInfo netInfo = {
		.mac   = { 0 },       // Mac address will be set later from EEPROM
		.ip    = { 0 },
		.sn    = { 0 },
		.gw    = { 0 },
		.dns   = { 0 },
		.dhcp  = NETINFO_DHCP //Using DHCP
};

void ip_assign(){
	getIPfromDHCP(netInfo.ip);
	getGWfromDHCP(netInfo.gw);
	getSNfromDHCP(netInfo.sn);
	getDNSfromDHCP(netInfo.dns);
	wizchip_setnetinfo(&netInfo);
	LL_GPIO_SetOutputPin(LED_DHCP_GPIO_Port, LED_DHCP_Pin);
}

void ip_conflict(){
	LL_GPIO_ResetOutputPin(LED_DHCP_GPIO_Port, LED_DHCP_Pin);
	while(1); //TODO better handling
}

//DHCP 1s timer located in stm32f0xx_it.c


namespace net{

void cs_sel() {
	LL_GPIO_ResetOutputPin(SPI1_NSS_GPIO_Port, SPI1_NSS_Pin); //CS LOW
}

void cs_desel() {
	LL_GPIO_SetOutputPin(SPI1_NSS_GPIO_Port, SPI1_NSS_Pin); //CS HIGH
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
	wiz_PhyConf phyconf;

	//Hard-reset W5500
	LL_GPIO_ResetOutputPin(W5500_RESET_GPIO_Port, W5500_RESET_Pin);
	HAL_Delay(1); //min reset cycle 500 us
	LL_GPIO_SetOutputPin(W5500_RESET_GPIO_Port, W5500_RESET_Pin);
	HAL_Delay(2); //PLL lock 1 ms max (refer datasheet)

	reg_wizchip_cs_cbfunc(cs_sel, cs_desel);
	reg_wizchip_spi_cbfunc(spi_rb, spi_wb);

	ctlwizchip(CW_INIT_WIZCHIP, (void*) memsize);
	wizphy_setphypmode(PHY_POWER_DOWN);

	wizphy_getphyconf(&phyconf);
	phyconf.by = PHY_CONFBY_SW;
	phyconf.duplex = PHY_DUPLEX_FULL;
	phyconf.mode = PHY_MODE_MANUAL;
	phyconf.speed = PHY_SPEED_100;
	wizphy_setphyconf(&phyconf);

	wizphy_setphypmode(PHY_POWER_NORM);

	getMAC(netInfo.mac);
	wizchip_setnetinfo(&netInfo);
	wizchip_getnetinfo(&netInfo);

	DHCP_init(6, gDATABUF);
	reg_dhcp_cbfunc(ip_assign, ip_assign, ip_conflict);

	socket(1, Sn_MR_UDP, 2000, 0x00);
	socket(2, Sn_MR_UDP, 3000, 0x00);
}

size_t network::create_status_string(){
	int ret;

	ret = snprintf((char*)this->status_string, sizeof(status_string),
		"MUEB FW version: %s\n"
		"anim_source: %#x\n"
		"telemetry_comm_buff: %#x\n"
		"frame_ether_buff: %#x\n"
		"dhcp_lease_time: %d\n"
		"dhcp_reamining_lease_time: %d\n"
		"SEM forever\n",
		mueb_version,
		main_state,
		getSn_RX_RSR(1),
		getSn_RX_RSR(2),
		getDHCPLeasetime(),
		getDHCPTimeBeforeLease()
	);

	return (ret>=0) ? ret : 1;
}

void network::do_remote_command(){
	size_t size = getSn_RX_RSR(1);

	if(size){
		HAL_GPIO_TogglePin(LED_COMM_GPIO_Port, LED_COMM_Pin);

		char buff[4];

		uint8_t resp_addr[6];
		uint16_t len;
		uint16_t resp_port;

		// Handle too small packages
		if(size < 4){
			len = recvfrom(1, (uint8_t *)buff, 4, resp_addr, &resp_port);
			return;
		}

		len = recvfrom(1, (uint8_t *)buff, 4, resp_addr, &resp_port);

		if(len!=4)  //todo consider code remove see above
			return;

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
				windows::right_window.blank();
				windows::left_window.blank();
				break;
			case turn_12v_off_left:
				windows::left_window.set_state(windows::window::vcc_12v_off);
				break;
			case turn_12v_off_right:
				windows::right_window.set_state(windows::window::vcc_12v_off);
				break;
			case reset_left_panel:
				windows::left_window.set_state(windows::window::discharge_caps);
				break;
			case reset_right_panel:
				windows::right_window.set_state(windows::window::discharge_caps);
				break;
			case reboot:
				NVIC_SystemReset();
				break;
			case get_status:
				sendto(1, status_string, network::create_status_string(), resp_addr, resp_port);
				break;
			case delete_anim_network_buffer:
				///To be implemented TODO
				break;
			default:
				break;
		}
	}
}

void network::fetch_frame(){
	size_t size= getSn_RX_RSR(2);
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
}

void network::step_network(){
	 if (wizphy_getphylink() == PHY_LINK_ON){
		LL_GPIO_SetOutputPin(LED_JOKER_GPIO_Port, LED_JOKER_Pin);

		do_remote_command();
		fetch_frame();

		//do DHCP task
		switch(DHCP_run()){
		case DHCP_IP_ASSIGN:
		case DHCP_IP_CHANGED:
			LL_GPIO_SetOutputPin(LED_DHCP_GPIO_Port, LED_DHCP_Pin);
			break;
		case DHCP_IP_LEASED:
			//HAL_GPIO_WritePin(LED_DHCP_GPIO_Port, LED_DHCP_Pin, GPIO_PIN_RESET);
			break;
		case DHCP_FAILED:
			LL_GPIO_ResetOutputPin(LED_DHCP_GPIO_Port, LED_DHCP_Pin);
			break;
		default:
			break;
		}
	 } else {
		 LL_GPIO_ResetOutputPin(LED_JOKER_GPIO_Port, LED_JOKER_Pin);

		 LL_GPIO_ResetOutputPin(LED_DHCP_GPIO_Port, LED_DHCP_Pin);
		 DHCP_renew();
	 }
}


/*****************************
 *
 *    Static instances
 *
 ****************************/
	network inetwork;
};

