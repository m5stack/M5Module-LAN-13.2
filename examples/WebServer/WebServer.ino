
/**
 * @file WebServer.ino
 * @author SeanKwok (shaoxiang@m5stack.com)
 * @brief webserver example
 * @version 0.1
 * @date 2023-10-26
 *
 *
 * @Hardwares: M5Core/Core2/CoreS3 + LAN Module 13.2
 * @Platform Version: Arduino M5Stack Board Manager v2.0.7
 * @Dependent Library:
 * M5_Ethernet: https://github.com/m5stack/M5-Ethernet
 * M5Unified: https://github.com/m5stack/M5Unified
 * M5GFX: https://github.com/m5stack/M5GFX
 */

#include <M5Unified.h>
#include <M5GFX.h>
#include <SPI.h>
#include <M5Module_LAN.h>

#define THEME_COLOR 0x0760

uint8_t cs_pin;
uint8_t rst_pin;
uint8_t int_pin;

byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0x89};
IPAddress ip(192, 168, 1, 177);

M5Module_LAN LAN;
EthernetServer server(80);

int screen_height;
int screen_width;

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

    // start the server
    server.begin();
    Serial.print("server is at ");
    Serial.println(Ethernet.localIP());

    M5.Display.drawString("server is at:", screen_width / 2,
                          screen_height / 2 - 40);
    M5.Display.fillSmoothRoundRect(2, screen_height / 2 - 20, screen_width - 4,
                                   40, 4, THEME_COLOR);
    M5.Display.drawString(Ethernet.localIP().toString().c_str(),
                          screen_width / 2, screen_height / 2);
}

void loop() {
    // listen for incoming clients
    EthernetClient client = server.available();
    if (client) {
        Serial.println("new client");
        // an http request ends with a blank line
        boolean currentLineIsBlank = true;
        while (client.connected()) {
            if (client.available()) {
                char c = client.read();
                Serial.write(c);
                // if you've gotten to the end of the line (received a newline
                // character) and the line is blank, the http request has ended,
                // so you can send a reply
                if (c == '\n' && currentLineIsBlank) {
                    // send a standard http response header
                    client.println("HTTP/1.1 200 OK");
                    client.println("Content-Type: text/html");
                    client.println(
                        "Connection: close");  // the connection will be closed
                                               // after completion of the
                                               // response
                    client.println("Refresh: 5");  // refresh the page
                                                   // automatically every 5 sec
                    client.println();
                    client.println("<!DOCTYPE HTML>");
                    client.println("<html>");
                    client.print("<h2>Hello M5Stack LAN Module!</h2>");
                    client.println("</html>");
                    break;
                }
                if (c == '\n') {
                    // you're starting a new line
                    currentLineIsBlank = true;
                } else if (c != '\r') {
                    // you've gotten a character on the current line
                    currentLineIsBlank = false;
                }
            }
        }
        // give the web browser time to receive the data
        delay(1);
        // close the connection:
        client.stop();
        Serial.println("client disconnected");
    }
}
