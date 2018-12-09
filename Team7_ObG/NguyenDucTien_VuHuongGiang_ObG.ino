// chia nhieu doan
// xe uu tien: xe uu tien di truoc, xe con lai di den diem tiep theo
// mang luu vi tri xoa sau moi lan di qua
// vi tri dau, xet 5 lan, uu tien 
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

const char* ssid = "Makerthon 1";
const char* password = "makerthon";
const char* mqtt_server = "192.168.43.215";
  
#define MQTT_USER "client"
#define MQTT_PASS "password"
#define TEAM_ID "7"

#define MAP_WIDTH 10
#define MAP_HEIGHT 4
#define TARGET_NUM 5
//Ver2
#define V 55
//Ver1
#define INT_MAX 5
#define MAXNODE 55


/*
 * '0' - Forward
 * '1' - Turn Right
 * '2' - Turn Left
 * '5' - End of a series of commands
*/
char *gDemoResult;

// Rows and Column tables
char gRows[MAP_HEIGHT + 1][MAP_WIDTH];//gRows[5][10]
char gCols[MAP_HEIGHT][MAP_WIDTH + 1];//gCols[4][11]
// Number of Cars
byte gCarNum = 0;//xe do(0;0) xe xanh (0;4)

byte gDataStatus = 0;
char gTargetsCoord[TARGET_NUM][2];

WiFiClient espClient;
PubSubClient client(espClient);

int convertAtoi(char c)
{
  switch (c)
  {
    case '0': return 0; break;
    case '1': return 1; break;
    case '2': return 2; break;
    case '3': return 3; break;
    default: break;
  }
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define MAX  60
int truoc[MAX], chuaxet[MAX], queue[MAX];//mảng đánh dấu.
int path[MAX]; //đường đi
int amountOfNodePassed = 0;
int s = 0;//đỉnh đầu.
int t = 0;//đỉnh cuối.
int angleCar = 270;
char driver1[60];
void Init(void)
{
  // freopen("lienth.IN", "r",stdin); 
  // cin>>n; 
  // cout<<"So dinh do thi: "<<n<<endl; 
  // cin>>s>>t;
  // cout<<"Dinh dau:"<<s<<endl;
  // cout<<"Dinh cuoi:"<<t<<endl;
  // for(int i=1; i<=n;i++)
  // { 
  //   for(int j=1; j<=n;j++)
  //   {
  //     cin>>A[i][j]; 
  //   }
  // } 
  for(int i=0; i < V;i++)
  { 
    chuaxet[i]=1; 
    truoc[i]=0;
    queue[i] = 0;
  }
  amountOfNodePassed = 0;
 // fclose(stdin);
}

void Result(int s, int t)
{ 
  if(truoc[t]==0)
  {
    // cout<<"Khong co duong di tu "<<s<< " den "<<t;
    // Serial.print("Khong co duong di tu ");
    // Serial.print(s);
    // Serial.print(" den ");
    // Serial.print(t);
    return; 
  } 
  // cout<<"Duong di tu "<<s<<" den "<<t<<" la: "; 
  // Serial.print("Duong di tu ");
  // Serial.print(s);
  // Serial.print(" den ");
  // Serial.print(t);
  // Serial.print(" la: ");

  int j = t;
  // cout<<t<<"<=";
  // Serial.print(t); 
  // Serial.print("<=");
  path[amountOfNodePassed] = t; amountOfNodePassed++;
  while(truoc[j]!= s)
  { 
    // cout<<truoc[j]<<"<=";
    // Serial.print(truoc[j]);
    // Serial.print("<=");
    path[amountOfNodePassed] = truoc[j]; amountOfNodePassed++;
    j=truoc[j]; 
  } 
  // cout<<s;
  path[amountOfNodePassed] = s; amountOfNodePassed++;
}

/* Breadth First Search */
void BFS(int s, int A[V][V])
{
  int dauQ, cuoiQ, u;
  dauQ=1;
  cuoiQ=1;//khởi tạo queue.
  queue[dauQ] = s;
  chuaxet[s] = 0; //thêm đỉnh đầu vào queue.
  while (dauQ <= cuoiQ)
  {//queue chưa rỗng.
    u = queue[dauQ];//lấy đỉnh u trong queue.
    dauQ=dauQ+1; 
    for (int p = 1; p <= V ; p++)
    {
      if(A[u][p] && chuaxet[p])
      { 
        cuoiQ = cuoiQ+1;
        queue[cuoiQ]=p; 
        chuaxet[p]=0;
        truoc[p]=u;
      } 
    }
  } 
}
int getNodePos(int row, int col)
{
  return (row * 11) + col; 
}
int turn(int angle, int mark)
{
  if(angle == mark) return 0;
  if((angle - mark) == 180) return 3;
  else
  {
    int temp1; temp1 = angle - 90;
    if(temp1 == (-90)) temp1 = 270;
    
    if((temp1 - mark) == 0) return 1;//Right
    
    int temp2; temp2 = angle + 90;
    if(temp2 == 360)
    {
      temp2 = 0;
    }
    if((temp2 - mark) == 0) return 2;//Left
  }
}
void logMap()
{
  // for (int i = 0; i < 20; ++i)
  // {
  //   driver1[i] = '';
  // }
  /*
  Serial.println("Rows");
  for (int i = 0; i < MAP_HEIGHT + 1; ++i)
  {
    for (int j = 0; j < MAP_WIDTH; ++j)
    {
      Serial.print(gRows[i][j]);
      Serial.print(" ");
    }
    Serial.println();
  }
  
  Serial.println();
  Serial.println("Cols");
  for (int i = 0; i < MAP_HEIGHT; ++i)
  {
    for (int j = 0; j < MAP_WIDTH + 1; ++j)
    {
      Serial.print(gCols[i][j]);
      Serial.print(" ");
    }
    Serial.println();
  }
  
  Serial.println();
  Serial.println("House");
  for (int i = 0; i < TARGET_NUM; ++i)
  {
    for (int j = 0; j < 2; ++j)
    {
      Serial.print(gTargetsCoord[i][j], DEC);
      Serial.print(" ");
    }
    Serial.println();
  }
  */
  Serial.println("Mapping.................................");
  int mapPing[55][55];
  for (int i = 0; i < 55; ++i)
  {
    for (int j = 0; j < 55; ++j)
    {
      mapPing[i][j] = 0;
    }
  }
  int p, q;
  for(int i = 0; i < 5; i++)
  {
    for (int j = 0; j < 10; j++)
    {
      p = i * 11 + j;
      q = p + 1;
      mapPing[p][q] = convertAtoi(gRows[i][j]);
      mapPing[q][p] = mapPing[p][q];
    }
  }
  for(int i; i < 4; i++)
  {
    for (int j = 0; j < 11; j++)
    {
      p = i * 11 + j;
      q = p + 11;
      mapPing[p][q] = convertAtoi(gCols[i][j]);
      mapPing[q][p] = mapPing[p][q];
    }
  }

  /*
  for (int i = 0; i < 55; ++i)
  {
    for (int j = 0; j < 55; ++j)
    {
      //Serial.print(mapPing[i][j]);
      //Serial.print("  ");
      printf("%d", mapPing[i][j]);
      printf("   ");
    }
    //Serial.println();
    printf("\n");
  }
  */
  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //strcat()
  for (int i = 0; i < TARGET_NUM; i++)
  {
    Init();
    t = getNodePos(gTargetsCoord[i][0], gTargetsCoord[i][1]);
    BFS(s, mapPing);
    Result(s, t);

    Serial.print("s: ");
    Serial.println(s);

    Serial.print("t: ");
    Serial.println(t);

    Serial.println("In path:");
    for (int i = 0; i < amountOfNodePassed; i++)
    {
      Serial.print(path[i]);
      Serial.print(" ");
    }
    Serial.println();

    int ccc;
    for(int i = amountOfNodePassed - 2; i >= 0; i--)
    {
      // xac dinh huong di
      Serial.print("Path[i]: " );
      Serial.println(path[i]);
      if (s < path[i])// ben phai
      {
        if( (path[i] - s) == 1)//cung hang
        {
          ccc = turn(angleCar, 0);//xoay
          angleCar = 0;
        }
        else//hang duoi
        {
          ccc = turn(angleCar, 270);//xoay
          angleCar = 270;
        }
      }
      else //ben trai
      {
        if( (s - path[i]) == 1)//cung hang
        {
          ccc = turn(angleCar, 180);//xoay
          angleCar = 180;
        }
        else// hang tren
        {
          ccc = turn(angleCar, 90);//xoay
          angleCar = 90;
        }
      }
      switch(ccc)
      {
        case 1: strcat(driver1, "1"); break;
        case 2: strcat(driver1, "2"); break;
        case 3: strcat(driver1, "22"); break;
        case 0: break;
      }
      strcat(driver1, "0");
      s = path[i];
    }
  }
  Serial.println(driver1);
  for (int i = 0; i < 60; ++i)
  {
    Serial.print(driver1[i]);
    Serial.print(" ");
  }
  Serial.println();

  gDemoResult = driver1;

}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
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
  delay(1);
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

  // Update these with values suitable for your network.

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
    Serial.println(gDemoResult);
    client.publish("Feedback", strCombine(strCombine(TEAM_ID, "done "), gDemoResult).c_str());
    Serial.println("Done! Do nothing until reset!\n");
    while (1)
    {
      yield();
    }
  }
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void mainProcess()
{
  //delay(100);
  logMap();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool mapCheck()
{
  for (byte i = 0; i < MAP_HEIGHT + 1; i++)
  {
    for (byte j = 0; j < MAP_WIDTH; j++)
    {
      if (gRows[i][j] > '3' || gRows[i][j] < '0')
      {
        return false;
      }
    }
  }

  for (byte i = 0; i < MAP_HEIGHT; i++)
  {
    for (byte j = 0; j < MAP_WIDTH + 1; j++)
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
  for (byte i = 0; i < MAP_HEIGHT + 1; i++)
  {
    for (byte j = 0; j < MAP_WIDTH; j++)
    {
      gRows[i][j] = payload[index++];
    }
  }

  for (byte i = 0; i < MAP_HEIGHT; i++)
  {
    for (byte j = 0; j < MAP_WIDTH + 1; j++)
    {
      gCols[i][j] = payload[index++];
    }
  }
}
