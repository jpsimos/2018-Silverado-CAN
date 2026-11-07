// Author Jacob A. Psimos

#include <FlexCAN_T4.h>
#include "Semaphore.h"
#include "Datum.h"

using namespace semaphore;

#define XCEIVER_SB 2

FlexCAN_T4<CAN1, RX_SIZE_256, TX_SIZE_16> can1;
FlexCAN_T4<CAN2, RX_SIZE_256, TX_SIZE_16> can2;
Datum datum;
int8_t loggingBusses = 0;

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(XCEIVER_SB, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  digitalWrite(XCEIVER_SB, LOW);
  Serial.begin(115200);
  can1.begin();
  can2.begin();
  can1.setBaudRate(500000);
  can1.setMaxMB(16);
  can1.enableFIFO();
  can1.enableFIFOInterrupt();
  //can1.enableLoopBack(true);
  can1.onReceive(can1Receive);
  can2.setBaudRate(33333);
  can2.setMaxMB(16);
  can2.enableFIFO();
  can2.enableFIFOInterrupt();
  //can2.enableLoopBack(true);
  can2.onReceive(can2Receive);
}

void loop() {
  can1.events();
  can2.events();
  datum.loop();
  serial();
}

void can1Receive(const CAN_message_t &msg){
  datum.processMessage(msg);
  if(loggingBusses & 1){
    printMsg(msg);
  }
}

void can2Receive(const CAN_message_t &msg){
  if(loggingBusses & 2){
    printMsg(msg);
  }
}

void serial(){
  char line[60];
  int len = readline(line, 60);
  if(len > 0){
    if(line[0] == 'l'){
      int8_t logWhichBusses = 0;
      if(line[1] != '0') logWhichBusses |= 1;
      if(line[2] != '0') logWhichBusses |= 2;
      loggingBusses = logWhichBusses;
      digitalWrite(LED_BUILTIN, logWhichBusses > 0);
    }
  }
}

int readline(char *out, const int maximum){
  int len = 0;
  bool gotline = false;
  if(maximum > 0){
    while(Serial.available() && len < maximum && !gotline){
      char c = Serial.read();
      if(c == '\r' || c == '\n'){
        gotline = true;
        c = '\0';
      }
      out[len++] = c;
      if(c == '\0') break;
    }
    out[len] = '\0';
  }
  return out[0] == '\0' && gotline ? 0 : len;
}

void printMsg(const CAN_message_t &msg){
  const char *msgFormat = "{\"bus\": %u, \"id\": %u, \"len\": %u, \"r\": %u, \"e\": %u, \"o\": %u, \"data\": [%u, %u, %u, %u, %u, %u, %u, %u]}";
  char buffer[150];
  snprintf(buffer, 150, msgFormat, 
     msg.bus, msg.id, msg.len, msg.flags.remote, msg.flags.extended, msg.flags.overrun, 
     msg.buf[0], msg.buf[1], msg.buf[2], msg.buf[3], msg.buf[4], msg.buf[5], msg.buf[6], msg.buf[7]);
  Serial.println(buffer);
}

void simu(){
  static uint32_t last = 0;
  if(!last){
    last = millis() + 7;
  }else if(last <= millis()){
    const uint32_t ids[10] = { 0x100, 0x101, 0x105, 0x4f0, 0x7e0, 0x10aabbcc, 0x12aabbcc, 0x15aabbcc, 0x19aabbcc};
    CAN_message_t msg;
    msg.id = ids[random(0, 10)] + random(0, 14);
    msg.flags.extended = msg.id > 0x7e0;
    msg.len = 8;
    for(int i = 0; i < 8; i++){
      msg.buf[i] = (uint8_t)random(0, 256);
    }
    if(msg.flags.extended){
      can1.write(msg);
    }else{
      can2.write(msg);
    }
    last = 0;
  }
}
