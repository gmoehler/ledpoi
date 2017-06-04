
#include <Arduino.h>
#include <ws2812.h>
#include <WiFi.h>
#include "WiFiCredentials.h"
#include "ledpoi.h"
#include "PoiActionRunner.h"
#include "PoiTimer.h"
#include "OneButton.h"

enum PoiState { POI_INIT,               // 0
                POI_NETWORK_SEARCH,     // 1
                POI_CLIENT_CONNECTING,  // 2
                POI_RECEIVING_DATA,     // 3
                POI_DEMO_MODE,          // 4
                POI_IP_CONFIG,          // 5
                NUM_POI_STATES};        // only used for enum size

LogLevel logLevel = QUIET; // CHATTY, QUIET or MUTE

const int DATA_PIN = 23; // was 18 Avoid using any of the strapping pins on the ESP32
const int LED_PIN = 2;
const int BUTTON_PIN = 0;
OneButton button1(BUTTON_PIN, true);

const int connTimeout=20;     // client connection timeout in secs
const int maxLEDLevel = 200;  // restrict max LED brightness due to protocol

const uint8_t aliveTickModulo = 10;
uint8_t aliveTickCnt = 0;

// WiFi credentials (as defined in WiFiCredentials.h)
extern const char* WIFI_SSID;
extern const char* WIFI_PASS;

WiFiServer server(1110);
WiFiClient client;
IPAddress clientIP;

PoiState poiState =   POI_INIT;
PoiState nextPoiState = poiState;

PoiTimer ptimer(logLevel);
PoiActionRunner runner(ptimer, logLevel);

uint32_t lastSignalTime = 0; // time when last wifi signal was received, for timeout
char cmd [7];                 // command read from server
int cmdIndex=0;              // index into command read from server
char c;
bool loadingImgData = false; // tag to suppress log during image loading

bool startDemoOnReset = false; // demo mode: instantly start with the program after reset

uint8_t ipIncrement = 0; // increment to set the ip
rgbVal ipSetupLed[N_POIS];
uint8_t baseIpAdress[4] = {192, 168, 1, 127};


void blink(int m){
  for (int n=0;n<m;n++){
    digitalWrite(LED_PIN,HIGH);
    delay(50);
    digitalWrite(LED_PIN,LOW);
    delay(50);
  }
}

// Interrupt at interval determined by program
void IRAM_ATTR ptimer_intr()
{
  // printf cannot be used within interrupt
  //Serial.print("Interrupt at ");
  //Serial.println(millis());

  runner.onInterrupt();
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
  uint8_t ip4 = baseIpAdress[3] + ipIncrement;
  printf("My address: %d.%d.%d.%d\n", baseIpAdress[0],baseIpAdress[1],baseIpAdress[2],ip4);
  IPAddress myIP(baseIpAdress[0],baseIpAdress[1],baseIpAdress[2],ip4);
  IPAddress gateway(192, 168, 1, 1);
  IPAddress subnet(255, 255, 255, 0);

  if (logLevel != MUTE){
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
     if (logLevel != MUTE) Serial.print("Connecting...");

     while (WiFi.status() != WL_CONNECTED) {
       // Check to see if connecting failed.
       // This is due to incorrect credentials
       delay(500);
       if (logLevel != MUTE) Serial.print(".");
     }
     if (WiFi.status() == WL_CONNECT_FAILED) {
       if (logLevel != MUTE) Serial.println("Connection Failed. Retrying...");
       blink(1);
     }
     else {
       blink(10);
       connectedToWifi=1;
       if (logLevel != MUTE) {
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
    if (logLevel != MUTE) printf("Client connected.\n" );
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
  if (logLevel != MUTE) Serial.println("Connection closed.");
}

void longPressStop1() {
  if (poiState == POI_IP_CONFIG) {
    ipSetupLed[ipIncrement]= makeRGBVal(0,0,0);
    ws2812_setColors(N_POIS, ipSetupLed);
    nextPoiState = POI_NETWORK_SEARCH;
  }
  else {
    nextPoiState = POI_IP_CONFIG;
    ipSetupLed[ipIncrement]= makeRGBVal(8, 8, 8);
    ws2812_setColors(N_POIS, ipSetupLed);
  }
}

void click1() {
  if (poiState == POI_IP_CONFIG){
    // set back the ip led to black
    ipSetupLed[ipIncrement]= makeRGBVal(0,0,0);
    ipIncrement++;
    if (ipIncrement + 1 > N_POIS){
      ipIncrement = 0;
    }
    // display colored led (first one less bright for each)
    uint8_t b = 64;
    if (ipIncrement %2 == 0){
      b=8;
    }
    rgbVal color = makeRGBVal(b, b, b);
    switch(ipIncrement/2){
      case 1:
      color = makeRGBVal(b, 0, 0);
      break;

      case 2:
      color = makeRGBVal(0, b, 0);
      break;

      case 3:
      color = makeRGBVal(0, 0, b);
      break;

      case 4:
      color = makeRGBVal(b, 0, b);
      break;
    }
    ipSetupLed[ipIncrement]= color;
    ws2812_setColors(N_POIS, ipSetupLed);
    //printf("IP Increment: %d\n", ipIncrement);
  }
}

void setup()
{
  //  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(LED_PIN, OUTPUT);
  //  blink(5);
  delay(500);
  Serial.begin(115200);
  if (logLevel != MUTE)  {
    Serial.println();
    Serial.println("Starting...");
  }

  button1.attachLongPressStop(longPressStop1);
  button1.attachClick(click1);
  // init runner
  runner.setup();

  // init LEDs
  if(ws2812_init(DATA_PIN, LED_WS2812B)){
    Serial.println("LED Pixel init error.");
  }
  #if DEBUG_WS2812_DRIVER
  dumpDebugBuffer(-2, ws2812_debugBuffer);
  #endif

  runner.displayOff();
  #if DEBUG_WS2812_DRIVER
  dumpDebugBuffer(-1, ws2812_debugBuffer);
  #endif
  if (logLevel != MUTE) Serial.println("Init LEDs complete");
  blink(2);

  // init timer
  ptimer.init(ptimer_intr);
}

void print_cmd(){
  printf("CMD: %d %d %d %d %d %d\n", cmd[0], cmd[1], cmd[2], cmd[3], cmd[4], cmd[5]);
}

void realize_cmd(){

  switch(cmd[0]){
    case 254:
    switch (cmd[1]){  // setAction
      case 0:
      runner.showCurrent();
      break;

      case 1:
      runner.showStaticFrame(cmd[2], cmd[3], cmd[4], cmd[5]);
      break;

      case 2:  // (fade to) black
      runner.fadeToBlack(cmd[2], cmd[3]);
      break;

      case 3:
      runner.startProg();
      break;

      case 4:
      runner.pauseProg();
      break;

      case 5:
      runner.continueProg();
      break;

      case 6:
      // TODO: changet this back to jumptoSync
      //runner.jumptoSync(cmd[2]);
      runner.saveScene(cmd[2]);
      break;

      case 7:
      //setIP(cmd[2],cmd[3],cmd[4],cmd[5]);
      break;

      case 8:
      //setGW(cmd[2],cmd[3],cmd[4],cmd[5]);
      break;

      case 9:
      case 10:
      if (logLevel != MUTE) Serial.println("Connection close command received.");
      client_disconnect();
      nextPoiState = POI_CLIENT_CONNECTING;
      break;

      default:
      if (logLevel != MUTE) {
        printf("Protocoll Error: Unknown command received: " );
        print_cmd();
      }
      break;
    };  // end setAction
    break;

    case 253:  // define programs
    runner.addCmdToProgram(cmd);
    break;

    case 252: // directly play scene
    runner.playScene(cmd[1],cmd[2],cmd[3],cmd[4],cmd[5]);
    break;

    // 0...200
    default:
    runner.setPixel(cmd[1], cmd[2], cmd[0], cmd[3], cmd[4], cmd[5]);
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
    //printf("READ: %d\n", c);

    // start byte detected
    if (c== 255) {
      aliveTickCnt++;
      if (logLevel != MUTE && !loadingImgData && (aliveTickCnt % aliveTickModulo) == 0) {
        Serial.print("*");
        aliveTickCnt = 0;
      }
      protocoll_clean_cmd();
      resetTimeout();
    }

    else if (cmdIndex > 5){
      Serial.println("Protocol Error. More than 6 bytes transmitted.");
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
  button1.tick();
  bool state_changed = nextPoiState != poiState;

  // exit actions
  if (state_changed){
    if (logLevel != MUTE) printf("State changed: %d -> %d\n", (poiState), (nextPoiState));

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

      case POI_IP_CONFIG:
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
      if (startDemoOnReset){
        nextPoiState = POI_DEMO_MODE;
      }
      else {
        nextPoiState = POI_NETWORK_SEARCH;
      }
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
      if (logLevel != MUTE) printf("Waiting for client...\n");
    }
    client_connect();
    break;

    case POI_DEMO_MODE:
    printf("  Starting demo...\n" );
    runner.startProg();
    printf("  Demo finished\n" );
    nextPoiState = POI_NETWORK_SEARCH;
    break;

    case POI_IP_CONFIG:

    break;

    case POI_RECEIVING_DATA:
    if (state_changed){
      digitalWrite(LED_PIN,HIGH);
    }
    protocoll_receive_data();
    if (protocoll_cmd_complete()){
      if (logLevel != MUTE){
        if (logLevel == CHATTY || ( !protocol_is_data() && !protocol_is_sendalive() )){
          print_cmd();
        }
      }

      if (protocol_is_data()){
        // only print once
        if (logLevel != MUTE && !loadingImgData){
          printf("Reading image data... \n");
          // currently required since we write directly into image memory
          // TODO: add start command for image loading (with scene id)
          //       which will remove current image from memory
          // TODO: then remove this line again - it does not work anyway ;-)
          runner.clearImageMap();
        }
        loadingImgData = true;
      }
      else {
        loadingImgData = false;
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
