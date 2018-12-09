#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#define MQTT_USER "client"
#define MQTT_PASS "password"
#define TEAM_ID "4"

#define MAP_WIDTH 10
#define MAP_HEIGHT 4
#define TARGET_NUM 5

int dem=0,t_step;
int d_point=0; 
char th_point[55][2];
int s, visit[5][11];
static char currentDirection= 3;
int b_count =0;
char str[2];
//char tha_1[5][2], tha_2[5][2];
//int a=0,b=0;

// Update these with values suitable for your network.
const char* ssid = "Makerthon 1";
const char* password = "makerthon";
const char* mqtt_server = "192.168.43.215";

struct thanh{
  int sum;
  int steps=0;
  char journey[55][2];  
} thanh;
struct thanh t[55];

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
// Number of Cars
int gCarNum = 0;
int gDataStatus=0;

char gTargetsCoord[TARGET_NUM][2];

WiFiClient espClient;
PubSubClient client(espClient);

void setup()
{
  // Initialize the Motor Shield, Serial and Wifi
  Serial.begin(115200);
  setup_wifi();
  visit_reset();
  clear_bodem(gDemoResult);
  // Configure mqtt client
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  ////////////////////////////
}

void loop()
{
  demoLoop(mainProcess);
}

void visit_reset(){
  int i,j;
  for(i=0;i<5;i++){
    for(j=0;j<11;j++){
      visit[i][j]=0;  
    }  
  }
}

char testLine(int currentDir, int x_i, int y_i, int x_d, int y_d)
{
  switch(currentDir)
  {
    case 1: // facing north
      if(x_i>x_d) return 0;
      if(y_i>y_d) { currentDir= 4; return (2+0x30);}
      if(y_i<y_d) { currentDir= 2; return (1+0x30);}
      break;
    case  2: // facing East
      if(x_i>x_d)      {currentDir = 1; return (2+0x30);}  
      if(x_i<x_d) {currentDir = 3; return (1+0x30);}
      if(y_i<y_d) return 0;
      break;
    case  3: // facing South
      if(x_i<x_d) return 0;  
      if(y_i<y_d) {currentDir =2; return (2+0x30);}
      if(y_i>y_d) {currentDir =4; return (1+0x30);}
      break;
    case 4: // facing West
      if(y_i>y_d) return 0;  
      if(x_i<x_d) {currentDir =3; return (0x30+2);}
      if(x_i>x_d) {currentDir =1; return (1+0x30);}
    break;
  }
}

void copy_journey(int d,int a){
  int tha;
  for(tha=0;tha<a;tha++){
    t[d].journey[tha][0]=th_point[tha][0];
    t[d].journey[tha][1]=th_point[tha][1];  
  }
}

char char_2_ascii(char t){
//  if(t<10){
   char c;
    c=t+0x30;
    return c; 
//  }
//  else {
//    str[0]=(t/10)+0x30;
//    str[1]=t%10+0x30;  
//  }
}

bool find_road(char x_i, char y_i, char x_d, char y_d){ // x_i: x initial, y_i: y initial, x_d: x destination, y_d: y destination
    int k=0;
    if(x_i>x_d&&y_i>y_d+1){         // đã fix dòng này
      return false;  
    }
    if(x_i<0&&y_i<0) return false;
    visit[x_i][y_i]=1;
    if(x_i==x_d&&y_i==y_d){
      d_point=1;
      return true;  
    }
    if(visit[x_i+1][y_i]==0&&(gCols[x_i][y_i]!=0x30)){
      s+=gCols[x_i][y_i]; th_point[t_step][0]=x_i+1; th_point[t_step][1]=y_i; t_step++;
      if(find_road(x_i+1,y_i,x_d,y_d)==true){
        //visit[x_i+1,y_i]=0;
        if(d_point){
          d_point=0;
          t[dem].steps=t_step;
          t[dem].sum=s;
          copy_journey(dem,t_step);
          dem++;  
        }
        k++;
      }
      visit[x_i+1][y_i]=0;
      s-=gCols[x_i][y_i]; t_step--; th_point[t_step][0]='\0'; th_point[t_step][1]='\0';  
    }
    if(visit[x_i][y_i+1]==0&&(gRows[x_i][y_i]!=0x30)){
      s+=gCols[x_i][y_i]; th_point[t_step][0]=x_i; th_point[t_step][1]=y_i+1; t_step++;
      if(find_road(x_i,y_i+1,x_d,y_d)==true){
        //visit[x_i,y_i+1]=0;
        if(d_point){
          d_point=0;
          t[dem].steps=t_step;
          t[dem].sum=s;
          copy_journey(dem,t_step);
          dem++;  // dem so truong hop den duoc dich
        }
        k++;  
      }
      visit[x_i][y_i+1]=0;
      s-=gCols[x_i][y_i]; t_step--; th_point[t_step][0]='\0'; th_point[t_step][1]='\0';  
    }
    if(x_i){
    if(visit[x_i-1][y_i]==0&&(gCols[x_i-1][y_i]!=0x30)){
      s+=gCols[x_i-1][y_i]; th_point[t_step][0]=x_i-1; th_point[t_step][1]=y_i; t_step++; 
      if(find_road(x_i-1,y_i,x_d,y_d)==true){
        //visit[x_i-1,y_i]=0;
        if(d_point){
          d_point=0;
          t[dem].steps=t_step;
          t[dem].sum=s;
          copy_journey(dem,t_step);
          dem++;  
        }
        k++;   
      }
      visit[x_i-1][y_i]=0;
      s-=gCols[x_i-1][y_i]; t_step--; th_point[t_step][0]='\0'; th_point[t_step][1]='\0';  
    }}
    if(y_i){
    if(visit[x_i][y_i-1]==0&&(gRows[x_i][y_i-1]!=0x30)){
      s+=gCols[x_i][y_i-1]; th_point[t_step][0]=x_i; th_point[t_step][1]=y_i-1; t_step++;
      if(find_road(x_i,y_i-1,x_d,y_d)==true){
        //visit[x_i,y_i-1]=0;
        if(d_point){
          d_point=0;
          t[dem].steps=t_step;
          t[dem].sum=s;
          copy_journey(dem,t_step);
          dem++;  
        }
        k++;  
      }
      visit[x_i][y_i-1]=0;
      s-=gCols[x_i][y_i-1]; t_step--; th_point[t_step][0]='\0'; th_point[t_step][1]='\0';  
    }
    }
    if(k) return true;
    else return false;
}

int filter(){
// ham nay se so sanh cac gia tri sum voi nhau cai nao nho nhat thi luu trong so cua no vao, roi lay cai journey ra la co toa do cac diem di qua
  int i,j=t[0].sum;
  int a;
  for(i=0;i<dem-1;i++){
    if(t[i].sum>t[i+1].sum) j=t[i+1].sum;  
  }
  for(i=0;i<dem;i++){
    if(j==t[i].sum) {a=i; break;}  
  }
  return a;  
}
void clear_bodem(char* g){
  int i;
  for(i=0;g[i]!='\0';i++) g[i]='\0';  
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
//  int i,j;
//  char ttt, tam_1[5][2], tam_2[5][2];
      gDataStatus=1;
      Serial.println("Received map & targets' coords & car num");
      receiveMap(payload);
      memcpy(gTargetsCoord, payload + MAP_WIDTH * MAP_HEIGHT * 2 + MAP_WIDTH + MAP_HEIGHT, TARGET_NUM * 2);
      gCarNum = payload[MAP_WIDTH * MAP_HEIGHT * 2 + MAP_WIDTH + MAP_HEIGHT + TARGET_NUM * 2];
//    `  for(i=0;i<5;i++){
//        if(gTargetsCoord[i][0]<=1) {tam_1[a][0] = gTargetsCoord[i][0]; tam_1[a][1] = gTargetsCoord[i][1]; a++;}
//        else if(gTargetsCoord[i][0]>=2){tam_2[b][0]=gTargetsCoord[i][0]; tam_2[b][1]=gTargetsCoord[i][1]; b++;}  
//      }
//      for(i=0;i<a-1;i++){
//        for(j=a-2;j>=i;j--){
//         if(tam_1[j][1]>tam_1[j+1][1]){
//            ttt=tam_1[j][1];
//            tam_1[j][1]=tam_1[j+1][1];
//            tam_1[j+1][1]=ttt;
//          }
//        } 
//      }
//      for(i=0;i<a;i++){
//        tha_1[i][0]=tam_1[i][0];
//      }
//      for(i=0;i<b-1;i++){
//        for(j=b-2;j>=i;j++){
//         if(tam_2[j][1]>tam_2[j+1][1]){
//            ttt=tam_2[j][1];
//            tam_2[j][1]=tam_2[j+1][1];
//            tam_2[j+1][1]=ttt;
//          }
//        } 
//      }
//      for(i=0;i<b;i++){
//        tha_2[i][0]=tam_2[i][0];
//      }
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

void clear_all(){
  int i;
  for(i=0;i<dem;i++){
    t[i].sum=0;
    t[i].steps=0;  
  }
  dem=0;  
}

//void char_2_ascii(char t){
//  if(t<10) str[0]=t+0x30;
//  else if(t==10) {
//    str[0]=t/10+0x30;
//    str[1]=t%10+0x30;
//    }  
//}
void mainProcess()
{
  int th,i;
  char c;
  if(find_road(0,0,gTargetsCoord[0][0],gTargetsCoord[0][1])==true){
    th=filter();
    gDemoResult[b_count] = testLine(currentDirection,0,0,t[th].journey[0][0],t[th].journey[0][1]);
    b_count++;
    for( i=1;i<t[th].steps+1;i++){
    for(int j=0;j<2;j++){
      //Serial.print(t[th_1].journey[i][j]);
      c=char_2_ascii(t[th].journey[i-1][j]);
      Serial.print(c);
//      clear_bodem(str);
    }
    Serial.print("\r\n");
    gDemoResult[b_count] = testLine(currentDirection,t[th].journey[i-1][0],t[th].journey[i-1][1],t[th].journey[i][0],t[th].journey[i][1]);
    b_count++;
  }
  clear_all();
  }
  else Serial.println("khong the tim duoc duong den check point yeu cau");
  Serial.println(gDemoResult);
  
  if(find_road(gTargetsCoord[0][0],gTargetsCoord[0][1],gTargetsCoord[1][0],gTargetsCoord[1][1])==true){
    th=filter();
    gDemoResult[b_count] = testLine(currentDirection,gTargetsCoord[0][0],gTargetsCoord[0][1],t[th].journey[1][0],t[th].journey[1][1]);
    b_count++;
    for( i=0;i<t[th].steps;i++){
    for(int j=0;j<2;j++){
      //Serial.print(t[th_1].journey[i][j]);
      c=char_2_ascii(t[th].journey[i][j]);
      Serial.print(c);
//      clear_bodem(str);
    }
    Serial.print("\r\n");
    gDemoResult[b_count] = testLine(currentDirection,t[th].journey[i-1][0],t[th].journey[i-1][1],t[th].journey[i][0],t[th].journey[i][1]);
    b_count++;
  }
  clear_all();
  }
  else Serial.println("khong the tim duoc duong den check point yeu cau");

  Serial.println(gDemoResult);

  if(find_road(gTargetsCoord[1][0],gTargetsCoord[1][1],gTargetsCoord[2][0],gTargetsCoord[2][1])==true){
    th=filter();
    gDemoResult[b_count] = testLine(currentDirection,gTargetsCoord[1][0],gTargetsCoord[1][1],t[th].journey[2][0],t[th].journey[2][1]);
    b_count++;
    for(int i=0;i<t[th].steps;i++){
    for(int j=0;j<2;j++){
      //Serial.print(t[th_1].journey[i][j]);
      c=char_2_ascii(t[th].journey[i][j]);
      Serial.print(c);
//      clear_bodem(str);
    }
    Serial.print("\r\n");
    gDemoResult[b_count] = testLine(currentDirection,t[th].journey[i-1][0],t[th].journey[i-1][1],t[th].journey[i][0],t[th].journey[i][1]);
    b_count++;
  }
  clear_all();
  }
  else Serial.println("khong the tim duoc duong den check point yeu cau");
  Serial.println(gDemoResult);
  if(find_road(gTargetsCoord[2][0],gTargetsCoord[2][1],gTargetsCoord[3][0],gTargetsCoord[3][1])==true){
    th=filter();
    gDemoResult[b_count] = testLine(currentDirection,gTargetsCoord[2][0],gTargetsCoord[2][1],t[th].journey[3][0],t[th].journey[3][1]);
    b_count++;
    for(int i=0;i<t[th].steps;i++){
    for(int j=0;j<2;j++){
      //Serial.print(t[th_1].journey[i][j]);
      c=char_2_ascii(t[th].journey[i][j]);
      Serial.print(c);
//      clear_bodem(str);
    }
    Serial.print("\r\n");
    gDemoResult[b_count] = testLine(currentDirection,t[th].journey[i-1][0],t[th].journey[i-1][1],t[th].journey[i][0],t[th].journey[i][1]);
    b_count++;
  }
  clear_all();
  }
  else Serial.println("khong the tim duoc duong den check point yeu cau");
  Serial.println(atoi(gDemoResult));
  if(find_road(gTargetsCoord[3][0],gTargetsCoord[3][1],gTargetsCoord[4][0],gTargetsCoord[4][1])==true){
    th=filter();
    gDemoResult[b_count] = testLine(currentDirection,gTargetsCoord[3][0],gTargetsCoord[3][1],t[th].journey[4][0],t[th].journey[4][1]);
    b_count++;
    for(int i=0;i<t[th].steps;i++){
    for(int j=0;j<2;j++){
      //Serial.print(t[th_1].journey[i][j]);
      c=char_2_ascii(t[th].journey[i][j]);
      Serial.print(c);
//      clear_bodem(str);
    }
    Serial.print("\r\n");
    gDemoResult[b_count] = testLine(currentDirection,t[th].journey[i-1][0],t[th].journey[i-1][1],t[th].journey[i][0],t[th].journey[i][1]);
    b_count++;
  }
  clear_all();
  }
  else Serial.println("khong the tim duoc duong den check point yeu cau");
  Serial.println(gDemoResult);
 
//  for(i=0;i<4;i++){
//    if(find_road(gTargetsCoord[i][0],gTargetsCoord[i][1],gTargetsCoord[i+10][0],gTargetsCoord[i+1][1])==true){
//    th=filter();
//    for(int i=0;i<t[th].steps;i++){
//    for(int j=0;j<2;j++){
//      //Serial.print(t[th_1].journey[i][j]);
//      c=char_2_ascii(t[th].journey[i][j]);
//      Serial.print(c);
////      clear_bodem(str);
//    }
//    Serial.print("\r\n");
//  }
//  clear_all();
//  }
//  else Serial.println("khong the tim duoc duong den check point yeu cau");
//  }
//  for(i=0;i<a-1;i++){
//    if(find_road(tha_1[i][0],tha_1[i][1],tha_1[i+1][0],tha_1[i+1][1])==true){
//        th=filter();
//        for(int i=0;i<t[th].steps;i++){
//        for(int j=0;j<2;j++){
//      //Serial.print(t[th_1].journey[i][j]);
//      c=char_2_ascii(t[th].journey[i][j]);
//      Serial.print(c);
////      clear_bodem(str);
//    }
//    Serial.print("\r\n");
//    }  
//  }
//    else Serial.println("khong the tim duoc duong den check point yeu cau");
//  }
//  if(find_road(4,0,tha_1[0][0],tha_1[0][1])==true){
//    th=filter();
//    for(int i=0;i<t[th].steps;i++){
//    for(int j=0;j<2;j++){
//      //Serial.print(t[th_1].journey[i][j]);
//      c=char_2_ascii(t[th].journey[i][j]);
//      Serial.print(c);
////      clear_bodem(str);
//    }
//    Serial.print("\r\n");
//  }
//  }
//  else Serial.println("khong the tim duoc duong den check point yeu cau");
//  for(i=0;i<b-1;i++){
//    if(find_road(tha_1[i][0],tha_1[i][1],tha_1[i+1][0],tha_1[i+1][1])==true){
//        th=filter();
//        for(int i=0;i<t[th].steps;i++){
//        for(int j=0;j<2;j++){
//      //Serial.print(t[th_1].journey[i][j]);
//      c=char_2_ascii(t[th].journey[i][j]);
//      Serial.print(c);
////      clear_bodem(str);
//    }
//    Serial.print("\r\n");
//    }  
//  }
//    else Serial.println("khong the tim duoc duong den check point yeu cau");
//  }  
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
      Serial.print(gRows[i][j]);
    }
    Serial.print("\r\n");
  }

  for (int i = 0; i < MAP_HEIGHT; i++)
  {
    for (int j = 0; j < MAP_WIDTH + 1; j++)
    {
      gCols[i][j] = payload[index++];
      Serial.print(gCols[i][j]);
    }
    Serial.print("\r\n");
  }
}
