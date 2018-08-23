
/* INCLUDES */ 
#include "drv_temp.h"

/* DEFINES */

/* GLOBAL VARIABLES */
i2c_port_t _port;
/* static methods if necessary */

/**
 * @brief Driver Template Initialization
 *
 * This function initializes template device.
 *
 * @params param1:
 *	       ...
 * @return DRV_TEMP_OK: <case1>
 *		   DRV_TEMP_ERROR: <case2>
 *		   ...
 */
 
 
char* DRV_TEMP_Init(DRV_TEMP_Config_t *cfg)
{
	esp_err_t ret;
	_port = cfg->i2c_port;
	
	// setup i2c controller
	i2c_config_t i2c_config;
	i2c_config.mode = I2C_MODE_MASTER;
	i2c_config.sda_io_num = cfg->sda_pin;
	i2c_config.scl_io_num = cfg->scl_pin;
	i2c_config.sda_pullup_en = cfg->sda_internal_pullup;
	i2c_config.scl_pullup_en = cfg->scl_internal_pullup;
	i2c_config.master.clk_speed = MASTER_CLOCK_SPEED;
	ret = i2c_param_config(cfg->i2c_port, &i2c_config);
	if(ret != ESP_OK) 
	{
		return SI7021_ERR_CONFIG;
	}
		
	// installation i2c driver
	ret = i2c_driver_install(cfg->i2c_port, I2C_MODE_MASTER, 0, 0, 0);
	if(ret != ESP_OK)
	{
		return SI7021_ERR_INSTALL;
	}			
	// verifying if the sensor is present | not
	i2c_cmd_handle_t cmd = i2c_cmd_link_create();
	i2c_master_start(cmd);
	i2c_master_write_byte(cmd, (SI7021_ADDR << 1) | I2C_MASTER_WRITE, true);
	i2c_master_stop(cmd);
	if(i2c_master_cmd_begin(cfg->i2c_port, cmd, 1000 / portTICK_RATE_MS) != ESP_OK)
	{
		return SI7021_ERR_NOTFOUND;
	}
			
	return SI7021_ERR_OK;
}

/**
 * @brief Driver Template Read
 *
 * This function reads data from template device.
 *
 * @params param1:
 * 		   ...
 * @return DRV_TEMP_OK: <case1>
 *		   DRV_TEMP_ERROR: <case2>
 *		   ...
 */
float DRV_TEMP_Read()
{
	// getting the raw temp. value from the sensor
	uint16_t raw_temperature = read_value(TRIGGER_TEMP_MEASURE_NOHOLD);
	if(raw_temperature == 0)
	{
		return -404;
	}		
	// return the actual value according to the formula in the doc. 
	return (raw_temperature * 175.72 / 65536.0) - 46.85;
}
float DRV_HUM_Read()
{
	// get the raw humidity value from the sensor
	uint16_t raw_humidity = read_value(TRIGGER_HUMD_MEASURE_NOHOLD);
	if(raw_humidity == 0)
	{
		return -404;
	}		
	
	// return the actual value according to the formula in the doc. 
	return (raw_humidity * 125.0 / 65536.0) - 6.0;
}

/**
 * @brief Driver Template Deinitialization
 *
 * This function deinitializes template device.
 *
 * @params param1:
 *		   ...
 * @return DRV_TEMP_OK: <case1>
 *		   DRV_TEMP_ERROR: <case2>
 *		   ...
 */
char* DRV_TEMP_Deinit(DRV_TEMP_Config_t *cfg)
{
	esp_err_t ret;
	ret = i2c_driver_delete(cfg->i2c_port);
	if(ret==ESP_OK)
	{
		return SI7021_ERR_OK;
	}
	else
	{
		return SI7021_ERR_FAIL;
	}
}

uint16_t read_value(uint8_t command) {
	
	esp_err_t ret;
	
	// send the command
	i2c_cmd_handle_t cmd = i2c_cmd_link_create();
	i2c_master_start(cmd);
	i2c_master_write_byte(cmd, (SI7021_ADDR << 1) | I2C_MASTER_WRITE, true);
	i2c_master_write_byte(cmd, command, true);
	i2c_master_stop(cmd);
	ret = i2c_master_cmd_begin(_port, cmd, 1000 / portTICK_RATE_MS);
	i2c_cmd_link_delete(cmd);
	if(ret != ESP_OK)
	{
		return -404;
		printf("%s",SI7021_ERR_FAIL);
	}		
	
	// wait for the sensor (50ms)
	vTaskDelay(50 / portTICK_RATE_MS);
	
	// receive the answer
	uint8_t msb, lsb, crc;
	cmd = i2c_cmd_link_create();
	i2c_master_start(cmd);
	i2c_master_write_byte(cmd, (SI7021_ADDR << 1) | I2C_MASTER_READ, true);
	i2c_master_read_byte(cmd, &msb, 0x00);
	i2c_master_read_byte(cmd, &lsb, 0x00);
	i2c_master_read_byte(cmd, &crc, 0x01);
	i2c_master_stop(cmd);
	ret = i2c_master_cmd_begin(_port, cmd, 1000 / portTICK_RATE_MS);
	i2c_cmd_link_delete(cmd);
	if(ret != ESP_OK)
	{
		return -404;
		printf("%s",SI7021_ERR_FAIL);
	}		
	
	uint16_t raw_value = ((uint16_t) msb << 8) | (uint16_t) lsb;
	if(!is_crc_valid(raw_value, crc)) printf("CRC invalid\r\n");
	return raw_value & 0xFFFC;
}

// verify the CRC, algorithm can be found in the data-sheet
bool is_crc_valid(uint16_t value, uint8_t crc) {
	
	// line the bits representing the input in a row
	uint32_t row = (uint32_t)value << 8; // value shifted 8 bit to left to make room for CRC value.
	row |= crc;
	
	// polynomial = x^8 + x^5 + x^4 + 1 
	// padded with zeros corresponding to the bit length of the CRC
	uint32_t divisor = (uint32_t)0x988000;
	
	for (int i = 0 ; i < 16 ; i++) {
		
		// if the input bit above the leftmost divisor bit is 1, 
		// the divisor is XORed into the input
		if (row & (uint32_t)1 << (23 - i)) row ^= divisor;
		
		// the divisor is then shifted one bit to the right
		divisor >>= 1;
	}
	
	// the remainder should be equal to zero if there are no detectable errors.
	return (row == 0);
}