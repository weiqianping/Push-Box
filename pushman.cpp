#include <iostream>
#include<fstream>
#include<sstream>
#include<windows.h>
#include<stdio.h>
#include<cassert>
#include <conio.h>
#include<cstring>
#include <string.h>
#include <stdbool.h> //c语言能一直用布尔类型如true
#include <stdlib.h>  //利用system清屏
#include<easyx.h>
#include <graphics.h>
//加音乐的头文件与库
#include<mmsystem.h>
#pragma comment(lib,"WINMM.LIB")
using namespace std;
// gcc -o puzzle pushman.cpp
//./pushman.exe
/*
分析推箱子
1.推箱子怎么写？界面上有哪些东西？这些东西用什么存储？
  数据：箱子 墙 目的地 空地 玩家
  表示: 3    1  2     0    4
  如何存储: 二维数组
2.人物如何移动，箱子如何移动
  移动人:
  推箱子:
  控制台版本-完成
  图形界面版本
  72 上
  80 下
  75 左
  77 右
*/
// 宏定义
#define SPACE 0  // 空地
#define WALL 1   // 墙
#define DEST 2   // 目的地
#define BOX 3    // 箱子
#define PLAYER 4 // 玩家
#define ROWS 16  // 地图行和列
#define COLS 16
int flag=0;
int top = -1;
int stack[1000];
int sflag[1000];
//定义图片数组
IMAGE imgs[9];
void loadImg_and_mp3()
{
    loadimage(imgs + 0, "./images/0.jpg", 60, 60);
    loadimage(imgs + 1, "./images/1.jpg", 60, 60);
    loadimage(imgs + 2, "./images/2.jpg", 60, 60);
    loadimage(imgs + 3, "./images/3.jpg", 60, 60);
    loadimage(imgs + 4, "./images/4.jpg", 60, 60);
    loadimage(imgs + 5, "./images/5.jpg", 60, 60);
    loadimage(imgs + 6, "./images/6.jpg", 60, 60);
    loadimage(imgs + 7, "./images/logo.jpg", 960, 400);
    loadimage(imgs + 8, "./images/black.webp,", 960, 560);
    mciSendString("open ./images/back.mp3 alias bgm", 0, 0, 0);
    mciSendString("play bgm repeat", 0, 0, 0);
}
// 定义地图
int map[ROWS][COLS];
int levels = 1;
// 初始化地图
void initMap()
{
    //memcpy(map, tmap, sizeof(tmap));
    string filname = "puzzle/map" + to_string(levels) + ".txt";
    //FILE* file_open=fopen(filname.c_str(), "rb");//.c_str()转换为c语言风格字符串'\0'结尾的字符数组
    ifstream file_open;
    file_open.open(filname.data());
    assert(file_open.is_open());
    string t;
    int n,i=0;
    while (getline(file_open,t))
    {
        n = t.length();
        for (int k = 0; k < n; ++k)
        {
            map[i][k] = t[k] - '0';
        }
        i++;
    }
    file_open.close();
}
// 输出地图
void show()
{
    system("cls"); // 清屏
    for (int i = 0; i < ROWS; ++i)
    {
        for (int k = 0; k < COLS; ++k)
        {
            // printf("%d ", map[i][k]);
            // 把数据转为图形输出
            switch (map[i][k])
            {
            case SPACE:
                cout << " ";
                break;
            case WALL:
                cout << "#";
                break;
            case DEST:
                cout << "⭐";
                break;
            case BOX:
                cout << "□";
                break;
            case PLAYER:
                cout << "♀";
                break;
                // 玩家在目的地上
            case PLAYER + DEST:
                cout << "♥";
                break;
                // 箱子在目的地上
            case BOX + DEST:
                cout << "♂";
                break;
            }
        }
        printf("\n");
    }
}
//把数据输出到图形界面库
void draw()
{
    //system("cls"); // 清屏
    for (int i = 0; i < ROWS; ++i)
    {
        for (int k = 0; k < COLS; ++k)
        {
            //求每个图片左上的坐标
            int x = k * 60;
            int y = i * 60;
            putimage(x, y, imgs + map[i][k]);
            // printf("%d ", map[i][k]);
            // 把数据转为图形输出
            /*switch (map[i][k])
            {
            case SPACE:
                
                putimage(x, y, imgs + 0);//第三个参数为图片指针
                break;
            case WALL:
                
                putimage(x, y, imgs + 1);
                break;
            case DEST:
                
                putimage(x, y, imgs + 2);
                break;
            case BOX:
                
                putimage(x, y, imgs + 3);
                break;
            case PLAYER:
                
                putimage(x, y, imgs + 4);
                break;
                
            case PLAYER + DEST:
                
                putimage(x, y, imgs + 5);
                break;
                
            case BOX + DEST:
                
                putimage(x, y, imgs + 6);
                break;
            }*/
        }
    }
    string s = "                               已经闯到第" + to_string(levels) + "关                                              ";
    outtextxy(0, 0, _T(s.c_str()));
}
// 移动箱子
void move()
{
    // 玩家当前所在的下标r,c
    int r = -1, c = -1;
    // 查找玩家的位置并赋值给r和c
    for (int i = 0; i < ROWS; ++i)
    {
        for (int k = 0; k < COLS; ++k)
        {
            if (map[i][k] == PLAYER || map[i][k] == PLAYER + DEST)
            {
                r = i;
                c = k;
                goto end; // 实现跳出两层循环
            }
        }
    }
end:;
    int key = _getch();
    switch (key)
    {
        // 只有人前面是空地或者目的地的时候才可以移动
    case 72: // 上
        top++;
        stack[top] = 72;
        if (map[r - 1][c] == SPACE || map[r - 1][c] == DEST)
        {
            map[r - 1][c] += PLAYER;
            map[r][c] -= PLAYER;
        }
        // 玩家的前面是箱子
        else if (map[r - 1][c] == BOX || map[r - 1][c] == BOX+DEST)
        {
            sflag[top] = 1;
            // 箱子的前面是空地或者目的地或者箱子到达目的地
            if (map[r - 2][c] == SPACE || map[r - 2][c] == DEST)
            {
                // 把箱子移动到指定位置
                map[r - 2][c] += BOX;
                map[r - 1][c] -= BOX;
                map[r - 1][c] += PLAYER;
                map[r][c] -= PLAYER;
            }
        }
        break;
    case 80: // 下,利用Ctrl+s实现块代码查找替换关键字
        top++;
        stack[top] = 80;
        if (map[r + 1][c] == SPACE || map[r + 1][c] == DEST)
        {
            map[r + 1][c] += PLAYER;
            map[r][c] -= PLAYER;
        }
        // 玩家的前面是箱子
        else if (map[r + 1][c] == BOX || map[r + 1][c] == BOX+DEST)
        {
            sflag[top] = 1;
            // 箱子的前面是空地或者目的地
            if (map[r + 2][c] == SPACE || map[r + 2][c] == DEST)
            {
                // 把箱子移动到指定位置
                map[r + 2][c] += BOX;
                map[r + 1][c] -= BOX;
                map[r + 1][c] += PLAYER;
                map[r][c] -= PLAYER;
            }
        }
        break;
    case 75: // 左
        top++;
        stack[top] = 75;
        if (map[r][c - 1] == SPACE || map[r][c - 1] == DEST)
        {
            map[r][c - 1] += PLAYER;
            map[r][c] -= PLAYER;
        }
        // 玩家的前面是箱子
        else if (map[r][c - 1] == BOX || map[r][c - 1] == BOX+DEST)
        {
            sflag[top] = 1;
            // 箱子的前面是空地或者目的地
            if (map[r][c - 2] == SPACE || map[r][c - 2] == DEST)
            {
                // 把箱子移动到指定位置
                map[r][c - 2] += BOX;
                map[r][c - 1] -= BOX;
                map[r][c - 1] += PLAYER;
                map[r][c] -= PLAYER;
            }
        }
        break;
    case 77: // 右
        top++;
        stack[top] = 77;
        if (map[r][c + 1] == SPACE || map[r][c + 1] == DEST)
        {
            map[r][c + 1] += PLAYER;
            map[r][c] -= PLAYER;
        }
        // 玩家的前面是箱子
        else if (map[r][c + 1] == BOX || map[r][c + 1] == BOX+DEST)
        {
            sflag[top] = 1;
            // 箱子的前面是空地或者目的地
            if (map[r][c + 2] == SPACE || map[r][c + 2] == DEST)
            {
                // 把箱子移动到指定位置
                map[r][c + 2] += BOX;
                map[r][c + 1] -= BOX;
                map[r][c + 1] += PLAYER;
                map[r][c] -= PLAYER;
            }
        }
        break;
    case 'c':
    case 'C':
        flag = 3;
        break;
    case 'q' :
    case 'Q' :
        flag = 4;
        break;
    case 's':
    case 'S':
        flag = 5;
        break;
    case 'z':
    case 'Z':
        if (top > -1)
        {
            switch (stack[top])
            {
            case 72:
                if ((map[r - 1][c] ==BOX || map[r-1][c] ==BOX+DEST) && sflag[top] == 1)
                {
                    map[r][c] -= PLAYER;
                    map[r + 1][c] += PLAYER;
                    map[r - 1][c] -= BOX;
                    map[r][c] += BOX;
                }
                else
                {
                    map[r][c] -= PLAYER;
                    map[r + 1][c] += PLAYER;
                }
                top--;
                break;
            case 80:
                if ((map[r + 1][c] == BOX || map[r + 1][c] == BOX + DEST) && sflag[top]==1)
                {
                    map[r][c] -= PLAYER;
                    map[r - 1][c] += PLAYER;
                    map[r + 1][c] -= BOX;
                    map[r][c] += BOX;
                }
                else
                {
                    map[r][c] -= PLAYER;
                    map[r - 1][c] += PLAYER;
                }
                top--;
                break;
            case 75:
                if ((map[r][c - 1] == BOX || map[r][c - 1] == BOX + DEST) && sflag[top]==1)
                {
                    map[r][c] -= PLAYER;
                    map[r][c + 1] += PLAYER;
                    map[r][c - 1] -= BOX;
                    map[r][c] += BOX;
                }
                else
                {
                    map[r][c] -= PLAYER;
                    map[r][c + 1] += PLAYER;
                }
                top--;
                break;
            case 77:
                if ((map[r][c+1] == BOX || map[r][c+1] == BOX + DEST) && sflag[top]==1)
                {
                    map[r][c] -= PLAYER;
                    map[r][c - 1] += PLAYER;
                    map[r][c + 1] -= BOX;
                    map[r][c] += BOX;
                }
                else
                {
                    map[r][c] -= PLAYER;
                    map[r][c - 1] += PLAYER;
                }
                top--;
                break;
            }
        }
        break;
    }
    PlaySound("./images/Boxmove.WAV", nullptr, SND_FILENAME | SND_ASYNC);
}
//判断输赢,1.箱子碰到死角无法推动为输
//         2.地图上没有箱子为赢
void judge()
{
    if (flag != 0) return;
    flag = 2;
    for (int i = 0; i < ROWS; ++i)
    {
        for (int k = 0; k < COLS; ++k)
        {
            if (map[i][k] == BOX)
            {
                flag = 0;
                //if ((map[i - 1][k] == WALL || map[i + 1][k] == WALL) || (map[i-1][k]== BOX || map[i+1][k]==BOX) || (map[i-1][k]==BOX+DEST || map[i+1][k]==BOX+DEST))
                if(map[i - 1][k] == WALL || map[i + 1][k] == WALL)
                {
                    //if ((map[i][k - 1] == WALL || map[i][k + 1] == WALL) || (map[i][k - 1] == BOX || map[i][k + 1] == BOX) || (map[i][k - 1] == BOX+DEST || map[i][k + 1] == BOX+DEST))
                    if (map[i][k - 1] == WALL || map[i][k + 1] == WALL)
                    {
                        flag = 1;
                        return;
                    }
                }
            }
        }
    }
}
//提示盒子
void box()
{
    if (flag == 1)
    {
        MessageBox(GetHWnd(),"抱歉，你输了", "LOSE", MB_OK);
    }
    else if(flag==2)
    {
        PlaySound("./images/success.wav", nullptr, SND_FILENAME | SND_ASYNC);
        MessageBox(GetHWnd(), "恭喜，你赢了。", "VICTORY", MB_OK);
    }
    else if (flag == 3)
    {
        string ts = "再接再厉，相信你能赢！";
        MessageBox(GetHWnd(), ts.c_str(), "CAPITULATE", MB_OK);
    }
    else if (flag == 4)
    {
        MessageBox(GetHWnd(), "确定退出？", "QUIT", MB_OK);
    }
    else if (flag == 5)
    {
        string ts = "你已经闯到第" + to_string(levels) + "关";
        MessageBox(GetHWnd(), ts.c_str(), "SAVE", MB_OK);
    }
}
//封面
void tip()
{
    putimage(0, 0, imgs+7);
    putimage(0, 400, imgs + 8);
    settextcolor(LIGHTCYAN);
    settextstyle(24, 0, _T("黑体"));
    outtextxy(280, 360, _T("开始游戏    请按 B/b "));
    outtextxy(280, 410, _T("移动请按    ↑↓←→(上下左右)"));
    outtextxy(280, 460, _T("撤回请按     z或者Z"));
    outtextxy(280, 510, _T("投降请按     c或者C"));
    outtextxy(280, 560, _T("保存请按     s或者S"));
    outtextxy(280, 610, _T("退出请按     q或者Q"));
    outtextxy(280, 660, _T("重玩请按     r或者R"));
    outtextxy(280, 710, _T("选关请按     t或者T"));
    outtextxy(280, 760, _T("! 试试把箱子推到目的地 ！"));
    outtextxy(265, 810, _T("-----23211870310韦乾平-----"));
}
void playgame()
{
    while (true)
    {
        // 先渲染后移动，移动后又渲染
        show();
        draw();
        move();
        judge();
        if (flag) break;
        //清屏
        system("cls");
    }
    draw();
    box();
}
void saveMap()
{
    string filname = "puzzle/save.txt";
    //FILE* file_open=fopen(filname.c_str(), "rb");//转换为c语言风格字符串'\0'结尾的字符数组
    ofstream file_open;
    file_open.open(filname.data());
    assert(file_open.is_open());
    for (int i = 0; i < ROWS; ++i)
    {
        for (int k = 0; k < COLS; ++k)
        {
            char a = map[i][k] + '0';
            file_open << a;
        }
            file_open << endl;
    }
    file_open.close();
}
void initSaveMap()
{
    string filname = "puzzle/save.txt";
    //FILE* file_open=fopen(filname.c_str(), "rb");//转换为c语言风格字符串'\0'结尾的字符数组
    ifstream file_open;
    file_open.open(filname.data());
    assert(file_open.is_open());
    string t;
    int n, i = 0;
    while (getline(file_open, t))
    {
        n = t.length();
        for (int k = 0; k < n; ++k)
        {
            map[i][k] = t[k] - '0';
        }
        i++;
    }
    file_open.close();
}
int main()
{
    // 初始化图形界面
    initgraph(960, 960, EX_SHOWCONSOLE); // 宽度高度,第三个参数让控制台也显示
    char c;
    loadImg_and_mp3();
    tip();
    while (true)
    {
        if (flag == 1 || flag==3 || flag==5)
        {
            tip();
            if(flag!=5) flag = 0;
        }
        c = _getch();
        if (c == 'B' || c == 'b' || c=='R' || c=='r' || c=='T' || c=='t')
        {
            if (c == 'R' || c == 'r') levels = 1;
            else if (c == 'T' || c == 't')
            {
                initgraph(960, 960);

                // 提示用户输入数字
                int number;
                outtextxy(200, 200, _T("****************************************************************"));
                outtextxy(200, 500, _T("*                    请输入选择的关卡(1-42)                    *"));
                outtextxy(200, 800, _T("****************************************************************"));
                
                
                cin >> number;
                levels = number;
                // 暂停等待用户关闭窗口
                closegraph();

                // 关闭图形界面
                initgraph(960, 960, EX_SHOWCONSOLE); // 宽度高度,第三个参数让控制台也显示
                loadImg_and_mp3();
                tip();
            }
            while (levels < 43)
            {
                if (flag == 5)
                {
                    initSaveMap();
                    flag = 0;
                }
                else initMap();
                show();
                playgame();
                if (flag == 1 || flag==3 || flag==5)
                {
                    if (flag == 5)
                    {
                        saveMap();
                    }
                    break;
                }
                else if(flag==2)
                {
                    levels++;
                    memset(map, 0, sizeof(map));
                    flag = 0;
                    top = -1;
                }
                else if(flag==4)
                {
                    closegraph();
                    return 0;
                }
            }
        }
        else if (c == 'Q' || c == 'q')
        {
            MessageBox(GetHWnd(), "确定退出？", "GOOD BYE", MB_OK);
            closegraph();
            return 0;
        }
    }
}