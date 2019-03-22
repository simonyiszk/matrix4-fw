#include "window.hpp"
#include "gpioes.h"

using namespace windows;

/*****************************
 *    Class pixel_data
 *****************************/

pixel_data::pixel_data() :
		red(0),
		green(200),
		blue(0),
		stat(pixel_data::buffer_full)
{

}

void pixel_data::set(unsigned char red, unsigned char green, unsigned char blue){
	this->red=red;
	this->green=green;
	this->blue=blue;
	this->stat=pixel_data::buffer_full;
}

void pixel_data::flush(){
	this->stat=pixel_data::buffer_free;
}

bool pixel_data::isFull(){
	return this->stat == pixel_data::buffer_full;
}

/*****************************
 *    Class window
 *****************************/

extern "C" uint8_t sec_cntr_window;

void window::step_state(){
	switch(this->status){
		case vcc_12v_on:
			this->update_image();
			break;
		case vcc_12v_off:
			break;
		case vcc_3v3_off:
			if(sec_cntr_window>4)
				this->set_state(vcc_3v3_on);
			break;
		case vcc_3v3_on:
			if(sec_cntr_window>1){ //TODO reference time point
				if(check_uart_welcome_message())
					this->set_state(vcc_12v_on);
				else
					this->set_state(vcc_3v3_off);
			}
			break;
		case discharge_caps:
			if(sec_cntr_window>10)
				this->set_state(vcc_3v3_off);
			break;
		default:
			this->set_state(discharge_caps);
	}
}

window::window(	GPIO_TypeDef* gpio_port_3v3,
		uint16_t gpio_pin_3v3,
		GPIO_TypeDef* gpio_port_power,
		uint16_t gpio_pin_power,
		USART_TypeDef *USARTx,
		DMA_TypeDef* DMAx,
		uint32_t DMA_Channel):
		status(vcc_3v3_off),
		gpio_port_3v3(gpio_port_3v3),
		gpio_port_power(gpio_port_power),
		gpio_pin_3v3(gpio_pin_3v3),
		gpio_pin_power(gpio_pin_power),
		DMAx(DMAx),
		DMA_Channel(DMA_Channel),
		uart_handler(USARTx),
		transmitted_before(false)
{
    LL_DMA_DisableChannel(DMAx, DMA_Channel);

	while(LL_DMA_IsEnabledChannel(DMAx, DMA_Channel));

	LL_USART_Enable(uart_handler);
	LL_USART_EnableDMAReq_TX(uart_handler);
	LL_USART_EnableDirectionTx(uart_handler);


	LL_DMA_SetDataTransferDirection(DMAx, DMA_Channel, LL_DMA_DIRECTION_MEMORY_TO_PERIPH);
	LL_DMA_SetChannelPriorityLevel(DMAx, DMA_Channel, LL_DMA_PRIORITY_MEDIUM);
	LL_DMA_SetMode(DMAx, DMA_Channel, LL_DMA_MODE_NORMAL);
	LL_DMA_SetPeriphIncMode(DMAx, DMA_Channel, LL_DMA_PERIPH_NOINCREMENT);
	LL_DMA_SetMemoryIncMode(DMAx, DMA_Channel, LL_DMA_MEMORY_INCREMENT);
	LL_DMA_SetPeriphSize(DMAx, DMA_Channel, LL_DMA_PDATAALIGN_BYTE);
	LL_DMA_SetMemorySize(DMAx, DMA_Channel, LL_DMA_MDATAALIGN_BYTE);
	LL_DMA_SetPeriphAddress(DMAx, DMA_Channel, (uint32_t)&(uart_handler->TDR));
	LL_DMA_SetMemoryAddress(DMAx, DMA_Channel, (uint32_t)DMA_buffer);
	LL_DMA_SetMemorySize(DMAx, DMA_Channel, LL_DMA_MDATAALIGN_BYTE);

	LL_DMA_DisableIT_HT(DMAx, DMA_Channel);
	LL_DMA_DisableIT_TC(DMAx, DMA_Channel);
	LL_DMA_DisableIT_TE(DMAx, DMA_Channel);

	if(DMA_Channel == LL_DMA_CHANNEL_2)
		LL_DMA_ClearFlag_GI2(DMAx);
	else if (DMA_Channel == LL_DMA_CHANNEL_4)
		LL_DMA_ClearFlag_GI4(DMAx);
	else
		while(1);

	LL_USART_ClearFlag_TC(uart_handler);

	LL_DMA_SetDataLength(DMAx, DMA_Channel, 1);

	//Do a first tranfer, to set TC flag
	DMA_buffer[0]=0xF0;
	LL_DMA_EnableChannel(DMAx, DMA_Channel);
    
    
	this->set_state(discharge_caps);
	DMA_buffer[0]=0xF0;
};

window::twindow_status window::get_state(){
	return this->status;
}

void window::set_state(enum twindow_status new_stat){
	switch(new_stat){
		default:
			new_stat = discharge_caps;
		case discharge_caps:
			sec_cntr_window=0;
			LL_GPIO_SetOutputPin(gpio_port_3v3, gpio_pin_3v3);
			break;
		case vcc_3v3_off:
			sec_cntr_window=0;
			LL_GPIO_SetOutputPin(gpio_port_3v3, gpio_pin_3v3);
			LL_GPIO_ResetOutputPin(gpio_port_power, gpio_pin_power);
			break;
		case vcc_3v3_on:
			sec_cntr_window=0;
			//EMPTY DMA BUFFER
			LL_GPIO_ResetOutputPin(gpio_port_3v3, gpio_pin_3v3);
			LL_GPIO_ResetOutputPin(gpio_port_power, gpio_pin_power);
			break;
		case vcc_12v_on:
			LL_GPIO_SetOutputPin(gpio_port_power, gpio_pin_power);
			break;
		case vcc_12v_off:
			LL_GPIO_ResetOutputPin(gpio_port_power, gpio_pin_power);
			break;
		}
		this->status = new_stat;
}

bool window::check_uart_welcome_message(){
	//TODO DMA things
	return true;
}

void window::update_image(){
	if(i2c_panel)
	{
		update_image_i2c();
		return;
	}

	if( (!LL_USART_IsActiveFlag_TC(uart_handler)) )
		return;

	LL_DMA_DisableChannel(DMAx, DMA_Channel);

	if(DMA_Channel == LL_DMA_CHANNEL_2)
		LL_DMA_ClearFlag_GI2(DMAx);
	else if (DMA_Channel == LL_DMA_CHANNEL_4)
		LL_DMA_ClearFlag_GI4(DMAx);
	else
		while(1);

	DMA_buffer[0] = 0xF0; //always this value, never changes
	size_t transfer_size = 0; //besides the first F0 byte

	for(size_t j=0; j<num_of_pixels; j++){
		if(pixels[j].isFull()){
			pixels[j].flush();
			uint8_t base = (j & 3) * 3;

			transfer_size++;
			DMA_buffer[transfer_size] = (uint8_t)( ((base + 0) << 4) | (uint8_t)( (pixels[j].red   & (uint8_t)0xE0) >> 5 ) );
			transfer_size++;
			DMA_buffer[transfer_size] = (uint8_t)( ((base + 1) << 4) | (uint8_t)( (pixels[j].green & (uint8_t)0xE0) >> 5 ) );
			transfer_size++;
			DMA_buffer[transfer_size] = (uint8_t)( ((base + 2) << 4) | (uint8_t)( (pixels[j].blue  & (uint8_t)0xE0) >> 5 ) );
		}
	}

	if(transfer_size > 0){
		LL_USART_ClearFlag_TC(uart_handler);

		LL_DMA_SetDataLength(DMAx, DMA_Channel, transfer_size+1);

		LL_DMA_SetPeriphAddress(DMAx, DMA_Channel, (uint32_t)&(uart_handler->TDR));    // --|
		LL_DMA_SetMemoryAddress(DMAx, DMA_Channel, (uint32_t)DMA_buffer);             //  --| --> Maybe unnecesary???

		LL_DMA_EnableChannel(DMAx, DMA_Channel);
	}
}

void window::blank(){
	for(size_t j=0; j<num_of_pixels; j++)
		pixels[j].set(0,0,0);
}

//I2C
//----------------------------------------------------------------------------
void window::init_I2C(uint8_t i2c_addr_base, GPIO_TypeDef* SCL_Port, uint32_t SCL_PinMask,
		GPIO_TypeDef* SDA_Port, uint32_t SDA_PinMask,
		TIM_TypeDef* timer, uint32_t F_CLK, uint32_t speed)
{
	i2c_panel = true;
	red_addr = i2c_addr_base;
	green_addr = i2c_addr_base+1;
	blue_addr = i2c_addr_base+2;
	LL_USART_Disable(uart_handler);
	i2c.init(SCL_Port, SCL_PinMask, SDA_Port, SDA_PinMask, timer, F_CLK, speed);
}

void window::init_leds()
{
	uint8_t init_data[24];
	init_data[REG_MODE1] = INIT_MODE1;
	init_data[REG_MODE2] = INIT_MODE2;
	for (uint32_t i = 0; i < 16; ++i) {
		init_data[REG_PWM0 + i] = INIT_PWM;
	}
	init_data[REG_GRPPWM] = INIT_GRPPWM;
	init_data[REG_GRPFREQ] = INIT_GRPFREQ;
	init_data[REG_LEDOUT0] = INIT_LEDOUT;
	init_data[REG_LEDOUT1] = INIT_LEDOUT;
	init_data[REG_LEDOUT2] = INIT_LEDOUT;
	init_data[REG_LEDOUT3] = INIT_LEDOUT;
	i2c.mem_write_bytes(red_addr, REG_MODE1 | AUTO_INC_ALL, init_data, 24);
	i2c.mem_write_bytes(green_addr, REG_MODE1 | AUTO_INC_ALL, init_data, 24);
	i2c.mem_write_bytes(blue_addr, REG_MODE1 | AUTO_INC_ALL, init_data, 24);
}

void window::update_image_i2c()
{
	uint8_t data_red[16] = {0};
	uint8_t data_green[16] = {0};
	uint8_t data_blue[16] = {0};
	for (uint32_t i = 0; i < num_of_pixels; ++i) {
		for (uint32_t j = 0; j < 3; ++j) {
			data_red[3*i + j] = pixels[i].red;
			data_green[3*i + j] = pixels[i].green;
			data_blue[3*i + j] = pixels[i].blue;
		}
		data_red[12 + i] = pixels[i].red;
		data_green[12 + i] = pixels[i].green;
		data_blue[12 + i] = pixels[i].blue;
	}
	i2c.mem_write_bytes(red_addr, REG_PWM0 | AUTO_INC_PWM, data_red, 16);
	i2c.mem_write_bytes(green_addr, REG_PWM0 | AUTO_INC_PWM, data_green, 16);
	i2c.mem_write_bytes(blue_addr, REG_PWM0 | AUTO_INC_PWM, data_blue, 16);
}

void windows::set_group_dim_red(uint8_t dim) {
	i2c.mem_write_byte(red_addr, REG_GRPPWM, dim);
}

void windows::set_group_dim_green(uint8_t dim) {
	i2c.mem_write_byte(green_addr, REG_GRPPWM, dim);
}

void windows::set_group_dim_blue(uint8_t dim) {
	i2c.mem_write_byte(blue_addr, REG_GRPPWM, dim);
}


/*****************************
 *  Instance of static members
 *****************************/

uint8_t sec_cntr_window = 0;



