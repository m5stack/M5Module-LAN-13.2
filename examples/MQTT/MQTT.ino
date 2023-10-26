/**
 * @file MQTT.ino
 * @author SeanKwok (shaoxiang@m5stack.com)
 * @brief MQTT example
 * @version 0.1
 * @date 2023-10-26
 * 
 * 
 * @Hardwares: M5Core/Core2/CoreS3 + LAN Module 13.2
 * @Platform Version: Arduino M5Stack Board Manager v2.0.7
 * @Dependent Library:
 * M5_Ethernet: https://github.com/m5stack/M5-Ethernet
 * PubSubClient: https://github.com/knolleary/pubsubclient
 * M5Unified: https://github.com/m5stack/M5Unified
 * M5GFX: https://github.com/m5stack/M5GFX
 */

#include <M5Unified.h>
#include <M5GFX.h>
#include <SPI.h>
#include <M5Module_LAN.h>
#include <PubSubClient.h>

#define THEME_COLOR  0x0760
#define PUB_INTERVAL 3000

uint8_t cs_pin;
uint8_t rst_pin;
uint8_t int_pin;

int screen_height;
int screen_width;

byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0x89};

M5Module_LAN LAN;

// Configure the name and password of the connected wifi and your MQTT Serve
const char* mqtt_server = "mqtt.m5stack.com";

EthernetClient ethClient;
PubSubClient client(ethClient);

void callback(char* topic, byte* payload, unsigned int length) {
    Serial.print("Message arrived [");
    Serial.print(topic);
    Serial.print("] ");
    for (int i = 0; i < length; i++) {
        Serial.print((char)payload[i]);
    }
    Serial.println();
}

void reconnect() {
    // Loop until we're reconnected
    while (!client.connected()) {
        Serial.print("Attempting MQTT connection...");
        // Attempt to connect
        if (client.connect("arduinoClient")) {
            M5.Display.fillSmoothRoundRect(2, screen_height / 2 - 20,
                                           screen_width - 4, 40, 4,
                                           THEME_COLOR);
            M5.Display.drawString("MQTT Connected!", screen_width / 2,
                                  screen_height / 2);

            Serial.println("connected");

            // Once connected, publish an announcement...
            client.publish("outTopic", "hello world");
            // ... and resubscribe
            client.subscribe("inTopic");
        } else {
            M5.Display.fillSmoothRoundRect(2, screen_height / 2 - 20,
                                           screen_width - 4, 40, 4,
                                           THEME_COLOR);
            M5.Display.drawString("MQTT Failed...", screen_width / 2,
                                  screen_height / 2);

            Serial.print("failed, rc=");
            Serial.print(client.state());
            Serial.println(" try again in 5 seconds");
            // Wait 5 seconds before retrying
            delay(5000);
        }
    }
}

void setup() {
    M5.begin();
    M5.Display.begin();
    M5.Display.setTextDatum(middle_center);
    M5.Display.setTextColor(WHITE);
    M5.Display.setTextFont(&fonts::FreeSansBoldOblique18pt7b);
    screen_height = M5.Display.height();
    screen_width  = M5.Display.width();
    M5.Display.fillSmoothRoundRect(2, screen_height / 2 - 20, screen_width - 4,
                                   40, 4, THEME_COLOR);
    M5.Display.drawString("Wait For Ethernet", screen_width / 2,
                          screen_height / 2);

    m5::board_t board = M5.getBoard();
    switch (board) {
        case m5::board_t::board_M5Stack: {
            cs_pin  = 5;
            rst_pin = 0;
            int_pin = 35;
        } break;
        case m5::board_t::board_M5StackCore2: {
            cs_pin  = 33;
            rst_pin = 0;
            int_pin = 35;
        } break;
        case m5::board_t::board_M5StackCoreS3: {
            cs_pin  = 1;
            rst_pin = 0;
            int_pin = 10;
        } break;
    }

    SPI.begin(SCK, MISO, MOSI, -1);

    LAN.setResetPin(rst_pin);
    LAN.reset();
    LAN.init(cs_pin);

    while (LAN.begin(mac) != 1) {
        Serial.println("Error getting IP address via DHCP, trying again...");
        delay(2000);
    }

    // Check for Ethernet hardware present
    if (Ethernet.hardwareStatus() == EthernetNoHardware) {
        Serial.println(
            "Ethernet shield was not found.  Sorry, can't run without "
            "hardware. :(");
        while (true) {
            delay(1);  // do nothing, no point running without Ethernet hardware
        }
    }
    if (Ethernet.linkStatus() == LinkOFF) {
        Serial.println("Ethernet cable is not connected.");
    }

    client.setServer(mqtt_server, 1883);
    client.setCallback(callback);
}

long lastTime;

void loop() {
    if (!client.connected()) {
        reconnect();
    }
    client.loop();

    if (millis() - lastTime > PUB_INTERVAL) {
        lastTime = millis();
        client.publish("outTopic", "hello world");
    }
}
