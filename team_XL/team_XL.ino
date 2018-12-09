#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#define MQTT_USER "client"
#define MQTT_PASS "password"
#define TEAM_ID "0"

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
char *gDemoResult;

// Rows and Column tables
char gRows[MAP_HEIGHT + 1][MAP_WIDTH]; //[11][4]
char gCols[MAP_HEIGHT][MAP_WIDTH + 1];
// Number of Cars
int gCarNum = 0;

int gDataStatus = 0;
char gTargetsCoord[TARGET_NUM][2];

WiFiClient espClient;
PubSubClient client(espClient);

int n = 55;
int chiSo[55][4];

int diQua[100];


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

void timChisoduongdi()
{
    int i, j;
    chiSo[10][0] = '0';
    chiSo[10][1] = gCols[0][10];
    chiSo[10][2] = '0';

    chiSo[54][0] = '0';
    chiSo[54][1] = '0';
    chiSo[54][2] = gCols[3][10];

    chiSo[21][0] = '0';
    chiSo[21][1] = gCols[1][10];
    chiSo[21][2] = gCols[0][10];

    chiSo[32][0] = '0';
    chiSo[32][1] = gCols[2][10];
    chiSo[32][2] = gCols[1][10];

    chiSo[43][0] = '0';
    chiSo[43][1] = gCols[3][10];
    chiSo[43][2] = gCols[2][10];

    for (i = 0; i < 55; ++i)
    {
        chiSo[i][3] = '0';
        if (i >= 0 && i < 10)
        {
            chiSo[i][0] = gRows[0][i];
            chiSo[i][1] = gCols[0][i];
            chiSo[i][2] = '0';
        }
        else if (i >= 11 && i < 21)
        {
            chiSo[i][0] = gRows[1][i - 11];
            chiSo[i][1] = gCols[1][i - 11];
            chiSo[i][2] = gCols[0][i - 11];
        }
        else if (i >= 22 && i < 32)
        {
            chiSo[i][0] = gRows[2][i - 22];
            chiSo[i][1] = gCols[2][i - 22];
            chiSo[i][2] = gCols[1][i - 22];
        }
        else if (i >= 33 && i < 43)
        {
            chiSo[i][0] = gRows[3][i - 33];
            chiSo[i][1] = gCols[3][i - 33];
            chiSo[i][2] = gCols[2][i - 33];
        }
        else if (i >= 44 && i < 54)
        {
            chiSo[i][0] = gRows[4][i - 44];
            chiSo[i][1] = '0';
            chiSo[i][2] = gCols[3][i - 44];
        }
    }
    /*
    for (int i = 0; i < 55; ++i)
    {
        Serial.print(chiSo[i][0]);
        Serial.print("   ");
        Serial.print(chiSo[i][1]);
        Serial.print("   ");
        Serial.print(chiSo[i][2]);
        Serial.print("   ");
        Serial.println(chiSo[i][3]);
    }
    */
}

void timDuongmin(int a, int b)
{
    int i, j, k, sum = 0;
    int S[n], Len[n], P[n];
    int dinh, checkDinh;

    // nhap ma tran va tinh gia tri vo cung (sum)
    for (i = 0; i < 55; ++i)
    {
        for (j = 0; j < 4; ++j)
        {
            sum = sum + chiSo[i][j];
        }
    }

    // dat vo cung cho tat ca cap canh khong noi voi nhau
    for (i = 0; i < 55; i++) 
    {
        for (j = 0; j < 4; j++) 
        {
            if (j != 3 && chiSo[i][j] == 0)
            {
                chiSo[i][j] = sum;
            }
        }
    }
    
    for (i = 0; i < n; i++) 
    {
        Len[i] = sum;                   // khoi tao do dai tu a toi moi dinh la vo cung
        S[i] = 0;                       // danh sach cac diem da xet
        P[i] = a;                       // dat diem bat dau cua moi diem la a
    }
  
    Len[a] = 0;                         // dat do dai tu a -> a la 0
    
    // tim duong di ngan nhat tu 1 dinh den moi dinh khac thi thay bang vong for:
    //for (int k = 0; k < n; k++)
    while (S[b] == 0) 
    {                 // trong khi diem cuoi chua duoc xet
    
        // tim 1 vi tri ma khong phai la vo cung
        for (i = 0; i < n; i++)        
        {
            if (!S[i] && Len[i] < sum)
            {
                break;
            }
        }
        
        // i >=n tuc la duyet het cac dinh ma khong the tim thay dinh b -> thoat
        if (i >= n)
        {
            printf("done dijkstra\n");
            break;
        }
        
        // tim diem co vi tri ma do dai la min
        for (j = 0; j < n; j++) 
        {
            if (!S[j] && Len[i] > Len[j]) 
            {
                i = j;
            }
        }
        S[i] = 1;     // cho i vao danh sach xet roi
        // tinh lai do dai cua cac diem chua xet
        
        for (j = 0; j < 3; ++j)
        {
            if (chiSo[i][j] == sum)
            {
                continue;
            }
            if (j == 0)
            {
                dinh = i + 1;
            }
            else if (j == 1)
            {
                dinh = i + 11;
            }
            else if (j == 2)
            {
                dinh = i - 11;
            }

            if (!S[dinh] && Len[i] + chiSo[i][j] < Len[dinh])
            {
                Len[dinh] = Len[i] + chiSo[i][j];
                P[dinh] = i;
            }
        }
    }

    /* Do ta dang tinh toan tu dinh 0 nen
     muon hien thi tu dinh 1 thi can dung i + 1 de phu hop */
    int luuGiatri[100];
    luuGiatri[0] = -1;
    k = 0;
    if (Len[b] > 0 && Len[b] < sum) 
    {
        int temp = i;
        while (i != a) 
        {
            luuGiatri[k] = temp;
            temp = P[temp];
            k++;
        }
        luuGiatri[k] = -1;
    } 
    else 
    {
        //fprintf(fo, "khong co duong di tu %d den %d\n", a + 1, b + 1);
    }
    k = k - 1;
    i = 0;
    while(i != -1)
    {
        i++;
    }
    while (k != -1)
    {
        diQua[i] = luuGiatri[k];
        k--;
        i++;
    }
    diQua[i] = -1;
}


/*
void hienthi()
{
    Serial.println("Rows");
    for (int i = 0; i < MAP_HEIGHT + 1; ++i)
    {
        for (int j = 0; j < MAP_WIDTH; ++j)
        {
            Serial.print(gRows[i][j]);
            Serial.print("\t");
        }
        Serial.println("");
    }
    Serial.println("Cols");
    for (int i = 0; i < MAP_HEIGHT; ++i)
    {
        for (int j = 0; j < MAP_WIDTH + 1; ++j)
        {
            Serial.print(gCols[i][j]);
            Serial.print("\t");
        }
        Serial.println("");
    }

    for (int i = 0; i < TARGET_NUM; ++i)
    {
        Serial.print("TargetsCoord = [");
        Serial.print(gTargetsCoord[i][0]);
        Serial.print(" , ");
        Serial.print(gTargetsCoord[i][1]);
        Serial.println("]");
    }
}
*/
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

void mainProcess()
{
    diQua[0] = -1;
    timChisoduongdi();
    gTargetsCoord[TARGET_NUM][2];
    int a, b;
    int tThai = 1;
    a = 0;
    b = gTargetsCoord[0][0] * 11 + gTargetsCoord[0][1];
    timDuongmin(a, b);
    a = b;
    b = gTargetsCoord[1][0] * 11 + gTargetsCoord[1][1];
    timDuongmin(a, b);
    a = b;
    b = gTargetsCoord[2][0] * 11 + gTargetsCoord[2][1];
    timDuongmin(a, b);
    a = b;
    b = gTargetsCoord[3][0] * 11 + gTargetsCoord[3][1];
    timDuongmin(a, b);
    a = b;
    b = gTargetsCoord[4][0] * 11 + gTargetsCoord[4][1];
    timDuongmin(a, b);
    int i = 0;
    while(diQua[i] != -1)
    {
        i++;
    }
    diQua[i] = 0;
    char duongDi[i + 10];
    int k = 0;
    while(i != 0)
    {
        if (diQua[i - 1] - diQua[i] == 1)
        {
            if (tThai == 0)
            {
                duongDi[k] = '0';
                tThai = 0;
            }
            else if (tThai == 1)
            {
                duongDi[k] = '2';
                tThai = 0;
            }
            else if (tThai == 2)
            {
                duongDi[k] = '1';
                tThai = 0;
            }
        }
        if (diQua[i - 1] - diQua[i] > 1)
        {
            if (tThai == 0)
            {
                duongDi[k] = '1';
                tThai = 1;
            }
            else if (tThai == 1)
            {
                duongDi[k] = '0';
                tThai = 1;
            }
            else if (tThai == 2)
            {
                tThai = 1;
            }
        }
        if (diQua[i - 1] - diQua[i] < 1)
        {
            if (tThai == 0)
            {
                duongDi[k] = '2';
                tThai = 2;
            }
            else if (tThai == 1)
            {
                tThai = 2;
            }
            else if (tThai == 2)
            {
                duongDi[k] = '0';
                tThai = 2;
            }
        }
        k++;
        i--;
    }
    duongDi[k] = '5';
    gDemoResult = &duongDi[0];
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

    return    true;
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
