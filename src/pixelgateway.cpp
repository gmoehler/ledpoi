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
                POI_RECEIVING_DATA,     // 3
                NUM_POI_STATES};        // only used for enum size

Verbosity logVerbose = QUIET; // CHATTY, QUIET or MUTE

const int DATA_PIN = 23; // was 18 Avoid using any of the strapping pins on the ESP32
const int LED_PIN = 2;

const int connTimeout=20;     // client connection timeout in secs
const int maxLEDLevel = 200;  // restrict max LED brightness due to protocol

// WiFi credentials (as defined in WiFiCredentials.h)
extern const char* WIFI_SSID;
extern const char* WIFI_PASS;

WiFiServer server(1110);
WiFiClient client;
IPAddress clientIP;

PoiProgramRunner runner(logVerbose);

PoiState poiState = POI_INIT;
PoiState nextPoiState = poiState;

hw_timer_t *timer0;
int TCPtimeoutCt=0;   // interrupt ms count
uint32_t lastSignalTime = 0; // time when last wifi signal was received
char cmd[7];          // command read from poi
char c;
int cmdIndex=0;
bool last_was_imgdata = false;

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
  // printf cannot be used within interrupt
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

  if (logVerbose != MUTE){
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(WIFI_SSID);
  }
  // Set WiFi to station mode and disconnect from an AP if it was previously connected
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);

  bool connectedToWifi=false;
  WiFi.config(myIP,gateway,subnet);
   while (!connectedToWifi){
     WiFi.begin(WIFI_SSID, WIFI_PASS);
     if (logVerbose != MUTE){
       Serial.print("Connecting...");
     }

     while (WiFi.status() != WL_CONNECTED) {
       // Check to see if connecting failed.
       // This is due to incorrect credentials
       delay(500);
       if (logVerbose != MUTE){
         Serial.print(".");
       }
     }
     if (WiFi.status() == WL_CONNECT_FAILED) {
       if (logVerbose != MUTE){
        Serial.println("Connection Failed. Retrying...");
       }
       blink(1);
     }
     else {
       blink(10);
       connectedToWifi=1;
       if (logVerbose != MUTE) {
         Serial.println("Connected.");
         printWifiStatus();
       }
     }
   }

 //  printWifiStatus();
   digitalWrite(LED_PIN, LOW); // Turn off LED
   server.begin();    // important

   nextPoiState = POI_CLIENT_CONNECTING;
}

void resetTimeout(){
  lastSignalTime = millis();
}

bool reachedTimeout(){
  return (millis() - lastSignalTime > connTimeout * 1000);
}

// connect to a client if available
void client_connect(){
  client = server.available();

  if (client.connected()){
    if (logVerbose != MUTE) {
      printf("Client connected.\n" );
    }
    resetTimeout();
    nextPoiState = POI_RECEIVING_DATA;
  }
  else {
    // slow down a bit
    delay(100);
  }
}

void client_disconnect(){
  client.stop();
  if (logVerbose != MUTE){
    Serial.println("Connection closed.");
  }
}

void setup()
{
  //  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(LED_PIN, OUTPUT);
  //  blink(5);
  delay(500);
  Serial.begin(115200);
  if (logVerbose != MUTE)  {
    Serial.println();
    Serial.println("Starting...");
  }

  // init runner
  runner.setup();

  // init LEDs
  if(ws2812_init(DATA_PIN, LED_WS2812B)&& logVerbose != MUTE){
    Serial.println("LED Pixel init error");
  }
  #if DEBUG_WS2812_DRIVER
  dumpDebugBuffer(-2, ws2812_debugBuffer);
  #endif

  runner.displayOff();
  #if DEBUG_WS2812_DRIVER
  dumpDebugBuffer(-1, ws2812_debugBuffer);
  #endif
  if (logVerbose != MUTE){
    Serial.println("Init LEDs complete");
  }
  blink(2);

  // init timer
  timer_init();
}

void print_cmd(){
  printf("CMD: %d %d %d %d %d %d\n", cmd[0], cmd[1], cmd[2], cmd[3], cmd[4], cmd[5]);
}

void realize_cmd(){

  switch(cmd[0]){
    case 254:
    switch (cmd[1]){  // setAction
      case 0:
      //runner.showCurrent();
      break;

      case 1:
      runner.showFrame(cmd[2],cmd[3]);
      break;

      case 2:  // black with options
      if (cmd[2]==0) runner.displayOff();  // keep BlackSofort-Pixel
      else runner.fadeToBlack();           // fadeToBlack
      break;

      case 3:
      //runner.startProg(cmd[2],cmd[3],cmd[4],cmd[5]);
      break;

      case 4:
      //runner.pauseProg();
      break;

      case 5:
      //runner.resetProg((PoiProgram) cmd[2]);
      break;

      case 6:
      //runner.saveProg();
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
      if (logVerbose != MUTE) {
        Serial.println("Connection close command received.");
      }
      client_disconnect();
      nextPoiState = POI_CLIENT_CONNECTING;
      return;

      case 11:
      // keep alive signal
      if (logVerbose != MUTE) {
        Serial.print("*");
      }
      break;

      default:
        if (logVerbose != MUTE) {
          printf("Protocoll Error: Unknown command received: " );
          print_cmd();
        }
      break;
    };  // end setAction
    break;

    case 253:  // define programs
      // set unset commands bytes to 0
      for (int i=cmdIndex+1; i<6; i++){
        cmd[i] = 0;
      }
      runner.addToProgram(cmd);
    break;

    case 252: // directly play scene
    timer_disable();
    runner.playScene(cmd[1],cmd[2],cmd[3],cmd[4],cmd[5]);
    if (logVerbose != MUTE) {
      printf("Setting timer interval to %d ms\n", runner.getDelay());
    }
    timer_set_interval( runner.getDelay() );
    timer_enable();
    break;

    // 0...200
    default:
    rgbVal pixel = makeRGBVal(cmd[3],cmd[4],cmd[5]);
    runner.setPixel(cmd[1],cmd[2],cmd[0], pixel);
    break;
  }

}

bool protocol_is_data(){
  return (cmd[0] < 200);
}

bool protocol_is_sendalive(){
  return (cmd[0] == 254 && cmd[1] == 11);
}

void protocoll_clean_cmd(){
  cmdIndex=0;
  for (int ix=0;ix<6;ix++) cmd[ix]=0;
}

bool protocoll_cmd_complete(){
  return (cmdIndex >= 6);
}

// wait for data and handle incomming data
void protocoll_receive_data(){
  // data available
  if (client.available()){
    char c = client.read();

    // start byte detected
    if (c== 255) {
      if (logVerbose == CHATTY){
          printf("Start byte detected.\n");
      }
      protocoll_clean_cmd();
      resetTimeout();
    }

    else if (cmdIndex > 5){
      if (logVerbose != MUTE) {
        Serial.println("Protocol Error. More than 6 bytes transmitted.");
      }
    }

    // command
    else {
      cmd[cmdIndex++]=c;
    }
  }

  // no data - disconnect after timeout
  else if (reachedTimeout()){
      client_disconnect();
      nextPoiState = POI_CLIENT_CONNECTING;
  }

  // no longer connected
  else if (!client.connected()){
    client_disconnect(); // required?
    nextPoiState = POI_CLIENT_CONNECTING;
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
    if (logVerbose != MUTE){
      printf("State changed: %d -> %d\n", (poiState), (nextPoiState));
    }

    switch(poiState){
      case POI_INIT:
      break;

      case POI_NETWORK_SEARCH:
      break;

      case POI_CLIENT_CONNECTING:
      break;

      case POI_RECEIVING_DATA:
        // switch off led if we leave this state
        digitalWrite(LED_PIN,LOW);
      break;


      default:
      break;
    }
  }

  // update state
  // need to do this *here* since following functions may set nextPoiState
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
      if (logVerbose != MUTE) {
        printf("Waiting for client...\n");
      }
    }
    client_connect();
    break;

    case POI_RECEIVING_DATA:
    if (state_changed){
      digitalWrite(LED_PIN,HIGH);
    }
    protocoll_receive_data();
    if (protocoll_cmd_complete()){
      if (logVerbose != MUTE){
        if (logVerbose == CHATTY || ( !protocol_is_data() && !protocol_is_sendalive() )){
          print_cmd();
        }
      }

      if (protocol_is_data()){
        // only print once
        if (logVerbose != MUTE && !last_was_imgdata){
          printf("Reading image data... \n");
        }
        last_was_imgdata = true;
      }
      else {
        last_was_imgdata = false;
      }

      // carry out and clean command
      realize_cmd();
      protocoll_clean_cmd();
    }
    break;

    default:
    break;
  }

  runner.loop();
}
