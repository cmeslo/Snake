#include <windows.h>
#include <list>
#include <stdlib.h>
#include <time.h>

#define LEFT 0
#define RIGHT 1
#define UP 2
#define DOWN 3

#define COL_NUM 20
#define ROW_NUM 20
#define BLOCK_SIZE 25

void Game_Init(HWND hwnd);
void SetFood();
void Game_Exit();
void Game_Render(HDC hdc);
bool Game_Update(HWND hwnd);

bool clicked = false;

struct Block
{
	int col;
	int row;

	bool operator== (Block& b) {
		return col == b.col && row == b.row;
	}
	bool operator!= (Block& b) {
		return !((*this) == b);
	}
};

std::list<Block> snake;
Block food;
int direction;
HBRUSH redBrush, orangeBrush, whiteBrush;

inline void Fill_Block(HDC hdc, Block& b);

/*  Declare Windows procedure  */
LRESULT CALLBACK WindowProcedure (HWND, UINT, WPARAM, LPARAM);

/*  Make the class name into a global variable  */


int WINAPI WinMain (HINSTANCE hInstance,
                    HINSTANCE hPrevInstance,
                    LPSTR lpszArgument,
                    int iCmdShow)
					
{
    HWND hwnd;               /* This is the handle for our window */
    MSG msg;            /* Here messages to the application are saved */
    WNDCLASS wndclass;        /* Data structure for the windowclass */
	
	static char szClassName[ ] = "WindowsApp";
	
    /* Step1 */
    wndclass.hInstance = hInstance;
    wndclass.lpszClassName = szClassName;
    wndclass.lpfnWndProc = WindowProcedure;      /* This function is called by windows */
    wndclass.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;                 /* Catch double-clicks */
    wndclass.cbWndExtra = 0;
	wndclass.cbClsExtra = 0;
	wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wndclass.lpszMenuName = NULL;
	
    /* Register the window class, and if it fails quit the program */
    if (!RegisterClass (&wndclass))
        return 0;
	
    /* Step 2 */
	hwnd = CreateWindow(
		szClassName, 
		TEXT("MyApp"), 
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,// initial x position
		CW_USEDEFAULT,// initial y position       
		CW_USEDEFAULT,// initial x size       
		CW_USEDEFAULT,// initial y size
		NULL,
		NULL,
		hInstance,
		NULL
		);
	
    /* Step 3 */
    ShowWindow (hwnd, iCmdShow);
	UpdateWindow(hwnd);
	
    /* Step 4 */
    while (GetMessage (&msg, NULL, 0, 0))
    {
        /* Translate virtual-key messages into character messages */
        TranslateMessage(&msg);
        /* Send message to WindowProcedure */
        DispatchMessage(&msg);
    }
	
    /* The program return-value is 0 - The value that PostQuitMessage() gave */
    return msg.wParam;
}


/*  This function is called by the Windows function DispatchMessage()  */

LRESULT CALLBACK WindowProcedure (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;
	PAINTSTRUCT ps;

    switch (message)                  /* handle the messages */
    {
    case WM_CREATE:
    	Game_Init(hwnd);
    	return 0;
    case WM_PAINT:
    	hdc = BeginPaint(hwnd, &ps);
    	Game_Render(hdc);
    	EndPaint(hwnd, &ps);
    	return 0;
	case WM_TIMER:
		if (wParam == 1) {
			if (!Game_Update(hwnd)) {
				KillTimer(hwnd, 1);
				MessageBox(hwnd, "Game Over! :(", NULL, 0);
				DestroyWindow(hwnd);
			}
		}
		return 0;
	case WM_KEYDOWN:
		if (clicked) return 0;

		clicked = true;
		if (wParam == VK_LEFT && direction != RIGHT) direction = LEFT;
		if (wParam == VK_RIGHT && direction != LEFT) direction = RIGHT;
		if (wParam == VK_UP && direction != DOWN) direction = UP;
		if (wParam == VK_DOWN && direction != UP) direction = DOWN;
		return 0;
	case WM_LBUTTONDOWN:
		MessageBox(hwnd, "Don't touch me!", "Message", 0);
		break;
	case WM_DESTROY:
		Game_Exit();
		PostQuitMessage (0);       /* send a WM_QUIT to the message queue */
		break;
	default:                      /* for messages that we don't deal with */
		return DefWindowProc (hwnd, message, wParam, lParam);
    }
	
    return 0;
}

void Game_Init(HWND hwnd)
{
	srand(time(NULL));

	direction = LEFT;

	Block b;
	b.row = 9;
	b.col = 9;

	// create a snake
	int i;
	for (i = 0; i < 4; ++i)
	{
		snake.push_back(b);
		b.col++;
	}

	SetFood();

	// create some burhses
	HDC hdc = GetDC(hwnd);
	redBrush = (HBRUSH)CreateSolidBrush(RGB(255, 0, 0));
	orangeBrush = (HBRUSH)CreateSolidBrush(RGB(255, 102, 0));
	whiteBrush = (HBRUSH)GetStockObject(WHITE_BRUSH);
	ReleaseDC(hwnd, hdc);

	SetTimer(hwnd, 1, 200, NULL);
}

void SetFood()
{
	while (true) {
		food.row = rand() % ROW_NUM;
		food.col = rand() % COL_NUM;

		// if food is not in snake position, quit loop
		std::list<Block>::iterator it = snake.begin();
		for (; it != snake.end(); ++it)
		{
			if (food == (*it)) {
				break;
			}
		}

		if (it == snake.end()) break;
	}
}

void Game_Exit()
{
	DeleteObject(redBrush);
	DeleteObject(orangeBrush);
}

void Fill_Block(HDC hdc, Block& b)
{
	Rectangle(hdc, b.col * BLOCK_SIZE, b.row * BLOCK_SIZE, (b.col + 1) * BLOCK_SIZE, (b.row + 1) * BLOCK_SIZE);
}

void Game_Render(HDC hdc)
{
	SelectObject(hdc, (HPEN)GetStockObject(NULL_PEN));
	// draw white area
	Rectangle(hdc, 0, 0, COL_NUM * BLOCK_SIZE, ROW_NUM * BLOCK_SIZE);

	// draw snake
	SelectObject(hdc, redBrush);
	std::list<Block>::iterator it = snake.begin();
	for (; it != snake.end(); ++it)
	{
		Fill_Block(hdc, *it);
	}

	// draw food
	SelectObject(hdc, orangeBrush);
	Fill_Block(hdc, food);
}

bool Game_Update(HWND hwnd)
{
	Block head = snake.front();
	switch (direction) {
	case LEFT:
		head.col--;
		if (head.col < 0) return false;
		break;
	case RIGHT:
		head.col++;
		if (head.col >= COL_NUM) return false;
		break;
	case UP:
		head.row--;
		if (head.row < 0) return false;
		break;
	case DOWN:
		head.row++;
		if (head.row >= ROW_NUM) return false;
		break;
	}

	std::list<Block>::iterator it = snake.begin();
	for (; it != snake.end(); it++)
	{
		if (head == *it)
			return false;
	}

	snake.push_front(head);

	HDC hdc = GetDC(hwnd);
	SelectObject(hdc, (HPEN)GetStockObject(NULL_PEN));
	SelectObject(hdc, redBrush);
	Fill_Block(hdc, head);

	if (head == food) {
		SetFood();
		SelectObject(hdc, orangeBrush);
		Fill_Block(hdc, food);
	} else {
		SelectObject(hdc, whiteBrush);
		Block tail = snake.back();
		snake.pop_back();
		Fill_Block(hdc, tail);
	}

	ReleaseDC(hwnd, hdc);

	clicked = false;
	return true;
}
