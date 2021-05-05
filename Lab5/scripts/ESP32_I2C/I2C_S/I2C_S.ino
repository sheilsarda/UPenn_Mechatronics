/* i2c_slave_test - Example
 *  Slave i2c expecting slave ESP32 on i2c_master_test
 *  modified from 
   https://github.com/espressif/esp-idf/tree/master/examples/peripherals/i2c/i2c_self_test
   For other examples please check:
   https://github.com/espressif/esp-idf/tree/master/examples

    Attach SCL wire to GPIO 26, and  SDA to GPIO 25
   
   See README.md file to get detailed usage of this example.
   This example code is in the Public Domain (or CC0 licensed, at your option.)
   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include "esp_log.h"
#include "driver/i2c.h"
#include "sdkconfig.h"


#define DATA_LENGTH 128                  /*!< Data buffer length of test buffer */
#define RW_TEST_LENGTH 20               /*!< Data length for r/w test, [0,DATA_LENGTH] */

#define I2C_SLAVE_SCL_IO (gpio_num_t)26               /*!< gpio number for i2c slave clock */
#define I2C_SLAVE_SDA_IO (gpio_num_t)25               /*!< gpio number for i2c slave data */
#define I2C_SLAVE_TX_BUF_LEN (2* DATA_LENGTH)              /*!< I2C slave tx buffer size */
#define I2C_SLAVE_RX_BUF_LEN (2 * DATA_LENGTH)              /*!< I2C slave rx buffer size */

#define ESP_SLAVE_ADDR       0x28 /*!< ESP32 slave address, you can set any 7bit value */

/**
 * @brief i2c slave initialization
 */
static esp_err_t i2c_slave_init()
{
    i2c_port_t i2c_slave_port = I2C_NUM_0;
    i2c_config_t conf_slave;
    conf_slave.sda_io_num = I2C_SLAVE_SDA_IO;
    conf_slave.sda_pullup_en = GPIO_PULLUP_ENABLE;
    conf_slave.scl_io_num = I2C_SLAVE_SCL_IO;
    conf_slave.scl_pullup_en = GPIO_PULLUP_ENABLE;
    conf_slave.mode = I2C_MODE_SLAVE;
    conf_slave.slave.addr_10bit_en = 0;
    conf_slave.slave.slave_addr = ESP_SLAVE_ADDR;
    i2c_param_config(i2c_slave_port, &conf_slave);
    return i2c_driver_install(i2c_slave_port, conf_slave.mode,
                              I2C_SLAVE_RX_BUF_LEN,
                              I2C_SLAVE_TX_BUF_LEN, 0);
}

uint8_t data_rd[DATA_LENGTH];
uint8_t data_wr[] = "Message from slave     ";

void setup() {
  Serial.begin(115200);  // put your setup code here, to run once:
  i2c_slave_init();
}

void loop() {
    if (i2c_slave_read_buffer(I2C_NUM_0, data_rd, RW_TEST_LENGTH, 0) > 0 ) { // last term is timeout period, 0 means don't wait  
      if (data_rd[0] == 'G' && data_rd[1] == 'O')
         Serial.println("GO!");
      Serial.printf("READ: %s\n",data_rd);
      if (i2c_slave_write_buffer(I2C_NUM_0, data_wr, RW_TEST_LENGTH, 10 / portTICK_RATE_MS) ) {
        Serial.printf("WRITE: %s\n",data_wr);
      }  
    } // otherwise no data to read
    delay(10); // pretend we're doing something else here.
}
