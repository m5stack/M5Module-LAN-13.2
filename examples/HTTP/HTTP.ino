/**
 * @file HTTP.ino
 * @author SeanKwok (shaoxiang@m5stack.com)
 * @brief HTTP example
 * @version 0.1
 * @date 2023-10-26
 *
 *
 * @Hardwares: M5Core/Core2/CoreS3 + LAN Module 13.2
 * @Platform Version: Arduino M5Stack Board Manager v2.0.7
 * @Dependent Library:
 * M5_Ethernet: https://github.com/m5stack/M5-Ethernet
 * ArduinoHttpClient: https://github.com/arduino-libraries/ArduinoHttpClient
 * M5Unified: https://github.com/m5stack/M5Unified
 * M5GFX: https://github.com/m5stack/M5GFX
 */

#include <M5Unified.h>
#include <M5GFX.h>
#include <SPI.h>
#include <M5Module_LAN.h>
#include <ArduinoHttpClient.h>

#define THEME_COLOR 0x0760

#define SERVER "httpbin.org"

uint8_t cs_pin;
uint8_t rst_pin;
uint8_t int_pin;

int screen_height;
int screen_width;

byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0x89};

M5Module_LAN LAN;

M5Canvas canvas(&M5.Display);

EthernetClient ethClient;

HttpClient client = HttpClient(ethClient, SERVER);

void setup() {
    M5.begin();
    M5.Display.begin();
    M5.Display.setTextColor(WHITE);
    M5.Display.setTextDatum(middle_center);
    M5.Display.setTextFont(&fonts::FreeSansBoldOblique18pt7b);
    screen_height = M5.Display.height();
    screen_width  = M5.Display.width();

    M5.Display.fillSmoothRoundRect(2, 2, screen_width - 4, 36, 4, THEME_COLOR);
    M5.Display.drawString("Wait For Ethernet", screen_width / 2, 22);

    M5.Display.drawRoundRect(2, 40, screen_width - 4, 192, 4, THEME_COLOR);

    canvas.createSprite(280, 160);
    canvas.setTextColor(WHITE);
    canvas.setTextFont(&fonts::FreeSansBoldOblique9pt7b);
    canvas.setTextScroll(true);

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
}

void loop() {
    M5.Display.fillSmoothRoundRect(2, 2, screen_width - 4, 36, 4, THEME_COLOR);
    M5.Display.drawString("HTTP GET", screen_width / 2, 22);

    Serial.println("making GET request");
    canvas.println("making GET request");
    canvas.pushSprite(20, 55);

    client.get("/get");
    // read the status code and body of the response
    int statusCode  = client.responseStatusCode();
    String response = client.responseBody();

    Serial.print("Status code: ");
    Serial.println(statusCode);
    Serial.print("Response: ");
    Serial.println(response);
    Serial.println("Wait five seconds");

    canvas.print("Status code: ");
    canvas.println(statusCode);
    canvas.print("Response: ");
    canvas.println(response);
    canvas.pushSprite(20, 55);

    delay(5000);

    M5.Display.fillSmoothRoundRect(2, 2, screen_width - 4, 36, 4, THEME_COLOR);
    M5.Display.drawString("HTTP POST", screen_width / 2, 22);

    Serial.println("making POST request");
    canvas.println("making POST request");
    canvas.pushSprite(20, 55);

    String contentType = "application/x-www-form-urlencoded";
    String postData    = "name=Alice&age=12";

    client.post("/post", contentType, postData);

    // read the status code and body of the response
    statusCode = client.responseStatusCode();
    response   = client.responseBody();

    Serial.print("Status code: ");
    Serial.println(statusCode);
    Serial.print("Response: ");
    Serial.println(response);

    canvas.print("Status code: ");
    canvas.println(statusCode);
    canvas.print("Response: ");
    canvas.println(response);
    canvas.pushSprite(20, 55);
    Serial.println("Wait five seconds");
    delay(5000);
}
