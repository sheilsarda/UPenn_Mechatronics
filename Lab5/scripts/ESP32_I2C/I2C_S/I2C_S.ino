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

#define DATA_LENGTH 100                  /*!< Data buffer length of test buffer */
// #define RW_TEST_LENGTH 20               /*!< Data length for r/w test, [0,DATA_LENGTH] */

#define I2C_SLAVE_SCL_IO (gpio_num_t)26               /*!< gpio number for i2c slave clock */
#define I2C_SLAVE_SDA_IO (gpio_num_t)25               /*!< gpio number for i2c slave data */
#define I2C_SLAVE_TX_BUF_LEN (2* DATA_LENGTH)              /*!< I2C slave tx buffer size */
#define I2C_SLAVE_RX_BUF_LEN (2 * DATA_LENGTH)              /*!< I2C slave rx buffer size */

#define ESP_SLAVE_ADDR       0x28 /*!< ESP32 slave address, you can set any 7bit value */

//****************************
//********* Ultrasonic sensor stuff:
//****************************
#define trigPin1 18 // 18
#define echoPin1 19 // 19

#define trigPin2 23 // 23
#define echoPin2 22 // 22

#define SAMPLEFREQ 15   // TOF can use 30, Ultrasonic maybe 15

long duration; // variable for the duration of sound wave travel
int distance; // variable for the distance measurement
  int trigPin, echoPin;

int rangeSonar(char select) {    // return range distance in mm
   switch(select){
    case '1': trigPin = trigPin1;
            echoPin = echoPin1;    
            break;
    default: trigPin = trigPin2;
            echoPin = echoPin2;
}

  // Clears the trigPin condition
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  // Sets the trigPin HIGH (ACTIVE) for 10 microseconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin, HIGH);
  // Calculating the distance
  distance = duration * 0.034 / 2; // Speed of sound wave divided by 2 (go and back)
  // Displays the distance on the Serial Monitor
  return distance * 10;
}

void printScan(int range){
    int offset = 10;
    Serial.print(range);
    for (int j=0; j<range; j+=offset){
        Serial.print(".");
        offset *= 1.5;
    }
    Serial.println("#");
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


void setup() {
  Serial.begin(115200);  // put your setup code here, to run once:
  i2c_slave_init();
  Serial.println("starting");
  
  pinMode(trigPin2, OUTPUT);
  pinMode(echoPin2, INPUT);
  pinMode(trigPin1, OUTPUT);
  pinMode(echoPin1, INPUT);
    memset(data_wr, 0, sizeof(data_wr));
}

// #define DEBUG
void loop() {

  static uint32_t lastUpdate = micros();
  static uint32_t lastmicros = micros();
  static uint32_t us = micros();
  static int range1, range2;


    if (i2c_slave_read_buffer(I2C_NUM_0, data_rd, DATA_LENGTH, 0) > 0 ) { // last term is timeout period, 0 means don't wait  
      // if (data_rd[0] == 'G' && data_rd[1] == 'O')
      //    Serial.println("GO!");
      // Serial.printf("READ: %s\n",data_rd);
    snprintf((char *) data_wr, DATA_LENGTH, "RANGE1, %d, RANGE2, %d", range1, range2);
      
      if (i2c_slave_write_buffer(I2C_NUM_0, data_wr, DATA_LENGTH, 10 / portTICK_RATE_MS) ) {
        Serial.printf("WRITE: %s\n",data_wr);
      }  
    } // otherwise no data to read
  
  
  us = micros();
  if (us-lastUpdate > 1000000/SAMPLEFREQ) { // update the servo position
    range1 = rangeSonar('1');   // uncomment if using Sonar
    range2 = rangeSonar('2');   // uncomment if using Sonar
    #ifdef DEBUG
        Serial.printf("SIDE SENSOR\n");
        printScan(range1);
        Serial.printf("FRONT SENSOR\n");
        printScan(range2);
    #endif
    lastUpdate = us;
  }
}
