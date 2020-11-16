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
	int MinVerticalBlank = 0; //�ּ� ���� ����ġ�� �����ϴ� ����

	if (tetrisBoard == NULL) {
		printf("�й� : 00000000\tVer 1.0\n");
		return;
	}

	char copy_tetrisBoard[25][12]; //��Ʈ���� ���� ������ ����. ����� ���� �� �������� ����.
	int	i, j;

	//��Ʈ���� ���� ����
	for (i = 0; i < 25; i++) {
		for (j = 0; j < 12; j++) {
			copy_tetrisBoard[i][j] = tetrisBoard[i][j];
		}
	}
	
	//���� �ٱ����� ����� �ʵ��� ���� ȭ�� ����(��)�� ���� 0���� �ִ� Location�� �����س� ����.
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

	//�ּ� ���� ����ġ�� �����س� ����
	int VerticalBlank[4][10] = //rotation�� ������ 0~3 location�� ������ �ִ� 0~9
	{
		{0, },
	};

	//�ּ� ���� ����ġ�� ã�´�.
	MinVerticalBlank = FindMinVerticalBlank(copy_tetrisBoard, tetrisBoard, thisBlock, MaxLocationOfRotationBlock, VerticalBlank, location, rotation);

	//������ ��Ʈ���� ���� �ʱ�ȭ�Ѵ�.
	RestoreBoard(copy_tetrisBoard, tetrisBoard);

	//�ּ� ���� ����ġ�� ������ �����߿��� �ִ� ���� ����ġ�� ������ ��츦 ã�´�.
	FindMaxHorizontalBlock(copy_tetrisBoard, tetrisBoard, thisBlock, MaxLocationOfRotationBlock, VerticalBlank, MinVerticalBlank, location, rotation);
	
	//printf("\n\n\n**** rotation: %d location: %d ****\n\n\n", *rotation, *location);
}

/* ----------------------------------------------------------------------------
					    �ּ� ���� ����ġ�� ��ȯ�ϴ� �Լ�
---------------------------------------------------------------------------- */
int FindMinVerticalBlank(char copy_tetrisBoard[25][12], char tetrisBoard[25][12], int thisBlock, int MaxLocationOfRotationBlock[7][4], int VerticalBlank[4][10], int* location, int* rotation)
{ 
	int MinVerticalBlank = 99999999; //�ּ� ���� ���� ������ ������ �ʱ�ȭ�Ǳ� ����. 
	int CurrentVerticalBlank = 0;

	for (int CurrentRotation = 0; CurrentRotation <= 3; CurrentRotation++)
	{
		for (int CurrentLocation = 0; CurrentLocation <= MaxLocationOfRotationBlock[thisBlock][CurrentRotation]; CurrentLocation++) //�� ���� �� ���� rotation���¿� �°� location���� ������ ����
		{
			placeBlock(copy_tetrisBoard, thisBlock, CurrentLocation, CurrentRotation); //����� ���忡 ���� ���´�.
			CurrentVerticalBlank = CountVerticalBlank(copy_tetrisBoard);
			VerticalBlank[CurrentRotation][CurrentLocation] = CurrentVerticalBlank; //���� ����ġ�� �����Ѵ�.

			if (CurrentVerticalBlank < MinVerticalBlank) MinVerticalBlank = CurrentVerticalBlank; //���� ����ġ�� �ּҸ�, �ּ� ����ġ�� �����Ѵ�.

			RestoreBoard(copy_tetrisBoard, tetrisBoard); //���带 �ʱ�ȭ�Ѵ�.
		}
	}
	return MinVerticalBlank; //�ּ� ���� ���� ����ġ�� ��ȯ�Ѵ�.
}

/* ----------------------------------------------------------------------------
						   ���� ����ġ�� ��ȯ�ϴ� �Լ�

			���� ���� ���� �� �Ʒ� ������ ������ ����ġ�� �����Ǳ� �����Ѵ�.
			������ ����ġ�� 3, ����� 2�� ���Ѵ�.
---------------------------------------------------------------------------- */
int CountVerticalBlank(char copy_tetrisBoard[25][12])
{
	int count = 0;
	for (int column = 1; column <= 10; column++)
	{
		for (int row = 4; row <= 23; row++) 
		{
			if (copy_tetrisBoard[row][column] == 1 && copy_tetrisBoard[row+1][column] == 0) //Blank�� ������ (���O->���X)
			{
				count++;
				for (int start = row + 2; start <= 23; start++)
				{
					count++;
				}
				break; //���� column�� �˻�
			}
		}
	}
	return count;
}

/* ----------------------------------------------------------------------------
			�� ��츶�� ����� ������ ���� ���带 �ʱ�ȭ�ϴ� �Լ�
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
			�ּ� ���� ����ġ�� ������ ���� ��� �߿���
			�ִ� ���� ����ġ�� ������ ��쿡 ����� ���� �Լ�
---------------------------------------------------------------------------- */
void FindMaxHorizontalBlock(char copy_tetrisBoard[25][12], char tetrisBoard[25][12], int thisBlock, int MaxLocationOfRotationBlock[7][4], int VerticalBlank[4][10], int MinVerticalBlank, int* location, int* rotation)
{
	int TempRotation = 0;
	int TempLocation = 0;
	//int NumOfCompletedLine = 0;

	int MaxHorizontalBlock[20] = { 0, }; //�ִ� ���� ����ġ�� ������ ��츦 ����
	int TempHorizontalBlock[20] = { 0, }; //��Ʈ���� ������ �Ʒ������� ���� ä���� ����� ������ ����.

	for (int CurrentRotation = 0; CurrentRotation <= 3; CurrentRotation++)
	{
		for (int CurrentLocation = 0; CurrentLocation <= MaxLocationOfRotationBlock[thisBlock][CurrentRotation]; CurrentLocation++) //�� ���� �� ���� rotation���¿� �°� location���� ������ ����
		{
			if (MinVerticalBlank == VerticalBlank[CurrentRotation][CurrentLocation]) //���� ���� �ּ� ����ġ�� ���� ���� ������ ����̶��(�ּ� 1�� ����)
			{
				placeBlock(copy_tetrisBoard, thisBlock, CurrentLocation, CurrentRotation); //����� ���忡 ���� ���´�.

				CountHorizontalBlock(copy_tetrisBoard, TempHorizontalBlock); //���� ����ġ�� �����Ѵ�.
				CompareHorizontalBlock(MaxHorizontalBlock, TempHorizontalBlock, &TempLocation, &TempRotation, CurrentLocation, CurrentRotation); //������ ���� ����ġ�� �ִ� ���� ����ġ���� ���Ѵ�.
				
				ClearBlock(TempHorizontalBlock); //������ ���� ����ġ�� �ʱ�ȭ�Ѵ�.

				RestoreBoard(copy_tetrisBoard, tetrisBoard); //���带 �ʱ�ȭ�Ѵ�.
			}
		}
	}

	ClearBlock(MaxHorizontalBlock); //�ִ� ���� ����ġ�� �ʱ�ȭ�Ѵ�.

	(*rotation) = TempRotation; //�ּ� ���� ����ġ��, �ִ� ���� ����ġ�� ���� ���� Rotation�� ��ȯ.
	(*location) = TempLocation; //�ּ� ���� ����ġ��, �ִ� ���� ����ġ�� ���� ���� Location�� ��ȯ.
}

/* ----------------------------------------------------------------------------
						���� ����ġ�� �����ϴ� �Լ�
---------------------------------------------------------------------------- */
void CountHorizontalBlock(char copy_tetrisBoard[25][12], int TempHorizontalBlock[20])
{
	int height = 0;
	//�Ʒ������� ���� ���� 0~19
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
					�ִ� ���� ����ġ���� ���ϴ� �Լ�
		����: ������ �Ʒ����� ���� ���� ���� �����Ƿ� �Ʒ������� ���Ѵ�.
---------------------------------------------------------------------------- */
void CompareHorizontalBlock(int MaxHorizontalBlock[20], int TempHorizontalBlock[20], int* TempLocation, int* TempRotation,  int CurrentLocation, int CurrentRotation)
{
	for (int i = 0; i <=19; i++)
	{
		if (MaxHorizontalBlock[i] > TempHorizontalBlock[i]) //���� �Ʒ������� ������ �� Max�� �� ũ�� �ݺ��� ������
		{
			break;
		}
		else if (MaxHorizontalBlock[i] < TempHorizontalBlock[i]) //Temp�� �� ũ��
		{
			//������ ���� �ٲٱ�
			(*TempRotation) = CurrentRotation;
			(*TempLocation) = CurrentLocation;
			for (int j = 0; j <= 19; j++)
			{
				MaxHorizontalBlock[j] = TempHorizontalBlock[j];
			}
			break;
		}
		else continue; //��� �ݺ��ϱ�, ������ �ᱹ �ٲ��� �ʴ´�.
	}
}

/* ----------------------------------------------------------------------------
				���� ����ġ�� ������ ������ �ʱ�ȭ�ϴ� �Լ�
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