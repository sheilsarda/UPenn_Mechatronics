/*
 * Wireless 2 ch oscilloscope - on ESP32
 * Analog input pin CHANNEL1 is GPIO-36, 
 * Digial input CHANNEL2 is GPIO-39, 
 * Square wave and sine wave output on GPIO-25
 * 
 * Analog mode 12bit 100KHz max bandwidth (limited by ESP32 DMA ADC)
 * Digital mode (3.3V logic) max bandwidth ~300KHz (limited by TCP)
 * features: horiz, vert (gnd) adj, 10uSdiv to 1S/div, sinewave square wave generator up to 100K, autoset (buggy)
 * display modes: singleshot, auto (normal), 
 * 
 * Uses javascript and AJAX to do all display work.
 * (c) all rights reserved Mark Yim
 * May 23,2021
 */
 
// To start in Station mode undefine APMODE and change ssid and password with your router
#define APMODE
String ssid = "Oscillosorta";
String pass;  

#include <esp_wifi.h> // needed to turn off power save mode
#include "index.h"  //Web page header file
#include "html510.h"
#include "MCP6S21.h"

#include <driver/i2s.h>
int timerOnFlag=0;

#include "cosineGenerator.h"
#include "driver/dac.h"
#include "driver/adc.h" // for ADC calibration
#include "esp_adc_cal.h"
MCP6S21 mcp6s21;

HTML510Server h(80);

// scope display variables

#define XRANGE 400
#define YRANGE 500
#define TIMEDIVPERSCREEN    10
#define USPERSCREEN   (TIMEDIVPERSCREEN*usPerDiv)
#define USPERSAMPLE  (((long)usPerDiv/50*YRANGE)/resolution)
#define ADCcountPerV   1000*4096/3300  // ADC is 12 bit (4096 values), 3.3V ref.
#define DATAPTS       2048
int16_t x1[DATAPTS];
int16_t x2[DATAPTS];
uint32_t y[DATAPTS];
volatile uint32_t indx;
int resolution=250;
int currentCh=1;
int16_t trigV=30;
int trigCh=1;
int triggered=0;
uint32_t usPerDiv = 1000;   // 10 divs per screen -> usecsPerScreen/10
int mvPerDiv[] = {1000,1000};      // 8 divs per screen   -> mvoltsPerScreen/8
int chOn[2] = {1,0} ;
int risingEdge;
const int ADCpin[2] = {36, 39};
const uint16_t VDial[] = {10,20,50,100,200,500,1000,2000,5000};
int iVDial[]={6,6};
const uint32_t TDial[] = {2,5,10,20,50,100,200,500,1000,2000,5000,10000,20000,50000};
int iTDial=10;
esp_adc_cal_characteristics_t *adc_chars;

/**************************************/ 
/* function generator specific rounties */

#define SQUARE 1
#define SINE 2
#define LEDC_CHANNEL       0 // use first channel of 16  
#define LEDC_RES_BITS  8
#define LEDC_RES  ((1<<LEDC_RES_BITS)-1) 
#define FG_PIN           25           // use either 25 or 26
#define DAC_CHANNEL    DAC_CHANNEL_1  // DAC_CHANNEL_1 is for FG_PIN2=25, DAC_CHANNEL_2 for 26

int fgFreq=100, Duty=50, waveform;
int sinePrecision=8;
int sineScale =1;
int sineOffset=0;

void ledcAnalogWrite(uint8_t channel, uint32_t value, uint32_t valueMax = 255) {            
  uint32_t duty =  LEDC_RES * min(value, valueMax) / valueMax;   
  ledcWrite(channel, duty);  // write duty to LEDC 
}

void handleTestFq() {
  fgFreq = h.getVal();

  if (waveform == SQUARE) {
    ledcSetup(LEDC_CHANNEL, fgFreq, LEDC_RES_BITS); // channel, freq, bits
    ledcAttachPin(FG_PIN, LEDC_CHANNEL);       // also sets pinMode to output
    ledcAnalogWrite(LEDC_CHANNEL, (Duty*255)/100, 255); // 50% duty cycle
  }
  else if (waveform == SINE) {
     dac_CW_set( fgFreq,sinePrecision);
  }
  h.sendplain(""); // acknowledge
}

void handleDuty() {
  Duty = h.getVal();
  if (waveform == SQUARE)
    ledcAnalogWrite(LEDC_CHANNEL, (Duty*255)/100, 255); 
  h.sendplain(""); // acknowledge
}

void handleSineOffset() {
  int8_t val = h.getVal();
  if (waveform == SINE) {
     dac_offset_set(DAC_CHANNEL, val);
     sineOffset = val;
  }
  h.sendplain(""); // acknowledge
}

void handleSineScale() {
  uint8_t val = h.getVal();
  if (waveform == SINE && (val < 4)) {
     dac_scale_set(DAC_CHANNEL, val);
     sineScale = val;
  }
  h.sendplain(""); // acknowledge
}

void handleSinePrecision() {
  uint8_t val = h.getVal();
  if (waveform == SINE && (val < 9)) {
    dac_CW_set(fgFreq,sinePrecision);
    sinePrecision = val;
  }
  h.sendplain(""); // acknowledge
}

void handleSine() { // turn on sine wave (off square)
 // ledcAnalogWrite(LEDC_CHANNEL, 0, 255); // give 0 duty
  ledcDetachPin(FG_PIN);
  pinMode(FG_PIN,INPUT);
  dac_cosine_enable(DAC_CHANNEL);
  dac_output_enable(DAC_CHANNEL);
  dac_scale_set(DAC_CHANNEL, sineScale);
  dac_CW_set(fgFreq,sinePrecision);
  dac_offset_set(DAC_CHANNEL, sineOffset);
  waveform = SINE;
  h.sendplain(""); // acknowledge
}

void handleSquare() { // turn on square wave (off sine)
  dac_output_disable(DAC_CHANNEL);
  ledcSetup(LEDC_CHANNEL, fgFreq, LEDC_RES_BITS); // channel, freq, bits
  ledcAttachPin(FG_PIN, LEDC_CHANNEL);  // sets pinmode OUTPUT)
  ledcAnalogWrite(LEDC_CHANNEL, (255*Duty)/100, 255); // 50% duty cycle
  waveform = SQUARE;
  h.sendplain(""); // acknowledge
}

void handleWaveOff() { // turn on square wave (off sine)
  dac_output_disable(DAC_CHANNEL);
  ledcAnalogWrite(LEDC_CHANNEL, 0, 255); // give 0 duty
  ledcDetachPin(FG_PIN);
  pinMode(FG_PIN,INPUT);
  waveform = 0;
  h.sendplain(""); // acknowledge
}
/* end of function generator specific routines */
/***********************************************/ 


/****************************/ 
/* Oscope specific routines */

int freqmultiplier=1;

uint16_t chan2_buff[DATAPTS];
uint16_t chan1_buff_a[1024]; // max DMA buff 1024 records
uint16_t chan1_buff_b[1024]; // max DMA buff 1024 records
int32_t currentDMAtime;
int lastindx;

QueueHandle_t i2sQueue;
hw_timer_t* timer = NULL;

// use macro for digitalRead to be faster and in IRAM and no cache issues
#define mydigitalread(pin)  ((pin < 32) ? ((GPIO.in >> (pin)) & 0x1) : ((GPIO.in1.val >> ((pin) - 32)) & 0x1))
    
void IRAM_ATTR storeCh2pin() {
  chan2_buff[indx % DATAPTS] = ((long)XRANGE * 3300* mydigitalread(ADCpin[1]))/(8*mvPerDiv[1]);  
  indx++;  
}

int usPerSample() {
    if (USPERSAMPLE<2) return 2;
    else return USPERSAMPLE;
}

int sampleFreq(){
    return 1000000/usPerSample();
}

void stopTimer(int settask) {
  timerAlarmDisable(timer);
  timerDetachInterrupt(timer);  
  timerEnd(timer);      
  timerOnFlag=0;
}

void startTimer(int settask) {
  timer = timerBegin(0, 80, true);   // Use timer 0 [0:3], Set prescaler = 80. 
  timerAttachInterrupt(timer, &storeCh2pin, true); // Attach onTimer() to our timer.
  timerAlarmWrite(timer, usPerSample(), true); // Repeat the alarm (third parameter)
  timerAlarmEnable(timer); 
  pinMode(ADCpin[1],INPUT);
  timerOnFlag=1;
}

void stopi2s() {
  i2s_adc_disable(I2S_NUM_0);
  i2s_driver_uninstall(I2S_NUM_0);
}

void starti2s(int buffersize) {
  i2s_config_t i2s_config = {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX | I2S_MODE_ADC_BUILT_IN),  
    .sample_rate = 100000,                               // in samples per sec 44.1k is typ audio
    .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,         // 16 bit I2S
    .channel_format = I2S_CHANNEL_FMT_ALL_LEFT,          // double mem, but noisy if use ONLY_LEFT
    .communication_format =  I2S_COMM_FORMAT_I2S_LSB,    // doesn't really matter
    .intr_alloc_flags = ESP_INTR_FLAG_LEVEL3,             // max level for I2S alloc.
    .dma_buf_count = 2,                                  // only 2 to minimize latency
    .dma_buf_len = buffersize,                           // number of samples
    .use_apll = false,                                   // true-> use PLL for timing accuracy
  };

  i2s_driver_install(I2S_NUM_0, &i2s_config, 4, &i2sQueue);  // install I2S 0 driver

  i2s_set_adc_mode(ADC_UNIT_1, ADC1_CHANNEL_0);
  i2s_set_sample_rates(I2S_NUM_0,freqmultiplier*sampleFreq());
  i2s_adc_enable(I2S_NUM_0);
}

void updateSampling() {
  while (freqmultiplier  *sampleFreq() < 5200)    freqmultiplier*=2; // 5200 is lower limit of DMA
  while (freqmultiplier/2*sampleFreq() > 5200)    freqmultiplier/=2;

  if (chOn[1]) {  // restart timer to set new sampling time
    if ( timerOnFlag) stopTimer(0);
    if (!timerOnFlag) startTimer(0); 
    if (usPerSample() < 4) { // too fast for timer interrupt on channel 2
      resolution = 250;
      iTDial = 5;
      usPerDiv = TDial[iTDial];
      Serial.printf("Samples too FAST for interrupts now set to %d ",sampleFreq());
    }
  }
  else if (timerOnFlag)  stopTimer(1);
  
  if (chOn[0]) {
    stopi2s();
    starti2s(resolution);
    Serial.printf("DMA=%dHz freqmult %d usPer %d dial=%d ",freqmultiplier*sampleFreq(), freqmultiplier, usPerSample(),iTDial);
  }
}

int dblbuff=0;

int dumpdata() {
    size_t bytesRead = 0;
    char *buff;
    long int firstt=micros();

    dblbuff = (dblbuff+1)%2;

    if (dblbuff) buff = (char *)chan1_buff_a;
    else buff = (char *)chan1_buff_b;
    
    if (i2s_read(I2S_NUM_0, buff, 2*sizeof(int16_t)*resolution, &bytesRead, 0)!= ESP_OK)
      Serial.printf("error\n");

    if (bytesRead>0) {
      currentDMAtime = firstt;//micros();
      lastindx=indx;   // use to sync ch 1 and 2
    
   //   if ((int)currentDMAtime-firstt > 50) Serial.printf("delay %d\n",currentDMAtime-firstt);
     }
     return bytesRead;    
}

void updateDMAbuffer() {
    i2s_event_t evt; 

    if (xQueueReceive(i2sQueue, &evt, 0) == pdTRUE)  { //  ticks-to-wait = 0 for ISR, portMAX_DELAY to block
      if (evt.type == I2S_EVENT_RX_DONE) {
        dumpdata();
        if (findTrigger()) triggered = 1;
      }
    }
}

int lastlastindx; // use to sync ch 1 and 2
void getDMAsamples() {
    static int32_t lastDMAtime;
    int j=0;
    uint16_t *buff;
    int startUs = lastDMAtime; // just used to reduce # of digits transmitted in time index

    do 
    {
        while (currentDMAtime == lastDMAtime )  {
            updateDMAbuffer();
        }  
        lastDMAtime = currentDMAtime;
        lastlastindx = lastindx;       
        if (freqmultiplier >3) lastlastindx -= resolution/freqmultiplier; //why is timeshift in ch2 needed when freqmult>3? not sure...
            
        if (dblbuff) buff = chan1_buff_a;
        else buff = chan1_buff_b;
 
        for (int i=0; i<resolution/freqmultiplier; i++) {
            uint32_t mv = esp_adc_cal_raw_to_voltage(buff[freqmultiplier*i*2], adc_chars);
            x1[j] = ((long)XRANGE * mv)/(8*mvPerDiv[0]);
            y[j] = lastDMAtime - (resolution/freqmultiplier - i)*usPerSample() - startUs;
            j++;
        }      
    } while (j < resolution);

    if (freqmultiplier ==1) {
      if (dblbuff) buff = chan1_buff_b; // switch to prior buffer
      else buff = chan1_buff_a;
      for (int i=0; i<resolution; i++) { // add an extra frame of older data in case single mode
        uint32_t mv = esp_adc_cal_raw_to_voltage(buff[freqmultiplier*i*2], adc_chars);
        x1[j] = ((long)XRANGE * mv)/(8*mvPerDiv[0]);
        y[j] = lastDMAtime - (2*resolution - i)*usPerSample() - startUs;
        j++;
      }
    }
}

void getCh2samples() { 
    for (int i=0; i< resolution; i++) {               // add (itDial-12) hack to calibrate ch2 to ch1 sync
        x2[i] = chan2_buff[(lastlastindx - resolution + i + (-200/(usPerSample()+15)) )%DATAPTS] ;
        // store second pre-half  for single shot mode
        x2[i+resolution] = chan2_buff[(lastlastindx - 2*resolution + i + (-200/(usPerSample()+15)) )%DATAPTS] ;
    }
}

void initChs() { 
  adc_chars = (esp_adc_cal_characteristics_t *)calloc(1, sizeof(esp_adc_cal_characteristics_t));
  esp_adc_cal_value_t val_type = esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_DB_11, ADC_WIDTH_BIT_12, 3300, adc_chars);
//  Serial.printf("m %d, b %d, vref%d, 4095=%dmv\n",adc_chars->coeff_a,adc_chars->coeff_b,adc_chars->vref, esp_adc_cal_raw_to_voltage(4095,adc_chars));
  esp_wifi_set_ps(WIFI_PS_NONE); // Turn off WiFi powersave mode so ADC DMA works
  starti2s(resolution);        
  i2s_set_sample_rates(I2S_NUM_0,freqmultiplier*sampleFreq());  
}

int findTrigger() {
  static   int16_t oldx,x;
  int ctrigV;
  uint16_t *buff;

    if (dblbuff) buff = chan1_buff_a;
    else buff = chan1_buff_b;

//  if (trigCh == 1) oldx = ((long)XRANGE * chan1_buff[0])/(ADCcountPerV*(8*mvPerDiv[0])/1000);
//  if (trigCh == 2) oldx = chan2_buff[(lastlastindx - resolution + 0 + (iTDial - 12) )%DATAPTS]; 
  if (trigCh==1) {
    ctrigV = (long)(ADCcountPerV*8*mvPerDiv[0])*trigV/XRANGE/1000;
    for (int i = 0; i < resolution; i++) {
      x = buff[i*2];
      if (risingEdge && oldx > ctrigV && x <= ctrigV) { return i; } 
      else if (!risingEdge && oldx < ctrigV && x >= ctrigV) { return i; }
      oldx = x; 
    }
  } else
  for (int i = 0; i < resolution; i++) {
    x = chan2_buff[(lastlastindx - resolution + i + (-200/(usPerSample()+15)) )%DATAPTS];
    if (risingEdge && oldx > trigV && x <= trigV) { return i; } 
    else if (!risingEdge && oldx < trigV && x >= trigV) { return i; }
    oldx = x; 
  }
  return 0;
}

/***********************************************
 * Webhandlers
 * *********************************************
 */

void handleLocalWiFi() {
  String ssidpass =  h.getText();
   ssid = ssidpass.substring(0, ssidpass.indexOf('+')); 
   pass = ssidpass.substring(ssidpass.indexOf('+')+1, ssidpass.length());
  Serial.print("ssid "); Serial.print(ssid); Serial.println("EOL");
  Serial.print("pass "); Serial.print(pass); Serial.println("EOL");
  h.sendplain("");
  WiFi.disconnect();
  initWiFiStationMode();
}

void handleADCchars() {
  String s;
  s = String(esp_adc_cal_raw_to_voltage(0,adc_chars))+","+String(esp_adc_cal_raw_to_voltage(4095,adc_chars));
  h.sendplain(s);
}

void handleResolution() {
  String s;
  resolution = h.getVal();
  
  updateSampling();
  s=String(resolution);
  h.sendplain(s);
}

void handleMCPgain() {
  String s;
  mcp6s21.setGain((MCPgain)h.getVal());
  
  h.sendplain("");
}

void handleSpeedup() {  
  String s;
  iTDial++;
  if (iTDial>=sizeof(TDial)/sizeof(uint32_t)) iTDial = sizeof(TDial)/sizeof(uint32_t)-1;
  usPerDiv = TDial[iTDial];
  
  updateSampling();
  s=String(usPerDiv);
  h.sendplain(s);
  Serial.printf("speedup %d us\n",usPerDiv);
}

void handleSpeeddown() {
  String s;
  iTDial--;
  if (iTDial<1) iTDial = 0;
  usPerDiv = TDial[iTDial];
  
  updateSampling();
  s=String(usPerDiv);
  h.sendplain(s);
  Serial.printf("speeddown %d\n",usPerDiv);
}

void handleRun() {
  int val = h.getVal();
  switch(val) {
    case 1: chOn[0] = 1; chOn[1] = 0; break;
    case 2: chOn[0] = 0; chOn[1] = 1; break;
    case 3: chOn[0] = 1; chOn[1] = 1; break;
    default: case 0: chOn[0] = 0; chOn[1] = 0; break; 
  }
  if (chOn[1]) {
    if (!timerOnFlag) startTimer(1);
  } 
  else if (timerOnFlag) stopTimer(1);
  
  h.sendplain(""); // acknowledge
  Serial.printf("channelOn = %d\n", val);
}

void handleUpdate() {
  String s = String(chOn[0])+","+String(chOn[1]); // header has channel info
  if (chOn[0]) getDMAsamples();    
  if (chOn[1]) getCh2samples();
  
  for (int i=0; i<resolution; i++) {
    s = s+","+y[i];
    if (chOn[0]) s = s+","+x1[i];
    if (chOn[1]) s = s+","+x2[i];
  }
  h.sendplain(s);
  triggered = 0;
}

void handleChannel() {  
  currentCh = h.getVal();
  h.sendplain("");
  Serial.printf("select channel %d\n", currentCh);
}

void handleSingle() {
  String s = String(chOn[0])+","+String(chOn[1]); // header has channel info
  if (chOn[0]) getDMAsamples();    
  if (chOn[1]) getCh2samples();
  
  if (triggered) {      
    if (freqmultiplier==1) { // also send previous screenfull 
      for (int i=resolution; i<2*resolution; i++) {
        s = s+","+y[i];
        if (chOn[0]) s = s+","+x1[i];
        if (chOn[1]) s = s+","+x2[i];
      }
    }
    for (int i=0; i<resolution; i++) {
      s = s+"," + y[i];
      if (chOn[0]) s = s+","+x1[i];
      if (chOn[1]) s = s+","+x2[i];
    }
  }
  h.sendplain(s);
}

void handleEdge() {  
  risingEdge = h.getVal();
  h.sendplain("");
  Serial.printf("select edge %d\n", risingEdge);
}

void handleTrigCh() {  
  trigCh = h.getVal();
  h.sendplain("");
  Serial.printf("select trigch %d\n", trigCh);
}

void handleTrigV() {  
  trigV = h.getVal();
  h.sendplain("");
  Serial.printf("select trigV %d\n", trigV);
}

void handleVoltup() {  
  if (++iVDial[currentCh] >= sizeof(VDial)/sizeof(uint16_t)) iVDial[currentCh] = sizeof(VDial)/sizeof(uint16_t)-1;
  mvPerDiv[currentCh] = VDial[iVDial[currentCh]];
  String s = String(mvPerDiv[currentCh]);
  h.sendplain(s);
  Serial.printf("voltup %d %d\n",mvPerDiv[currentCh], iVDial[currentCh]);
}

void handleVoltdown() {
  if (--iVDial[currentCh]< 1 ) iVDial[currentCh] = 0;
  mvPerDiv[currentCh] = VDial[iVDial[currentCh]];
  String s = String(mvPerDiv[currentCh]);
  h.sendplain(s);
  Serial.printf("voltdown %d %d\n",mvPerDiv[currentCh], iVDial[currentCh]);
}



/* end of oscilloscope specific routines */
/***********************************************/

/********************/
/* standard web stuff */

void handleFavicon(){
  h.sendplain(""); // acknowledge
}

void handleRoot() {
  h.sendhtml(body);
}

void initWiFiStationMode() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid.c_str(), pass.c_str());
  WiFi.config(IPAddress(192, 168, 1, 2),
              IPAddress(192, 168, 1, 1),
              IPAddress(255, 255, 255, 0));
  while(WiFi.status()!= WL_CONNECTED ) { 
    delay(500);
    Serial.print("."); 
  }
  // Print the IP address
  Serial.print("Use this URL to connect: http://");  
  Serial.print(WiFi.localIP()); Serial.println("/");
}

void initWiFiAPMode() {
  WiFi.softAP(ssid.c_str() );
  delay(100); // hack need to Wait 100 ms for AP_START...
  WiFi.softAPConfig(IPAddress(192, 168, 1, 2),
              IPAddress(192, 168, 1, 2),
              IPAddress(255, 255, 255, 0)); 
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
}

void setup() 
{
  Serial.begin(115200);

  Serial.print("Set softAPConfig "); Serial.println(ssid);

#ifdef APMODE
  initWiFiAPMode();
#else // STATION MODE
  initWiFiStationMode();
#endif
  
  h.begin();
  Serial.println("HTTP server started");

  initChs();
  mcp6s21.begin();
  Serial.println("i2s started");

  pinMode(21,OUTPUT);
  // function generator handlers
  h.attachHandler("/testfq?val=",handleTestFq);
  h.attachHandler("/duty?val=",handleDuty);
  h.attachHandler("/sinescale?val=",handleSineScale);
  h.attachHandler("/sineoffset?val=",handleSineOffset);
  h.attachHandler("/sineprecision?val=",handleSinePrecision);
  h.attachHandler("/sineOn",handleSine);
  h.attachHandler("/squareOn",handleSquare);
  h.attachHandler("/waveOff",handleWaveOff);

  // oscope handlers
  h.attachHandler("/minmaxADC",handleADCchars);
  h.attachHandler("/ch?val=",handleChannel);
  h.attachHandler("/chOn?val=",handleRun);
  h.attachHandler("/speedup",handleSpeedup);
  h.attachHandler("/speeddown",handleSpeeddown);
  h.attachHandler("/voltup",handleVoltup);
  h.attachHandler("/voltdown",handleVoltdown);
  h.attachHandler("/resolution?val=",handleResolution);
  h.attachHandler("/mcpgain?val=",handleMCPgain);
  h.attachHandler("/risingEdge?val=",handleEdge);
  h.attachHandler("/trigV?val=",handleTrigV);
  h.attachHandler("/trigCh?val=",handleTrigCh);
  h.attachHandler("/single",handleSingle);
  h.attachHandler("/localWiFi?val=",handleLocalWiFi);

  // webpage handlers
  h.attachHandler("/up",handleUpdate);
  h.attachHandler("/favicon.ico",handleFavicon);
  h.attachHandler("/ ",handleRoot);
}

void loop()
{ 
  static uint32_t lastmicros = micros();
  uint32_t us;

  us = micros();
  if (us-lastmicros > 5000){  
    lastmicros = us;
    h.serve();    
  }
  
  updateDMAbuffer();
  delayMicroseconds(20);
//  yield();
}
