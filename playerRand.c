//
// playerRand.c
// Random Player (Level 0)
//
// Created on 2014. 11. 11.
// Last Modified on 2014. 11. 11.
//
// Copyright(c) 2014 Nerrtica and STKim in CAUCSE. All rights reserved.
//

#include "tetrisbattle.h"

int FindMinVerticalBlank(char copy_tetrisBoard[25][12], char tetrisBoard[25][12], int thisBlock, int MaxLocationOfRotationBlock[7][4], int VerticalBlank[4][10], int* location, int* rotation);
int CountVerticalBlank(char copy_tetrisBoard[25][12]);
void RestoreBoard(char copy_tetrisBoard[25][12], char tetrisBoard[25][12]);
void FindMaxHorizontalBlock(char copy_tetrisBoard[25][12], char tetrisBoard[25][12], int thisBlock, int MaxLocationOfRotationBlock[7][4], int VerticalBlank[4][10], int MinVerticalBlank, int* location, int* rotation);
void CountHorizontalBlock(char copy_tetrisBoard[25][12], int TempHorizontalBlock[20]);
void CompareHorizontalBlock(int MaxHorizontalBlock[20], int TempHorizontalBlock[20], int* TempLocation, int* TempRotation, int CurrentLocation, int CurrentRotation);
void ClearBlock(int Block[20]);
//int B_deleteCompletedLine(char tetrisBoard[25][12]);

void player_20190942(char tetrisBoard[25][12], int thisBlock, int nextBlock, int stage, int* location, int* rotation) 
{	
	int MinVerticalBlank = 0; //최소 수직 가중치를 저장하는 변수

	if (tetrisBoard == NULL) {
		printf("학번 : 00000000\tVer 1.0\n");
		return;
	}

	char copy_tetrisBoard[25][12]; //테트리스 판을 복사할 변수. 경우의 수를 다 따져보기 위함.
	int	i, j;

	//테트리스 보드 복사
	for (i = 0; i < 25; i++) {
		for (j = 0; j < 12; j++) {
			copy_tetrisBoard[i][j] = tetrisBoard[i][j];
		}
	}
	
	//블럭이 바깥으로 벗어나지 않도록 블럭의 화전 상태(열)에 따라 0부터 최대 Location을 저장해논 변수.
	int MaxLocationOfRotationBlock[7][4] =
	{
		{6,9,6,9},
		{7,8,7,8},
		{7,8,7,8},
		{8,8,8,8},
		{7,8,7,8},
		{7,8,7,8},
		{7,8,7,8}
	};

	//최소 수직 가중치를 저장해논 변수
	int VerticalBlank[4][10] = //rotation의 범위는 0~3 location의 범위는 최대 0~9
	{
		{0, },
	};

	//최소 수직 가중치를 찾는다.
	MinVerticalBlank = FindMinVerticalBlank(copy_tetrisBoard, tetrisBoard, thisBlock, MaxLocationOfRotationBlock, VerticalBlank, location, rotation);

	//복사한 테트리스 판을 초기화한다.
	RestoreBoard(copy_tetrisBoard, tetrisBoard);

	//최소 수직 가중치를 가지는 상태중에서 최대 수평 가중치를 가지는 경우를 찾는다.
	FindMaxHorizontalBlock(copy_tetrisBoard, tetrisBoard, thisBlock, MaxLocationOfRotationBlock, VerticalBlank, MinVerticalBlank, location, rotation);
	
	//printf("\n\n\n**** rotation: %d location: %d ****\n\n\n", *rotation, *location);
}

/* ----------------------------------------------------------------------------
					    최소 수직 가중치를 반환하는 함수
---------------------------------------------------------------------------- */
int FindMinVerticalBlank(char copy_tetrisBoard[25][12], char tetrisBoard[25][12], int thisBlock, int MaxLocationOfRotationBlock[7][4], int VerticalBlank[4][10], int* location, int* rotation)
{ 
	int MinVerticalBlank = 99999999; //최소 수직 공백 개수가 무조건 초기화되기 위함. 
	int CurrentVerticalBlank = 0;

	for (int CurrentRotation = 0; CurrentRotation <= 3; CurrentRotation++)
	{
		for (int CurrentLocation = 0; CurrentLocation <= MaxLocationOfRotationBlock[thisBlock][CurrentRotation]; CurrentLocation++) //각 블럭과 그 블럭의 rotation상태에 맞게 location값의 범위를 지정
		{
			placeBlock(copy_tetrisBoard, thisBlock, CurrentLocation, CurrentRotation); //복사된 보드에 블럭을 놓는다.
			CurrentVerticalBlank = CountVerticalBlank(copy_tetrisBoard);
			VerticalBlank[CurrentRotation][CurrentLocation] = CurrentVerticalBlank; //수직 가중치를 저장한다.

			if (CurrentVerticalBlank < MinVerticalBlank) MinVerticalBlank = CurrentVerticalBlank; //만약 가중치가 최소면, 최소 가중치를 갱신한다.

			RestoreBoard(copy_tetrisBoard, tetrisBoard); //보드를 초기화한다.
		}
	}
	return MinVerticalBlank; //최소 수직 공백 가중치를 반환한다.
}

/* ----------------------------------------------------------------------------
						   수직 가중치를 반환하는 함수

			블럭을 놓고 나서 그 아래 공백이 있으면 가중치가 누적되기 시작한다.
			공백은 가중치에 3, 블록은 2를 더한다.
---------------------------------------------------------------------------- */
int CountVerticalBlank(char copy_tetrisBoard[25][12])
{
	int count = 0;
	for (int column = 1; column <= 10; column++)
	{
		for (int row = 4; row <= 23; row++) 
		{
			if (copy_tetrisBoard[row][column] == 1 && copy_tetrisBoard[row+1][column] == 0) //Blank가 있으면 (블록O->블록X)
			{
				count++;
				for (int start = row + 2; start <= 23; start++)
				{
					count++;
				}
				break; //다음 column을 검사
			}
		}
	}
	return count;
}

/* ----------------------------------------------------------------------------
			매 경우마다 블록이 놓아진 복사 보드를 초기화하는 함수
---------------------------------------------------------------------------- */
void RestoreBoard(char copy_tetrisBoard[25][12], char tetrisBoard[25][12])
{
	int i, j;
	for (i = 0; i < 25; i++) {
		for (j = 0; j < 12; j++) {
			copy_tetrisBoard[i][j] = tetrisBoard[i][j];
		}
	}
}

/* ----------------------------------------------------------------------------
			최소 수직 가중치를 가지는 여러 경우 중에서
			최대 수평 가중치를 가지는 경우에 블록을 놓는 함수
---------------------------------------------------------------------------- */
void FindMaxHorizontalBlock(char copy_tetrisBoard[25][12], char tetrisBoard[25][12], int thisBlock, int MaxLocationOfRotationBlock[7][4], int VerticalBlank[4][10], int MinVerticalBlank, int* location, int* rotation)
{
	int TempRotation = 0;
	int TempLocation = 0;
	//int NumOfCompletedLine = 0;

	int MaxHorizontalBlock[20] = { 0, }; //최대 수평 가중치를 가지는 경우를 저장
	int TempHorizontalBlock[20] = { 0, }; //테트리스 보드의 아래서부터 위로 채워진 블록의 개수를 센다.

	for (int CurrentRotation = 0; CurrentRotation <= 3; CurrentRotation++)
	{
		for (int CurrentLocation = 0; CurrentLocation <= MaxLocationOfRotationBlock[thisBlock][CurrentRotation]; CurrentLocation++) //각 블럭과 그 블럭의 rotation상태에 맞게 location값의 범위를 지정
		{
			if (MinVerticalBlank == VerticalBlank[CurrentRotation][CurrentLocation]) //만약 수직 최소 가중치와 같은 값을 가지는 블록이라면(최소 1개 존재)
			{
				placeBlock(copy_tetrisBoard, thisBlock, CurrentLocation, CurrentRotation); //복사된 보드에 블럭을 놓는다.

				CountHorizontalBlock(copy_tetrisBoard, TempHorizontalBlock); //수평 가중치를 저장한다.
				CompareHorizontalBlock(MaxHorizontalBlock, TempHorizontalBlock, &TempLocation, &TempRotation, CurrentLocation, CurrentRotation); //저장한 수평 가중치가 최대 수평 가중치인지 비교한다.
				
				ClearBlock(TempHorizontalBlock); //저장한 수평 가중치를 초기화한다.

				RestoreBoard(copy_tetrisBoard, tetrisBoard); //보드를 초기화한다.
			}
		}
	}

	ClearBlock(MaxHorizontalBlock); //최대 수평 가중치를 초기화한다.

	(*rotation) = TempRotation; //최소 수직 가중치중, 최대 수평 가중치를 가진 블럭의 Rotation을 반환.
	(*location) = TempLocation; //최소 수직 가중치중, 최대 수평 가중치를 가진 블럭의 Location을 반환.
}

/* ----------------------------------------------------------------------------
						수평 가중치를 측정하는 함수
---------------------------------------------------------------------------- */
void CountHorizontalBlock(char copy_tetrisBoard[25][12], int TempHorizontalBlock[20])
{
	int height = 0;
	//아래서부터 위로 저장 0~19
	for (int row = 23; row >= 4; row--)
	{
		for (int column = 1; column <= 10; column++)
		{
			if (copy_tetrisBoard[row][column] == 1) TempHorizontalBlock[height]++;
		}
		height++;
	}
}

/* ----------------------------------------------------------------------------
					최대 수평 가중치인지 비교하는 함수
		설명: 보드의 아랫층에 블럭이 많을 수록 좋으므로 아랫층부터 비교한다.
---------------------------------------------------------------------------- */
void CompareHorizontalBlock(int MaxHorizontalBlock[20], int TempHorizontalBlock[20], int* TempLocation, int* TempRotation,  int CurrentLocation, int CurrentRotation)
{
	for (int i = 0; i <=19; i++)
	{
		if (MaxHorizontalBlock[i] > TempHorizontalBlock[i]) //만약 아랫층부터 비교했을 때 Max가 더 크면 반복문 나가기
		{
			break;
		}
		else if (MaxHorizontalBlock[i] < TempHorizontalBlock[i]) //Temp가 더 크면
		{
			//정보를 서로 바꾸기
			(*TempRotation) = CurrentRotation;
			(*TempLocation) = CurrentLocation;
			for (int j = 0; j <= 19; j++)
			{
				MaxHorizontalBlock[j] = TempHorizontalBlock[j];
			}
			break;
		}
		else continue; //계속 반복하기, 같으면 결국 바뀌지 않는다.
	}
}

/* ----------------------------------------------------------------------------
				수평 가중치를 저장한 변수를 초기화하는 함수
---------------------------------------------------------------------------- */
void ClearBlock(int Block[20])
{
	for (int i = 0; i <= 19; i++) Block[i] = 0;
}

/*
int B_deleteCompletedLine(char tetrisBoard[25][12])
{
	int i, j, k, l, numOfCompletedLine = 0;

	for (i = 4; i < 24; i++) {
		for (j = 1; j < 11; j++) {
			if (tetrisBoard[i][j] != 1 && tetrisBoard[i][j] != 2) { break; }
		}
		if (j == 11) {
			numOfCompletedLine++;
			for (k = i; k > 0; k--) {
				for (l = 1; l < 11; l++) {
					tetrisBoard[k][l] = tetrisBoard[k - 1][l];
				}
			}
			for (l = 1; l < 11; l++) {
				tetrisBoard[0][l] = 0;
			}
		}
	}

	return numOfCompletedLine;
}
*/