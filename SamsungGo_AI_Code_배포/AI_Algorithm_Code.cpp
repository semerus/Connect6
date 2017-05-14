// Samsung Go Tournament Form C (g++-4.8.3)

/*
[���� �� �Լ�]
myturn(int cnt) : �ڽ��� AI �ڵ带 �ۼ��ϴ� ���� �Լ� �Դϴ�.
int cnt (myturn()�Լ��� �Ķ����) : ���� �� �� �־��ϴ��� ���ϴ� ����, cnt�� 1�̸� ���� ���� ��  �� ����  �δ� ��Ȳ(�� ��), cnt�� 2�̸� �� ���� ���� �δ� ��Ȳ(�� ��)
int  x[0], y[0] : �ڽ��� �� ù �� ° ���� x��ǥ , y��ǥ�� ����Ǿ�� �մϴ�.
int  x[1], y[1] : �ڽ��� �� �� �� ° ���� x��ǥ , y��ǥ�� ����Ǿ�� �մϴ�.
void domymove(int x[], int y[], cnt) : �� ������ ��ǥ�� �����ؼ� ���

//int board[BOARD_SIZE][BOARD_SIZE]; �ٵ��� �����Ȳ ��� �־� �ٷλ�� ������. ��, ���������ͷ� ���� �������
// ������ ���� ��ġ�� �ٵϵ��� ������ �ǰ��� ó��.

boolean ifFree(int x, int y) : ���� [x,y]��ǥ�� �ٵϵ��� �ִ��� Ȯ���ϴ� �Լ� (������ true, ������ false)
int showBoard(int x, int y) : [x, y] ��ǥ�� ���� ���� �����ϴ��� �����ִ� �Լ� (1 = �ڽ��� ��, 2 = ����� ��, 3 = ��ŷ)

<-------AI�� �ۼ��Ͻ� ��, ���� �̸��� �Լ� �� ���� ����� �������� �ʽ��ϴ�----->
*/

#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>
#include <time.h>
#include "Connect6Algo.h"
#include <math.h>

char info[] = { "TeamName:Hideout,Department:KU" };


// ------------------------------------------------for tss--------------------------------------------------------------------------
const int TABLESIZE = 177147;
const int BOARDSIZE = 19;
const int PATTERNSIZE = 11;
const int WINDOWSIZE = 6;

int virtualBoard[19][19]; // ���� �м��� ����
int threatTable[TABLESIZE]; // threat ���� ���� ���̺�
bool IsFirstMove = true;

int power[11] = { 59049, 19683, 6561, 2187, 729, 243, 81, 27, 9, 3, 1 };

//-------------------------------------------------for calculating E-Value-------------------------------------------------------
double const dValue[5] = { 0.99999909069 ,1.0f, 1.00000181862, 1.00000363725, 1.00000726562 };
int const wValue[5] = { 4096 ,2048 ,1024 ,512 ,256 };
int const epsilon = 2;
int const border = 1;

int dIndex;
int dModifier;

double scoreBoard[19][19];
double maxE;

int oplastx[2], oplasty[2];
int xNext, yNext;

int player;
//------------------------------------------------------------------------------------------------------------------------------------
// S
// define a struct
int num_entry = 0;
int indicator_array = 0;

Struct_Score_Point * array_struct[19*19];
// 
FILE * fp;


float Search_Side(int tempX, int tempY, int horizontal, int vertical)
{
	float sideValue = 1;

	for (int i = 1; i <= 5; i++)
	{
		if (tempX + (i *horizontal) < 0 || tempY + (i *vertical) < 0
			|| tempX + (i *horizontal) > 18 || tempY + (i *vertical) > 18)
		{
			sideValue *= border;
			dModifier--;
			break;
		}

		int tempBoard = virtualBoard[tempX + (i *horizontal)][tempY + (i *vertical)];

		if (player == 1)
		{
			if (tempBoard == 2 || tempBoard == 3)
			{
				sideValue *= border;
				dModifier--;
				break;
			}
			else if (tempBoard == 0)
			{
				sideValue *= epsilon;
			}
			else if (tempBoard == 1)
			{
				sideValue *= wValue[i - 1];
			}
		}
		else if (player == 2)
		{
			if (tempBoard == 1 || tempBoard == 3)
			{
				sideValue *= border;
				dModifier--;
				break;
			}
			else if (tempBoard == 0)
			{
				sideValue *= epsilon;
			}
			else if (tempBoard == 2)
			{
				sideValue *= wValue[i - 1];
			}
		}


	}
	return sideValue;
}

float Search_Horizontal(int tempX, int tempY)
{
	dModifier = 0;

	float piValue = 1;
	piValue *= Search_Side(tempX, tempY, -1, 0);
	piValue *= Search_Side(tempX, tempY, +1, 0);

	if (dModifier < 0)
		dIndex--;
	return piValue;
}
float Search_Vertical(int tempX, int tempY)
{
	dModifier = 0;

	float piValue = 1;
	piValue *= Search_Side(tempX, tempY, 0, +1);
	piValue *= Search_Side(tempX, tempY, 0, -1);

	if (dModifier < 0)
		dIndex--;
	return piValue;
}
float Search_LeftDiagonal(int tempX, int tempY)
{
	dModifier = 0;

	float piValue = 1;
	piValue *= Search_Side(tempX, tempY, +1, +1);
	piValue *= Search_Side(tempX, tempY, -1, -1);

	if (dModifier < 0)
		dIndex--;
	return piValue;
}
float Search_RightDiagonal(int tempX, int tempY)
{
	dModifier = 0;

	float piValue = 1;
	piValue *= Search_Side(tempX, tempY, +1, -1);
	piValue *= Search_Side(tempX, tempY, -1, +1);

	if (dModifier < 0)
		dIndex--;
	return piValue;
}

double Score_Point(int tempX, int tempY)
{
	double eValue = 0;
	dIndex = 4;

	eValue += Search_Horizontal(tempX, tempY);
	eValue += Search_Vertical(tempX, tempY);
	eValue += Search_RightDiagonal(tempX, tempY);
	eValue += Search_LeftDiagonal(tempX, tempY);

	return eValue * dValue[dIndex];
}
void Score_Matrix(int xLastTemp[], int yLastTemp[], int scanLength, int playerParam)
{
	Struct_Score_Point point;

	player = playerParam;

	int xMax, xMin, yMax, yMin;

	EmptyQueue();
	for (int i = 0; i < 2; i++)
	{
		if (xLastTemp[i] == -1 || yLastTemp[i] == -1)
			continue;
		// break -> continue�� �׽�Ʈ�غ�

		if (xLastTemp[i] <= scanLength)
		{
			xMin = 0;
			xMax = xLastTemp[i] + scanLength;
		}
		else if (xLastTemp[i] >= 18 - scanLength)
		{
			xMin = xLastTemp[i] - scanLength;
			xMax = 18;
		}
		else
		{
			xMin = xLastTemp[i] - scanLength;
			xMax = xLastTemp[i] + scanLength;
		}

		if (yLastTemp[i] <= scanLength)
		{
			yMin = 0;
			yMax = yLastTemp[i] + scanLength;
		}
		else if (yLastTemp[i] >= 18 - scanLength)
		{
			yMin = yLastTemp[i] - scanLength;
			yMax = 18;
		}
		else
		{
			yMin = yLastTemp[i] - scanLength;
			yMax = yLastTemp[i] + scanLength;
		}

		
		// ���� ����쿡 0�� �ƴϸ� ��� �� ���ΰ�? ����ó�� �ʿ���
		for (int tempX = xMin; tempX <= xMax; tempX++)
		{
			for (int tempY = yMin; tempY <= yMax; tempY++)
			{
				if (virtualBoard[tempX][tempY] == 0)
				{
					scoreBoard[tempX][tempY] = Score_Point(tempX, tempY);

					// ddd
					point.Score_Point = Score_Point(tempX, tempY);
					point.tempX = tempX;
					point.tempY = tempY;

					enqueue(&num_entry, point);
				}
			}
		}
	}
}

void Find_MaxE_Position(int tempX, int tempY, int scanLength)
{

	int xMax, xMin, yMax, yMin;

	if (tempX <= scanLength)
	{
		xMin = 0;
		xMax = tempX + scanLength;
	}
	else if (tempX >= 18 - scanLength)
	{
		xMin = tempX - scanLength;
		xMax = 18;
	}
	else
	{
		xMin = tempX - scanLength;
		xMax = tempX + scanLength;
	}

	if (tempY <= scanLength)
	{
		yMin = 0;
		yMax = tempY + scanLength;
	}
	else if (tempY >= 18 - scanLength)
	{
		yMin = tempY - scanLength;
		yMax = 18;
	}
	else
	{
		yMin = tempY - scanLength;
		yMax = tempY + scanLength;
	}


	for (int x = xMin; x <= xMax; x++)
	{
		for (int y = yMin; y <= yMax; y++)
		{
			if (scoreBoard[x][y] > maxE)
			{
				maxE = scoreBoard[x][y];
				xNext = x;
				yNext = y;
			}
			else if (scoreBoard[x][y] == maxE)
			{
				int xDif = abs(x - 9) - abs(xNext - 9);
				int yDif = abs(y - 9) - abs(yNext - 9);

				if ((xDif + yDif) < 0)
				{
					maxE = scoreBoard[x][y];
					xNext = x;
					yNext = y;
				}
			}
		}
	}
}
void Initialize_ScoreBoard()
{
	for (int i = 0; i < 19; i++)
	{
		for (int j = 0; j < 19; j++)
		{
			scoreBoard[i][j] = 0;
			maxE = 0;
		}
	}
}

void myturn(int cnt) {
	int x[2], y[2];

	int curThreat;
	int processThreat;
	int nextx[2], nexty[2]; // ���� ���� ���� �μ��̰� �������� ����


							// ù��° �� �� ��� threat table �ۼ� !! ó���� ����
	if (IsFirstMove) {
		GetInitialBoard();

		CreateTable();
		IsFirstMove = false;

		//fopen_s(&fp, "./log.txt", "wt");
		//fclose(fp);
	}

	
	if (cnt == 1) {

		oplastx[0] = oplasty[0] = 9;
		oplastx[1] = oplasty[1] = -1;

		Initialize_ScoreBoard();
		Score_Matrix(oplastx, oplasty, 9, 1);
		Find_MaxE_Position(oplastx[0], oplasty[0], 9);

		x[0] = xNext;
		y[0] = yNext;

		UpdateBoard(x[0], y[0], 1);
		
		domymove(x, y, cnt);
		return;

	}
	else {
		// ���� ������Ʈ
		for (int i = 0; i < cnt; i++) {
			if (oplastx[i] == -1 || oplasty[i] == -1)
				continue;
			UpdateBoard(oplastx[i], oplasty[i], 2);
		}
	}

	processThreat = CalTotalThreat(oplastx, oplasty, cnt);
	

	for (int i = 0; i < cnt; i++)
	{
		if (oplastx[i] == -1 || oplasty[i] == -1)
		{
			oplastx[1] = oplastx[0];
			oplasty[1] = oplasty[0];
		}

		Initialize_ScoreBoard();
		Score_Matrix(oplastx, oplasty, 2, 2);
		//Find_MaxE_Position(oplastx[i], oplasty[i], 2);

		while (dequeue(&nextx[i], &nexty[i], &num_entry))
		{
			UpdateBoard(nextx[i], nexty[i], 1);

			if (processThreat <= 0) {
				break;
			}

			if (CalTotalThreat(oplastx, oplasty, cnt) < processThreat) {
				processThreat = CalTotalThreat(oplastx, oplasty, cnt);
				break;
			}
			else {
				// ������ ���� ��� 0���� ȸ��
				UpdateBoard(nextx[i], nexty[i], 0);
			}
		}

		

		x[i] = nextx[i];
		y[i] = nexty[i];

		UpdateBoard(x[i], y[i], 1);
		
	}


	//curThreat = CalTotalThreat(oplastx, oplasty, cnt);

	// �� �κп��� �ڽ��� ���� ���� ����Ͻʽÿ�.
	// �ʼ� �Լ� : domymove(x�迭,y�迭,�迭ũ��)
	// ���⼭ �迭ũ��(cnt)�� myturn()�� �Ķ���� cnt�� �״�� �־���մϴ�.

	/*
	for (int i = 0; i < cnt; i++) {
	UpdateBoard(x[i], y[i], 1);
	}
	*/
	domymove(x, y, cnt);
}

// -----------------------------------------------------------���⼭���� �Լ� ����� �Դϴ�---------------------------------------------------------------

// ------------------------�μ��� �߰� �Լ� �κ� ���� 3�Լ� ��ġ�� �κ� Ȯ���� ��
// ù ���� ���� (Blocking ����) ȣ��
void GetInitialBoard() {
	for (int i = 0; i < 19; i++)
	{
		for (int j = 0; j < 19; j++)
		{
			virtualBoard[i][j] = showBoard(i, j);
		}
	}
}

void UpdateBoard(int x, int y, int stone) {
	virtualBoard[x][y] = stone;
}

// ����ó��, ���� �ѹ� ����غ���.
int ReadMyBoard(int x, int y) {
	if (x < 0 || x > 18) {
		return 3;
	}
	if (y < 0 || y > 18) {
		return 3;
	}
	return virtualBoard[x][y];
}

// ------------------------------ ������� -------------------------------------------------------------------

int Convert3toHash(int * line_ptr) {
	int hash = 0;
	for (int i = 0; i < PATTERNSIZE; i++) {
		hash += line_ptr[i] * power[i];
	}
	return hash;
}

void ConvertHashto3(int hash, int * line_ptr) {
	int remainder = hash;
	for (int i = 0; i < PATTERNSIZE; i++) {
		line_ptr[i] = remainder / power[i];
		remainder = remainder % power[i];
	}
}

// threat ���� ���̺� ����
void CreateTable() {
	for (int i = 0; i < TABLESIZE; i++) {
		int line[PATTERNSIZE];
		ConvertHashto3(i, line);
		threatTable[i] = FindThreat(line);
	}
}

// 1���� ����(����)���� threat ã��
int FindThreat(int * line_ptr) {
	int threat = 0;

	// ���ʿ��� ���������� ��ĵ
	for (int i = 0; i < PATTERNSIZE; i++) {
		int count_marked = 0; // ǥ�� ���� ����(3���η� ǥ��)
		int count_one = 0; // 1���� ����(����)
		int count_two = 0; // 2���� ����(����)

		if (i > PATTERNSIZE - WINDOWSIZE)
			break;

		for (int j = i; j < WINDOWSIZE + i; j++) {
			switch (line_ptr[j])
			{
			case 1:
				count_one++;
				break;
			case 2:
				count_two++;
				break;
			case 3:
				count_marked++;
				break;
			case 0:
			default:
				break;
			}
		}

		if (!(count_marked > 0 || count_two > 0) && count_one >= 4) {
			threat++;

			// �� ���� �� ���� ������ �ϳ��� ��ũ
			for (int j = i; j < WINDOWSIZE + i; j++) {
				if (line_ptr[j] == 0) {
					if (++count_one >= 6)
						line_ptr[j] = 3;
				}
			}
		}
	}
	return threat;
}


void SearchLine(int x, int y, int horizontal, int vertical, int * line) {
	for (int i = 0; i < PATTERNSIZE; i++) {
		line[i] = ReadMyBoard(-5 * horizontal + x + i * horizontal, -5 * vertical + y + i * vertical);
	}
}

// ���⼭ �޴� ��ǥ�� ������ �������� �־��� ��
int CalTotalThreat(int * x, int * y, int cnt) {
	int threat = 0;
	int opx[2];
	int opy[2];
	int line_hor[11];
	int line_ver[11];
	int line_diaright[11];
	int line_dialeft[11];

	for (int i = 0; i < cnt; i++) {
		opx[i] = x[i];
		opy[i] = y[i];
	}

	// ���Ͽ� ���� ����
	for (int i = 0; i < cnt; i++) {
		SearchLine(opx[i], opy[i], 1, 0, line_hor);
		SearchLine(opx[i], opy[i], 0, 1, line_ver);
		SearchLine(opx[i], opy[i], 1, -1, line_diaright);
		SearchLine(opx[i], opy[i], 1, 1, line_dialeft);

		ConvertStone(line_hor, 2);
		ConvertStone(line_ver, 2);
		ConvertStone(line_diaright, 2);
		ConvertStone(line_dialeft, 2);

		threat += threatTable[Convert3toHash(line_hor)];
		threat += threatTable[Convert3toHash(line_ver)];
		threat += threatTable[Convert3toHash(line_diaright)];
		threat += threatTable[Convert3toHash(line_dialeft)];

		//fprintf_s(fp, "%d\n", threat);
	}

	if (threat < 1) {
		return threat;
	}

	// threat �ߺ� Ȯ�� �� ���� (���� �� �� ��)
	if (cnt > 1) { //�� �� �̻� Ȯ��
				   // x���� ���� ��
		if (opx[0] == opx[1]) {
			if (opy[0] < opy[1]) {
				for (int i = 1; i <= abs(opy[0] - opy[1]); i++) {
					if (!(ReadMyBoard(opx[0], opy[0] + i) == 3 || ReadMyBoard(opx[0], opy[0] + i) == 1)) {
						if (abs(opy[0] - opy[1]) <= 6) {
							threat = threat / 2;
							break;
						}
						else if (abs(opy[0] - opy[1]) >= 7 && abs(opy[0] - opy[1]) <= 11) {
							threat -= 1;
							break;
						}
					}
				}
			}
			else {
				for (int i = 1; i <= abs(opy[0] - opy[1]); i++) {
					if (!(ReadMyBoard(opx[0], opy[1] + i) == 3 || ReadMyBoard(opx[0], opy[1] + i) == 1)) {
						if (abs(opy[0] - opy[1]) <= 6) {
							threat = threat / 2;
							break;
						}
						else if (abs(opy[0] - opy[1]) >= 7 && abs(opy[0] - opy[1]) <= 11) {
							threat -= 1;
							break;
						}
					}
				}
			}
		}
		// y ���� ���� ��
		else if (opy[0] == opy[1]) {
			if (opx[0] < opx[1]) {
				for (int i = 1; i <= abs(opx[0] - opx[1]); i++) {
					if (!(ReadMyBoard(opx[0] + i, opy[0]) == 3 || ReadMyBoard(opx[0] + i, opy[0]) == 1)) {
						if (abs(opx[0] - opx[1]) <= 6) {
							threat = threat / 2;
							break;
						}
						else if (abs(opx[0] - opx[1]) >= 7 && abs(opx[0] - opx[1]) <= 11) {
							threat -= 1;
							break;
						}
					}
				}
			}
			else {
				for (int i = 1; i <= abs(opx[0] - opx[1]); i++) {
					if (!(ReadMyBoard(opx[1] + i, opy[0]) == 3 || ReadMyBoard(opx[1] + i, opy[0]) == 1)) {
						if (abs(opx[0] - opx[1]) <= 6) {
							threat = threat / 2;
							break;
						}
						else if (abs(opx[0] - opx[1]) >= 7 && abs(opx[0] - opx[1]) <= 11) {
							threat -= 1;
							break;
						}
					}
				}
			}
		}
		// �밢��
		// y = 1x
		else if (abs(opx[0] - opx[1]) == abs(opy[0] - opy[1])) {
			if ((opy[0] - opy[1]) / (opx[0] - opx[1]) > 0) {
				if (opx[0] < opx[1]) {
					for (int i = 1; i <= abs(opx[0] - opx[1]); i++) {
						if (!(ReadMyBoard(opx[0] + i, opy[0] + i) == 3 || ReadMyBoard(opx[0] + i, opy[0] + i) == 1)) {
							if (abs(opx[0] - opx[1]) <= 6) {
								threat = threat / 2;
								break;
							}
							else if (abs(opx[0] - opx[1]) >= 7 && abs(opx[0] - opx[1]) <= 11) {
								threat -= 1;
								break;
							}
						}
					}
				}
				else {
					for (int i = 1; i <= abs(opx[0] - opx[1]); i++) {
						if (!(ReadMyBoard(opx[1] + i, opy[1] + i) == 3 || ReadMyBoard(opx[1] + i, opy[1] + i) == 1)) {
							if (abs(opx[0] - opx[1]) <= 6) {
								threat = threat / 2;
								break;
							}
							else if (abs(opx[0] - opx[1]) >= 7 && abs(opx[0] - opx[1]) <= 11) {
								threat -= 1;
								break;
							}
						}
					}
				}
			}
			// y=-x
			else
			{
				if ((opy[0] - opy[1]) / (opx[0] - opx[1]) > 0) {
					if (opx[0] < opx[1]) {
						for (int i = 1; i <= abs(opx[0] - opx[1]); i++) {
							if (!(ReadMyBoard(opx[0] + i, opy[0] - i) == 3 || ReadMyBoard(opx[0] + i, opy[0] - i) == 1)) {
								if (abs(opx[0] - opx[1]) <= 6) {
									threat = threat / 2;
									break;
								}
								else if (abs(opx[0] - opx[1]) >= 7 && abs(opx[0] - opx[1]) <= 11) {
									threat -= 1;
									break;
								}
							}
						}
					}
					else {
						for (int i = 1; i <= abs(opx[0] - opx[1]); i++) {
							if (!(ReadMyBoard(opx[1] + i, opy[1] - i) == 3 || ReadMyBoard(opx[1] + i, opy[1] - i) == 1)) {
								if (abs(opx[0] - opx[1]) <= 6) {
									threat = threat / 2;
									break;
								}
								else if (abs(opx[0] - opx[1]) >= 7 && abs(opx[0] - opx[1]) <= 11) {
									threat -= 1;
									break;
								}
							}
						}
					}
				}
			}
		}
	}

	return threat;
}

void ConvertStone(int * line_ptr, int attacker) {
	if (attacker == 1) {
		for (int i = 0; i < PATTERNSIZE; i++) {
			if (line_ptr[i] == 3) {
				line_ptr[i] = 2;
			}
		}
	}
	else if (attacker == 2) {
		for (int i = 0; i < PATTERNSIZE; i++) {
			switch (line_ptr[i])
			{
			case 1:
				line_ptr[i] = 2;
				break;
			case 2:
				line_ptr[i] = 1;
				break;
			case 3:
				line_ptr[i] = 2;
				break;
			case 0:
			default:
				break;
			}
		}
	}
}

void enqueue(int * num_entry, Struct_Score_Point point)
{
	Struct_Score_Point * que_struct = (Struct_Score_Point *)malloc(sizeof(Struct_Score_Point));
	Struct_Score_Point * temp;

	que_struct->Score_Point = point.Score_Point;
	que_struct->tempX = point.tempX;
	que_struct->tempY = point.tempY;

	if (*num_entry == 0) {
		array_struct[*num_entry] = que_struct;

	}
	else {
		for (int i = *num_entry - 1; i >= 0; i--)
		{
			fopen_s(&fp, "C:/Users/Admin/Documents/log1.txt", "wt");
			fprintf_s(fp, "%d %d\n", que_struct->tempX, que_struct->tempY);
			fclose(fp);

			if (que_struct->Score_Point > array_struct[i]->Score_Point)
			{
				temp = array_struct[i];
				array_struct[i] = que_struct;
				array_struct[i + 1] = temp;
			}
			else
			{
				break;
			}
		}
	}
	*num_entry++;
}

bool dequeue(int * x, int * y, int * entry)
{
	if (*entry < 1)
		return false;

	*x = array_struct[0]->tempX;
	*y = array_struct[0]->tempY;

	free(array_struct[0]);

	for (int i = 0; i < *entry - 1 ; i++)
	{
		array_struct[i] = array_struct[i + 1];
	}

	*entry--;
	return true;
}

void EmptyQueue()
{
	int x, y;
	while (dequeue(&x, &y, &num_entry))
	{
		;
	}
}