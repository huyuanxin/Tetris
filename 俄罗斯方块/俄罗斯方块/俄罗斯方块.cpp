// 俄罗斯方块-层叠.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "俄罗斯方块-层叠.h"

#define MAX_LOADSTRING 100



#include <cstdlib>
#include <time.h>

#define INFINITION 999

#define TIMER 19950104

#define MAX_LOADSTRING 100
#define GAME_FORM_WIDTH 400
#define GAME_FORM_HEIGHT 680
#define GAME_AREA_LEFT 10
#define GAME_AREA_RIGHT 263
#define GAME_AREA_BOTTOM 430

#define GAME_AREA_TOP 10
#define BC 220

#define MYTIMER 0000001

class Base;
class Block;
Base *ranewShape();

Block *Bp;

// 游戏对象活动区域
POINT areaPoint[4];

// 父类指针指向子类对象
Base *p = ranewShape();
Base *newShape = ranewShape();

// [n][m]：第n行第m个格子的值
int SolidBottomBorder[22][14][4];
int ShapeBlock[5][2];
int si, sj, sk, sn, ssi, ssj, Score;

bool StopStatus;
// 无边框笔
HPEN Hpen, oldHpen;
HBRUSH newBrush;
HGDIOBJ oldBrush;

RECT rcClip;

// 分数矩形
RECT ScoreRect;




class Block
{
public:
	// px，py为坐标
	Block(int px, int py);
	void draw(HDC hdc, int R, int G, int B)
	{
		newBrush = CreateSolidBrush(RGB(R, G, B));
		SelectObject(hdc, newBrush);
		Polygon(hdc, point, 4);
		// HGDIOBJ oldBrush;
		oldBrush = SelectObject(hdc, (HBRUSH)(newBrush));
		SelectObject(hdc, oldBrush);
		DeleteObject(newBrush);
	}
	POINT point[4];
};
Block::Block(int px = 0, int py = 0)
{
	point[0].x = px;  point[0].y = py;
	point[1].x = px + 0;  point[1].y = py + 20;
	point[2].x = px + 20;  point[2].y = py + 20;
	point[3].x = px + 20;  point[3].y = py + 0;
}


// 父类图形
class Base
{
public:
	Base(int px, int py) :x(px), y(py) { }
	void draw(HDC hdc)
	{
		setShape();
		for (int i = 0; i < 4; i++)
		{
			block[i].draw(hdc, R, G, B);
		}
	}
	void draw(HDC hdc, int r, int g, int b)
	{
		for (int i = 0; i < 4; i++)
		{
			block[i].draw(hdc, r, g, b);
		}
	}

	// 向左向右
	// 代码复用
	void goLeft(HDC hdc)
	{
		if (LBorder() - 21 >= GAME_AREA_LEFT)
		{
			invalid(hdc);
			x = x - 21;
			draw(hdc);
		}
	}
	void goRight(HDC hdc)
	{
		if (RBorder() + 21 <= GAME_AREA_RIGHT)
		{
			invalid(hdc);
			x = x + 21;
			draw(hdc);
		}
	}
	void goBottom(HDC hdc)
	{
		if (BBorder() + 21 <= GAME_AREA_BOTTOM)
		{
			invalid(hdc);
			y = y + 21;
			draw(hdc);
		}
	}
	// 向左向右
	// 多态调用
	virtual int LBorder() { return x; };
	virtual int RBorder() { return direction == 0 || direction == 2 ? x + 63 : x + 42; };
	virtual int BBorder() { return direction == 0 || direction == 2 ? y + 42 : y + 63; };

	//virtual void gooLeft(HDC hdc) { };

	virtual void setShapeBlock()
	{
		for (int ii = 1; ii < 5; ii++)
		{
			for (int jj = 0; jj < 2; jj++)
			{
				ShapeBlock[ii][jj] = (jj) == 0 ? (block[ii - 1].point[0].y - 9) / 21 + 1 : (block[ii - 1].point[0].x - 9) / 21 + 1;
			}
		}
	}

	virtual bool checkChange() { return true; };

	void invalid(HDC hdc)
	{
		Hpen = CreatePen(NULL_PEN, 0, RGB(220, 220, 220));
		oldHpen = (HPEN)SelectObject(hdc, Hpen);
		draw(hdc, BC, BC, BC);
		DeleteObject(Hpen);
		SelectObject(hdc, oldHpen);
	}


	virtual void setShape() { };
	virtual void initCenter() { };
	virtual void waitStatus()
	{
		x = 283; y = 40;
	}


	void setDire(int dire)
	{
		direction = dire;
	}
	void changeShape(HDC hdc)
	{
		p->direction++;
		if (p->direction >= 4)
		{
			p->direction = 0;
		}
		p->invalid(hdc);
		p->draw(hdc);
	}
	int R, G, B;
protected:
	Block block[4];
	int direction;
	int x, y;
	int leftBorder, rightBorder;
};


// 条状物I
class I :public Base
{
public:
	I(int dire) :Base(120, 0)
	{
		R = 6; G = 251; B = 253;
		direction = dire;
		setShape();
	}
	I(int px, int py, int dire) :Base(px, py)
	{
		R = 6; G = 251; B = 253;
		direction = dire;
		setShape();
	}
	void initCenter()
	{
		x = 84 + GAME_AREA_LEFT; y = GAME_AREA_TOP;
	}
	bool checkChange()
	{
		p->setShapeBlock();
		switch (direction)
		{
		case 0:
		case 2:
			for (int ii = 0; ii < 4; ii++)
			{
				if (SolidBottomBorder[ShapeBlock[2][0] + ii][ShapeBlock[2][1]][0] != 0)
					return false;
			}
			break;
		case 1:
		case 3:
			for (int ii = 0; ii < 4; ii++)
			{
				if (SolidBottomBorder[ShapeBlock[1][0]][ShapeBlock[1][1] - 1 + ii][0] != 0)
					return false;
			}
			break;
		}
	}
	void waitStatus()
	{
		x = direction == 0 || direction == 2 ? 284 : 295;
		y = direction == 0 || direction == 2 ? 73 : 41;
	}
	int LBorder()
	{
		return direction == 0 || direction == 2 ? x : x + 21;
	}
	int RBorder()
	{
		return direction == 0 || direction == 2 ? x + 84 : x + 42;
	}
	int BBorder()
	{
		return direction == 0 || direction == 2 ? y + 21 : y + 84;
	}
protected:
	void setShape()
	{
		switch (direction)
		{
			// 0横1竖
		case 1:
		case 3:
		{
			for (int i = 0; i < 4; i++)
				block[i] = Block(x + 21, y + i * 21);
		}
		break;
		case 0:
		case 2:
		{
			for (int i = 0; i < 4; i++)
				block[i] = Block(x + i * 21, y);
		}
		break;
		}
	}
};


// 枪状物L
class L :public Base
{
public:
	L(int dire) :Base(120, 0)
	{
		R = 6; G = 253; B = 116;
		direction = dire;
		x = 100 + GAME_AREA_LEFT; y = GAME_AREA_TOP;
		setShape();
	}
	L(int px, int py, int dire) :Base(px, py)
	{
		R = 6; G = 253; B = 116;
		direction = dire;
		setShape();
	}
	void initCenter()
	{
		x = 105 + GAME_AREA_LEFT; y = GAME_AREA_TOP;
	}
	bool checkChange()
	{
		p->setShapeBlock();
		switch (direction)
		{
		case 0:
			for (int ii = 0; ii < 3; ii++)
			{
				if (SolidBottomBorder[ShapeBlock[1][0]][ShapeBlock[1][1] + ii][0] != 0)
					return false;
			}
			break;
		case 1:
			for (int ii = 0; ii < 3; ii++)
			{
				if (SolidBottomBorder[ShapeBlock[2][0] + ii][ShapeBlock[2][1]][0] != 0)
					return false;
			}
			break;
		case 2:
			if (SolidBottomBorder[ShapeBlock[1][0]][ShapeBlock[1][1] + 1][0] != 0)
				return false;
			for (int ii = 0; ii < 3; ii++)
			{
				if (SolidBottomBorder[ShapeBlock[1][0] + 1][ShapeBlock[1][1] + 1 - ii][0] != 0)
					return false;
			}
			break;
		case 3:
			if (SolidBottomBorder[ShapeBlock[1][0] + 1][ShapeBlock[1][1] + 1][0] != 0)
				return false;
			for (int ii = 0; ii < 3; ii++)
			{
				if (SolidBottomBorder[ShapeBlock[1][0] + 1 - ii][ShapeBlock[1][1]][0] != 0)
					return false;
			}
			break;
		}
	}
	int RBorder()
	{
		return direction == 0 || direction == 2 ? x + 42 : x + 63;
	}
	int BBorder()
	{
		return direction == 0 || direction == 2 ? y + 63 : y + 42;
	}
	void waitStatus()
	{
		x = direction == 0 || direction == 2 ? 305 : 295;
		y = direction == 0 || direction == 2 ? 52 : 61;
	}
protected:
	void setShape()
	{
		switch (direction)
		{
		case 0:
			for (int i = 0; i < 3; i++)
				block[i] = Block(x, y + i * 21);
			block[3] = Block(x + 21, y + 2 * 21);
			break;
		case 1:
			for (int i = 0; i < 3; i++)
				block[i] = Block(x + i * 21, y);
			block[3] = Block(x, y + 21);
			break;
		case 2:
			for (int i = 0; i < 3; i++)
				block[i] = Block(x + 21, y + i * 21);
			block[3] = Block(x, y);
			break;
		case 3:
			for (int i = 0; i < 3; i++)
				block[i] = Block(x + i * 21, y + 21);
			block[3] = Block(x + 2 * 21, y);
			break;
		}
	}
};


// 枪状物J
class J :public Base
{
public:
	J(int dire) :Base(120, 0)
	{
		R = 255; G = 255; B = 0;
		direction = dire;
		x = 100 + GAME_AREA_LEFT; y = GAME_AREA_TOP;
		setShape();
	}
	J(int px, int py, int dire) :Base(px, py)
	{
		R = 255; G = 255; B = 0;
		direction = dire;
		setShape();
	}
	void initCenter()
	{
		x = 105 + GAME_AREA_LEFT; y = GAME_AREA_TOP;
	}
	bool checkChange()
	{
		p->setShapeBlock();
		switch (direction)
		{
		case 0:
			if (SolidBottomBorder[ShapeBlock[1][0]][ShapeBlock[1][1] - 1][0] != 0)
				return false;
			for (int ii = 0; ii < 3; ii++)
			{
				if (SolidBottomBorder[ShapeBlock[1][0] + 1][ShapeBlock[1][1] - 1 + ii][0] != 0)
					return false;
			}
			break;
		case 1:
			if (SolidBottomBorder[ShapeBlock[1][0] - 1][ShapeBlock[1][1] + 1][0] != 0)
				return false;
			if (SolidBottomBorder[ShapeBlock[1][0] + 1][ShapeBlock[1][1]][0] != 0)
				return false;
			break;
		case 2:
			if (SolidBottomBorder[ShapeBlock[4][0]][ShapeBlock[4][1] + 1][0] != 0)
				return false;
			if (SolidBottomBorder[ShapeBlock[4][0] + 1][ShapeBlock[4][1] + 1][0] != 0)
				return false;
			break;
		case 3:
			if (SolidBottomBorder[ShapeBlock[4][0]][ShapeBlock[4][1] - 1][0] != 0)
				return false;
			for (int ii = 0; ii < 2; ii++)
			{
				if (SolidBottomBorder[ShapeBlock[4][0] + 1][ShapeBlock[4][1] - 1 - ii][0] != 0)
					return false;
			}
			break;
		}
	}
	int RBorder()
	{
		return direction == 0 || direction == 2 ? x + 42 : x + 63;
	}
	int BBorder()
	{
		return direction == 0 || direction == 2 ? y + 63 : y + 42;
	}
	void waitStatus()
	{
		x = direction == 0 || direction == 2 ? 305 : 295;
		y = direction == 0 || direction == 2 ? 52 : 61;
	}
protected:
	void setShape()
	{
		switch (direction)
		{
		case 0:
			for (int i = 0; i < 3; i++)
				block[i] = Block(x + 21, y + i * 21);
			block[3] = Block(x, y + 2 * 21);
			break;
		case 1:
			for (int i = 0; i < 3; i++)
				block[i] = Block(x + i * 21, y + 21);
			block[3] = Block(x, y);
			break;
		case 2:
			for (int i = 0; i < 3; i++)
				block[i] = Block(x, y + i * 21);
			block[3] = Block(x + 21, y);
			break;
		case 3:
			for (int i = 0; i < 3; i++)
				block[i] = Block(x + i * 21, y);
			block[3] = Block(x + 2 * 21, y + 21);
			break;
		}
	}
};


// 鸭子兽Z
class Z :public Base
{
public:
	Z(int dire) :Base(120, 0)
	{
		R = 168; G = 0; B = 255;
		direction = dire;
		x = 100 + GAME_AREA_LEFT; y = GAME_AREA_TOP;
		setShape();
	}
	Z(int px, int py, int dire) :Base(px, py)
	{
		R = 168; G = 0; B = 255;
		direction = dire;
		setShape();
	}
	void initCenter()
	{
		x = 105 + GAME_AREA_LEFT; y = GAME_AREA_TOP;
	}
	bool checkChange()
	{
		p->setShapeBlock();
		switch (direction)
		{
		case 0:
		case 2:
			for (int ii = 0; ii < 2; ii++)
			{
				if (SolidBottomBorder[ShapeBlock[1][0] + 1 + ii][ShapeBlock[1][1]][0] != 0)
					return false;
			}
			break;
		case 1:
		case 3:
			if (SolidBottomBorder[ShapeBlock[1][0]][ShapeBlock[1][1] - 1][0] != 0)
				return false;
			if (SolidBottomBorder[ShapeBlock[2][0]][ShapeBlock[2][1] + 1][0] != 0)
				return false;
			break;
		}
	}
	void waitStatus()
	{
		x = direction == 0 || direction == 2 ? 295 : 305;
		y = direction == 0 || direction == 2 ? 61 : 52;
	}
protected:
	void setShape()
	{
		switch (direction)
		{
			// 0为正的Z
		case 0:
		case 2:
			for (int i = 0; i < 2; i++)
				block[i] = Block(x + i * 21, y);
			for (int i = 2; i < 4; i++)
				block[i] = Block(x + (i - 1) * 21, y + 21);
			break;
		case 1:
		case 3:
			for (int i = 0; i < 2; i++)
				block[i] = Block(x + 21, y + i * 21);
			for (int i = 2; i < 4; i++)
				block[i] = Block(x, y + (i - 1) * 21);
			break;
		}
	}
};


// 鸭子兽S
class S :public Base
{
public:
	S(int dire) :Base(120, 0)
	{
		R = 0; G = 78; B = 255;
		direction = dire;
		x = 100 + GAME_AREA_LEFT; y = GAME_AREA_TOP;
		setShape();
	}
	S(int px, int py, int dire) :Base(px, py)
	{
		R = 0; G = 78; B = 255;
		direction = dire;
		setShape();
	}
	void initCenter()
	{
		x = 105 + GAME_AREA_LEFT; y = GAME_AREA_TOP;
	}
	bool checkChange()
	{
		p->setShapeBlock();
		switch (direction)
		{
		case 0:
		case 2:
			if (SolidBottomBorder[ShapeBlock[1][0] - 1][ShapeBlock[1][1]][0] != 0)
				return false;
			if (SolidBottomBorder[ShapeBlock[2][0] + 1][ShapeBlock[2][1]][0] != 0)
				return false;
			break;
		case 1:
		case 3:
			for (int ii = 0; ii < 2; ii++)
			{
				if (SolidBottomBorder[ShapeBlock[1][0]][ShapeBlock[1][1] + 1 + ii][0] != 0)
					return false;
			}
			break;
		}
	}
	void waitStatus()
	{
		x = direction == 0 || direction == 2 ? 295 : 305;
		y = direction == 0 || direction == 2 ? 61 : 52;
	}
protected:
	void setShape()
	{
		switch (direction)
		{
		case 0:
		case 2:
			for (int i = 0; i < 2; i++)
				block[i] = Block(x + i * 21, y + 21);
			for (int i = 2; i < 4; i++)
				block[i] = Block(x + (i - 1) * 21, y);
			break;
		case 1:
		case 3:
			for (int i = 0; i < 2; i++)
				block[i] = Block(x, y + i * 21);
			for (int i = 2; i < 4; i++)
				block[i] = Block(x + 21, y + (i - 1) * 21);
			break;
		}
	}
};


// 土字T
class T :public Base
{
public:
	T(int dire) :Base(120, 0)
	{
		R = 255; G = 141; B = 8;
		direction = dire;
		x = 100 + GAME_AREA_LEFT; y = GAME_AREA_TOP;
		setShape();
	}
	T(int px, int py, int dire) :Base(px, py)
	{
		R = 255; G = 141; B = 8;
		direction = dire;
		setShape();
	}
	void initCenter()
	{
		x = 105 + GAME_AREA_LEFT; y = GAME_AREA_TOP;
	}
	bool checkChange()
	{
		p->setShapeBlock();
		switch (direction)
		{
		case 0:
			if (SolidBottomBorder[ShapeBlock[1][0] + 1][ShapeBlock[1][1]][0] != 0)
				return false;
			if (SolidBottomBorder[ShapeBlock[4][0] + 1][ShapeBlock[4][1]][0] != 0)
				return false;
			break;
		case 1:
			if (SolidBottomBorder[ShapeBlock[2][0]][ShapeBlock[2][1] + 1][0] != 0)
				return false;
			break;
		case 2:
			if (SolidBottomBorder[ShapeBlock[1][0] - 1][ShapeBlock[1][1]][0] != 0)
				return false;
			if (SolidBottomBorder[ShapeBlock[1][0] + 1][ShapeBlock[1][1]][0] != 0)
				return false;
			break;
		case 3:
			for (int ii = 0; ii < 2; ii++)
			{
				if (SolidBottomBorder[ShapeBlock[1][0]][ShapeBlock[1][1] + 1 + ii][0] != 0)
					return false;
			}
			break;
		}
	}
	void waitStatus()
	{
		x = direction == 0 || direction == 2 ? 295 : 305;
		y = direction == 0 || direction == 2 ? 61 : 52;
	}
protected:
	void setShape()
	{
		switch (direction)
		{
		case 0:
			for (int i = 0; i < 3; i++)
				block[i] = Block(x + i * 21, y);
			block[3] = Block(x + 21, y + 21);
			break;
		case 1:
			for (int i = 0; i < 3; i++)
				block[i] = Block(x + 21, y + i * 21);
			block[3] = Block(x, y + 21);
			break;
		case 2:
			for (int i = 0; i < 3; i++)
				block[i] = Block(x + i * 21, y + 21);
			block[3] = Block(x + 21, y);
			break;
		case 3:
			for (int i = 0; i < 3; i++)
				block[i] = Block(x, y + i * 21);
			block[3] = Block(x + 21, y + 21);
			break;
		}
	}
};


// 田字O
class O :public Base
{
public:
	O(int dire) :Base(120, 0)
	{
		R = 236; G = 32; B = 52;
		direction = dire;
		x = 100 + GAME_AREA_LEFT; y = GAME_AREA_TOP;
		setShape();
	}
	O(int px, int py, int dire) :Base(px, py)
	{
		R = 236; G = 32; B = 52;
		direction = dire;
		setShape();
	}
	void initCenter()
	{
		x = 105 + GAME_AREA_LEFT; y = GAME_AREA_TOP;
	}
	int RBorder()
	{
		return x + 42;
	}
	int BBorder()
	{
		return y + 42;
	}
	void waitStatus()
	{
		x = direction == 0 || direction == 2 ? 305 : 305;
		y = direction == 0 || direction == 2 ? 61 : 61;
	}
protected:
	void setShape()
	{
		for (int i = 0; i < 2; i++)
			block[i] = Block(x + i * 21, y);
		for (int i = 2; i < 4; i++)
			block[i] = Block(x + (i - 2) * 21, y + 21);
	}
};

void DebugText(HDC hdc);

void ScoreUpdate(HDC hdc)
{
	wchar_t str[10];
	_itow_s(Score, str, 10);
	DrawText(hdc, str, -1, &ScoreRect, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
}

void gameArea(HDC hdc)
{
	DebugText(hdc);

	// 记：
	HBRUSH newBrush = CreateSolidBrush(RGB(220, 220, 220));
	SelectObject(hdc, newBrush);

	POINT waitPoint[4];
	waitPoint[0].x = 280, waitPoint[0].y = 37;
	waitPoint[1].x = 371, waitPoint[1].y = 37;
	waitPoint[2].x = 371, waitPoint[2].y = 128;
	waitPoint[3].x = 280, waitPoint[3].y = 128;
	Polygon(hdc, waitPoint, 4);

	POINT areaPoint[4];
	areaPoint[0].x = GAME_AREA_LEFT - 1, areaPoint[0].y = GAME_AREA_TOP - 1;
	areaPoint[1].x = GAME_AREA_RIGHT - 1, areaPoint[1].y = GAME_AREA_TOP - 1;
	areaPoint[2].x = GAME_AREA_RIGHT - 1, areaPoint[2].y = GAME_AREA_BOTTOM;
	areaPoint[3].x = GAME_AREA_LEFT - 1, areaPoint[3].y = GAME_AREA_BOTTOM;
	Polygon(hdc, areaPoint, 4);

	// 分数区域的矩形
	ScoreRect.left = GAME_AREA_RIGHT + 10; ScoreRect.right = GAME_FORM_WIDTH - 10;
	ScoreRect.top = GAME_AREA_BOTTOM - 280; ScoreRect.bottom = GAME_AREA_BOTTOM - 255;

	ScoreUpdate(hdc);


	p->initCenter();
	//newShape = ranewShape();
	newShape->draw(hdc);

	p->draw(hdc);

	//// 记：
	//HBRUSH newBrush;
	HGDIOBJ oldBrush;
	newBrush = CreateSolidBrush(RGB(220, 220, 220));
	//oldBrush = SelectObject(hdc, (HBRUSH)(newBrush));

	oldBrush = SelectObject(hdc, (HBRUSH)(newBrush));


	SelectObject(hdc, oldBrush);
	DeleteObject(newBrush);
}


// 随机产生图形
Base *ranewShape()
{
	Base *p = new Base(0, 0);
	srand((unsigned)time(NULL));
	int shape_id = (rand() % 7);
	//shape_id = 0;
	int dire_id = (rand() % 4);

	switch (shape_id)
	{
	case 0:
		p = new I(dire_id);
		break;
	case 1:
		p = new L(dire_id);
		break;
	case 2:
		p = new J(dire_id);
		break;
	case 3:
		p = new Z(dire_id);
		break;
	case 4:
		p = new S(dire_id);
		break;
	case 5:
		p = new T(dire_id);
		break;
	case 6:
		p = new O(dire_id);
		break;
	}
	p->waitStatus();
	return p;
}

bool canGoBottom()
{
	if (StopStatus)
		return false;
	p->setShapeBlock();
	for (int ii = 1; ii < 5; ii++)
	{
		if (SolidBottomBorder[ShapeBlock[ii][0] + 1][ShapeBlock[ii][1]][0] != 0)
			return false;
	}
}
bool canGoLeft()
{
	if (StopStatus)
		return false;
	p->setShapeBlock();
	for (int ii = 1; ii < 5; ii++)
	{
		if (SolidBottomBorder[ShapeBlock[ii][0]][ShapeBlock[ii][1] - 1][0] != 0)
			return false;
	}
}
bool canGoRight()
{
	if (StopStatus)
		return false;
	p->setShapeBlock();
	for (int ii = 1; ii < 5; ii++)
	{
		if (SolidBottomBorder[ShapeBlock[ii][0]][ShapeBlock[ii][1] + 1][0] != 0)
			return false;
	}
}

void setBorder()
{
	for (int ii = 1; ii < 5; ii++)
	{
		SolidBottomBorder[ShapeBlock[ii][0]][ShapeBlock[ii][1]][0] = 1;
		SolidBottomBorder[ShapeBlock[ii][0]][ShapeBlock[ii][1]][1] = p->R;
		SolidBottomBorder[ShapeBlock[ii][0]][ShapeBlock[ii][1]][2] = p->G;
		SolidBottomBorder[ShapeBlock[ii][0]][ShapeBlock[ii][1]][3] = p->B;
	}
}

void DebugText(HDC hdc)
{
	POINT waitPoint[4];
	waitPoint[0].x = GAME_AREA_RIGHT + 10, waitPoint[0].y = GAME_AREA_BOTTOM - 30;
	waitPoint[1].x = GAME_FORM_WIDTH - 30, waitPoint[1].y = GAME_AREA_BOTTOM - 30;
	waitPoint[2].x = GAME_FORM_WIDTH - 30, waitPoint[2].y = GAME_AREA_BOTTOM - 5;
	waitPoint[3].x = GAME_AREA_RIGHT + 10, waitPoint[3].y = GAME_AREA_BOTTOM - 5;
	Polygon(hdc, waitPoint, 4);
}

void DragLimit(WINDOWPOS *pWinPos)
{
	if (pWinPos->x < rcClip.left)
		pWinPos->x = rcClip.left;
	if (pWinPos->x > rcClip.right)
		pWinPos->x = rcClip.right;
	if (pWinPos->y < rcClip.top)
		pWinPos->y = rcClip.top;
	if (pWinPos->y > rcClip.bottom)
		pWinPos->y = rcClip.bottom;
}

void EraseFullRow(HDC hdc)
{
	bool isAllOneBlock;
	int cScore = 0;
	for (si = 20; si > 0; si--)
	{
		isAllOneBlock = true;
		for (sj = 1; sj < 13; sj++)
		{
			if (SolidBottomBorder[si][sj][0] != 1)
			{
				isAllOneBlock = false;
				break;
			}
		}
		if (isAllOneBlock)
		{
			for (ssi = si; ssi > 0; ssi--)
			{
				for (ssj = 1; ssj < 13; ssj++)
				{
					SolidBottomBorder[ssi][ssj][0] = SolidBottomBorder[ssi - 1][ssj][0];
					SolidBottomBorder[ssi][ssj][1] = SolidBottomBorder[ssi - 1][ssj][1];
					SolidBottomBorder[ssi][ssj][2] = SolidBottomBorder[ssi - 1][ssj][2];
					SolidBottomBorder[ssi][ssj][3] = SolidBottomBorder[ssi - 1][ssj][3];
					if (bool(SolidBottomBorder[ssi][ssj][0]))
					{
						Hpen = CreatePen(NULL_PEN, 0, RGB(0, 0, 0));
					}
					else
					{
						Hpen = CreatePen(NULL_PEN, 0, RGB(220, 220, 220));
					}
					oldHpen = (HPEN)SelectObject(hdc, Hpen);
					Bp = new Block((ssj - 1)* 21 + 10, (ssi - 1) * 21 + 10);
					Bp->draw(hdc, SolidBottomBorder[ssi][ssj][1], SolidBottomBorder[ssi][ssj][2], SolidBottomBorder[ssi][ssj][3]);
					delete(Bp);
					DeleteObject(Hpen);
					SelectObject(hdc, oldHpen);
				}
			}
			cScore++;
			si++;
		}
	}
	Score += cScore*cScore;
	ScoreUpdate(hdc);

	oldHpen = (HPEN)SelectObject(hdc, Hpen);
	DeleteObject(Hpen);
	SelectObject(hdc, oldHpen);
}

void upgrade(HWND hWnd)
{
	if (Score >= 120)
	{
		KillTimer(hWnd, TIMER);
		SetTimer(hWnd, TIMER, 600, NULL);
	}
	else if (Score >= 100)
	{
		KillTimer(hWnd, TIMER);
		SetTimer(hWnd, TIMER, 700, NULL);
	}
	else if (Score >= 80)
	{
		KillTimer(hWnd, TIMER);
		SetTimer(hWnd, TIMER, 800, NULL);
	}
	else if (Score >= 50)
	{
		KillTimer(hWnd, TIMER);
		SetTimer(hWnd, TIMER, 1000, NULL);
	}
}



// 全局变量:
HINSTANCE hInst;								// 当前实例
TCHAR szTitle[MAX_LOADSTRING];					// 标题栏文本
TCHAR szWindowClass[MAX_LOADSTRING];			// 主窗口类名

// 此代码模块中包含的函数的前向声明:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	// TODO: 在此放置代码。
	MSG msg;
	HACCEL hAccelTable;

	// 初始化全局字符串
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_MY, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// 执行应用程序初始化:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_MY));

	// 主消息循环:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int) msg.wParam;
}



//
//  函数: MyRegisterClass()
//
//  目的: 注册窗口类。
//
//  注释:
//
//    仅当希望
//    此代码与添加到 Windows 95 中的“RegisterClassEx”
//    函数之前的 Win32 系统兼容时，才需要此函数及其用法。调用此函数十分重要，
//    这样应用程序就可以获得关联的
//    “格式正确的”小图标。
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MY));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_MY);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   函数: InitInstance(HINSTANCE, int)
//
//   目的: 保存实例句柄并创建主窗口
//
//   注释:
//
//        在此函数中，我们在全局变量中保存实例句柄并
//        创建和显示主程序窗口。
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // 将实例句柄存储在全局变量中

   hWnd = CreateWindowW(szWindowClass, szTitle, WS_CAPTION | WS_SYSMENU,
		500, 100, GAME_FORM_WIDTH, GAME_FORM_HEIGHT, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  函数: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目的: 处理主窗口的消息。
//
//  WM_COMMAND	- 处理应用程序菜单
//  WM_PAINT	- 绘制主窗口
//  WM_DESTROY	- 发送退出消息并返回
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;
	
	/*HDC hdcBuffer;*/

	switch (message)
	{// 按键表 https://www.cnblogs.com/liwenbin/archive/2012/06/30/2571489.html
	case WM_WINDOWPOSCHANGING:
	{
		WINDOWPOS *pWinPos = (WINDOWPOS *)lParam;
		DragLimit(pWinPos);
	}
	break;
	case WM_TIMER:
	{
		hdc = GetDC(hWnd);

		if (canGoBottom())
		{
			p->goBottom(hdc);
		}
		else
		{
			setBorder();
			delete(p);

			// 消除完整的一行
			EraseFullRow(hdc);

			upgrade(hWnd);

			newShape->invalid(hdc);				// 刷新等待区
			p = newShape;						// 从等待区获取新的图形
			p->initCenter();					// 初始化到游戏区域中间位置
			newShape = ranewShape();			// 获取等待区的新图形
			newShape->draw(hdc);
			p->draw(hdc);
		}
		ReleaseDC(hWnd, hdc);

	}
	break;
	case WM_KEYDOWN:
	{
		switch (wParam)
		{
		case VK_LEFT:
		{
			if (canGoLeft())
			{
				hdc = GetDC(hWnd);
				p->goLeft(hdc);
				ReleaseDC(hWnd, hdc);
			}
		}
		break;
		case VK_RIGHT:
		{
			if (canGoRight())
			{
				hdc = GetDC(hWnd);
				p->goRight(hdc);
				ReleaseDC(hWnd, hdc);
			}
		}
		break;
		case VK_DOWN:
		{
			if (canGoBottom())
			{
				hdc = GetDC(hWnd);
				p->goBottom(hdc);
				ReleaseDC(hWnd, hdc);
			}
		}
		break;
		case VK_RETURN:
		{
			if (p->checkChange() && !StopStatus)
			{
				hdc = GetDC(hWnd);
				p->changeShape(hdc);
				ReleaseDC(hWnd, hdc);
			}
		}
		break;
		}
	}
	break;
	case WM_KEYUP:
	{
		switch (wParam)
		{
		case VK_SPACE:
		{
			if (StopStatus)
			{
				SetTimer(hWnd, TIMER, 1200, NULL);
				StopStatus = false;
			}
			else
			{
				KillTimer(hWnd, TIMER);
				StopStatus = true;
			}
		}
		break;
		}
	}
	break;
	case WM_CREATE:
	{
		Score = 0;
		SystemParametersInfo(SPI_GETWORKAREA, 0, &rcClip, 0);
		rcClip.bottom -= GAME_FORM_HEIGHT;
		rcClip.right -= GAME_FORM_WIDTH;
		SetTimer(hWnd, TIMER, 1200, NULL);
		for (si = 1; si < 21; si++)
		{
			for (sj = 1; sj < 13; sj++)
			{
				SolidBottomBorder[si][sj][0] = 0;
				for (sk = 1; sk < 4; sk++)
					SolidBottomBorder[si][sj][sk] = 220;
			}
		}
		// 两边竖边界
		for (si = 0, sj = 0, sk = 13; si < 21; si++)
		{
			SolidBottomBorder[si][sj][0] = 1;
			SolidBottomBorder[si][sk][0] = 1;
			for (sn = 1; sn < 4; sn++)
			{
				SolidBottomBorder[si][sj][sn] = 220;
				SolidBottomBorder[si][sk][sn] = 220;
			}
		}
		// 底边界
		for (si = 21, sj = 0; sj < 14; sj++)
		{
			SolidBottomBorder[si][sj][0] = 1;
			for (sk = 1; sk < 4; sk++)
				SolidBottomBorder[si][sj][sk] = 220;
		}

		// 上边界
		for (si = 0, sj = 0; sj < 14; sj++)
		{
			SolidBottomBorder[si][sj][0] = 0;
			for (sk = 1; sk < 4; sk++)
				SolidBottomBorder[si][sj][sk] = 220;
		}

	}
	break;
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// 分析菜单选择:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO: 在此添加任意绘图代码...

		// 画游戏可玩区域
		gameArea(hdc);

		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		delete(p);
		KillTimer(hWnd, TIMER);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// “关于”框的消息处理程序。
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}
