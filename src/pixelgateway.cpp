/*
07.03.2017: Server funktioniert.
Komisch: Vivaldi fragt IMMMER zusätzlich das favicon ab (Mozilla nur bei der ersten Verbindung
Vivaldi öffnet nach der eigentlichen Anfrage noch eine TCP Verbindung, die aber nichts sendet, sondern nur die Verbindung offen hält.
Das freezed den Server, wenn man nicht einen einfachen Timeout implementiert (TCPtimeoutCt)

*/

#include <Arduino.h>
#include <ws2812.h>
#include <WiFi.h>
#include "WiFiCredentials.h"
#include "PoiProgramRunner.h"

enum PoiState { POI_INIT,               // 0
                POI_NETWORK_SEARCH,     // 1
                POI_CLIENT_CONNECTING,  // 2
                POI_AWAITING_DATA,      // 3
                POI_RECEIVING_DATA,     // 4
                NUM_POI_STATES};        // only used for enum size

const int DATA_PIN = 23; // was 18 Avoid using any of the strapping pins on the ESP32
const int LED_PIN = 2;

const int connTimeout=20;     // client connection timeout in secs
bool muteLog = false;         // mute most verbose logs

// WiFi credentials (as defined in WiFiCredentials.h)
extern const char* WIFI_SSID;
extern const char* WIFI_PASS;

WiFiServer server(1110);
WiFiClient client;
IPAddress clientIP;

PoiProgramRunner runner;

PoiState poiState = POI_INIT;
PoiState nextPoiState = poiState;
OperationMode mode = ASYNC;

hw_timer_t *timer0;
int TCPtimeoutCt=0;   // interrupt ms count
char cmd[7];          // command read from poi
char c;
int cmdIndex=0;

void blink(int m){
  for (int n=0;n<m;n++){
    digitalWrite(LED_PIN,HIGH);
    delay(50);
    digitalWrite(LED_PIN,LOW);
    delay(50);
  }
}

// Interrupt at interval determined by program
void IRAM_ATTR timer0_intr()
{
  //Serial.print("Interrupt at ");
  //Serial.println(millis());

  // do what needs to be done for the current program
  runner.onInterrupt();
}


void timer_init(){
  timer0 = timerBegin(3, 80, true);  // divider 80 = 1MHz
  timerAttachInterrupt(timer0, &timer0_intr, true); // attach timer0_inter, edge type interrupt
}

void timer_set_interval(uint32_t intervalMs){
  timerAlarmWrite(timer0, 1000 * intervalMs, true); // Alarm every timer0_int milli secs, auto-reload
}

void timer_enable(){
  timerAlarmEnable(timer0);
}

void timer_disable(){
  //timerDetachInterrupt(timer0);
  timerAlarmDisable(timer0);
}

void timer_stop(){
  timerEnd(timer0);
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

// synchronous method connecting to wifi
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

   nextPoiState = POI_CLIENT_CONNECTING;
}

void client_connect(){
  client = server.available();

  if (client.connected()){
    printf("Client connected.\n" );
    nextPoiState = POI_AWAITING_DATA;
  }
  else {
    delay(100);
  }
}

void resetTimeout(){
  TCPtimeoutCt=0;
}

void client_disconnect(){
  client.stop();
  Serial.println("Connection closed.");
  resetTimeout();
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

  runner.setup();

  // init LEDs
  if(ws2812_init(DATA_PIN, LED_WS2812B)) {
    Serial.println("LED Pixel init error");
  }
  #if DEBUG_WS2812_DRIVER
  dumpDebugBuffer(-2, ws2812_debugBuffer);
  #endif

  runner.displayOff();
  #if DEBUG_WS2812_DRIVER
  dumpDebugBuffer(-1, ws2812_debugBuffer);
  #endif
  Serial.println("Init LEDs complete");

  runner.displayOff();
  blink(2);

  timer_init();
}

void realize_cmd(){

  switch(cmd[0]){
    case 254:
    switch (cmd[1]){  // setAction
      case 0:  // showCurrent
      runner.showCurrent();
      break;

      case 1:  // showStatic
      runner.showFrame(cmd[2],cmd[3]);
      break;

      case 2:  // black mit Optionen
      if (cmd[2]==0) runner.displayOff();  // keep BlackSofort-Pixel
      else runner.fadeToBlack();           // fadeToBlack
      break;

      case 3:
//       runner.startProg(cmd[2],cmd[3],cmd[4],cmd[5]);
      break;

      case 4:
      runner.pauseProg();
      break;

      case 5:
      runner.resetProg((PoiProgram) cmd[2]);
      break;

      case 6:
      runner.saveProg();
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

      case 10:
      Serial.println("Connection close command received.");
      client_disconnect();
      nextPoiState = POI_CLIENT_CONNECTING;
      return;

      case 11:
      // keep alive signal
      if (!muteLog) {
        printf("Staying alive!\n");
      }
      break;

      default:
      break;
    };  // end setAction
    break;

    case 253:  // define programs
      // orginally this was split between 2 words: first one with program id, second with 6 commands
      runner.defineProgram((PoiProgram) cmd[2], cmd[3], cmd[4], cmd[5]);
    break;

    case 252:
    if (mode == ASYNC){
      timer_disable();
    }
    runner.playScene(cmd[1],cmd[2],cmd[3],cmd[4],cmd[5], mode);
    if (mode == ASYNC){
      uint32_t interval = runner.getDelay();
      printf("Setting timer interval to %d ms\n", interval);
      timer_set_interval(interval);
      timer_enable();
    }
    break;

     // 0...200
    default:
    if (!muteLog){
      printf("Reading data... \n");
    }
    muteLog = true;
    rgbVal pixel = makeRGBVal(cmd[3],cmd[4],cmd[5]);
    runner.setPixel(cmd[1],cmd[2],cmd[0], pixel);
    break;
  }

  //default: wait for next command
  nextPoiState = POI_AWAITING_DATA;
}

void protocoll_detect_start(){
  if (!client.connected()){
      client_disconnect();
      nextPoiState = POI_CLIENT_CONNECTING;
      return;
  }

  if (!client.available()){
    // no data available

    // if we have not received data for connTimeout seconds, lets disconnect
    // to avoid clients that do not talk to us
    TCPtimeoutCt++;
    muteLog = false;
    if (TCPtimeoutCt>connTimeout*1000){
      // close the client connection
      client_disconnect();
      nextPoiState = POI_CLIENT_CONNECTING;
    }
    else {
      delay(1);
    }
    return;
  }

  // data available
  char c = client.read();
  if (c== 255) {
    if (!muteLog){
        printf("Start byte detected.\n");
    }
    resetTimeout();
    // received start byte -> continue
    nextPoiState = POI_RECEIVING_DATA;
  }
  else {
      printf("Warning! Unknown data found while waiting for start byte: %d.\n", cmd[0]);
  }
}

void protocoll_clean_cmd(){
  cmdIndex=0;
  for (int ix=0;ix<7;ix++) cmd[ix]=0;
}

bool protocoll_cmd_complete(){
  return (cmdIndex >= 6);
}

void print_cmd(){
  printf("CMD: %d %d %d %d %d %d\n", cmd[0], cmd[1], cmd[2], cmd[3], cmd[4], cmd[5]);
}

void protocoll_receive_data(){
  if (!client.available()){
    nextPoiState = POI_CLIENT_CONNECTING;
    return;
  }
  char c = client.read();
  if (cmdIndex >  6){
    Serial.println("Error! More than 6 bytes transmitted.");
    return;
  }
  cmd[cmdIndex++]=c;

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
    if (!muteLog){
      printf("State changed: %d -> %d\n", (poiState), (nextPoiState));
    }

    switch(poiState){
      case POI_INIT:
      break;

      case POI_NETWORK_SEARCH:
      break;

      case POI_CLIENT_CONNECTING:
      break;

      case POI_AWAITING_DATA:
      // switch off led if we leave this state
      if (nextPoiState != POI_RECEIVING_DATA)
        digitalWrite(LED_PIN,LOW);
      break;

      case POI_RECEIVING_DATA:
      // switch off led if we leave this state
      if (nextPoiState != POI_AWAITING_DATA)
        digitalWrite(LED_PIN,LOW);
      break;


      default:
      break;
    }
  }

  // update state
  // need to do this here since these functions may set nextPoiState
  int prevPoiState = poiState;
  poiState = nextPoiState;

  // entry and state actions of state machine
  switch (poiState){

    case POI_INIT:
      // proceed to next state
      nextPoiState = POI_NETWORK_SEARCH;
    break;

    case POI_NETWORK_SEARCH:
    if (state_changed){
      digitalWrite(LED_PIN,LOW);
    }
    wifi_connect();
    break;

    case POI_CLIENT_CONNECTING:
    if (state_changed){
      resetTimeout();
      printf("Waiting for client...\n");
    }
    client_connect();
    break;

    case POI_AWAITING_DATA:
    if (state_changed && prevPoiState != POI_RECEIVING_DATA){
      digitalWrite(LED_PIN,HIGH);
    }
    protocoll_detect_start();
    break;

    case POI_RECEIVING_DATA:
    if (state_changed){
      if (prevPoiState != POI_AWAITING_DATA) {
        digitalWrite(LED_PIN,HIGH);
      }
      protocoll_clean_cmd();
    }
    protocoll_receive_data();
    if (protocoll_cmd_complete()){
      if (!muteLog){
        print_cmd();
      }
      // carry out command
      realize_cmd();
    }
    break;

    default:
    break;
  }

  runner.loop();

}
