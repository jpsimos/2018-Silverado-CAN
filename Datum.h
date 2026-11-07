#ifndef _DATUM__
#define _DATUM__

//Author Jacob Psimos 2020

#include <FlexCAN_T4.h>
#include "Semaphore.h"

using namespace semaphore;

typedef struct Variables{
  //Vehicle information
  uint8_t m_rpm;
  uint8_t m_mph;
  uint8_t m_kmh;
  uint8_t m_throttle;
  uint8_t m_throttlepos;
  bool m_leftblinker;
  bool m_signalingleft;
  bool m_rightblinker;
  bool m_signalingright;
  bool m_brakelight;
  bool m_foglights;
  uint16_t m_steeringwheelpos;
  uint8_t m_year;
  uint8_t m_month;
  uint8_t m_day;
  uint8_t m_hour;
  uint8_t m_minute;
  uint8_t m_second;
  uint8_t m_driverclimatetemp;       //0x00 is low, 0x20 is highest, 0x21 is lowest
  uint8_t m_passengerclimatetemp;    //0x00 is low, 0x20 is highest, 0x21 is lowest
  bool m_reardefroster;

  //Statistics
  uint64_t s_numrx;
  uint64_t s_numparsed;
};

class Datum{
  public:
    Datum();
    void loop();
    void processMessage(const CAN_message_t &msg);
    Variables vars;
  private:
    Semaphore semaphore;
};

Datum::Datum(){
  memset(&vars, 0, sizeof(vars));
}

void Datum::loop(){
  
}

void Datum::processMessage(const CAN_message_t &msg){
  if(msg.id == 0x10210040){
    uint16_t m_mph_r = ((((uint16_t)msg.buf[0]) << 8) + (uint16_t)msg.buf[1]);
    vars.m_mph = (m_mph_r % 100 >= 50) ? (m_mph_r + 1) / 100 : m_mph_r / 100;
    //uint8_t m_mph2kmh = (uint32_t)vars.m_mph * 16093 / 10000;
  }else if(msg.id == 0x102CA040){
    vars.m_rpm = (((uint16_t)msg.buf[2] << 8) + (uint16_t)msg.buf[3]) / 4;
    vars.m_throttle = msg.buf[4];
    vars.m_throttlepos = msg.buf[5];
  }else if(msg.id == 0x10240040){
    vars.m_steeringwheelpos = ((uint16_t)msg.buf[4] << 8) | (uint16_t)msg.buf[5];
  }else if(msg.buf == 0x102A8097){
    vars.m_year = msg.buf[0];
    vars.m_month = msg.buf[1];
    vars.m_day = msg.buf[2];
    vars.m_hour = msg.buf[4];
    vars.m_minute = msg.buf[5];
    vars.m_second = msg.buf[6];
  }else if(msg.id == 0x102AC097){
    vars.m_kmh = msg.buf[4];
    //uint8_t m_kmh2mph = (uint32_t)vars.m_kmh * 10000 / 16093;
  }else if(msg.id == 0x1020C040){
    vars.m_leftblinker = msg.buf[4] & 0x20;
    vars.m_signalingleft = msg.buf[0] & 0x20;
    vars.m_rightblinker = msg.buf[4] & 0x02;
    vars.m_signalingright = msg.buf[0] & 0x40;
    vars.m_brakelight = msg.buf[1] & 0x80;
    vars.m_foglights = (msg.buf[0] & 0x80) && (msg.buf[1] & 0x42 == 0x42);
  }else if(msg.id == 0x10B02099){
    if(msg.buf[3] == 0x03){
      m_driverclimatetemp = msg.buf[7];
    }else if(msg.buf[3] == 0x04){
      m_passengerclimatetemp = msg.buf[7];
    }else if(msg.buf[3] == 0x11){
      m_reardefroster = msg.buf[7];
    }
  }else{
    vars.s_numparsed += 1;
  }
  vars.s_numrx += 1;
}

#endif
