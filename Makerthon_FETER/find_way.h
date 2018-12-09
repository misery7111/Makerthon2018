//#include <iostream>
//----------------------------------------------------------------------------------------------------------------
#define Sum_point 55
int soDiemOfLine = Sum_point/(MAP_HEIGHT+1);
struct toaDo
{
    int x;
    int y;
    toaDo(){}
    toaDo(int _x, int _y)
    {
        x = _x;
        y = _y;
    }
    char huong;
    String diChuyen;
};

int xetDiem(toaDo diemThu1, toaDo diemThu2, int Number_point1, int Number_point2)
{
        if(Number_point1 == Number_point2)
        {
            int c = int(int('0') - 48);
            return c;
        }
        else
        {
            if(diemThu1.x == diemThu2.x)
            {
                if((diemThu1.y - diemThu2.y) == -1)
                {
                    char c = gRows[diemThu1.x-1][diemThu1.y-1];
                    return int(int(c) - 48);
                }
                else if((diemThu1.y - diemThu2.y) == 1)
                {
                    char c = gRows[diemThu2.x-1][diemThu2.y-1];
                    return int(int(c) - 48);
                }
                else
                {
                    char c = '0';
                    return int(int(c) - 48);
                }
            }

            if(diemThu1.y == diemThu2.y)
            {
                if((diemThu1.x - diemThu2.x) == -1)
                {
                    char c = gCols[diemThu1.x-1][diemThu1.y-1];
                    return int(int(c) - 48);
                }
                else if((diemThu1.x - diemThu2.x) == 1)
                {
                    char c = gCols[diemThu2.x-1][diemThu2.y-1];
                    return int(int(c) - 48);
                }
                else
                {
                    char c = '0';
                    return int(int(c) - 48);
                }
            }
            if((diemThu1.x != diemThu2.x)&&(diemThu1.y != diemThu2.y))
            {
                char c = '0';
                return int(int(c) - 48);
            }
        }

}
int tinhToaDoX(int i)
{
    return int((i-1)/soDiemOfLine+1);
}
int tinhToaDoY(int i)
{
    return int(i-soDiemOfLine*int((i-1)/soDiemOfLine));
}

class timDuongMin
{
public:
    int n;
    int **mat; // Graph
    int firstVer, lastVer; // Đỉnh đầu và đỉnh cuối
    int* label;
    int* length;
    int* prev;
    bool createPath();
    //Dijkstra() {}
    timDuongMin(int matran[][Sum_point],int pointOfLine, int _firstVer, int _lastVer);
    void findMinPath(int duongDi[],int &soNodeCanDi);
    //~Dijkstra();
};

timDuongMin::timDuongMin(int matran[Sum_point][Sum_point],int pointOfLine, int _firstVer, int _lastVer)
{
    n = pointOfLine;
    firstVer = _firstVer;
    lastVer = _lastVer;
    firstVer--;
    lastVer--;

    // Cấp phát động
    label = new int[n];
    length = new int[n];
    prev = new int[n];
    mat = new int*[n];
    for (int i = 0; i < n; i++) mat[i] = new int[n];

    // Khởi tạo
    for (int i = 0; i < n; i++)
    {
        label[i] = 1;
        length[i] = -1; // Trọng số = -1 có nghĩa là inf
        prev[i] = -1;
    }
    length[firstVer] = 0;

    // Đọc Graph matrix
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            mat[i][j] = matran[i][j];
        }
    }


}

bool timDuongMin::createPath()
{
    // Chừng nào đỉnh lastVer vẫn chưa được đánh dấu thì ta còn xét
    while (label[lastVer] == 1)
    {
        int min = -1;
        int vertex = -1;

        // Tìm min length
        for (int i = 0; i < n; i++)
        {
            if (label[i] == 1 && length[i] != -1 && (length[i] < min || min == -1))
            {
                min = length[i];
                vertex = i;
            }
        }

        // Nếu ta không tìm được min nào, có nghĩa là không có đường đi từ firstVer -> lastVer
        if (min == -1)
        {
            return false;
        }

        // Đánh dấu đỉnh vertex
        length[vertex] = min;
        label[vertex] = 0;


        for (int i = 0; i < n; i++)
        {
            // Nếu đỉnh chưa được đánh dấu, và có đường đi từ vertex -> i
            if (label[i] == 1 && mat[vertex][i] != 0)
            {
                // Nếu đường đi từ vertex -> i ngắn hơn đường đi đã lưu trong mảng length
                if (length[i] == -1 || length[i] > length[vertex] + mat[vertex][i])
                {
                    length[i] = length[vertex] + mat[vertex][i];
                    // Tạo vết chân
                    prev[i] = vertex;
                }
            }
        }
    }
    return true;
}

// Ta cũng có thể dùng stack để tìm đường đi, tuy nhiên mình xin phép không ghi ra ở đây
void timDuongMin::findMinPath(int duongDi[],int &soNodeCanDi)
{

    bool pathExists = this->createPath();
    if (!pathExists){}
    else
    {
        // Dò ngược từ đỉnh cuối về đỉnh đầu

        int i = 0;
        soNodeCanDi = 0;
        int k = lastVer;
        while (k != firstVer)
        {
            duongDi[i] = (k + 1);
            // Tìm ngược lại bằng mảng prev lưu đỉnh trước đó
            k = prev[k];
            i++;
            soNodeCanDi++;
        }
        //cout<<(firstVer + 1);
    }
}

void toPoint(int mang1[], int mang2[], int n)
{
    for(int i=0;i<n;i++)
    {
        mang2[i] = mang1[n-i-1];
    }
}
int tinhViTri(toaDo vt)
{
    return (vt.x-1)*soDiemOfLine+vt.y;
}
void find_DuongDi(int goWhere[],int &soDiem, toaDo from, toaDo to)
{

    toaDo point[Sum_point+1];
    for(int i=1;i<=Sum_point;i++)
    {
        point[i].x = tinhToaDoX(i);
        point[i].y = tinhToaDoY(i);
    }

    int mangLienKet[Sum_point][Sum_point];
    for(int i = 0;i<Sum_point;i++)
    {
       for(int j = 0;j<Sum_point;j++)
       {

           mangLienKet[i][j] = xetDiem(point[i+1],point[j+1],i+1,j+1); //xét 2 điểm thứ i+1 và thứ j+1
           //Serial.print(mangLienKet[i][j]);
       }
       //Serial.println();
    }

    //----------------------------------
    int b[Sum_point];
    int soDiemCanDi = 0;
    timDuongMin a(mangLienKet,Sum_point,tinhViTri(from),tinhViTri(to));
    a.findMinPath(b,soDiemCanDi);
    int duongCanDi[soDiemCanDi];
    toPoint(b,duongCanDi,soDiemCanDi);
    soDiem = soDiemCanDi;
    for(int i = 0;i<soDiemCanDi;i++)
    {
        goWhere[i] = duongCanDi[i];
    }

}
int tangY(toaDo &xe)
{
    char traVe;
    switch(xe.huong)
    {
    case 'f':
        traVe = '1'; //rẽ phải
        break;
    case 'd':
        traVe = '2';//rẽ trái
        break;
    case 'r':
        traVe = '0';
        break;
    }
    xe.huong = 'r';
    xe.y++;
    return int(traVe);

}

int giamY(toaDo &xe)
{
    char traVe;
    switch(xe.huong)
    {
    case 'f':
        traVe = '2';
        break;
    case 'd':
        traVe = '1';//rẽ phải
        break;
    case 'l':
        traVe = '0';
        break;
    }
    xe.huong = 'l';
    xe.y--;
    return int(traVe);

}
int tangX(toaDo &xe)
{
    char traVe;
    switch(xe.huong)
    {
    case 'l':
        traVe = '2'; //rẽ trái
        break;
    case 'd':
        traVe = '0';//tiến
        break;
    case 'r':
        traVe = '1';
        break;
    }
    xe.huong = 'd';
    xe.x++;
    return int(traVe);
}
int giamX(toaDo &xe)
{
    char traVe;
    switch(xe.huong)
    {
    case 'l':
        traVe = '1'; //rẽ phải
        break;
    case 'f':
        traVe = '0';//tiến
        break;
    case 'r':
        traVe = '2';
        break;
    }
    xe.huong = 'f';
    xe.x--;
    return int(traVe);
}
void conTrol(toaDo &xe,int mangGiaTri[], int n)
{
    for(int i=0;i<n;i++)
    {
        if((tinhToaDoX(mangGiaTri[i]) - xe.x) != 0)
        {
            if((xe.x - tinhToaDoX(mangGiaTri[i])) == -1)
            {

                xe.diChuyen += char(tangX(xe));
            }

            if((xe.x - tinhToaDoX(mangGiaTri[i])) == 1)
            {

                xe.diChuyen += char(giamX(xe));
            }
        }

        if((tinhToaDoY(mangGiaTri[i]) - xe.y) != 0)
        {
            if((xe.y - tinhToaDoY(mangGiaTri[i])) == -1)
            {

                xe.diChuyen += char(tangY(xe));
            }

            if((xe.y - tinhToaDoY(mangGiaTri[i])) == 1)
            {

                xe.diChuyen += char(giamY(xe));
            }
        }
    }
}
/*
int main()
{
    
    
    for(int i = 0;i<n;i++)
    {
        cout<<mangGiaTri[i]<<"-->";
    }
    
    return 0;
}
*/
