#pragma	once
#ifndef GREEDYSNAKE_H
#define GREEDYSNAKE_H
#include <vector>
#include <map>
#include <iostream>
#include <Windows.h>
using std::vector;
using std::ostream;
using std::map;

enum class keyboard { up_, down_, left_, right_, w_, s_, a_, d_, y_, n_, esc_, other_ };
#define KEYDOWN(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? 1 : 0) 

constexpr int point_o_x = 5;
constexpr int point_o_y = 5;
constexpr int board_width = 50;
constexpr int board_height = 25;

class Point;
class Controller;
class Snake;
class Food;


// 基础类：点
class Point
{
public:
	int x;
	int y;
	Point(int i, int j) :x(i), y(j) {};
	Point() :Point(0, 0) {};
	bool isequal(const Point&);
	//~Point();
};

// 游戏控制终端
class Controller
{
public:
	
	Controller();
	//~Controller();
	void run();

private:
	// 界面设置
	Point point_orgin;		//窗口原点
	int width;				//窗口宽度
	int heigth;				//窗口高度
	HANDLE hOut;			//窗口指针

	// 游戏性设置
	int score;				//得分
	int account;			//已吃的食物数量
	int difficulty;			//难度  
	int stage;				//关卡
	int life;				//生命值

	// 键盘交互
	map<keyboard, int> key_scan;
	keyboard get_the_key();
	   
	//画面输出
	void view_ini();		//启动界面
	void view_board();		//游戏界面
	void view_performance();//玩家表现
	void view_exit();		//退出界面
	void gotoxy(int, int);	//移动光标
	enum draw_mode { new_pic_, clear_, refresh_ };
	void print(const Snake&, Controller::draw_mode);//展示蛇
	void print(const Food&, Controller::draw_mode); //展示食物
};

// 食物
class Food
{
public:
	Food(const Snake&); 
	void refresh(const Snake&);
	Point pos;
};

// 蛇
class Snake
{
public:
	Snake(Point);
	void refresh(Point);
	void move(keyboard&, keyboard&);
	bool eat(const Food&);
	bool die(const Point&, int, int);
	vector<Point> body;
private:
	int length;
};


#endif // !GREEDYSNAKE_H


