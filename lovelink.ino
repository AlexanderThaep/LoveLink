#include <Scheduler.h>
#include <Task.h>
#include <LeanTask.h>
#include <SSD1306_I2C_A.h>
#include <SSD1306_Graphics.h>
#include <ESP8266WiFi.h>
#include <espnow.h>
// #include <FS.h>
// #include <LittleFS.h>

// 0x08, 0x3A, 0x8D, 0xE3, 0x3A, 0x94 //Base
// 0x08, 0x3A, 0x8D, 0xE3, 0x45, 0x31

#define SCL 5 //D1
#define SDA 4 //D2
#define BUTTON 12 //D5
#define LOW_READ 14 //D6

#define MAX_WIDTH 128
#define MAX_HEIGHT 64
#define BLUE_OFFSET 18 // Offset on Y to get into Blue area from Yellow

typedef struct msg {
    uint8_t mode;
    uint8_t data;
} msg;

// uint8_t broadcastAddress[] = {0x08, 0x3A, 0x8D, 0xE3, 0x3A, 0x94}; //Base
uint8_t broadcastAddress[] = {0x08, 0x3A, 0x8D, 0xE3, 0x45, 0x31};
uint8_t counter = 0;
uint8_t mode = -1;
msg incomingMessage;
msg outgoingMessage;
// const char *ssid = "Venus";
// const char *password = "password";

SSD1306 display;
// WiFiServer server(80);
// IPAddress local_IP(192, 168, 1, 1);
// IPAddress gateway(192, 168, 1, 1);
// IPAddress subnet(255, 255, 255, 0);

void OnDataRecv(uint8_t * mac, uint8_t *incomingData, uint8_t len) {
  counter++;
  memcpy(&incomingMessage, incomingData, sizeof(incomingMessage));
  mode = incomingMessage.mode;
}

IRAM_ATTR void ping() {
  volatile static unsigned long last_interrupt_time = 0;
  unsigned long interrupt_time = millis();

  if (interrupt_time - last_interrupt_time > 50UL)  // ignores interupts for 50milliseconds
  {
    outgoingMessage.mode = 0;
    esp_now_send(broadcastAddress, (uint8_t*) &outgoingMessage, sizeof(outgoingMessage));
  }
  last_interrupt_time = interrupt_time;
}

// void listDir(const char *dirname) {
//   display.clear();
//   display.setCursor(0, 0);
//   display.print("Listing directory: ");
//   display.print(dirname);

//   Dir root = LittleFS.openDir(dirname);

//   while (root.next()) {
    
//     File file = root.openFile("r");
//     display.print("  FILE: ");
//     display.print(root.fileName());
//     display.print("  SIZE: ");
//     display.print(file.size());
//   }

//   display.updateDisplay();
// }

void taskBlock(long ms) {
  delay(ms);
}

class displayTask : public LeanTask {
  private:
  protected:
  void loop() {
    display.clear();
    #ifdef DEBUG
      display.setCursor(0, 0);
      display.println(counter);
      display.println(mode);
    #endif
    display.updateDisplay();
    if (mode == 1) {
      long rssi = WiFi.RSSI();
      display.clear();
      display.setCursor(0, 0);
      display.println("Your partner needs help!");
      display.print("Proximity: ");
      display.print(rssi);
      display.updateDisplay();
    } else if (mode == 0) {
      mode = -1;
      long rand = 0;
      rand = random(0,12);
      switch (rand) {
        case 0:
          gen64x64(display, mochipeach1);
          break;
        case 1:
          genCatpet(display);
          break;
        case 2:
          genSleepycat(display);
          break;
        case 3:
          gen64x64(display, peachCat1);
          break;
        case 4:
          gen64x64(display, rocket1);
          display.setupScrollH(RIGHT, 0, 7, FRAMES_5);
          display.startScroll();
          taskBlock(4000);
          display.stopScroll();
          break;
        case 5:
          gen128x64(display, peachCat2);
          break;
        default:
          genBasicText(display);
          break;
      }
    }
    delay(1000);
  }
} display_task;

// class serverTask : public LeanTask {
//   private:
//   protected:
//   void loop() {
//     WiFiClient client = server.available();   // Listen for incoming clients

//     if (client) {                             // If a new client connects,
//       String currentLine = "";                // make a String to hold incoming data from the client
//       while (client.connected()) { // loop while the client's connected
//         if (client.available()) {             // if there's bytes to read from the client,
//           char c = client.read();             // read a byte, then
//           header += c;
//           if (c == '\n') {                    // if the byte is a newline character
//             // if the current line is blank, you got two newline characters in a row.
//             // that's the end of the client HTTP request, so send a response:
//             if (currentLine.length() == 0) {
//               // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
//               // and a content-type so the client knows what's coming, then a blank line:
//               client.println("HTTP/1.1 200 OK");
//               client.println("Content-type:text/html");
//               client.println("Connection: close");
//               client.println();
//             } else { // if you got a newline, then clear currentLine
//               currentLine = "";
//             }
//           } else if (c != '\r') {  // if you got anything else but a carriage return character,
//             currentLine += c;      // add it to the end of the currentLine
//           }
//         }
//       }
//       header = "";
//       // Close the connection
//       client.stop();
//     }
//   }
// } server_task;

void setup() {
  Wire.begin(SDA, SCL);
  Wire.setClock(400000L);
  display.begin(0x3C);
  randomSeed(analogRead(0));
  // if(!LittleFS.begin()){
  //   display.println("LittleFS Mount Failed");
  //   display.updateDisplay();
  //   LittleFS.format();
  //   delay(1000);
  //   return;
  // }
  pinMode(LOW_READ, INPUT_PULLUP);
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

  if (esp_now_init() != 0) {
    display.setCursor(0, 0);
    display.println("ESPNOW init error!");
    display.updateDisplay();
    return;
  }

  esp_now_set_self_role(ESP_NOW_ROLE_COMBO);
  esp_now_add_peer(broadcastAddress, ESP_NOW_ROLE_COMBO, 1, NULL, 0);
  esp_now_register_recv_cb(OnDataRecv);
  attachInterrupt(digitalPinToInterrupt(BUTTON), ping, HIGH);

  #ifdef DEBUG
    display.clear();
    display.setCursor(0, 0);
    display.println("ESPNOW init success!");
    display.println(WiFi.macAddress());
    display.println(esp_now_is_peer_exist(broadcastAddress));
    display.updateDisplay();
    delay(2000);
  #endif

  if (digitalRead(LOW_READ) == LOW) {
    // display.clear();
    // WiFi.softAP(ssid, password);
    // server.begin();
    // display.println(WiFi.softAPIP().toString());
    // display.println(ssid);
    // display.println(password);
    // display.updateDisplay();
    // delay(1000);

    // Scheduler.start(&server_task);
    // Scheduler.begin();
    // return;
    for (;;) {
      long rssi = WiFi.RSSI();
      display.clear();
      display.setCursor(0, 0);
      display.println("Do not panic yet.");
      display.println("Help is on the way");
      display.print("Proximity: ");
      display.print(rssi);
      display.updateDisplay();
      outgoingMessage.mode = 1;
      esp_now_send(broadcastAddress, (uint8_t*) &outgoingMessage, sizeof(outgoingMessage));
      delay(1000);
    }
  }

  genSplash(display);

  Scheduler.start(&display_task);
  Scheduler.begin();
}

// void test_sequence() {
//   display.println("Hello World!");
  
//   display.updateDisplay();
//   delay(2000);
//   display.clear();

//   for (int i = 0; i < MAX_WIDTH; i++) {
//     for (int j = BLUE_OFFSET; j < MAX_HEIGHT; j++) {
//       display.drawPixel(i, j, true);
//     }
//   }

//   display.updateDisplay();
//   delay(2000);

//   display.clear();
//   display.setCursor(0, 0);
//   display.println(WiFi.macAddress());
//   display.updateDisplay();

//   delay(500);   
//   display.clear();
//   display.setCursor(0, 0);
//   display.print("This message should be too long to fit on the screen at one time");
//   display.updateDisplay();
// }

void loop() {
  //For our purposes. no loop()
}