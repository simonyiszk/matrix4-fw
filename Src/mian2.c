#include "main2.h"
#include "main.h"
#include "wizchip_conf.h"
#include "socket.h"
#include "w5500.h"
#include "window_blocking.h"
#include "mac_eeprom.h"
#include "dhcp.h"

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

	set_pixel(left_window, 0, (szin == 0 ? i : 0), (szin == 1 ? i : 0),
			(szin == 2 ? i : 0));
	set_pixel(left_window, 1, (szin == 0 ? i : 0), (szin == 1 ? i : 0),
			(szin == 2 ? i : 0));
	set_pixel(left_window, 2, (szin == 0 ? i : 0), (szin == 1 ? i : 0),
			(szin == 2 ? i : 0));
	set_pixel(left_window, 3, (szin == 0 ? i : 0), (szin == 1 ? i : 0),
			(szin == 2 ? i : 0));
	i++;
	if (i == 0x8)
		i = 0;
	if (i == 0)
		szin++;
	if (szin == 3)
		szin = 0;
}

void main2(void){
	uint8_t retVal, sockStatus;
	int16_t rcvLen;
	uint8_t rcvBuf[20], bufSize[] = { 2, 2, 2, 2 };

	HAL_GPIO_WritePin(W5500_RESET_GPIO_Port, W5500_RESET_Pin, GPIO_PIN_RESET);
	HAL_Delay(1); //min reset cycle 500 us
	HAL_GPIO_WritePin(W5500_RESET_GPIO_Port, W5500_RESET_Pin, GPIO_PIN_SET);
	HAL_Delay(2); //PLL lock 1 ms max (refer datasheet)

	reg_wizchip_cs_cbfunc(cs_sel, cs_desel);
	reg_wizchip_spi_cbfunc(spi_rb, spi_wb);
	wizchip_init(bufSize, bufSize);
	wiz_NetInfo netInfo = { .mac = { 0 },// Mac address will be set later from EEPROM
			.ip = { 0 }, .sn = { 0 }, .gw = { 0 }, .dhcp = NETINFO_DHCP //Using DHCP
			};
	getMAC(netInfo.mac);
	wizchip_setnetinfo(&netInfo);
	wizchip_getnetinfo(&netInfo);

	while (!if_window(left_window)) { //Handling non-installed window
		HAL_Delay(1000);
		HAL_GPIO_TogglePin(LED_JOKER_GPIO_Port, LED_JOKER_Pin);
	}

	turn_12V_on(left_window);

	while (1) {
		step_internal_anim();
		HAL_Delay(500);
		HAL_GPIO_TogglePin(LED_HEARTH_GPIO_Port, LED_HEARTH_Pin);
	}
}
