/* i2c_master
 *  Master i2c expecting slave ESP32 on i2c_slave_test
   https://github.com/espressif/esp-idf/tree/master/examples/peripherals/i2c/i2c_self_test
    
    Attach SCL wire to GPIO 19, and  SDA to GPIO 18
   
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

#include <WiFi.h>
#include "html510.h"
#include "joyJS.h"
#include "tankJS.h"

//****************************
//********* Drivetrain stuff:
//****************************

#define RIGHT_CHANNEL0      0 // use first channel of 16  
#define LEFT_CHANNEL1       1 // use 2nd channel of 16
#define rRIGHT_CHANNEL6      6 // use first channel of 16  
#define rLEFT_CHANNEL7       7 // use 2nd channel of 16
#define SERVOPIN1    33       // PWM generating pin (33) for Right Motor
#define SERVOPIN2    32       // PWM generating pin (32) for left Motor
#define SERVOPIN3    27       // PWM generating pin (34) for Rear Right Motor  12, 25
#define SERVOPIN4    14       // PWM generating pin (35) for Rear left Motor   13, 26
#define SERVOFREQ    60       // Frequency of the PWM
#define LEDC_RESOLUTION_BITS  16  //LEDC resolution in bits
#define LEDC_RESOLUTION  ((1<<LEDC_RESOLUTION_BITS)-1)  //LEDC resolution

#define leftpin 15            //Direction pin for front left motor
#define rightpin 13           //Direction for front right motor
#define rleftpin 10            //Direction pin for rear left motor
#define rrightpin 9           //Direction for rear right motor
#define NEUTRAL 0             //Variable storing the no spin condition of motor
#define MAX 50*100            //Variable storing the full forward spin condition of motor
#define REVERSE -50*100       //Variable storing the full backward spin condition of motor

uint32_t LMduty;              //Duty Cycle variable for the left motor
uint32_t RMduty;              //Duty Cycle variable for the right motor
uint32_t rLMduty;              //Duty Cycle variable for the REAR left motor
uint32_t rRMduty;              //Duty Cycle variable for the REAR right motor
int leftmotor, rightmotor, rleftmotor, rrightmotor;    //Variables determining the spin condition of motors

void updateServos() {
    if(leftmotor < 0)
        digitalWrite(leftpin, LOW);
    else
        digitalWrite(leftpin, HIGH);

    if(rightmotor < 0)
        digitalWrite(rightpin, LOW);
    else
        digitalWrite(rightpin, HIGH);

    //Rear Motors
    if(rleftmotor < 0)
        digitalWrite(rleftpin, LOW);
    else
        digitalWrite(rleftpin, HIGH);

    if(rrightmotor < 0)
        digitalWrite(rrightpin, LOW);
    else
        digitalWrite(rrightpin, HIGH);


    LMduty = map(abs(leftmotor), NEUTRAL, MAX, 0, LEDC_RESOLUTION);
    RMduty = map(abs(rightmotor), NEUTRAL, MAX, 0, LEDC_RESOLUTION);
    rLMduty = map(abs(rleftmotor), NEUTRAL, MAX, 0, LEDC_RESOLUTION);
    rRMduty = map(abs(rrightmotor), NEUTRAL, MAX, 0, LEDC_RESOLUTION);

    ledcWrite(LEFT_CHANNEL1, LMduty);
    ledcWrite(RIGHT_CHANNEL0, RMduty);

    //Rear Motors
    ledcWrite(rLEFT_CHANNEL7, rLMduty);
    ledcWrite(rRIGHT_CHANNEL6, rRMduty);
}


//****************************
//********* Web interface stuff:
//****************************

WiFiServer server(80);
const char *body;

/********************/
/* HTML510  web   */
void handleFavicon() {
    sendplain(""); // acknowledge
}

void handleRoot() {
    sendhtml(body);
}

void handleSwitch() { // Switch between JOYSTICK and TANK mode
    String s="";
    static int toggle=0;
    if (toggle) body = joybody;
    else body = tankbody;
    toggle = !toggle;
    sendplain(s); //acknowledge
}

/************************/
/* joystick mode  code  */

int leftarm, rightarm;
int x,y;

void handleJoy() {
    int left, right;
    x = getVal(); // from -50 to +50
    y = getVal();
    String s = String(x) + "," + String(y);

    float Left;     //Left motor movement variable
    float Right;    //Right motor movement variable
    float rLeft;     //Rear Left motor movement variable
    float rRight;    //Rear Right motor movement variable


    float z = sqrt(x * x + y * y);   //hypotenuse
    // Serial.println("z = %d", z);


    float rad = acos(abs(x) / z);    //Computing the unsigned angle in radians


    if (isnan(rad) == true)          //Cater for NaN values
    {
        rad = 0;
    }


    float angle = rad * 180 / PI;    //Computing the unsigned angle (0-90) in degrees

    /**
     * Measuring turns with the angle of turn
     * for angles between 0-90, with angle 0 the turn co-efficient is -1, 
     * with angle 45 the co-efficient is 0, and with angle 90 it is 1
     * For a straight line path the turn coefficient will remain the same
     */

    float tcoeff = -1 + (angle / 90) * 2;     //turn co-efficient
    float turn = tcoeff * abs(abs(y) - abs(x));    //Variable determining the turn movement
    turn = round(turn * 100) / 100;


    float mov = max(abs(y), abs(x));      // Max of y or x is the movement

    // First and third quadrant
    if ((x >= 0 && y <= 0) || (x < 0 && y > 0))
    {
        Left = mov;
        Right = turn;
        rLeft = turn;
        rRight = mov;
    }
    else
    {
        Right = mov;
        Left = turn;
        rLeft = mov;
        rRight = turn;
    }

    // reverse spin direction condition
    if (y > 0) {
        Left = 0 - Left;
        Right = 0 - Right;
        rLeft = 0 - rLeft;
        rRight = 0 - rRight;
    }

    leftmotor = -100*Left;
    rightmotor = -100*Right;
    rleftmotor = 100*rLeft;
    rrightmotor = 100*rRight;

    sendplain(s);
    Serial.printf("received X,Y:=%d,%d\n",x,y);
}

void handleArmdown() {
    Serial.println("armdown");
    sendplain(""); //acknowledge
}

void handleArmup() {
    Serial.println("armup");
    sendplain(""); //acknowledge
}

/*********************/
/* tank mode  code  */
int leftstate, rightstate;
long lastLeverMs;

void handleLever() {
    leftarm = getVal();
    rightarm = getVal();
    leftstate = getVal();
    rightstate = getVal();
    String s = String(leftarm) + "," + String(rightarm) + "," +
               String(leftstate) + "," + String(rightstate);

    if (leftstate>0)      leftmotor =  REVERSE;
    else if (leftstate<0) leftmotor =  MAX;
    else                  leftmotor =  NEUTRAL;

    if (rightstate>0)      rightmotor =  REVERSE;
    else if (rightstate<0) rightmotor =  MAX;
    else                   rightmotor =  NEUTRAL;

    lastLeverMs = millis(); //timestamp command
    sendplain(s);
    Serial.printf("received %d %d %d %d \n",leftarm, rightarm, leftstate, rightstate); // move bot  or something
}

//****************************
//********* I2C stuff:
//****************************

#define DATA_LENGTH 128                  /*!< Data buffer length of test buffer */
#define RW_TEST_LENGTH 10                /*!< Data length for r/w test, [0,DATA_LENGTH] */

#define I2C_MASTER_SCL_IO (gpio_num_t)19               /*!< gpio number for I2C master clock */
#define I2C_MASTER_SDA_IO (gpio_num_t)18               /*!< gpio number for I2C master data  */
#define I2C_MASTER_FREQ_HZ 100000         /*!< I2C master clock frequency */
#define I2C_MASTER_TX_BUF_DISABLE 0                           /*!< I2C master doesn't need buffer */
#define I2C_MASTER_RX_BUF_DISABLE 0                           /*!< I2C master doesn't need buffer */

#define ESP_SLAVE_ADDR     0x28        /*!< ESP32 slave address, you can set any 7bit value */
#define WRITE_BIT     I2C_MASTER_WRITE          /*!< I2C master write */
#define READ_BIT      I2C_MASTER_READ           /*!< I2C master read */
#define ACK_CHECK_EN  0x1                       /*!< I2C master will check ack from slave*/
#define ACK_CHECK_DIS 0x0                       /*!< I2C master will not check ack from slave */
#define ACK_VAL       I2C_MASTER_ACK            /*!< I2C ack value */
#define NACK_VAL      I2C_MASTER_NACK           /*!< I2C nack value */

/**
 * @brief test code to read esp-i2c-slave
 *        We need to fill the buffer of esp slave device, then master can read them out.
 *
 * _______________________________________________________________________________________
 * | start | slave_addr + rd_bit +ack | read n-1 bytes + ack | read 1 byte + nack | stop |
 * --------|--------------------------|----------------------|--------------------|------|
 *
 */
static esp_err_t i2c_master_read_slave(i2c_port_t i2c_num, uint8_t *data_rd, size_t nsize)
{
    if (nsize == 0) {
        return ESP_OK;
    }
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (ESP_SLAVE_ADDR << 1) | READ_BIT, ACK_CHECK_EN); 
    if (nsize > 1) {
        i2c_master_read(cmd, data_rd, nsize - 1, ACK_VAL);
    }
    i2c_master_read_byte(cmd, data_rd + nsize - 1, NACK_VAL);
    i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(i2c_num, cmd, 1000 / portTICK_RATE_MS); // send all queued commands
    i2c_cmd_link_delete(cmd);
    return ret;
}

/**
 * @brief Test code to write esp-i2c-slave
 *        Master device write data to slave(both esp32),
 *        the data will be stored in slave buffer.
 *        We can read them out from slave buffer.
 *
 * ___________________________________________________________________
 * | start | slave_addr + wr_bit + ack | write n bytes + ack  | stop |
 * --------|---------------------------|----------------------|------|
 *
 */
static esp_err_t i2c_master_write_slave(i2c_port_t i2c_num, uint8_t *data_wr, size_t nsize)
{
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (ESP_SLAVE_ADDR << 1) | WRITE_BIT, ACK_CHECK_EN);
    i2c_master_write(cmd, data_wr, nsize, ACK_CHECK_EN);
    i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(i2c_num, cmd, 1000 / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);
    return ret;
}


/**
 * @brief i2c master initialization
 */
static esp_err_t i2c_master_init()
{
    i2c_port_t i2c_master_port = I2C_NUM_1;
    i2c_config_t conf;
    conf.mode = I2C_MODE_MASTER;
    conf.sda_io_num = I2C_MASTER_SDA_IO;
    conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
    conf.scl_io_num = I2C_MASTER_SCL_IO;
    conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
    conf.master.clk_speed = I2C_MASTER_FREQ_HZ;
    i2c_param_config(i2c_master_port, &conf);
    return i2c_driver_install(i2c_master_port, conf.mode,
                              I2C_MASTER_RX_BUF_DISABLE,
                              I2C_MASTER_TX_BUF_DISABLE, 0);
}
//****************************
//********* Wall following:
//****************************

double  leftservo, rightservo; 
int left_dir, right_dir;

int oldleftservo, oldrightservo;
int auto_state = 0;
int follow_state = 0;

void control(int desired_front, int measured_front, int desired_right, int measured_right ) {
  double kp=10;
  //double ki=0.01;
  //static int sumederror;
  
  double error_front=desired_front-measured_front;
  double error_right=desired_right-measured_right;
  //summederror=summederror+error;
  //if(error==0)summederror=0;
  
  int u_front = kp*error_front;
  int u_right = kp*error_right;
 
  if(u_front>0) follow_state=2; //back up
  else if(u_front<0) follow_state=1; //go straight

  if(u_right>0) follow_state=3; //turn slight left
  else if(u_right<0) follow_state=4; //turn slight right
  else follow_state=2; //go straight
}

void handleWallFollow() {
  static long last_right = millis();
  static long last_front = millis();
  static long since_turning_left = millis();
  static long since_turning_slight_left = millis();
  uint32_t ms2;

  ms2 = millis();
  
  int delay_bounce = 500; //how long (ms) to turn left after the right button is pressed
  int delay_back = 500; //how long to back up (ms) after the front button is pressed
  int delay_left = 500; //how long to turn left (ms) after backing up
  int delay_slight_left = 500; //how long to turn slight left (ms) after turning hard left - this returns the robot to close to the wall

  control(20,front_sensor,20,right_sensor); //controls and sets certain follow states
  Serial.printf("right: %d, front: %d, follow_state: %d \n", right_sensor, front_sensor, follow_state);

  if (follow_state == 0){ // Do nothing
    leftservo=SERVOOFF;
    rightservo=SERVOOFF;
  }

  if (follow_state == 1){ //Drive straight

    leftservo=FULLFRONT;
    left_dir=HIGH;
    rightservo=FULLFRONT;
    right_dir=LOW;
  }

  if (follow_state == 2) { //Drive backwards
    if(ms2 - last_front <= delay_back){

      leftservo=FULLBACK;
      left_dir=LOW;
      rightservo=FULLBACK;
      right_dir=HIGH;
    
    } else  follow_state=5;

   }
  
  if ((follow_state == 3) && (ms2-last_right <= delay_bounce)) { //slight left

      //insert motor driving code here
    leftservo=0.7*FULLFRONT;
    left_dir=HIGH;
    rightservo=FULLFRONT;
    right_dir=LOW;
  }

  if (follow_state == 4){ //slight right
    leftservo=FULLFRONT;
    left_dir=HIGH;
    rightservo=0.7*FULLFRONT;
    right_dir=LOW;
  }

  if ((follow_state == 5) && (ms2-since_turning_left > delay_left)) { //slight left
    leftservo=FULLBACK;
    left_dir=LOW;
    rightservo=FULLFRONT;
    right_dir=LOW;
  }
  
}

uint8_t data_wr[] = "GO";
uint8_t data_rd[DATA_LENGTH];

void setup() {
  Serial.begin(115200);  // put your setup code here, to run once:
  i2c_master_init();
    
    WiFi.mode(WIFI_MODE_STA);
    WiFi.begin(ssid, password);
    WiFi.config(IPAddress(192, 168, 1, 133), // change the last number to your assigned number
                IPAddress(192, 168, 1, 1),
                IPAddress(255, 255, 255, 0));
    while(WiFi.status()!= WL_CONNECTED ) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("WiFi connected");
    Serial.printf("Use this URL http://%s/\n",WiFi.localIP().toString().c_str());
    server.begin();                  //Start server

    // Servo initialization
    ledcSetup(RIGHT_CHANNEL0, SERVOFREQ, LEDC_RESOLUTION_BITS); // channel, freq, bits
    ledcAttachPin(SERVOPIN1, RIGHT_CHANNEL0);
    ledcSetup(LEFT_CHANNEL1, SERVOFREQ, LEDC_RESOLUTION_BITS); // channel, freq, bits
    ledcAttachPin(SERVOPIN2, LEFT_CHANNEL1);
    ledcSetup(rRIGHT_CHANNEL6, SERVOFREQ, LEDC_RESOLUTION_BITS); // channel, freq, bits
    ledcAttachPin(SERVOPIN3, rRIGHT_CHANNEL6);
    ledcSetup(rLEFT_CHANNEL7, SERVOFREQ, LEDC_RESOLUTION_BITS); // channel, freq, bits
    ledcAttachPin(SERVOPIN4, rLEFT_CHANNEL7);

    pinMode(leftpin, OUTPUT);
    pinMode(rightpin, OUTPUT);
    pinMode(rleftpin, OUTPUT);
    pinMode(rrightpin, OUTPUT);

    // HTML510 initialization
    attachHandler("/joy?val=",handleJoy);
    attachHandler("/armup",handleArmup);
    attachHandler("/armdown",handleArmdown);
    attachHandler("/switchmode",handleSwitch);
    attachHandler("/lever?val=",handleLever);
    body = joybody;

    attachHandler("/favicon.ico",handleFavicon);
    attachHandler("/ ",handleRoot);
}

#define I2CDelay 200 // ms

void loop() {
    static long lastWebCheck = millis();
    static long lastServoUpdate = millis();
    static long lastI2CSent = millis();
    static long lastI2CRec = millis();
    uint32_t ms;

    ms = millis();
    if (ms-lastWebCheck > 2) {
        serve(server,body);
        lastWebCheck = ms;
    }
    if (ms-lastServoUpdate > 1000/SERVOFREQ) {
        updateServos();
        lastServoUpdate = ms;
    }
    
    if (ms - lastI2CRec > I2CDelay && (i2c_master_read_slave(I2C_NUM_1, data_rd, DATA_LENGTH)== ESP_OK)){
      Serial.printf("Read: %s\n",data_rd);
        lastI2CRec = ms;
    }
    
    if (ms - lastI2CSent > I2CDelay && (i2c_master_write_slave(I2C_NUM_1, data_wr, RW_TEST_LENGTH) == ESP_OK)){
        lastI2CSent = ms;
   } 
}
