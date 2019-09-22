#include "i2c.hpp"

I2C::I2C(I2C_mode_t i2c_mode): i2c_mode(i2c_mode)
{
}

void I2C::init(GPIO_TypeDef* SCL_Port, uint32_t SCL_PinMask,
		GPIO_TypeDef* SDA_Port, uint32_t SDA_PinMask,
		TIM_TypeDef* timer, uint32_t F_CLK, uint32_t speed)
{
	this->SCL_Port = SCL_Port;
	this->SCL_PinMask = SCL_PinMask;
	this->SDA_Port = SDA_Port;
	this->SDA_PinMask = SDA_PinMask;
	this->timer = timer;//TODO
	this->F_CLK = F_CLK;
	this->speed = speed;
	this->delayVal = (this->F_CLK / this->speed) / 2;
	this->started = 0;

	LL_GPIO_InitTypeDef GPIO_InitStruct = {0};
	LL_GPIO_SetOutputPin(this->SDA_Port, this->SDA_PinMask);
	LL_GPIO_SetOutputPin(this->SCL_Port, this->SCL_PinMask);

	/**/
	GPIO_InitStruct.Pin = this->SCL_PinMask;
	GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
	GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
	GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
	GPIO_InitStruct.Pull = LL_GPIO_PULL_UP;
	LL_GPIO_Init(this->SCL_Port, &GPIO_InitStruct);

	/**/
	GPIO_InitStruct.Pin = this->SDA_PinMask;
	GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
	GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
	GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_OPENDRAIN;
	GPIO_InitStruct.Pull = LL_GPIO_PULL_UP;
	LL_GPIO_Init(this->SDA_Port, &GPIO_InitStruct);
}

void I2C::deinit()
{
	this->SCL_Port = NULL;
	this->SCL_PinMask = 0;
	this->SDA_Port = NULL;
	this->SDA_PinMask = 0;
	this->timer = NULL;
	this->F_CLK = 0;
	this->speed = 0;
	this->delayVal = 0;
	this->started = 0;
}

void I2C::start_cond(void) {
	if (this->started) {
		// if started, do a restart condition
		// set SDA to 1
		set_SDA();
		I2C_delay();
		set_SCL();
//		while (read_SCL() == 0) { // Clock stretching
//			// You should add timeout to this loop
//		}

		// Repeated start setup time, minimum 4.7us
		I2C_delay();
	}

//  if (read_SDA() == 0) {
//    arbitration_lost();
//  }

	// SCL is high, set SDA from 1 to 0.
	clear_SDA();
	I2C_delay();
	clear_SCL();
	I2C_delay();
	this->started = 1;
}

void I2C::stop_cond(void) {
	// set SDA to 0
	clear_SDA();
	I2C_delay();

	set_SCL();
	// Clock stretching
//  while (read_SCL() == 0) {
//    // add timeout to this loop.
//  }

	// Stop bit setup time, minimum 4us
	I2C_delay();

	// SCL is high, set SDA from 0 to 1
	set_SDA();
	I2C_delay();

//  if (read_SDA() == 0) {
//    arbitration_lost();
//  }

	this->started = 0;
}

// Write a bit to I2C bus
void I2C::write_bit(bool bit) {
	clear_SCL();
	if (bit) {
		set_SDA();
	} else {
		clear_SDA();
	}

	// SDA change propagation delay
	I2C_delay();

	// Set SCL high to indicate a new valid SDA value is available
	set_SCL();

	// Wait for SDA value to be read by slave, minimum of 4us for standard mode
	I2C_delay();

//  while (read_SCL() == 0) { // Clock stretching
//    // You should add timeout to this loop
//  }
//
//  // SCL is high, now data is valid
//  // If SDA is high, check that nobody else is driving SDA
//  if (bit && (read_SDA() == 0)) {
//    arbitration_lost();
//  }

	// Clear the SCL to low in preparation for next change
	clear_SCL();
	I2C_delay(); //TODO hangolni
}

// Read a bit from I2C bus
bool I2C::read_bit(void) {
	bool bit;

	// Let the slave drive data
	set_SDA();

	// Wait for SDA value to be written by slave, minimum of 4us for standard mode
	I2C_delay();

	// Set SCL high to indicate a new valid SDA value is available
	set_SCL();

//  while (read_SCL() == 0) { // Clock stretching
//    // You should add timeout to this loop
//  }

	// Wait for SDA value to be written by slave, minimum of 4us for standard mode
	I2C_delay();

	// SCL is high, read out bit
	bit = read_SDA();

	// Set SCL low in preparation for next operation
	clear_SCL();
	I2C_delay(); //TODO hangolni

	return bit;
}

// Write a byte to I2C bus. Return 0 if ack by the slave.
bool I2C::write_byte(bool send_start, bool send_stop, uint8_t byte, bool send_restart) {
	if (i2c_mode == IT_TIMER)
	{
		i2c_blocks[i2c_block_num].restart = send_restart;
		i2c_blocks[i2c_block_num].start = send_start;
		i2c_blocks[i2c_block_num].RW = false;
		i2c_blocks[i2c_block_num].hasdata = true;
		i2c_blocks[i2c_block_num].data = byte;
		i2c_blocks[i2c_block_num].stop = send_stop;
		i2c_block_num++;
		return false;
	}
	else
	{
		uint8_t bit;
		bool nack;

		if (send_start) {
			start_cond();
		}

		for (bit = 0; bit < 8; ++bit) {
			write_bit((byte & 0x80) != 0);
			byte <<= 1;
		}

		nack = read_bit();

		if (send_stop) {
			stop_cond();
		}

		return nack;
	}
}

// Read a byte from I2C bus
uint8_t I2C::read_byte(bool nack, bool send_stop) {
	uint8_t byte = 0;
	uint8_t bit;

	for (bit = 0; bit < 8; ++bit) {
		byte = (byte << 1) | read_bit();
	}

	write_bit(nack);

	if (send_stop) {
		stop_cond();
	}

	return byte;
}

uint8_t I2C::mem_read_byte(uint8_t slave_addr, uint8_t mem_addr) {
	if (i2c_mode == IT_TIMER)
	{
		return 0;
	}
	else
	{
		write_byte(1, 0, (slave_addr << 1));
		write_byte(0, 1, mem_addr);
		I2C_delay();
		I2C_delay();

		write_byte(1, 0, (slave_addr << 1) | 1);
		I2C_delay();
		I2C_delay();
		uint8_t res = read_byte(1, 1);

		return res;
	}
}

void I2C::mem_read_bytes(uint8_t slave_addr, uint8_t mem_addr, uint8_t* data_array, uint8_t len) {
	if (i2c_mode == IT_TIMER)
	{

	}
	else
	{
		write_byte(1, 0, (slave_addr << 1));
		write_byte(0, 1, mem_addr);
		I2C_delay();
		I2C_delay();

		write_byte(1, 0, (slave_addr << 1) | 1);
		I2C_delay();
		I2C_delay();
		for (int32_t i = 0; i < (int32_t)len-1; i++) {
			uint8_t res = read_byte(0, 0);
			data_array[i] = res;
		}
		if (len > 0)
		{
			uint8_t res = read_byte(1, 1);
			data_array[len - 1] = res;
		}
	}
}

void I2C::mem_write_byte(uint8_t slave_addr, uint8_t mem_addr, uint8_t data) {
	if (i2c_mode == IT_TIMER)
	{
		if (!IT_busy)
		{
			IT_busy = true;
			IT_block_idx = 0;
			i2c_block_num = 0;
			write_byte(1, 0, (slave_addr << 1));
			write_byte(0, 0, mem_addr);
			write_byte(0, 1, data);
			IT_state = I2C::BEGIN;
			//TODO törölni///////////
			I2C_delay();
			while(IT_busy)
			{
				volatile uint32_t t = this->timer->CNT;
				IT_timer_callback();
				while((this->timer->CNT - t) < this->delayVal);//timer->CNT,this->delayVal
			}
			/////////////////////////
		}
	}
	else
	{
		write_byte(1, 0, (slave_addr << 1));
		write_byte(0, 0, mem_addr);
		I2C_delay();
		I2C_delay();
		write_byte(0, 1, data);

	}
}

void I2C::mem_write_bytes(uint8_t salve_addr, uint8_t mem_addr, uint8_t* data_array, uint8_t len) {
	if (i2c_mode == IT_TIMER)
	{
		if (!IT_busy)
		{
			IT_busy = true;
			IT_block_idx = 0;
			i2c_block_num = 0;
			write_byte(1, 0, (salve_addr << 1));
			write_byte(0, 0, mem_addr);
			for (uint32_t i = 0; i < len; i++) {
				write_byte(0, 0, data_array[i]);
			}
			i2c_blocks[i2c_block_num].restart = false;
			i2c_blocks[i2c_block_num].start = false;
			i2c_blocks[i2c_block_num].RW = false;
			i2c_blocks[i2c_block_num].hasdata = false;
			i2c_blocks[i2c_block_num].data = 0;
			i2c_blocks[i2c_block_num].stop = true;
			i2c_block_num++;
			IT_state = I2C::BEGIN;

			//TODO törölni///////////
			I2C_delay();
			while(IT_busy)
			{
				//volatile uint32_t t = this->timer->CNT;
				//set_SCL();
				IT_timer_callback();
				I2C_delay();
				//clear_SCL();

				//while((this->timer->CNT - t) < this->delayVal);//timer->CNT,this->delayVal
			}
			/////////////////////////
		}
	}
	else
	{
		write_byte(1, 0, (salve_addr << 1));
		write_byte(0, 0, mem_addr);
		I2C_delay();
		I2C_delay();
		for (uint32_t i = 0; i < len; i++) {
			write_byte(0, 0, data_array[i]);
		}
		I2C_delay();
		stop_cond();
	}
}

void I2C::IT_timer_callback()
{
	bool exit = false;
	if (i2c_mode != IT_TIMER) return;
	if (IT_setSCL_next)
	{
		IT_setSCL_next = false;
		set_SCL();
		return;
	}

	do
	{
		switch (IT_state) {
		case I2C::IDLE:
			IT_busy = false;
			break;
		case I2C::BEGIN:
			if (i2c_blocks[IT_block_idx].restart)
				IT_state = RESTART1;
			else if (i2c_blocks[IT_block_idx].start)
				IT_state = START1;
			else if (i2c_blocks[IT_block_idx].hasdata)
				IT_state = D7;
			else if (i2c_blocks[IT_block_idx].stop)
				IT_state = STOP1;
			else
				IT_state = NEXT;
			continue;
			break;
		case I2C::RESTART1:
			set_SDA();
			IT_state = I2C::RESTART2;
			break;
		case I2C::RESTART2:
			set_SCL();
			IT_state = I2C::START1;
			break;
		case I2C::START1:
			clear_SDA();
			IT_state = I2C::START2;
			break;
		case I2C::START2:
			clear_SCL();
			if (i2c_blocks[IT_block_idx].hasdata)
				IT_state = I2C::D7;
			else if (i2c_blocks[IT_block_idx].stop)
				IT_state = STOP1;
			else
				IT_state = NEXT;
			break;
		case I2C::D7:
		case I2C::D6:
		case I2C::D5:
		case I2C::D4:
		case I2C::D3:
		case I2C::D2:
		case I2C::D1:
		case I2C::D0:
			if (!i2c_blocks[IT_block_idx].RW) //write
			{
				clear_SCL();
				uint8_t mask = 1 << (7 - (IT_state - int(I2C::D7)));
				if (i2c_blocks[IT_block_idx].data & mask)
					set_SDA();
				else
					clear_SDA();
				IT_state = IT_state_t(int(IT_state) + 1);
				IT_setSCL_next = true;
			}
			else
			{
			}
			break;
		case I2C::ACK1:
			if (!i2c_blocks[IT_block_idx].RW) //write
			{
				clear_SCL();
				set_SDA();
				IT_state = ACK2;
			}
			break;
		case I2C::ACK2:
			set_SCL();
			IT_state = ACK3;
			break;
		case I2C::ACK3:
			i2c_blocks[IT_block_idx].ack = read_SDA();
			if (i2c_blocks[IT_block_idx].stop)
			{
				IT_state = STOP1;
				continue;
			}
			else
			{
				IT_state = NEXT;
			}
			break;
		case I2C::STOP1:
			if (!i2c_blocks[IT_block_idx].RW) //write
			clear_SCL();
			clear_SDA();
			IT_state = STOP2;
			break;
		case I2C::STOP2:
			set_SCL();
			IT_state = STOP3;
			break;
		case I2C::STOP3:
			set_SDA();
			IT_state = NEXT;
			break;
		case I2C::NEXT:
			IT_block_idx++;
			if (IT_block_idx < i2c_block_num)
			{
				IT_state = BEGIN;
			}
			else
			{
				IT_state = IDLE;
				IT_busy = false;
			}
			break;
		default:
			IT_busy = false;
			IT_state = IDLE;
			break;
		}

		exit = true;
	} while(!exit);
}

void I2C::I2C_delay(void) {
	volatile uint32_t t = this->timer->CNT;
	while((this->timer->CNT - t) < this->delayVal);//timer->CNT,this->delayVal
}

bool I2C::read_SCL(void)  // Return current level of SCL line, 0 or 1
{
	return LL_GPIO_IsInputPinSet(this->SCL_Port, this->SCL_PinMask);
//	return LL_GPIO_IsInputPinSet(WINDOW_I2C_SCL_LEFT_GPIO_Port, WINDOW_I2C_SCL_LEFT_Pin);
}

bool I2C::read_SDA(void)  // Return current level of SDA line, 0 or 1
{
//	return LL_GPIO_IsInputPinSet(WINDOW_I2C_SDA_LEFT_GPIO_Port, WINDOW_I2C_SDA_LEFT_Pin);
	return LL_GPIO_IsInputPinSet(this->SDA_Port, this->SDA_PinMask);
}

void I2C::set_SCL(void)   // Do not drive SCL (set pin high-impedance)
{
//	LL_GPIO_SetOutputPin(WINDOW_I2C_SCL_LEFT_GPIO_Port, WINDOW_I2C_SCL_LEFT_Pin);
	LL_GPIO_SetOutputPin(this->SCL_Port, this->SCL_PinMask);
}

void I2C::clear_SCL(void) // Actively drive SCL signal low
{
//	LL_GPIO_ResetOutputPin(WINDOW_I2C_SCL_LEFT_GPIO_Port, WINDOW_I2C_SCL_LEFT_Pin);
	LL_GPIO_ResetOutputPin(this->SCL_Port, this->SCL_PinMask);
}
void I2C::set_SDA(void)   // Do not drive SDA (set pin high-impedance)
{
	//LL_GPIO_SetOutputPin(WINDOW_I2C_SDA_LEFT_GPIO_Port, WINDOW_I2C_SDA_LEFT_Pin);
	LL_GPIO_SetOutputPin(this->SDA_Port, this->SDA_PinMask);
}
void I2C::clear_SDA(void) // Actively drive SDA signal low
{
	//LL_GPIO_ResetOutputPin(WINDOW_I2C_SDA_LEFT_GPIO_Port, WINDOW_I2C_SDA_LEFT_Pin);
	LL_GPIO_ResetOutputPin(this->SDA_Port, this->SDA_PinMask);
}

//TODO ha nem kell ki lehet törölni
/*
uint32_t I2C::getF_CLK() const {
	return this->F_CLK;
}

void I2C::setF_CLK(uint32_t F_CLK) {
	this->F_CLK = F_CLK;
	this->delayVal = (this->F_CLK / this->speed) / 2;
}

uint32_t I2C::getSclPinMask() const {
	return this->SCL_PinMask;
}

const GPIO_TypeDef* I2C::getSclPort() const {
	return this->SCL_Port;
}

uint32_t I2C::getSdaPinMask() const {
	return this->SDA_PinMask;
}

const GPIO_TypeDef* I2C::getSdaPort() const {
	return this->SDA_Port;
}

const TIM_TypeDef* I2C::getTimer() const {
	return this->timer;
}
*/

uint32_t I2C::getSpeed() const {
	return this->speed;
}

void I2C::setSpeed(uint32_t speed) {
	this->speed = speed;
	this->delayVal = (this->F_CLK / this->speed) / 2;
}





