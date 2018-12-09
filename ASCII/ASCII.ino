#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#define MQTT_USER "client"
#define MQTT_PASS "password"
#define TEAM_ID "5"

#define MAP_WIDTH 10
#define MAP_HEIGHT 4
#define TARGET_NUM 5

// Update these with values suitable for your network.
const char* ssid = "Makerthon 2018";
const char* password = "makerthon";
const char* mqtt_server = "192.168.43.215";



/*
 * '0' - Forward
 * '1' - Turn Right
 * '2' - Turn Left
 * '5' - End of a series of commands
*/
char gDemoResult[] = "";
//                    2120000005102100210120015

// Rows and Column tables
char gRows[MAP_HEIGHT + 1][MAP_WIDTH];
char gCols[MAP_HEIGHT][MAP_WIDTH + 1];
// Number of Cars
int gCarNum = 0;

int gDataStatus = 0;
char gTargetsCoord[TARGET_NUM][2];

WiFiClient espClient;
PubSubClient client(espClient);

void receiveMap(const byte*);
bool mapCheck();
std::string strCombine(const std::string first, const std::string second);

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

std::string res("");
int huongxe=1;
void mainProcess()

{
  
  chiamap();
  for(int i = 0; i < res.length(); i++)
  {
    gDemoResult[i] = res[i];
  }


  // delay(5000);
}


void chiamap()
{
  int i,j,a;
  for(i=0;i<5;i++)
  {
    for (j=i+1;j<5;j++)
    {
      if (gTargetsCoord[i][1]>gTargetsCoord[j][1])
      {
        
        a=gTargetsCoord[i][0];
        gTargetsCoord[i][0]=gTargetsCoord[j][0];
        gTargetsCoord[j][0]=a;
        a=gTargetsCoord[i][1];
        gTargetsCoord[i][1]=gTargetsCoord[j][1];
        gTargetsCoord[j][1]=a;
      }
    }
  }
  for(i=0;i<3;i++)
  {
    for(j=i+1;j<3;j++)
    {
      if(gTargetsCoord[i][0]*gTargetsCoord[i][0]+gTargetsCoord[i][1]*gTargetsCoord[i][1]>gTargetsCoord[j][0]*gTargetsCoord[j][0]+gTargetsCoord[j][1]*gTargetsCoord[j][1])
      {
        a=gTargetsCoord[i][0];
        gTargetsCoord[i][0]=gTargetsCoord[j][0];
        gTargetsCoord[j][0]=a;
        a=gTargetsCoord[i][1];
        gTargetsCoord[i][1]=gTargetsCoord[j][1];
        gTargetsCoord[j][1]=a;
      }
    }
  }
  
      if(gTargetsCoord[3][0]*gTargetsCoord[3][0]+gTargetsCoord[3][1]*gTargetsCoord[3][1]>gTargetsCoord[4][0]*gTargetsCoord[4][0]+gTargetsCoord[4][1]*gTargetsCoord[4][1])
      {
        a=gTargetsCoord[3][0];
        gTargetsCoord[3][0]=gTargetsCoord[4][0];
        gTargetsCoord[4][0]=a;
        a=gTargetsCoord[3][1];
        gTargetsCoord[3][1]=gTargetsCoord[4][1];
        gTargetsCoord[4][1]=a;
      }
  
  haidiem(0,0,gTargetsCoord[0][0],gTargetsCoord[0][1]);
  haidiem(gTargetsCoord[0][0],gTargetsCoord[0][1],gTargetsCoord[1][0],gTargetsCoord[1][1]);
  haidiem(gTargetsCoord[1][0],gTargetsCoord[1][1],gTargetsCoord[2][0],gTargetsCoord[2][1]);
  res=res+"5";
  haidiem(0,4,gTargetsCoord[3][0],gTargetsCoord[3][1]);
  haidiem(gTargetsCoord[3][0],gTargetsCoord[3][1],gTargetsCoord[4][0],gTargetsCoord[4][1]);
  res=res+"5";
}

int tri(int a)
{
  if (a >= 0) return a;
  else return -a;
}

void haidiem(int a, int b, int x, int y)
{
  int canh;
  
  int i, j, i1, i2;
  int row = tri(x - a) + 1;
  int col = tri(y - b) + 1;
  //  int** gRows=new int*[10];
  //  for(k=0;k<10;k++) gRows[k]=new int;
  //  int** gCols=new int*[10];
  //  for(k=0;k<10;k++) gCols[k]=new int;
  //  for(i=0;i<10;i++) 
  //  {
  //      for (j=0;j<10;j++) gRows[i][j]=rand()%3+0;
  //      cout<<endl;
  //  }
  //  for(i=0;i<10;i++) 
  //  {
  //      for (j=0;j<10;j++) gCols[i][j]=rand()%3+0;
  //      cout<<endl;
  //  }
  canh = (x - a + 1) * (y - b + 1);    
  int **G = new int*[canh];
  for(int i = 0; i < canh; i++)
    G[i]=new int[canh];

  int** array=new int*[canh];
  for(i = 0; i < canh; i++)
    array[i]=new int[2];
    
  for(i = 0; i < canh; i++)
    for(j = 0; j < canh; j++)
      G[i][j]=0;
  i = 0, j = 0;
  while(1)
  {        
    for(i1 = 0; i1 < col - 1; i1++) 
    {
      if(gCols[j][i1] == 0)
        gCols[j][i1] = 1000;
      G[j * col + i1 + 1][i1 + j * col] = gCols[j][i1];
      G[i1 + j * col][j * col + i1 + 1] = gCols[j][i1];
      if(j * col + i1 + 1 >= canh - 1)  goto a; 
    }
    for(i1 = 0; i1 < col; i1++)
    {
      if(gRows[j][i1]==0)
        gRows[j][i1]=1000; 
      G[j * col + col + i1][i1 + j * col] = gRows[j][i1];
      G[i1 + j * col][j * col + col + i1] = gRows[j][i1];
  //              if((j*col+col+i1)>=canh-1) goto a;
    }
  //      i=i+col;
    j++;
  //      if(j==row) break;
  }
      
  //  for(i=0;i<10;i++) 
  //  {
  //      for (j=0;j<10;j++) cout<<gRows[i][j]<<" ";
  //      cout<<endl;
  //  }
  //  for(i=0;i<10;i++) 
  //  {
  //      for (j=0;j<10;j++) cout<<gCols[i][j]<<" ";
  //      cout<<endl;
  //  }
  //  for(i=0;i<canh;i++)
  //  {
  //      for(j=0;j<canh;j++) cout<<array[i][j]<<" ";
  //      cout<<endl;
  //  }
  a: i1 = 0;    
  for(i = a; i <= x; i++) 
  {
    for(j = b; j <= y; j++) 
    {
      array[i1][0] = i;
      array[i1][1] = j;
      i1++;
    }
    if (i1 == canh)
      break;
  }
  if (y >= b) dijkstra(0, canh,G,array);
  if (y < b) dijkstra(col - 1, canh - col + 1,G,array);
}
void dijkstra(int a, int b,int** G,int** array)
{
  
  int a1, a2; 
  int n,i, sum = 0;
//  int S[n], Len[n], P[n];
  int *S=new int[n];
  int *Len=new int[n];
  int *P=new int[n];
  for(i = 0; i < n; i++)
    for(int j = 0; j < n; j++) 
    {       
      sum += G[i][j];
    }
      // dat vo cung cho tat ca cap canh khong noi voi nhau
  for(int i = 0; i < n; i++) 
  {
    for(int j = 0; j < n; j++) 
    {
      if(i != j && G[i][j] == 0)
          G[i][j] = sum;
    }
  }
      /* Do mang tinh tu G[0][0] nen can giam vi tri
       di 1 don vi de tinh toan cho phu hop*/
  a--;
  b--;
    
  for(int i = 0; i < n; i++) 
  {
    Len[i] = sum;                   // khoi tao do dai tu a toi moi dinh la vo cung
    S[i] = 0;                       // danh sach cac diem da xet
    P[i] = a;                       // dat diem bat dau cua moi diem la a
  }
    
  Len[a] = 0;                         // dat do dai tu a -> a la 0
    
      // tim duong di ngan nhat tu 1 dinh den moi dinh khac thi thay bang vong for:
      //for (int k = 0; k < n; k++)
  while(S[b] == 0) 
  {                 // trong khi diem cuoi chua duoc xet
    for(i = 0; i < n; i++)          // tim 1 vi tri ma khong phai la vo cung
      if(!S[i] && Len[i] < sum)
        break;
    
          // i >=n tuc la duyet het cac dinh ma khong the tim thay dinh b -> thoat
    if(i >= n) 
    {
  //  cout << ("done dijkstra\n");
      break;
    }
    for(int j = 0; j < n; j++) 
    {    // tim diem co vi tri ma do dai la min
      if(!S[j] && Len[i] > Len[j]) 
      {
        i = j;
      }
    }
    
    S[i] = 1;                       // cho i vao danh sach xet roi
    
    for(int j = 0; j < n; j++) 
    {    // tinh lai do dai cua cac diem chua xet
      if(!S[j] && Len[i] + G[i][j] < Len[j]) 
      {
        Len[j] = Len[i] + G[i][j];      // thay doi len
        P[j] = i;                       // danh dau diem truoc no
      }
    }
  }
  int **array2 =  new int*[100];
  for(int i = 0 ; i < 100; i++)
  {
    array2[i] = new int[2];
  }  
      
      int k = 0;
      if(Len[b] > 0 && Len[b] < sum) 
      {
          
          // truy vet
          
          while(i != a) 
          {
              
              for(int j = 0 ; j < 2 ; j++)
              {
                  array2[k][j] = array[i + 1][j];
              }
              k++;
              i = P[i];
          }
          for(int j = 0; j <= k / 2; j++) 
          {
              int x = array2[j][0];
              array2[j][0] = array2[k - j][0];
              array2[k - j][0]=x;
              x = array2[j][1];
              array2[j][1] = array2[k - j][1];
              array2[k - j][1] = x;
          
          }
  
  for(i = 0; i < k - 1; i++)
  {
    if(huongxe == 1)
    {
      if(array2[i][0] != array2[i+1][0] && array2[i][1] == array2[i+1][1])
      {
        if(array2[i][0] > array2[i+1][0])
        {
          res += "1";
          huongxe = 2;
        }
        else
        {
          res += "2";
          huongxe = 2;
        }
      }
      else
      {
        res += "0";
        huongxe = 1;
      }
    }
    else if(huongxe == 2)
    {
      if(array2[i][0] == array2[i+1][0] && array2[i][1] != array2[i+1][1])
      {
        if(array2[i][1] > array2[i+1][1])
        {
          res += "2";
          huongxe = 3;
        }
        else
        {
          res += "1";
          huongxe = 1;
        }
      }
      else
      {
        res += "0";
        huongxe = 2;
      }
    }
    else if(huongxe == 3)
    {
      if(array2[i][0] != array2[i+1][0] && array2[i][1] == array2[i+1][1])
      {
        if(array2[i][0] > array2[i+1][0])
        {
          res += "2";
          huongxe = 2;
        }
        else
        {
          res += "1";
          huongxe = 2;
        }
      }
      else
      {
        res += "0";
        huongxe = 3;
      }
    }
  }
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
