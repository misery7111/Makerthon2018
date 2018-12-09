#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#define MQTT_USER "client"
#define MQTT_PASS "password"
#define TEAM_ID "6"

#define MAP_WIDTH 10
#define MAP_HEIGHT 4
#define TARGET_NUM 5
#define N 55

// Update these with values suitable for your network.
const char* ssid = "uet makerthon 2018";
const char* password = "makerthon2018";
const char* mqtt_server = "192.168.1.102";
const int a = 1;
const int b = 20;
//int count = 0;

/*
 * '0' - Forward
 * '1' - Turn Right
 * '2' - Turn Left
 * '5' - End of a series of commands
*/
char gDemoResult[] = "2120000005102100210120015";

// Rows and Column tables
char gRows[MAP_HEIGHT + 1][MAP_WIDTH];
char gCols[MAP_HEIGHT][MAP_WIDTH + 1];
byte dijkstra[55][55];
// Number of Cars
int gCarNum = 0;

int gDataStatus = 0;
char gTargetsCoord[TARGET_NUM][2];

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

    client.publish("Feedback", strCombine(strCombine(TEAM_ID, "done "), gDemoResult).c_str());
    Serial.println("Done! Do nothing until reset!\n");
    while (1)
    {
      yield();
    }
  }
}

byte charToNum(char num){
  switch (num){
    case '0': return 0; break;
    case '1': return 1; break;
    case '2': return 2; break;
    case '3': return 3; break;
  }
}

// thuat toan dijkstra
int dijkstraAlgorithm(byte G[N][N], int n, int a, int b, int P[]) {

    /* Do mang tinh tu G[0][0] nen can giam vi tri
     di 1 don vi de tinh toan cho phu hop*/
    a--;
    b--;
    int Len[100], S[100];
    int sum = 0;            // gia tri vo cung

    // tinh gia tri vo cung (sum)
    for (byte i = 0; i < n; i++) {
        for (byte j = 0; j < n; j++) {
            sum += G[i][j];
        }
    }

    // dat vo cung cho tat ca cap canh khong noi voi nhau
    for (byte i = 0; i < n; i++) {
        for (byte j = 0; j < n; j++) {
            if (i != j && G[i][j] == 0)
                G[i][j] = sum;
        }
    }

    for (byte i = 0; i < n; i++) {
        Len[i] = sum;       // khoi tao do dai tu a toi moi dinh la vo cung
        S[i] = 0;           // danh sach cac diem da xet
        P[i] = a;           // dat diem bat dau cua moi diem la a
    }

    Len[a] = 0;             // dat do dai tu a -> a la 0

    int i;

    // tim duong di ngan nhat tu 1 dinh den moi dinh khac thi thay bang vong for:
    //for (int k = 0; k < n; k++)
    while (S[b] == 0) {                 // trong khi diem cuoi chua duoc xet
        for (i = 0; i < n; i++)          // tim 1 vi tri ma khong phai la vo cung
            if (!S[i] && Len[i] < sum)
                break;

        // i >=n tuc la duyet het cac dinh ma khong the tim thay dinh b -> thoat
        if (i >= n) {
            return 0;
        }

        for (byte j = 0; j < n; j++) {    // tim diem co vi tri ma do dai la min
            if (!S[j] && Len[i] > Len[j])
                i = j;
        }

        S[i] = 1;                       // cho i vao danh sach xet roi

        for (byte j = 0; j < n; j++) {    // tinh lai do dai cua cac diem chua xet
            if (!S[j] && Len[i] + G[i][j] < Len[j]) {
                Len[j] = Len[i] + G[i][j];      // thay doi len
                P[j] = i;                       // danh dau diem truoc no
            }
        }
    }
    return Len[b];
}

//  truy vet duong di
char *back(int a, int b, int *P, int n, char *store) {

    /* Do mang tinh tu G[0][0] nen can giam vi tri
     di 1 don vi de tinh toan cho phu hop*/
    a--;
    b--;
    int i = b;
    int point[n];   // danh sach cac dinh cua duong di
    int count = 0;

    /* Do ta dang tinh toan tu dinh 0 nen
     muon hien thi tu dinh 1 thi can dung i + 1 de phu hop */

    point[count++] = i + 1;
    while (i != a) {
        i = P[i];
        point[count++] = i + 1;
    }

    
    strcpy(store, "");
    char temp[N];
    
    //int a = point[count - 2] - point[count - 1]; 
    for (i = count - 3; i > 0; i--) {
        //Serial.print(point[i]);
        //Serial.print("|");
        if(((point[i -1] - point[i]) == 1) || ((point[i -1] - point[i]) == -1)) {
          strcat(temp, "0");
        }
        else if((point[i -1] - point[i]) == 11) {
          strcat(temp, "1");
        }
        else if((point[i-1] - point[i]) == -11) {
          strcat(temp, "2");
        }
        else{
          strcat(temp, "5");
        }
    }
    strcat(temp, "5");
    return temp;
    
}



int distanceTwoPoint(byte dijktra[N][N], int n, int a, int b, int P[]) {
  return dijkstraAlgorithm(dijkstra, n, a, b, P);
}


void mainProcess()
{ 
  // initial array of 0
  for (byte i=0;i<55;i++){
    for (byte j=0;j<55;j++){
      dijkstra[i][j]=0;
      yield();
    }
  }
  // fill in array

  for (byte i = 0; i < MAP_HEIGHT + 1; i++)
  {
    for (int j = 0; j < MAP_WIDTH; j++)
    {
        int a = i*(MAP_WIDTH + 1)+j;
        int b = a + 1;
        dijkstra[a][b] = charToNum(gRows[i][j]);
        dijkstra[b][a] = charToNum(gRows[i][j]);
        
    }
  }

  for (byte i = 0; i < MAP_HEIGHT; i++)
  {
    for (byte j = 0; j < MAP_WIDTH + 1; j++)
    {
        byte a = i * (MAP_WIDTH + 1)+j;
        byte b = a + MAP_WIDTH + 1;
        dijkstra[a][b]=charToNum(gCols[i][j]);
        dijkstra[b][a]=charToNum(gCols[i][j]);
    }
  }

  int n = 55;
  int len;
    //cin >> a >> b;
  char store[100];
  int P[N];

  len = dijkstraAlgorithm(dijkstra, N, a, b, P);

  if (len > 0) {
     //back(a, b, P, N, store);
     //Serial.print(len);
     
     strcpy(gDemoResult, back(a, b, P, N, store)) ;
     Serial.println(gDemoResult);
     
     //Serial.println(distanceTwoPoint(dijkstra, a, b, n, P));
  } else {
    Serial.print("Khong co");
    Serial.print(len);
     //printf("Eo co");
  }

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
