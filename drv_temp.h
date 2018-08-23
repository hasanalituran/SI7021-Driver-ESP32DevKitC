#ifndef DRV_TEMP_H_
#define DRV_TEMP_H_

/* INCLUDES */
#include "driver/i2c.h"
#include "esp_err.h"
#include "freertos/task.h"

/* DEFINES */

//SI7021 Slave Address
#define SI7021_ADDR						0x40

// SI7021 Sensor Commands
#define TRIGGER_TEMP_MEASURE_HOLD  		0xE3
#define TRIGGER_HUMD_MEASURE_HOLD  		0xE5
#define TRIGGER_TEMP_MEASURE_NOHOLD  	0xF3
#define TRIGGER_HUMD_MEASURE_NOHOLD  	0xF5
#define WRITE_USER_REG  				0xE6
#define READ_USER_REG  					0xE7
#define SOFT_RESET  					0xFE

// SI7021 Driver Error Codes
#define SI7021_ERR_OK					"Operation returned OK."
#define SI7021_ERR_CONFIG				"Error on configration."
#define SI7021_ERR_INSTALL				"Error on installation."
#define SI7021_ERR_NOTFOUND				"Error - NOT FOUND"
#define SI7021_ERR_INVALID_ARG			"Error - Invalid Argument"
#define SI7021_ERR_FAIL		 			"Error - Failed"
#define SI7021_ERR_INVALID_STATE		"Error - Invalid State"
#define SI7021_ERR_TIMEOUT	 			"Error - Time-Out"

#define MASTER_CLOCK_SPEED 				100000

/* Driver Template status enumerations 
typedef enum {
	//DRV_TEMP_OK,
	//DRV_TEMP_ERROR,
	//DRV_TEMP_CONNECTION_ERROR,
	//DRV_TEMP_TIMEOUT_ERROR,
} DRV_TEMP_t;*/


/* Driver Template condif structure */
typedef struct {
	i2c_port_t i2c_port;
	gpio_num_t sda_pin;
	gpio_num_t scl_pin;
	gpio_pullup_t sda_internal_pullup;
	gpio_pullup_t scl_internal_pullup;
} DRV_TEMP_Config_t;


char* DRV_TEMP_Init(DRV_TEMP_Config_t *cfg);
char* DRV_TEMP_Deinit(DRV_TEMP_Config_t *cfg);
float DRV_TEMP_Read();
float DRV_HUM_Read();
uint16_t read_value(uint8_t command);
bool is_crc_valid(uint16_t value, uint8_t crc);

#endif
