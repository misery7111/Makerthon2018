#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#define MQTT_USER "client"
#define MQTT_PASS "password"
#define TEAM_ID "2"

#define MAP_WIDTH 10
#define MAP_HEIGHT 4
#define TARGET_NUM 5
// Update these with values suitable for your network.
const char* ssid = "Makerthon 1";
const char* password = "makerthon";
const char* mqtt_server = "192.168.43.215";

/*
 * '0' - Forward
 * '1' - Turn Right
 * '2' - Turn Left
 * '5' - End of a series of commands
*/
String gDemoResult = "";

// Rows and Column tables
char gRows[MAP_HEIGHT + 1][MAP_WIDTH];
char gCols[MAP_HEIGHT][MAP_WIDTH + 1];
// Number of Cars
int gCarNum = 0;

int gDataStatus = 0;
char gTargetsCoord[TARGET_NUM][2];

#include "find_way.h"

WiFiClient espClient;
PubSubClient client(espClient);

void setup()
{
  // Initialize the Motor Shield, Serial and Wifi
  Serial.begin(115200);
  setup_wifi();

  // Configure mqtt client
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop()
{
  demoLoop(mainProcess);
}

void setup_wifi()
{
  memset(gRows, -1, sizeof(gRows));
  memset(gCols, -1, sizeof(gCols));
  memset(gTargetsCoord, 0, sizeof(gTargetsCoord));

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length)
{
  if (gDataStatus == 0)
  {
    if (length == MAP_WIDTH * MAP_HEIGHT * 2 + MAP_WIDTH + MAP_HEIGHT)
    {
      gDataStatus = 1;
      Serial.println("Received map!");
      receiveMap(payload);
    }
    else if (length == MAP_WIDTH * MAP_HEIGHT * 2 + MAP_WIDTH + MAP_HEIGHT + TARGET_NUM * 2)
    {
      gDataStatus = 2;
      Serial.println("Received map & targets' coords");
      receiveMap(payload);
      memcpy(gTargetsCoord, payload + MAP_WIDTH * MAP_HEIGHT * 2 + MAP_WIDTH + MAP_HEIGHT, TARGET_NUM * 2);
      gCarNum = 2;
    }
    else if (length == MAP_WIDTH * MAP_HEIGHT * 2 + MAP_WIDTH + MAP_HEIGHT + TARGET_NUM * 2 + 1)
    {
      gDataStatus = 3;
      Serial.println("Received map & targets' coords & car num");
      receiveMap(payload);
      memcpy(gTargetsCoord, payload + MAP_WIDTH * MAP_HEIGHT * 2 + MAP_WIDTH + MAP_HEIGHT, TARGET_NUM * 2);
      gCarNum = payload[MAP_WIDTH * MAP_HEIGHT * 2 + MAP_WIDTH + MAP_HEIGHT + TARGET_NUM * 2];
    }
  }
}

void reconnect()
{
  // Loop until we're reconnected
  while (!client.connected())
  {
    Serial.print("Attempting MQTT connection...\n");
    // Create a random client ID
    String clientId = TEAM_ID;
    if (client.connect(clientId.c_str(), MQTT_USER, MQTT_PASS))
    {
      Serial.println("connected");
      client.subscribe("Data_Sub");
    }
    else
    {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void demoLoop(void (*analyzer)() )
{
  // Connect to mqtt server
  if (!client.connected())
  {
    reconnect();
  }

  client.loop();

  // Check data status
  if (gDataStatus)
  {
    // Check map
    if (mapCheck() == true)
    {  
      client.publish("Feedback", strCombine(TEAM_ID, " received").c_str());
      Serial.println("Map received successfully!");
    }
    else
    {
      client.publish("Feedback", strCombine(TEAM_ID, "error1").c_str());
      Serial.println("Error = Receiving map!");
      gDataStatus = 0;
      return;
    }

    //Processing
    analyzer();

    //client.publish("Feedback", strCombine(strCombine(TEAM_ID, "done ",gDemoResult).c_str()));
    Serial.println("Done! Do nothing until reset!\n");
    while (1)
    {
      yield();
    }
  }
}

void mainProcess()
{
  /*
  for(int i = 0;i<MAP_HEIGHT+1;i++)
  {
    for(int j = 0;j<MAP_WIDTH;j++)
    {
      Serial.print(gRows[i][j]);
    }
    Serial.println();
  }
Serial.println("------------------------------------------");
  for(int i = 0;i<MAP_HEIGHT+1;i++)
  {
    for(int j = 0;j<MAP_WIDTH;j++)
    {
      Serial.print(gCols[i][j]);
    }
    Serial.println();
  }
  Serial.println();
  for(int i =0;i<TARGET_NUM;i++)
  {
    Serial.println(int(gTargetsCoord[i][0]));
    Serial.println(int(gTargetsCoord[i][1]));
  }
  */
    int mangGiaTri[Sum_point];
    int n;
    toaDo xe1(1,1);xe1.huong = 'd',xe1.diChuyen = "";
    toaDo xe2(5,1);xe2.huong = 'f';xe2.diChuyen = "";
    toaDo nha[TARGET_NUM];
    for(int i = 0;i<TARGET_NUM;i++)
    {
        nha[i].x = int(gTargetsCoord[i][0])+1;
        nha[i].y = int(gTargetsCoord[i][1])+1;
    }
    /*
    for(int i = 0;i<TARGET_NUM;i++)
    {
        Serial.println(nha[i].x);
        Serial.println(nha[i].y);
    }
    */
    
    for(int i = 0;i<TARGET_NUM;i++)
    {
        if((0<=tinhViTri(nha[i])) && (tinhViTri(nha[i])<=55))
        {
            find_DuongDi(mangGiaTri,n,xe1,nha[i]);
            conTrol(xe1,mangGiaTri,n);
        }

        if((34<=tinhViTri(nha[i]))&& (tinhViTri(nha[i])<=55))
        {
            find_DuongDi(mangGiaTri,n,xe1,nha[i]);
            conTrol(xe2,mangGiaTri,n);
        }
        
        
    }
    
  gDemoResult = xe1.diChuyen + "5" + xe2.diChuyen + "5";
  Serial.println(gDemoResult.c_str());
  delay(5000);
}

bool mapCheck()
{
  for (int i = 0; i < MAP_HEIGHT + 1; i++)
  {
    for (int j = 0; j < MAP_WIDTH; j++)
    {
      if (gRows[i][j] > '3' || gRows[i][j] < '0')
      {
        return false;
      }
    }
  }

  for (int i = 0; i < MAP_HEIGHT; i++)
  {
    for (int j = 0; j < MAP_WIDTH + 1; j++)
    {
      if (gCols[i][j] > '3' || gCols[i][j] < '0')
      {
        return false;
      }
    }
  }

  return  true;
}

std::string strCombine(const std::string first, const std::string second)
{
  std::string result(first);
  result.append(second);
  return result;
}

void receiveMap(const byte *payload)
{
  int index = 0;
  for (int i = 0; i < MAP_HEIGHT + 1; i++)
  {
    for (int j = 0; j < MAP_WIDTH; j++)
    {
      gRows[i][j] = payload[index++];
    }
  }

  for (int i = 0; i < MAP_HEIGHT; i++)
  {
    for (int j = 0; j < MAP_WIDTH + 1; j++)
    {
      gCols[i][j] = payload[index++];
    }
  }
}
