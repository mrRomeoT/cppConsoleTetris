// Roman Trufanov 2010 KRSU
#include <stdio.h>
#include <conio.h>
#include <time.h>
#include <consoleapi.h>
#include <random>

using namespace std;

void gotoxy(int xpos, int ypos)
{
	COORD scrn;
	scrn.X = xpos - 1;
	scrn.Y = ypos - 1;
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), scrn);
}

void setTextColor(WORD C = 0)
{
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), C);
}

enum arrowKey
{
	UP = 72,
	ESC = 27,
	DOWN = 80,
	LEFT = 75,
	RIGHT = 77,
	SPACE = 32
};

enum brickTypes
{
	I, J, L, O, S, T, Z
};

struct sBrick
{
	brickTypes type;
	short W, H, X, Y, color;
	char ar[4][4];
};

struct sPlayer
{
	int score;
	short scoreFactor;
	char playerName[20];
};

struct sGame
{
	sBrick brick;
	sPlayer player;
	char glass[20][10];
	short nextBrick, nextScore, nextLivel, gameOver, bricksCount, livel;
	double speed, normalSpeed;
};

void createBrick(sGame *);
void rotateBrick(sGame *);
void drawBrick(sGame *, bool);
void moveBrick(sGame *, arrowKey);
void drawLogoAndInterface(sGame *);
void resetGame(sGame *);
void refreshGame(sGame *);
void checkGame(sGame *);
void fillBrick(brickTypes, char(&arr)[4][4]);

int main()
{
	system("cls");
	char CH;
	sGame *tetris = new sGame;
	drawLogoAndInterface(tetris);
	CONSOLE_CURSOR_INFO cci;
	cci.dwSize = 99;
	cci.bVisible = false;
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cci);
	resetGame(tetris);
	createBrick(tetris);
	refreshGame(tetris);
	clock_t timer = clock();
	clock_t control_delay = timer, down_delay = timer;
	while (1)
	{
		timer = clock();
		if (timer >= control_delay)
		{
			if (_kbhit())
			{
				CH = _getch();
				if (CH == UP || CH == DOWN || CH == SPACE || CH == RIGHT || CH == LEFT)
					moveBrick(tetris, (arrowKey)CH);
				if (CH == ESC)
				{
					setTextColor(15);
					gotoxy(14, 2);
					printf("Pause...");
					gotoxy(14, 4), printf("New game -- \"N\"!");
					gotoxy(14, 5);
					SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cci);
					printf("Exit -- \"E\"!");
					gotoxy(14, 6);
					printf("Continue game -- \"Esc\"!");
					while (1)
					{
						char CH = _getch();
						if (CH == 'N' || CH == 'n')
						{
							for (short c = 2; c < 8; c++)
							{
								for (short s = 0; s < 28 / 3; s++)
									printf("   ");
								gotoxy(14, c);
							}
							drawLogoAndInterface(tetris);
							resetGame(tetris);
							break;
						}
						if (CH == 'E' || CH == 'e')
						{
							delete tetris;
							tetris = NULL;
							return 0;
						}
						if (CH == ESC)
						{
							gotoxy(14, 2);
							for (short c = 2; c < 9; c++)
							{
								printf("                          ");
								gotoxy(14, c);
							}
							gotoxy(44, 4);
							for (short c = 2; c < 23; c++)
							{
								printf("                                  ");
								gotoxy(44, c);
							}
							break;
						}
					}
				}
			}
			control_delay = timer + (CLOCKS_PER_SEC / 6000);
		}
		if (timer >= down_delay)
		{
			moveBrick(tetris, DOWN);
			down_delay = (clock_t)(timer + (CLOCKS_PER_SEC * tetris->speed));
		}
	}
	return 0;
}

void createBrick(sGame *game)
{
	srand((unsigned)time(NULL));
	game->brick.color = rand() % 7 + 9;
	game->brick.X = 4;
	game->brick.Y = 0;
	game->brick.type = game->nextBrick < 0 ? (brickTypes)(rand() % 7) : (brickTypes)game->nextBrick;
	game->nextBrick = rand() % 6;

	switch (game->brick.type)
	{
	case I:
		game->brick.W = 4;
		game->brick.H = 1;
		break;
	case O:
		game->brick.W = game->brick.H = 2;
		break;
	default:
		game->brick.W = 3;
		game->brick.H = 2;
	}
	fillBrick(game->brick.type, game->brick.ar);
}

void fillBrick(brickTypes type, char(&arr)[4][4])
{

	for (short i = 0; i < 4; i++)
		for (short j = 0; j < 4; j++)
			arr[i][j] = ' ';
	char s = 2;
	switch (type)
	{
	case I:
		arr[0][0] = arr[0][1] = arr[0][2] = arr[0][3] = s;
		break;
	case J:
		arr[0][0] = arr[1][0] = arr[1][1] = arr[1][2] = s;
		break;
	case L:
		arr[0][2] = arr[1][0] = arr[1][1] = arr[1][2] = s;
		break;
	case O:
		arr[0][0] = arr[0][1] = arr[1][0] = arr[1][1] = s;
		break;
	case S:
		arr[0][1] = arr[0][2] = arr[1][0] = arr[1][1] = s;
		break;
	case T:
		arr[0][1] = arr[1][0] = arr[1][1] = arr[1][2] = s;
		break;
	case Z:
	default:
		arr[0][0] = arr[0][1] = arr[1][1] = arr[1][2] = s;
		break;
	}

}

void rotateBrick(sGame *game) {
	short t = game->brick.W;
	game->brick.W = game->brick.H;
	game->brick.H = t;
	char arT[4][4];
	short arSize = game->brick.W > game->brick.H ? game->brick.W : game->brick.H;
	for (short i = 0; i < arSize; i++)
		for (short j = 0; j < arSize; j++)
			arT[i][j] = game->brick.ar[game->brick.W - j - 1][i];
	for (short i = 0; i < arSize; i++)
		for (short j = 0; j < arSize; j++)
			game->brick.ar[i][j] = arT[i][j];
}

void drawBrick(sGame *game, bool isP = true)
{
	gotoxy(game->brick.X + 2, game->brick.Y + 1);
	setTextColor(isP ? game->brick.color : 8);
	for (short i = 0; i < game->brick.H; i++)
	{
		for (short j = 0; j < game->brick.W; j++)
			if (game->brick.ar[i][j] == 2)
				printf("%c", isP ? 2 : 250);
			else
				gotoxy(game->brick.X + j + 3, game->brick.Y + i + 1);
		gotoxy(game->brick.X + 2, game->brick.Y + i + 2);
	}
}

void moveBrick(sGame *game, arrowKey action) {
	drawBrick(game, false);
	const char ss = (char)250;
	if (action == UP) {
		sGame tmpGame = *game;
		sGame *pTmpGame = &tmpGame;
		rotateBrick(pTmpGame);
		if (pTmpGame->brick.Y + pTmpGame->brick.H > 20) {
			drawBrick(game);
			return;
		}
		if (pTmpGame->brick.X + pTmpGame->brick.W > 10)
			game->brick.X = game->brick.type == I ? 6 : game->brick.X--;

		if (pTmpGame->brick.X + pTmpGame->brick.W > 10)
			game->brick.X = game->brick.type == S || game->brick.type == L ||  game->brick.type == J || game->brick.type == T ? 7 : game->brick.X--;

		for (short i = 0; i < pTmpGame->brick.H; i++)
			for (short j = 0; j < pTmpGame->brick.W; j++)
				if (pTmpGame->brick.ar[i][j] == 2 && game->glass[game->brick.Y + i][game->brick.X + j] != ss)
				{
			game->brick.X = pTmpGame->brick.X;
			drawBrick(game);
			return;
				}
		rotateBrick(game);
		drawBrick(game);
		return;
	}
	if (action == LEFT && game->brick.X > 0)
		game->brick.X--;
	if (action == RIGHT && (game->brick.X + game->brick.W) < 10)
		game->brick.X++;
	if (action == DOWN)
		game->brick.Y++;
	if (action == SPACE)
		game->speed = 0.001;

	for (short i = 0; i < game->brick.H; i++)
		for (short j = 0; j < game->brick.W; j++)
			if (game->brick.ar[i][j] == 2 && game->glass[game->brick.Y + i][game->brick.X + j] != ss)
			{
		if (action == LEFT)
			game->brick.X++;
		if (action == RIGHT)
			game->brick.X--;
		if (action == DOWN)
			checkGame(game);
			}

	drawBrick(game);
}

void resetGame(sGame *game)
{
	gotoxy(2, 1);
	setTextColor(8);
	for (short i = 0; i < 20; i++)
	{
		for (short j = 0; j < 10; j++)
		{
			game->glass[i][j] = (char)250;
			printf("%c", game->glass[i][j]);
		}
		gotoxy(2, 2 + i);
	}
	game->gameOver = game->player.scoreFactor = game->player.score = 0;
	game->bricksCount = 1;
	game->nextScore = 1200;
	refreshGame(game);
}

void checkGame(sGame *game) {
	short i = 0;
	game->speed = game->normalSpeed;
	game->brick.Y--;
	drawBrick(game);
	for (i = 0; i < game->brick.H; i++)
		for (short j = 0; j < game->brick.W; j++)
			if (game->brick.ar[i][j] == 2)
				game->glass[i + game->brick.Y][j + game->brick.X] = game->brick.ar[i][j];
	short checkLine = 0;
	char tmp[20][10];
	for (i = 19; i >= 0; i--) {
		for (short j = 0; j < 10; j++) {
			if (game->glass[i][j] == 2)
				checkLine++;
			if (i == 1 && game->glass[i][j] == 2) {
				game->gameOver = 1;
				refreshGame(game);
				return;
			}
		}
		if (checkLine == 10) {
			for (short n = 0; n < i; n++)
				for (short m = 0; m < 10; m++)
					tmp[n + 1][m] = game->glass[n][m];
			for (short n = 0; n < 10; n++)
				tmp[0][n] = (char)250;
			for (short n = 0; n < i + 1; n++)
				for (short m = 0; m < 10; m++)
					game->glass[n][m] = tmp[n][m];
			i++;
			game->player.scoreFactor++;
			refreshGame(game);
		}
		checkLine = 0;
	}
	createBrick(game);
	game->bricksCount++;
	refreshGame(game);
}

void drawLogoAndInterface(sGame *game) {
	setTextColor(15);
	system("cls");
	gotoxy(1, 3);
	printf(" Level: ");
	game->livel = 1;
	gotoxy(9, 3);
	printf("%d", game->livel);
	char CH;
	while (1) {
		CH = _getch();
		if (CH == UP) {
			gotoxy(9, 3);
			if (game->livel < 13)
				printf("   ");
			gotoxy(9, 3);
			if (game->livel < 13)
				printf("%d", ++game->livel);
		}
		if (CH == DOWN) {
			gotoxy(9, 3);
			if (game->livel > 1)
				printf("   ");
			gotoxy(9, 3);
			if (game->livel > 1)
				printf("%d", --game->livel);
		}
		if (CH == 13)
			break;
	}
	game->nextLivel = game->livel + 1;
	game->normalSpeed = game->speed = 1.1 - game->livel * 0.08;
	gotoxy(1, 5);
	printf("Press any key!");
	_getch();
	system("cls");
	const int v = 21, g = 12;
	char area[v][g];
	for (short x = 0; x < v; x++)
	{
		for (short y = 0; y < g; y++)
		{
			area[x][y] = ' ';
			if (y == 0)
				area[x][y] = (char)179;
			if (y == 11)
				area[x][y] = (char)179;
			if (x == 20)
				area[x][y] = (char)196;
			if (x == 20 && y == 11)
				area[x][y] = (char)217;
			if (x == 20 && y == 0)
				area[x][y] = (char)192;
		}
	}
	setTextColor(15);
	for (short x = 0; x < v; x++)
	{
		for (short y = 0; y < g; y++)
			printf("%c", area[x][y]);
		printf("\n");
	}
}

void refreshGame(sGame *game)
{
	char ar[4][4];
	gotoxy(2, 1);
	setTextColor(8);
	for (short i = 0; i < 20; i++)
	{
		for (short j = 0; j < 10; j++)
			printf("%c", game->glass[i][j]);
		gotoxy(2, 2 + i);
	}
	if (game->gameOver)
	{
		setTextColor(15);
		gotoxy(14, 2);
		printf("Game over!");
		gotoxy(14, 4);
		printf("New game -- \"N\"!");
		gotoxy(14, 5);
		printf("Exit -- \"E\"!");
		gotoxy(46, 9);
		printf("                              ");
		while (1)
		{
			char CH = _getch();
			if (CH == 'N' || CH == 'n')
			{
				for (short c = 2; c < 8; c++)
				{
					for (short s = 0; s < 26 / 3; s++)
						printf("   ");
					gotoxy(14, c);
				}
				drawLogoAndInterface(game);
				resetGame(game);
				break;
			}
			if (CH == 'E' || CH == 'e')
			{
				delete game;
				game = NULL;
				exit(0);
			}
		}
	}
	if (game->player.scoreFactor)
		game->player.score += (game->player.scoreFactor * 200) + 100;
	game->player.scoreFactor = 0;
	if (game->player.score > game->nextScore)
	{
		game->livel++;
		game->nextScore += 1500;
	}
	if (game->livel == game->nextLivel)
	{
		game->normalSpeed -= 0.08;
		game->nextLivel++;
	}
	game->speed = game->normalSpeed;


	fillBrick((brickTypes)game->nextBrick, ar);

	gotoxy(18, 19);
	setTextColor(4);
	for (short i = 0; i < 4; i++)
	{
		for (short j = 0; j < 4; j++)
			printf("%c", ar[i][j]);
		gotoxy(18, 19 + i + 1);
	}
	setTextColor(15);
	gotoxy(14, 11);
	printf("Score: ");
	setTextColor(9);
	printf("%d    ", game->player.score);
	setTextColor(15);
	gotoxy(14, 13);
	printf("Bricks: ");
	setTextColor(14);
	printf("%d   ", game->bricksCount);
	setTextColor(15);
	gotoxy(14, 15);
	printf("Level: ");
	setTextColor(8);
	printf("%d  ", game->livel);
	gotoxy(14, 17);
	setTextColor(15);
	printf("Next brick");
}