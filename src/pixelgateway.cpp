/*
07.03.2017: Server funktioniert.
Komisch: Vivaldi fragt IMMMER zusätzlich das favicon ab (Mozilla nur bei der ersten Verbindung
Vivaldi öffnet nach der eigentlichen Anfrage noch eine TCP Verbindung, die aber nichts sendet, sondern nur die Verbindung offen hält.
Das freezed den Server, wenn man nicht einen einfachen Timeout implementiert (TCPtimeoutCt)

*/

#include <Arduino.h>
#include <ws2812.h>
#include "WiFi.h"
#include "WiFiCredentials.h"

#define NUM_PIXELS 60
#define NUM_SCENES 1
#define NUM_FRAMES 200

enum PoiState { POI_INIT,           // 0
                POI_NETWORK_SEARCH, // 1
                POI_ACTIVE,         // 2
                NUM_POI_STATES};    // only used for enum size

hw_timer_t *timer0;
uint32_t timer0_int = 0;
const int connTimeout=10;

const int DATA_PIN = 23; // was 18 Avoid using any of the strapping pins on the ESP32

uint8_t MAX_COLOR_VAL = 200; // Limits brightness

rgbVal pixelMap[NUM_PIXELS][NUM_SCENES][NUM_FRAMES];
rgbVal pixels[NUM_PIXELS];

// WiFi credentials (defined in WiFiCredentials.h)
extern const char* WIFI_SSID;
extern const char* WIFI_PASS;

int keyIndex = 0;    // your network key Index number (needed only for WEP)

int status = WL_IDLE_STATUS;
int on;
int TCPtimeoutCt=0;

WiFiServer server(1110);
IPAddress clientIP;

const int LED_PIN = 2;

int cmdIndex=0;
char cmd[7];
char c;

uint8_t progIx=0;
uint8_t progDef[254][5];
uint8_t progCurrIx=0;
uint8_t progLastIx=0;
uint8_t progState=0;

PoiState poiState = POI_INIT;
PoiState nextPoiState = POI_INIT;

void printLine()
{
  Serial.println();
  for (int i=0; i<30; i++)
  Serial.print("-");
  Serial.println();
}

void printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}

void displayOff() {
  for (int i = 0; i < NUM_PIXELS; i++) {
    pixels[i] = makeRGBVal(0, 0, 0);
  }
  ws2812_setColors(NUM_PIXELS, pixels);
  //ws2812_setColors(NUM_PIXELS, pixels);
}

void displayTest() {
  for (int i = 0; i < NUM_PIXELS; i++) {
    pixels[i] = makeRGBVal(0, 33, 0);
  }
  ws2812_setColors(1, pixels);
  //ws2812_setColors(NUM_PIXELS, pixels);
}

void statusIO() {
  pixels[0] = makeRGBVal(0, 33, 0);
  ws2812_setColors(1, pixels);
}

void statusNIO() {
  pixels[0] = makeRGBVal(33, 0, 0);
  ws2812_setColors(1, pixels);
}

void blink(int m){
  for (int n=0;n<m;n++){
    digitalWrite(LED_PIN,HIGH);
    delay(50);
    digitalWrite(LED_PIN,LOW);
    delay(50);
  }
}

void loadPixels(uint8_t scene, uint8_t frame){
  for (int i=0;i<NUM_PIXELS;i++)
    pixels[i]=pixelMap[i][constrain(scene,0,NUM_SCENES-1)][frame];
}

void playScene(uint8_t scene, uint8_t frameStart,uint8_t frameEnd, uint8_t speed, uint8_t loops){
  for (uint8_t runner=0;runner<loops;runner++){
    for (int i=frameStart;i<frameEnd;i++){
      loadPixels(scene,i);
      ws2812_setColors(NUM_PIXELS, pixels);  // LEDs updaten
      delay(speed);
    }
  }
}

void fadeToBlack(){  //TODO

}

void startProg(){
  progState=1;
}

void pauseProg(){
  progState=0;
}

void resetProg(uint8_t index){
  progState=0;
  progCurrIx=index;
}

void saveProg(){}  //TODO

void IRAM_ATTR timer0_intr()  // Interrupt im [ms]-Takt
{
  Serial.print("Interrupt at ");
  Serial.println(millis());
  if (poiState){

  }
/*  timer0_int++;
  switch (playState){
    case PSreset:
      PScurrentFrame=PSstartframe;
      break;
    case PSplay:

  } */

}


void timer_init(){
  timer0 = timerBegin(3, 80, true);  // divider 80 = 1MHz
  timerAlarmWrite(timer0, 20000000, true); // Alarm every 1000 µs, auto-reload
}

void timer_start(){
  timerAttachInterrupt(timer0, &timer0_intr, true); // attach timer0_inter, edge type interrupt  (db) timer macht GURU
  timerAlarmEnable(timer0);
}

void timer_stop(){
  timerDetachInterrupt(timer0);
  timerAlarmDisable(timer0);
}

void wifi_connect(){

  IPAddress myIP(192, 168, 1, 127);
  IPAddress gateway(192, 168, 1, 1);
  IPAddress subnet(255, 255, 255, 0);

  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WIFI_SSID);

  // Set WiFi to station mode and disconnect from an AP if it was previously connected
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);

  bool connectedToWifi=false;
  WiFi.config(myIP,gateway,subnet);
   while (!connectedToWifi){
     WiFi.begin(WIFI_SSID, WIFI_PASS);
     Serial.print("Connecting...");

     while (WiFi.status() != WL_CONNECTED) {
       // Check to see if connecting failed.
       // This is due to incorrect credentials
       delay(500);
       Serial.print(".");
     }
     if (WiFi.status() == WL_CONNECT_FAILED) {
       Serial.println("Connection Failed. Retrying...");
       blink(1);
     }
     else {
       blink(10);
       connectedToWifi=1;
       Serial.println("Connected.");
       printWifiStatus();
     }
   }

 //  printWifiStatus();
   digitalWrite(LED_PIN, LOW); // Turn off LED
   server.begin();    // important
}


void setup()
{
  //  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(LED_PIN, OUTPUT);
  //  blink(5);
  delay(500);
  Serial.begin(115200);
  Serial.println();
  Serial.println("Starting...");

  // init LEDs
  if(ws2812_init(DATA_PIN, LED_WS2812B)) {
    Serial.println("LED Pixel init error");
  }
  #if DEBUG_WS2812_DRIVER
  dumpDebugBuffer(-2, ws2812_debugBuffer);
  #endif
//  pixels = (rgbVal*)malloc(sizeof(rgbVal) * NUM_PIXELS*256*256);  //[pixel][scene][frame]
  displayOff();
  #if DEBUG_WS2812_DRIVER
  dumpDebugBuffer(-1, ws2812_debugBuffer);
  #endif
  Serial.println("Init LEDs complete");

  displayOff();
  blink(2);

  timer_init();
}

void receive_data(char c) {
  if (c==255) {
    cmdIndex=0;
    for (int ix=0;ix<7;ix++) cmd[ix]=0;
  }
  else {
    cmd[cmdIndex++]=c;
  }

  if (cmdIndex>=6) {
    switch(cmd[0]){
      case 254:
      switch (cmd[1]){  // setAction
        case 0:  // showCurrent
        ws2812_setColors(NUM_PIXELS, pixels);  // update LEDs
        break;

        case 1:  // showStatic
        loadPixels(cmd[2],cmd[3]);
        ws2812_setColors(NUM_PIXELS, pixels);  // update LEDs
        break;

        case 2:  // black mit Optionen
        if (cmd[2]==0) displayOff();  // keep BlackSofort-Pixel
        else fadeToBlack();           // fadeToBlack
        break;

        case 3:
//        startProg(cmd[2],cmd[3],cmd[4],cmd[5]);
        break;

        case 4:
        pauseProg();
        break;

        case 5:
        resetProg(cmd[2]);
        break;

        case 6:
        saveProg();
        break;

        case 7:
        //savePix(cmd[2],cmd[3]);
        break;

        case 8:
        //setIP(cmd[2],cmd[3],cmd[4],cmd[5]);
        break;

        case 9:
        //setGW(cmd[2],cmd[3],cmd[4],cmd[5]);
        break;

        default:
        break;
      };  // end setAction
      break;

      case 253:  // define programs
      if (cmd[1]==0){
        progDef[progIx][0]=0;
        progIx=0;  // new program follows
      }
      else {
        progDef[progIx][0]=cmd[1];
        progDef[progIx][1]=cmd[2];
        progDef[progIx][2]=cmd[3];
        progDef[progIx][3]=cmd[4];
        progDef[progIx][4]=cmd[5];
        progIx++;
      }
      break;
      case 252: playScene(cmd[1],cmd[2],cmd[3],cmd[4],cmd[5]);
      break;

      default:
//      Serial.print(cmd[1]);
      pixelMap[constrain(cmd[0],0,NUM_PIXELS-1)][constrain(cmd[1],0,NUM_SCENES-1)][constrain(cmd[2],0,NUM_FRAMES-1)]=makeRGBVal(cmd[3],cmd[4],cmd[5]);
      break;
    }
  }
}

void wifi_read_and_act(){
  WiFiClient client = server.available();
  //Serial.println("starting server");
  if (client) {
//    blink(3);
    digitalWrite(LED_PIN,HIGH);
    while (client.connected()) {
      if (client.available() ) {
    	TCPtimeoutCt=0
        char c = client.read();
        receive_data(c);
      }
      else {
        // some conncted clients don't speak...get out of here after some timeout
        TCPtimeoutCt++;
        delay(1);
      }

      if (TCPtimeoutCt>connTimeout*1000){
        // close the connection:
        client.stop();
        TCPtimeoutCt=0;
        Serial.println("client disonnected");
      }
    }
    digitalWrite(LED_PIN,LOW);
  }

  else if (WiFi.status() != WL_CONNECTED){
    timer_stop();
    poiState = POI_NETWORK_SEARCH;
    return;
  }
}

// ===============================================
// ====  LOOP ====================================
// ===============================================

// state machine with entry actions, state actions and exit actions
void loop()
{

  bool state_changed = nextPoiState != poiState;

  // exit actions
  if (state_changed){
    switch(poiState){
      case POI_INIT:
      break;

      case POI_NETWORK_SEARCH:
        timer_stop();
      break;

      case POI_ACTIVE:
      break;

      default:
      break;
    }
  }

  // update state
  poiState = nextPoiState;

  switch (poiState){

    case POI_INIT:
      // proceed to next state
      nextPoiState = POI_NETWORK_SEARCH;
    break;

    case POI_NETWORK_SEARCH:
      wifi_connect();
      nextPoiState = POI_ACTIVE;
    break;

    case POI_ACTIVE:
      // entry action
      if (state_changed){
        timer_start();
      }
      wifi_read_and_act();
    break;

    default:
    break;

  }
}
