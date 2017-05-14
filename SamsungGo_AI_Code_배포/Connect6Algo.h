// Samsung Go Tournament Form C Connect6Algo Header (g++-4.8.3)

// <--------------- 이 Code를 수정하면  작동하지 않을 수 있습니다 ------------------>

#pragma once
extern int width, height;
extern int terminateAI;
extern unsigned s_time;

extern int isFree(int x, int y);
extern int showBoard(int x, int y);
extern void init();
extern void restart();
extern void mymove(int x[], int y[], int cnt = 2);
extern void opmove(int x[], int y[], int cnt = 2);
extern void block(int, int);
extern int setLine(char *fmt, ...);
extern void domymove(int x[], int y[], int cnt = 2);

void myturn(int cnt = 2);
char info[];

void CreateTable();
int FindThreat(int * line_ptr);
int Convert3toHash(int * line_ptr);
void ConvertHashto3(int hash, int * line_ptr);
int CalTotalThreat(int * x, int * y, int cnt);
void ConvertStone(int * line_ptr, int attacker);
void SearchLine(int x, int y, int horizontal, int vertical, int * line);
void GetInitialBoard();
void UpdateBoard(int x, int y, int stone);
int ReadMyBoard(int a, int b);

struct Struct_Score_Point
{
	double Score_Point;
	int tempX;
	int tempY;
};

void enqueue(Struct_Score_Point point);
bool dequeue(int * x, int * y);
void EmptyQueue();

