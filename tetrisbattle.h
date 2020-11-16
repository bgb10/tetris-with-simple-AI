//
// tetrisbattle.h
// tetrisbattle Header File
//
// Created on 2014. 08. 21.
// Last Modified on 2014. 11. 11.
//
// Copyright(c) 2014 Nerrtica and STKim in CAUCSE. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <float.h>
#include <Windows.h>

#pragma warning (disable : 4996)

#define false 0
#define true 1

#define _DEBUG_
#define DEBUGSTEP 50	
#define DISPLAYSTEP 1
//#define NOPROMPT

// Platform Function

void initialize (char[25][12], char[25][12]);								//�ʱ� ������ �ϴ� �Լ�
void playTurn (char[25][12], char*, int, int, int, int, int, int*, int*);	//���� �����ϴ� �Լ�
void correctError(int, int*, int*);											//AI�� �Է� ������ �����ϴ� �Լ�
int placeBlock (char[25][12], int, int, int);								//����� ��Ʈ���� �ǿ� ���� �Լ�
int placeBlockForUI (char[25][12], int, int, int);							//����� ��Ʈ���� �ǿ� ���� �Լ�, ���� �������� ���� ���� ��ġ�� ���� �絵�� ��.
void deleteCompletedLine (char[25][12], int*, int*);						//�ϼ��� ���� �����ϴ� �Լ�
void checkGameEnd (char[25][12], char*);									//���� ���� ���θ� �Ǻ��ϴ� �Լ�
void doThirdStageRule (char[25][12], char[25][12], int);					//3���������� ���� �����ϴ� �Լ�
int setStage (int);															//���������� �����ϴ� �Լ�
void spawnBlock (int[2][7], int);											//�����ϰ� ����� �����ϴ� �Լ�
void changeTwoToOne (char[25][12]);											//��Ʈ���� ���� 2�� 1�� �ٲٴ� �Լ�

void printTetrisBoard (char[25][12], char[25][12], char, char, int, int);	//ȭ�鿡 ��Ʈ���� ���� ����ϴ� �Լ�


// Player Function

void A_Player (char[25][12], int, int, int, int*, int*);						//A �÷��̾� �Լ�
void player_20190942 (char[25][12], int, int, int, int*, int*);						//B �÷��̾� �Լ�
