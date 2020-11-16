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

void initialize (char[25][12], char[25][12]);								//초기 설정을 하는 함수
void playTurn (char[25][12], char*, int, int, int, int, int, int*, int*);	//턴을 진행하는 함수
void correctError(int, int*, int*);											//AI의 입력 오류를 수정하는 함수
int placeBlock (char[25][12], int, int, int);								//블록을 테트리스 판에 놓는 함수
int placeBlockForUI (char[25][12], int, int, int);							//블록을 테트리스 판에 놓는 함수, 가장 마지막에 내링 블럭의 위치를 눈에 띄도록 함.
void deleteCompletedLine (char[25][12], int*, int*);						//완성된 줄을 삭제하는 함수
void checkGameEnd (char[25][12], char*);									//게임 종료 여부를 판별하는 함수
void doThirdStageRule (char[25][12], char[25][12], int);					//3스테이지의 룰을 수행하는 함수
int setStage (int);															//스테이지를 설정하는 함수
void spawnBlock (int[2][7], int);											//랜덤하게 블록을 생성하는 함수
void changeTwoToOne (char[25][12]);											//테트리스 판의 2를 1로 바꾸는 함수

void printTetrisBoard (char[25][12], char[25][12], char, char, int, int);	//화면에 테트리스 판을 출력하는 함수


// Player Function

void A_Player (char[25][12], int, int, int, int*, int*);						//A 플레이어 함수
void player_20190942 (char[25][12], int, int, int, int*, int*);						//B 플레이어 함수
