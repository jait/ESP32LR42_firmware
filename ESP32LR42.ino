
#include <Preferences.h>
#include <WiFi.h>

const char ver[] = {"1.4"};

Preferences nvm;
WiFiServer server(80);
WiFiServer tcpServer(0);

IPAddress local_IP(192, 168, 0, 121);
IPAddress gateway(192, 168, 0, 1);
IPAddress subnet(255, 255, 0, 0);
IPAddress primaryDNS(8, 8, 8, 8); //optional
IPAddress secondaryDNS(8, 8, 4, 4); //optional

// hour
#define DEFAULT_RELAY_SHUTOFF_TIME 60*60*1000
// milliseconds
uint32_t relayShutoffTime = DEFAULT_RELAY_SHUTOFF_TIME;

#define Rly1  33
#define Rly2  25
#define Rly3  26
#define Rly4  27

#define Inp1  4
#define Inp2  16

#define Led 23
#define BUFSIZE 50

char buffer[BUFSIZE];
char ssid[BUFSIZE];
char password[BUFSIZE];
char password2[BUFSIZE];
uint AsciiPort;
char MqttServer[BUFSIZE];
char MqttID[BUFSIZE];
uint MqttPort;
char R1Topic[BUFSIZE];    // Relay topics
char R2Topic[BUFSIZE];
char R3Topic[BUFSIZE];
char R4Topic[BUFSIZE];
char R1PubTopic[BUFSIZE];    // Relay topics where state is published
char R2PubTopic[BUFSIZE];
char R3PubTopic[BUFSIZE];
char R4PubTopic[BUFSIZE];
char N1Topic[BUFSIZE];    // Input topics
char N2Topic[BUFSIZE];


char Inputs[8] = {2,2,2,2,2,2,2,2};
char Relays[4] = {-1,-1,-1,-1}; // relay states, initially unknown

void setup()
{
    pinMode(Rly1, OUTPUT);      // set the pin modes
    pinMode(Rly2, OUTPUT);
    pinMode(Rly3, OUTPUT);
    pinMode(Rly4, OUTPUT);
    pinMode(Led, OUTPUT);
    digitalWrite(Rly1, LOW);
    digitalWrite(Rly2, LOW);
    digitalWrite(Rly3, LOW);
    digitalWrite(Rly4, LOW);
    digitalWrite(Led, HIGH);
    pinMode(Inp1, INPUT);
    pinMode(Inp2, INPUT);
    
    Serial.begin(115200);
    delay(10);
    
    relayShutoffTime = nvm.getUInt("shutoffTimer", 0);
    if (relayShutoffTime > 0) {
      relayShutoffTime *= (60*1000); // minutes to milliseconds
    }
    else {
      relayShutoffTime = DEFAULT_RELAY_SHUTOFF_TIME;
    }
    wifi_connect();
}

void loop(){
  modeHttp();
  modeAscii();
  modeMQTT();
  modeRelay();
  serialMonitor();
  if (WiFi.status() != WL_CONNECTED)
    wifi_connect();
}

void wifi_connect(void)
{
    unsigned int x;

    digitalWrite(Led, HIGH);
    Serial.println("");
    nvm.begin("devantech", false);    // Note: Namespace name is limited to 15 chars
    local_IP = nvm.getUInt("IPAddress", 0);
    gateway = nvm.getUInt("GateWay", 0);
    subnet = nvm.getUInt("SubNet", 0);
    primaryDNS = nvm.getUInt("primaryDNS", 0);
    secondaryDNS = nvm.getUInt("secondaryDNS", 0);
    nvm.getString("ssid", ssid, sizeof(ssid)-1);
    nvm.getString("password", password, sizeof(password)-1);
    strcpy(password2, "********");
    AsciiPort = nvm.getUInt("AsciiPort", 17123);
    nvm.getString("MqttServer", MqttServer, BUFSIZE-1);
    nvm.getString("MqttID", MqttID, BUFSIZE-1);
    MqttPort = nvm.getUInt("MqttPort", 0);            // 0 means do not connect, normally should be 1883
    nvm.getString("R1Topic", R1Topic, BUFSIZE-1);    
    nvm.getString("R2Topic", R2Topic, BUFSIZE-1);    
    nvm.getString("R3Topic", R3Topic, BUFSIZE-1);    
    nvm.getString("R4Topic", R4Topic, BUFSIZE-1);
    nvm.getString("R1PubTopic", R1PubTopic, BUFSIZE-1);    
    nvm.getString("R2PubTopic", R2PubTopic, BUFSIZE-1);    
    nvm.getString("R3PubTopic", R3PubTopic, BUFSIZE-1);    
    nvm.getString("R4PubTopic", R4PubTopic, BUFSIZE-1);    
    nvm.getString("N1Topic", N1Topic, BUFSIZE-1);    
    nvm.getString("N2Topic", N2Topic, BUFSIZE-1);   

    // We start by connecting to a WiFi network
    Serial.print("Connecting to ");
    Serial.print(ssid);
    Serial.println("...");

    if(local_IP != 0) {
      if (!WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS)) {
        Serial.println("STA Failed to configure");
      }
    }
    WiFi.mode(WIFI_STA);

    while(WiFi.status() != WL_CONNECTED){
      WiFi.disconnect();
      delay(100);
      WiFi.begin(ssid, password);
      for(x = 0; x < 300; x++){
        delay(10);
        serialMonitor();
      }
    }
    digitalWrite(Led, LOW);
    Serial.println("");
    Serial.println("WiFi connected.");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    server.begin();
    tcpServer.close();
    tcpServer = WiFiServer(AsciiPort);
    tcpServer.begin();
    setupMQTT();
}
