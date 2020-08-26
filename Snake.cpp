#include "Snake.h"
#include <numeric>
#include <conio.h>
#include <ctime>
using std::cout;
using std::endl;


Controller::Controller()
{
	//设置基本参数
	point_orgin = Point(point_o_x, point_o_y);		//基准点
	width = board_width;							//宽度
	heigth = board_height;							//高度
	score = account = 0;
	difficulty = stage = 1;
	life = 3;

	//设置界面
	hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_CURSOR_INFO Cursorlnfo;
	GetConsoleCursorInfo(hOut, &Cursorlnfo);//获取控制台光标信息
	Cursorlnfo.bVisible = false;			//隐藏控制台光标信息
	SetConsoleCursorInfo(hOut, &Cursorlnfo);//设置控制台光标信息
	system("mode con cols=100 lines=40");	//设置窗口大小

	//设置键盘
	key_scan = {
		{keyboard::up_, VK_UP},
		{keyboard::down_, VK_DOWN},
		{keyboard::left_, VK_LEFT},
		{keyboard::right_, VK_RIGHT},
		{keyboard::w_, 'W'},
		{keyboard::s_, 'S'},
		{keyboard::a_, 'A'},
		{keyboard::d_, 'D'},
		{keyboard::y_, 'Y'},
		{keyboard::n_, 'N'},
		{keyboard::esc_, VK_ESCAPE}
	};

}
keyboard Controller::get_the_key()
{
	auto it = find_if(key_scan.begin(), key_scan.end(),
		[](map<keyboard, int>::value_type a) -> bool { return KEYDOWN(a.second); });

	if (it == key_scan.cend())
		return keyboard::other_;
	else
		return it->first;
}
void Controller::run()
{
	//初始化界面
	view_ini();
	view_board();

	//初始化游戏元素
	Snake snake({ point_orgin.x + 2, point_orgin.y + heigth / 2 });
	Food food(snake);
	print(snake, new_pic_);
	print(food, new_pic_);

	// 主循环
	keyboard key_prev = keyboard::d_, key_curr;
	while (true)
	{
		// 刷新按键状态 / 判定蛇移动方向
		key_curr = get_the_key();
		if (key_curr != keyboard::esc_)
		{
			print(snake, refresh_);
			snake.move(key_curr, key_prev);
			print(snake, new_pic_);
		}
		else
			view_exit();

		//判定是否吃掉食物
		if (snake.eat(food))
		{
			// 刷新吃掉食物后图像
			food.refresh(snake);
			print(snake, new_pic_);
			print(food, new_pic_);

			// 记录得分
			account++;
			score += difficulty * 10;//分数增加
			if (account % 5 == 0)
			{
				difficulty++;
				stage++;
			}
		}

		//判定死亡
		if (snake.die(point_orgin, width, heigth))
		{
			Sleep(300);
			if (--life)
			{
				//清理旧图像
				print(snake, clear_);
				print(food, clear_);

				//生成新对象
				key_prev = keyboard::d_;
				snake.refresh({ point_orgin.x + 2, point_orgin.y + heigth / 2 });
				food.refresh(snake);

				//刷新游戏界面
				view_board();
				print(snake, new_pic_);
				print(food, new_pic_);
			}
			else
			{
				view_exit();
				break;
			}
		}

		// 统计玩家表现
		view_performance();

		// 刷新界面
		if (difficulty < 10)
			Sleep(200 - difficulty * 20);
		else
			Sleep(10);

	};

}

Snake::Snake(Point base)
{
	length = 5;
	for (int i = 0; i != length; ++i)
	{
		if (i == 0)
			body.push_back(base);
		else
			body.push_back({ body[i - 1].x + 2, body[i - 1].y });
	}
}

void Snake::refresh(Point bp)
{
	Snake temp(bp);
	*this = temp;
}

void Snake::move(keyboard& key_curr, keyboard& key_prev)
{
	// 蛇身处理
	for (auto i = 0; i != body.size() - 1; ++i)
		body[i] = body[i + 1];

	// 蛇头处理
	// 方向判定
	if (key_curr != key_prev && key_curr != keyboard::other_ && key_prev != keyboard::other_)
		key_prev = key_curr;

	// 判定新头部位置
	auto it = body.rbegin();
	switch (key_prev)
	{
	case keyboard::up_: case keyboard::w_:
		it->y -= 1;
		break;

	case keyboard::down_: case keyboard::s_:
		it->y += 1;
		break;

	case keyboard::left_: case keyboard::a_:
		it->x -= 2;
		break;

	case keyboard::right_: case keyboard::d_:
		it->x += 2;
		break;
	}
	return;
}

bool Snake::eat(const Food& f)
{
	if (body.back().isequal(f.pos))
	{
		body.push_back(f.pos);
		length++;
		return true;

	}
	return false;
}

bool Snake::die(const Point& bp, int width, int height)
{
	//判定是否超界
	auto it = body.rbegin();
	if (it->x <= bp.x || it->x >= bp.x + width)
		return true;
	else if (it->y <= bp.y || it->y >= bp.y + height)
		return true;
	else
	{
		for (auto i = 0; i != length - 2; ++i)
			if (body.back().isequal(body[i]) && i != length - 2)
				return true;
	}
	return false;
}


Food::Food(const Snake& s)
{
	//设置随机种子
	//产生食物的条件：1.在游戏窗口内 2.不在蛇的身上
	srand(static_cast<unsigned int>(time(0)));
	int x, y;
	while (1)
	{
		//得到食物的横坐标 / 纵坐标
		x = point_o_x + 2 * (rand() % (board_width / 2 - 2)) + 2;
		y = point_o_y + rand() % (board_height - 2) + 1;
		auto it = find_if(s.body.begin(), s.body.end(),
			[x, y](Point a) -> bool {return a.x == x && a.y == y; });
		if (it != s.body.end())
			continue;
		break;
	}
	pos.x = x, pos.y = y;
}

void Food::refresh(const Snake& s)
{
	Food temp(s);
	*this = temp;
}

void Controller::print(const Snake& s, Controller::draw_mode m)
{
	switch (m)
	{
	case new_pic_:
	{
		//重新作图
		SetConsoleTextAttribute(hOut, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
		for (auto i = s.body.begin(); i != s.body.end(); ++i)
		{
			gotoxy(i->x, i->y);
			if (i == s.body.begin())
				cout << "○";				//打印蛇尾
			else if (i == s.body.end() - 1)
				cout << "¤";				//打印蛇头
			else
				cout << "◎";				//打印蛇身
		}
		return;
	}
	case refresh_:
	{
		//正常刷新
		auto i = s.body.begin();
		gotoxy(i->x, i->y);
		cout << " ";
		return;
	}
	case clear_:
	{
		//清理模式
		for (auto i = s.body.begin(); i != s.body.end(); ++i)
		{
			gotoxy(i->x, i->y);
			cout << " ";
		}
		return;
	}
	}
}
void Controller::print(const Food& f, Controller::draw_mode m)
{
	switch (m)
	{
	case new_pic_:
	{
		//重新作图
		SetConsoleTextAttribute(hOut, FOREGROUND_RED | FOREGROUND_INTENSITY);
		gotoxy(f.pos.x, f.pos.y);
		cout << "⊙";
		return;
	}
	case refresh_:
	{
		return;
	}
	case clear_:
	{
		//清理模式
		gotoxy(f.pos.x, f.pos.y);
		cout << " ";
		return;
	}
	}
}

void Controller::gotoxy(int x, int y)
{
	COORD pos;
	pos.X = x;
	pos.Y = y;
	SetConsoleCursorPosition(hOut, pos);
}

void Controller::view_ini()
{
	SetConsoleTextAttribute(hOut, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
	cout << "\n\n\n\n";
	cout << "          ****      *      *       *          ****     *      *\n";
	cout << "        *　　  *    **     *       *        *      *   *    *\n";
	cout << "        *           * *    *      ***      *           *  *\n";
	cout << "        　***       *  *   *      * *      *           **\n";
	cout << "   　     　　*     *   *  *     *   *     *           *  *\n";
	cout << "   　 　    　　*   *    * *     *****     *           *    *\n";
	cout << "        *　 　 *    *     **    *     *     *　　　*   *      *\n";
	cout << "          ****      *      *   **     **   　 ****     *        *\n";
	SetConsoleTextAttribute(hOut, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
	cout << "\n\n\n\n";
	cout << "               /^\\/^\\ " << endl;
	cout << "             _|__|  O|" << endl;
	cout << "    \\/     /~     \_/  \\  " << endl;
	cout << "     \\____|__________/  \\  " << endl;
	cout << "            \\_______      \\  " << endl;
	cout << "                    `\\     \\                  \\  " << endl;
	cout << "                      |     |                   \\  " << endl;
	cout << "                     /      /                     \\  " << endl;
	cout << "                    /     /                       \\\\  " << endl;
	cout << "                  /      /                         \\ \\  " << endl;
	cout << "                 /     /                            \\  \\  " << endl;
	cout << "               /     /             _----_            \\   \\  " << endl;
	cout << "              /     /           _-~      ~-_         |   |" << endl;
	cout << "             (      (        _-~    _--_    ~-_     _/   |" << endl;
	cout << "              \\      ~-____-~    _-~    ~-_    ~-_-~    /" << endl;
	cout << "                ~-_           _-~          ~-_       _-~" << endl;
	cout << "                   ~--______-~                ~-___-~" << endl;

	gotoxy(12, 34);
	cout << "按任意键继续......";
	while (!_kbhit())
	{
	}
	system("cls");
}

void Controller::view_exit()
{
	system("CLS");
	system("color 0D");
	cout << "\n  　　***        *           **     **       *********";
	cout << "\n  　*　　*       *           **     **       *        ";
	cout << "\n  *　　　       ***         *  *   *  *      *        ";
	cout << "\n  *　　　       * *        *    * *    *     ******** ";
	cout << "\n  *　　***     *   *      *      *      *    *        ";
	cout << "\n  *　　　*     *****      *      *      *    *        ";
	cout << "\n　  *　　*    *     *     *      *      *    *        ";
	cout << "\n　  　**　   **     **    *      *      *    *********";
	cout << "\n\n\n\n\n\n";
	cout << "\n     ***      **       **   **********    ******      ";
	cout << "\n   *　　 *  　 *　　   *　　 *　　　　     *      *    ";
	cout << "\n  *       *     *     *     *             *      *    ";
	cout << "\n  *       *      *    *     ********      ******      ";
	cout << "\n  *       *      *   *      *             *    *      ";
	cout << "\n  *       *       * *       *             *     *     ";
	cout << "\n   *     *        ***       *             *     **    ";
	cout << "\n     ***           *        **********    *      **   ";

	SetConsoleTextAttribute(hOut, FOREGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_INTENSITY);
	gotoxy(point_orgin.x + width / 2, 27);
	cout << "总得分：" << score;
	Sleep(200);
	exit(0);
}

void Controller::view_board()
{
	SetConsoleTextAttribute(hOut, FOREGROUND_BLUE | FOREGROUND_INTENSITY);
	//打印上边界
	gotoxy(point_orgin.x, point_orgin.y);
	cout << "┏";
	for (int i = 0; i < width / 2 - 2; i++)
		cout << "━";
	cout << "┓";
	//打印下边界
	gotoxy(point_orgin.x, point_orgin.y + heigth);
	cout << "┗";
	for (int i = 0; i < width / 2 - 2; i++)
		cout << "━";
	cout << "┛";
	//打印左右边界
	for (int i = 0; i < heigth - 1; i++)
	{
		gotoxy(point_orgin.x, point_orgin.y + 1 + i);
		cout << "┃";
		gotoxy(point_orgin.x + width - 2, point_orgin.y + 1 + i);
		cout << "┃";
	}
	SetConsoleTextAttribute(hOut, FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY);
	gotoxy(point_orgin.x + width + 3, point_orgin.y + 1);
	cout << "按键说明：方向键或WSAD键控制";
	gotoxy(point_orgin.x + width + 3, point_orgin.y + 3);
	cout << "  长按加速";
	gotoxy(point_orgin.x + width + 3, point_orgin.y + 5);
	cout << "  按ESC退出游戏";
}

void Controller::view_performance()
{
	gotoxy(point_orgin.x + 3, point_orgin.y - 2);
	cout << "分数：" << score;
	gotoxy(point_orgin.x + width - 11, point_orgin.y - 2);
	cout << "生命值：" << life;
	gotoxy(point_orgin.x + width - 30, point_orgin.y - 2);
	cout << "等级：" << stage;
}

bool Point::isequal(const Point& p)
{
	if (this->x == p.x && this->y == p.y)
		return true;
	return false;
}