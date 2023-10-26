#ifndef _M5_MODULE_LAN_H
#define _M5_MODULE_LAN_H

#include <Arduino.h>
#include "M5_Ethernet.h"

class M5Module_LAN : public EthernetClass {
   private:
    uint8_t reset_pin;

   public:
    void setResetPin(uint8_t pin);
    void reset();
};

#endif
