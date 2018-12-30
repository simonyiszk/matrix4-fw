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

typedef struct
{
  uint32_t Pin;          /*!< Specifies the GPIO pins to be configured.
                              This parameter can be any value of @ref GPIO_LL_EC_PIN */

  uint32_t Mode;         /*!< Specifies the operating mode for the selected pins.
                              This parameter can be a value of @ref GPIO_LL_EC_MODE.

                              GPIO HW configuration can be modified afterwards using unitary function @ref LL_GPIO_SetPinMode().*/

  uint32_t Speed;        /*!< Specifies the speed for the selected pins.
                              This parameter can be a value of @ref GPIO_LL_EC_SPEED.

                              GPIO HW configuration can be modified afterwards using unitary function @ref LL_GPIO_SetPinSpeed().*/

  uint32_t OutputType;   /*!< Specifies the operating output type for the selected pins.
                              This parameter can be a value of @ref GPIO_LL_EC_OUTPUT.

                              GPIO HW configuration can be modified afterwards using unitary function @ref LL_GPIO_SetPinOutputType().*/

  uint32_t Pull;         /*!< Specifies the operating Pull-up/Pull down for the selected pins.
                              This parameter can be a value of @ref GPIO_LL_EC_PULL.

                              GPIO HW configuration can be modified afterwards using unitary function @ref LL_GPIO_SetPinPull().*/

  uint32_t Alternate;    /*!< Specifies the Peripheral to be connected to the selected pins.
                              This parameter can be a value of @ref GPIO_LL_EC_AF.

                              GPIO HW configuration can be modified afterwards using unitary function @ref LL_GPIO_SetAFPin_0_7() and LL_GPIO_SetAFPin_8_15().*/
} LL_GPIO_InitTypeDef;



	#define USE_FULL_LL_DRIVER 1
#include "stm32f0xx_ll_dma.h"
#include "stm32f0xx_ll_usart.h"
#include "stm32f0xx_ll_rcc.h"
#include "stm32f0xx_ll_system.h"
#include "stm32f0xx_ll_gpio.h"
#include "stm32f0xx_ll_exti.h"
#include "stm32f0xx_ll_bus.h"
#include "stm32f0xx_ll_cortex.h"
#include "stm32f0xx_ll_utils.h"
#include "stm32f0xx_ll_pwr.h"
	#undef USE_FULL_LL_DRIVER

	#include "stm32f0xx_ll_spi.h"
	#include "stm32f0xx_ll_bus.h"

ErrorStatus LL_GPIO_Init(GPIO_TypeDef *GPIOx, LL_GPIO_InitTypeDef *GPIO_InitStruct);
};


#include "network.hpp"
#include "main2.hpp"
#include "window.hpp"
#include <cstdio>
#include "version.hpp"
//#include "matrix2_package_format.hpp"

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

// Will be handled on the server side
void ip_conflict(){
	LL_GPIO_ResetOutputPin(LED_DHCP_GPIO_Port, LED_DHCP_Pin);
	//TODO blinking LED e.g. JOKER or SERVER
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
	while(LL_SPI_IsActiveFlag_BSY(SPI1));

    while (LL_SPI_IsActiveFlag_RXNE(SPI1))
        (void)LL_SPI_ReceiveData8(SPI1);      // flush any FIFO content

    while (!LL_SPI_IsActiveFlag_TXE(SPI1));

    LL_SPI_TransmitData8(SPI1, 0xFF);   // send dummy byte
    while (!LL_SPI_IsActiveFlag_RXNE(SPI1));

    return(LL_SPI_ReceiveData8(SPI1));
}

void spi_wb(uint8_t b) {
	while(LL_SPI_IsActiveFlag_BSY(SPI1));
	LL_SPI_TransmitData8(SPI1, b);
	(void)LL_SPI_ReceiveData8(SPI1);
}

/*********************************
 *  network Class function defs
 ********************************/

network::network(){
}

void network::init(){
    LL_GPIO_InitTypeDef GPIO_InitStruct;

	/* Peripheral clock enable */
	LL_APB1_GRP2_EnableClock(LL_APB1_GRP2_PERIPH_SPI1);

    /**SPI1 GPIO Configuration
    PA5     ------> SPI1_SCK
    PA6     ------> SPI1_MISO
    PA7     ------> SPI1_MOSI
    */
    GPIO_InitStruct.Pin = LL_GPIO_PIN_5|LL_GPIO_PIN_6|LL_GPIO_PIN_7;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
    GPIO_InitStruct.Alternate = LL_GPIO_AF_0;
    LL_GPIO_Init(GPIOA, &GPIO_InitStruct);



    LL_SPI_Disable(SPI1);

	LL_SPI_SetBaudRatePrescaler(SPI1, LL_SPI_BAUDRATEPRESCALER_DIV2);
	LL_SPI_SetClockPolarity(SPI1, LL_SPI_POLARITY_LOW);
	LL_SPI_SetClockPhase(SPI1, LL_SPI_PHASE_1EDGE);
    LL_SPI_SetTransferDirection(SPI1, LL_SPI_FULL_DUPLEX);
    LL_SPI_SetTransferBitOrder(SPI1, LL_SPI_MSB_FIRST);
    LL_SPI_DisableCRC(SPI1);
    LL_SPI_SetMode(SPI1, LL_SPI_MODE_MASTER);

    LL_SPI_SetRxFIFOThreshold(SPI1, LL_SPI_RX_FIFO_TH_QUARTER);
    LL_SPI_SetDataWidth(SPI1, LL_SPI_DATAWIDTH_8BIT);
    LL_SPI_SetStandard(SPI1, LL_SPI_PROTOCOL_MOTOROLA);

    LL_SPI_SetNSSMode(SPI1, LL_SPI_NSS_SOFT);

    LL_SPI_Enable(SPI1);


	//uint8_t memsize[2][8] = { { 2, 2, 2, 2, 2, 2, 2, 2 }, { 0, 1, 2, 8, 2, 1, 1, 1 } }; //TODO reassign buffer sizes
    uint8_t memsize[2][8] = { { 2, 2, 2, 2, 2, 2, 2, 2 }, { 2, 2, 2, 2, 2, 2, 2, 2 } }; //TODO reassign buffer sizes
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
	socket(3, Sn_MR_UDP, 10000, 0x00);
}

size_t network::create_status_string(){
	int ret;

	ret = snprintf((char*)this->status_string, sizeof(status_string),
		"MUEB FW version: %s\n"
		"MUEB MAC: %x:%x:%x:%x:%x:%x\n"
		"anim_source: %#x\n"
		"telemetry_comm_buff: %#x\n"
		"frame_ether_buff: %#x\n"
		"dhcp_lease_time: %d\n"
		"dhcp_reamining_lease_time: %d\n"
		"SEM forever\n",
		mueb_version,
		netInfo.mac[0],netInfo.mac[1],netInfo.mac[2],netInfo.mac[3],netInfo.mac[4],netInfo.mac[5],
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
			case get_mac:
				char mac[17];
				sprintf(mac, "%x:%x:%x:%x:%x:%x", netInfo.mac[0],netInfo.mac[1],netInfo.mac[2],netInfo.mac[3],netInfo.mac[4],netInfo.mac[5]);
				sendto(1, (uint8_t*) mac, 17, resp_addr, resp_port);
				break;
			case delete_anim_network_buffer:
				///To be implemented TODO
				break;
			case ping:
				sendto(1, (uint8_t*) "pong", 4, resp_addr, resp_port);
				break;
			default:
				break;
		}
	}
}

inline static void fetch_frame_unicast_proto(){
	size_t size= getSn_RX_RSR(2);
		if(size){
			main_state=external_anim;

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

inline static void fetch_frame_multicast_proto(){ //TODO clean the code
	const uint8_t& szint= emelet_szam;
	const uint8_t& szoba = szoba_szam;

	size_t size= getSn_RX_RSR(3);
	if(size == 0)
		return;

	if(szint==0 || szoba == 0)
		return;

	main_state=external_anim;

	HAL_GPIO_TogglePin(LED_COMM_GPIO_Port, LED_COMM_Pin);
	//todo treat big and small datagrams

	uint8_t buff[314]; //TODO exception handling

	uint8_t svr_addr[6];
	uint16_t  svr_port;
	uint16_t len;
	len = recvfrom(3, (uint8_t *)buff, size, svr_addr, &svr_port);

	//todo check indexes, datagram size


	if(buff[0] != 0x01)
		return;

	uint8_t pn_expected = ( ( (18-szint)*16 + (szoba-5)*2  )/52  );
	uint8_t pn          = buff[1] - 1;

	if(pn != pn_expected)
		return;

	uint32_t base_offset = (((18-szint)*8 + (szoba-5))%26)* 12 + 2;
	size_t  running_offset = 0;

	//----------------------------------

	uint8_t r, g, b;

		r =
				(buff[ (base_offset+running_offset)  ]  & 0xf0) << 1;
		g =
				(buff[ (base_offset+running_offset++)]  & 0x0f) << 5;
		b=
				(buff[ (base_offset+running_offset)  ]  & 0xf0) << 1;


	windows::right_window.pixels[0].set(
			r,
			g,
			b);

	r =
			(buff[ (base_offset+running_offset++)]  & 0x0f) << 5;
	g =
			(buff[ (base_offset+running_offset)  ]  & 0xf0) << 1;
	b=
			(buff[ (base_offset+running_offset++)]  & 0x0f) << 5;

	windows::right_window.pixels[1].set(
			r,
								g,
								b);

	r =
			(buff[ (base_offset+running_offset)  ]  & 0xf0) << 1;
	g =
			(buff[ (base_offset+running_offset++)]  & 0x0f) << 5;
	b=
			(buff[ (base_offset+running_offset)  ]  & 0xf0) << 1;

	windows::right_window.pixels[2].set(
			r,
								g,
								b);

	r =
						(buff[ (base_offset+running_offset++)]  & 0x0f) << 5;
				g =
						(buff[ (base_offset+running_offset)  ]  & 0xf0) << 1;
				b=
						(buff[ (base_offset+running_offset++)]  & 0x0f) << 5;

	windows::right_window.pixels[3].set(
			r,
								g,
								b);




	//---------------------------------

	r =
			(buff[ (base_offset+running_offset)  ]  & 0xf0) << 1;
	g =
			(buff[ (base_offset+running_offset++)]  & 0x0f) << 5;
	b=
			(buff[ (base_offset+running_offset)  ]  & 0xf0) << 1;

	windows::left_window.pixels[0].set(
			r,
								g,
								b);

	r =
						(buff[ (base_offset+running_offset++)]  & 0x0f) << 5;
				g =
						(buff[ (base_offset+running_offset)  ]  & 0xf0) << 1;
				b=
						(buff[ (base_offset+running_offset++)]  & 0x0f) << 5;

	windows::left_window.pixels[1].set(
			r,
								g,
								b);

	r =
			(buff[ (base_offset+running_offset)  ]  & 0xf0) << 1;
	g =
			(buff[ (base_offset+running_offset++)]  & 0x0f) << 5;
	b=
			(buff[ (base_offset+running_offset)  ]  & 0xf0) << 1;
	windows::left_window.pixels[2].set(
			r,
								g,
								b);

	r =
						(buff[ (base_offset+running_offset++)]  & 0x0f) << 5;
				g =
						(buff[ (base_offset+running_offset)  ]  & 0xf0) << 1;
				b=
						(buff[ (base_offset+running_offset++)]  & 0x0f) << 5;
	windows::left_window.pixels[3].set(
			r,
								g,
								b);

}

void network::fetch_frame(){
	fetch_frame_unicast_proto();
	fetch_frame_multicast_proto();
}

void network::step_network(){
	 if (wizphy_getphylink() == PHY_LINK_ON){
		LL_GPIO_SetOutputPin(LED_JOKER_GPIO_Port, LED_JOKER_Pin);

		do_remote_command();
		fetch_frame();

		wizchip_getnetinfo(&netInfo);
		emelet_szam=netInfo.ip[2];
		szoba_szam=netInfo.ip[3];

		//do DHCP task
		switch(DHCP_run()){
		case DHCP_IP_ASSIGN:
		case DHCP_IP_CHANGED:
		case DHCP_IP_LEASED:
			LL_GPIO_SetOutputPin(LED_DHCP_GPIO_Port, LED_DHCP_Pin);
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
