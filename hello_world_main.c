/* Hello World Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "drv_temp.h"

void app_main()
{
	
    printf("Hello world!\n");
	
	i2c_port_t port = 0;
	
	DRV_TEMP_Config_t cfg;
	cfg.i2c_port = port;
	cfg.sda_pin = GPIO_NUM_18;
	cfg.scl_pin = GPIO_NUM_19;
	cfg.sda_internal_pullup = GPIO_PULLUP_ENABLE;
	cfg.scl_internal_pullup = GPIO_PULLUP_ENABLE;
	
    char* ret = DRV_TEMP_Init(&cfg);
	printf("Init returned-> %s\n",ret);

    for (int i = 10; i >= 0; i--) {
        printf("Restarting in %d seconds...\n", i);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
		//float temp = DRV_TEMP_ReadTemp();
		float temp = DRV_TEMP_Read();
		printf("Temperature is -> %2.2f Celsius Degree\n",temp);
		float humidity = DRV_HUM_Read();
		printf("Humidity is -> %2.2f percent\n",humidity);
    }
	char* ret2 = DRV_TEMP_Deinit(&cfg);
	printf("De-Init returned-> %s\n",ret2);
    printf("Restarting now.\n");
    fflush(stdout);
    esp_restart();
}
