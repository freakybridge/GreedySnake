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


// �����ࣺ��
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

// ��Ϸ�����ն�
class Controller
{
public:
	
	Controller();
	//~Controller();
	void run();

private:
	// ��������
	Point point_orgin;		//����ԭ��
	int width;				//���ڿ��
	int heigth;				//���ڸ߶�
	HANDLE hOut;			//����ָ��

	// ��Ϸ������
	int score;				//�÷�
	int account;			//�ѳԵ�ʳ������
	int difficulty;			//�Ѷ�  
	int stage;				//�ؿ�
	int life;				//����ֵ

	// ���̽���
	map<keyboard, int> key_scan;
	keyboard get_the_key();
	   
	//�������
	void view_ini();		//��������
	void view_board();		//��Ϸ����
	void view_performance();//��ұ���
	void view_exit();		//�˳�����
	void gotoxy(int, int);	//�ƶ����
	enum draw_mode { new_pic_, clear_, refresh_ };
	void print(const Snake&, Controller::draw_mode);//չʾ��
	void print(const Food&, Controller::draw_mode); //չʾʳ��
};

// ʳ��
class Food
{
public:
	Food(const Snake&); 
	void refresh(const Snake&);
	Point pos;
};

// ��
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


