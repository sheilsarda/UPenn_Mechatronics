/* i2c_secondary
 *  Slave i2c expecting slave ESP32 on i2c_master_test
   https://github.com/espressif/esp-idf/tree/master/examples/peripherals/i2c/i2c_self_test

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

#define DATA_LENGTH 128 /*!< Data buffer length of test buffer */
// #define RW_TEST_LENGTH 20               /*!< Data length for r/w test, [0,DATA_LENGTH] */

#define I2C_SLAVE_SCL_IO (gpio_num_t)26        /*!< gpio number for i2c slave clock */
#define I2C_SLAVE_SDA_IO (gpio_num_t)25        /*!< gpio number for i2c slave data */
#define I2C_SLAVE_TX_BUF_LEN (2 * DATA_LENGTH) /*!< I2C slave tx buffer size */
#define I2C_SLAVE_RX_BUF_LEN (2 * DATA_LENGTH) /*!< I2C slave rx buffer size */

#define ESP_SLAVE_ADDR 0x28 /*!< ESP32 slave address, you can set any 7bit value */

//****************************
//********* Can gripping servo
//****************************
#define SG90FREQ 50                                      // Frequency of the PWM

int arm;

Servo leftServo, rightServo;

void updateGripper()
{
    rightServo.write(180-arm);
    leftServo.write(arm);
    delay(10);
}


void handleopen(){
  Serial.println("Opening Arm");
  arm = 0;
    updateGripper();
}

void handleclose()
{
  Serial.println("Closing Arm");
  arm = 180;
    updateGripper();
}

//****************************
//********* I2C stuff:
//****************************
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
uint8_t data_wr[DATA_LENGTH];

void setup()
{
  Serial.begin(115200); // put your setup code here, to run once:
  i2c_slave_init();
  Serial.println("starting");
  
    memset(data_wr, 0, sizeof(data_wr));
    
    // Servo initialization
    leftServo.attach(21);
    rightServo.attach(12);
    rightServo.setPeriodHertz(50);
    leftServo.setPeriodHertz(50);
}

#define DEBUG 1
void loop()
{

  static uint32_t lastUpdate = micros();
  static uint32_t lastmicros = micros();
  static uint32_t us = micros();
  static int range1, range2, range3;

  if (i2c_slave_read_buffer(I2C_NUM_0, data_rd, DATA_LENGTH, 0) > 0)
  { 
        if (data_rd[0] == '0') {
            handleClosed();
        } else handleOpen();

        Serial.printf("READ: %s\n",data_rd);

    if (i2c_slave_write_buffer(I2C_NUM_0, data_wr, DATA_LENGTH, 10 / portTICK_RATE_MS))
    {
      Serial.printf("WRITE: %s\n", data_wr);
    }
  } 

}
