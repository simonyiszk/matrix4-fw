#ifndef __I2C_H
#define __I2C_H

#include <stdint.h>
#include "gpioes.h"

typedef struct
{
	bool restart;
	bool start;
	bool RW; //read=true; write=false
	bool hasdata;
	uint8_t data;
	bool ack;
	bool stop;
} I2C_block;

typedef enum I2C_mode_t {BITBANG=0, IT_TIMER, DMA} I2C_mode_t;

class I2C {
public:
	/*I2C(GPIO_TypeDef* SCL_Port, uint32_t SCL_PinMask,
			GPIO_TypeDef* SDA_Port, uint32_t SDA_PinMask,
			TIM_TypeDef* timer, uint32_t F_CLK, uint32_t speed = 100000);*/
	I2C(I2C_mode_t i2c_mode = BITBANG);

	void init(GPIO_TypeDef* SCL_Port, uint32_t SCL_PinMask,
			GPIO_TypeDef* SDA_Port, uint32_t SDA_PinMask,
			TIM_TypeDef* timer, uint32_t F_CLK, uint32_t speed = 100000);
	//User must call GPIO clock enable: LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOx)
	//User must init Timer with HAL

	void deinit();
	uint8_t mem_read_byte(uint8_t slave_addr, uint8_t mem_addr);
	void mem_write_byte(uint8_t slave_addr, uint8_t mem_addr, uint8_t data);
	void mem_read_bytes(uint8_t slave_addr, uint8_t mem_addr, uint8_t* data_array, uint8_t len);
	void mem_write_bytes(uint8_t slave_addr, uint8_t mem_addr, uint8_t* data_array, uint8_t len);
	void IT_timer_callback();

	//TODO ha nem kell ki lehet törölni
	/*uint32_t getF_CLK() const;
	void setF_CLK(uint32_t F_CLK);
	uint32_t getSclPinMask() const;
	const GPIO_TypeDef* getSclPort() const;
	uint32_t getSdaPinMask() const;
	const GPIO_TypeDef* getSdaPort() const;
	const TIM_TypeDef* getTimer() const;
	*/

	uint32_t getSpeed() const;
	void setSpeed(uint32_t speed);

private:
	GPIO_TypeDef* SCL_Port;
	uint32_t SCL_PinMask;
	GPIO_TypeDef* SDA_Port;
	uint32_t SDA_PinMask;
	TIM_TypeDef* timer;
	uint32_t F_CLK = 0;
	uint32_t speed = 0;
	uint32_t delayVal = 0;
	uint8_t started = 0; // global data
	volatile I2C_mode_t i2c_mode = BITBANG; // interruptból bitbangeljünk-e
	typedef enum {IDLE = 0, BEGIN, RESTART1, RESTART2,
		START1, START2,
		D7, D6, D5, D4, D3, D2, D1, D0,
		ACK1, ACK2, ACK3, STOP1, STOP2, STOP3, NEXT} IT_state_t;
	volatile IT_state_t IT_state = IDLE; // állapotváltozó
	volatile bool IT_setSCL_next = false;
	volatile uint32_t IT_block_idx = 0;
	volatile bool IT_busy = false;

	volatile I2C_block i2c_blocks[64];
	volatile uint32_t i2c_block_num = 0;

	void I2C_delay(void);
	bool read_SCL(void);  // Return current level of SCL line, 0 or 1
	bool read_SDA(void);  // Return current level of SDA line, 0 or 1
	void set_SCL(void);   // Do not drive SCL (set pin high-impedance)
	void clear_SCL(void); // Actively drive SCL signal low
	void set_SDA(void);   // Do not drive SDA (set pin high-impedance)
	void clear_SDA(void); // Actively drive SDA signal low
	void start_cond(void);
	void stop_cond(void);
	void write_bit(bool bit);
	bool read_bit(void);
	bool write_byte(bool send_start, bool send_stop, uint8_t byte, bool send_restart = false);
	uint8_t read_byte(bool nack, bool send_stop);
};

#endif /* __I2C_H */


