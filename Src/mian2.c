#include "window.h"
#include "main2.h"
#include "main.h"
#include "wizchip_conf.h"
#include "socket.h"
#include "w5500.h"

#include "mac_eeprom.h"
#include "dhcp.h"

wiz_NetInfo netInfo = {
		.mac   = { 0 },       // Mac address will be set later from EEPROM
		.ip    = { 0 },
		.sn    = { 0 },
		.gw    = { 0 },
		.dhcp  = NETINFO_DHCP //Using DHCP
	};

#define DATA_BUF_SIZE   2048
uint8_t gDATABUF[DATA_BUF_SIZE];

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

void step_internal_anim(){
	static uint32_t i = 0;
	static char szin = 0;

	for(size_t j=0;j<2;j++)
		for(size_t k=0;k<num_of_pixels;k++){
			pixels[j][k].red=(szin == 0 ? i : 0);
			pixels[j][k].green=(szin == 1 ? i : 0);
			pixels[j][k].blue=(szin == 2 ? i : 0);
			pixels[j][k].stat = buffer_full;
		}
	i++;
	if (i == 0x8)
		i = 0;
	if (i == 0)
		szin++;
	if (szin == 3)
		szin = 0;
}

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

void main2(void){
	uint8_t bufSize[] = { 2, 2, 2, 2 };

	reset_w5500();

	reg_wizchip_cs_cbfunc(cs_sel, cs_desel);
	reg_wizchip_spi_cbfunc(spi_rb, spi_wb);
	wizchip_init(bufSize, bufSize);

	getMAC(netInfo.mac);
	wizchip_setnetinfo(&netInfo);

	DHCP_init(6, gDATABUF);
	reg_dhcp_cbfunc(ip_assign, ip_assign, ip_conflict);


	while (1){
		step_internal_anim();
		HAL_Delay(500);
		//HAL_GPIO_TogglePin(LED_HEARTH_GPIO_Port, LED_HEARTH_Pin);

		step_window_state(right_window);
		//step_window_state(right_window);

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

	//todo error handling, use DHCP led, use lease ticker
}
