//
// TetrisBattlePlatform.c   V1.10
// Main Management Module
//
// Created on 2014. 08. 21.
// Last Modified on 2014. 11.24.
//
// - debugging in correctError()
// - displaying the block stage in ending.
// Copyright(c) 2014 Nerrtica and STKim in CAUCSE. All rights reserved.
//

#include "tetrisbattle.h"


int main () {
	char A_tetrisBoard[25][12], B_tetrisBoard[25][12],						//각 플레이어의 게임판. 실제 크기보다 큰 이유는 블록을 위치시키고 게임종류를 판별하는 데의 편의성을 위함.
		 A_isDead = false, B_isDead = false,								//각 플레이어의 생존 여부
		 select;
	int  thisBlock, nextBlock, numOfBlock = 0,								//현재 블록, 다음 블록, 지금까지 총 블록의 갯수
		 blockBag[2][7],													//최근 7개의 블록
		 stage,																//현재 스테이지
		 A_combo = 0, B_combo = 0,											//각 플레이어의 콤보수
		 A_score = 0, B_score = 0;											//각 플레이어의 점수

	srand((unsigned)time(NULL));
	initialize(A_tetrisBoard, B_tetrisBoard);
	spawnBlock(blockBag, numOfBlock);

	printf("A Player\n");
	A_Player(NULL, 0, 0, 0, NULL, NULL);
	printf("B Player\n");
	A_Player(NULL, 0, 0, 0, NULL, NULL);
	while (true) {
//		system("cls");
		stage = setStage(numOfBlock);
		spawnBlock(blockBag, numOfBlock);
		thisBlock = blockBag[0][numOfBlock % 7];
		nextBlock = blockBag[0][numOfBlock % 7 + 1];

		if (stage == 3) { doThirdStageRule(A_tetrisBoard, B_tetrisBoard, numOfBlock); }			//3스테이지일 경우 3번째 규칙을 적용한다
		if (!A_isDead) { playTurn(A_tetrisBoard, &A_isDead, 0, thisBlock, nextBlock, stage, numOfBlock, &A_combo, &A_score); }
		if (!B_isDead) { playTurn(B_tetrisBoard, &B_isDead, 1, thisBlock, nextBlock, stage, numOfBlock, &B_combo, &B_score); }

/* DEBUG 모드일 경우, 화면에 일정 간격으로 각 플레이어의 테트리스 판 상태를 출력한다. */
#ifdef _DEBUG_
		Sleep(100);
		if ((numOfBlock + 1) % DISPLAYSTEP == 0) {
			printTetrisBoard(A_tetrisBoard, B_tetrisBoard, A_isDead, B_isDead, A_score, B_score);
			printf("현재 진행 상황 : 블록 %d개 진행중\n", numOfBlock + 1);
			printf("다음 블록 값 : %d\n", nextBlock);
			if ((numOfBlock + 1) % DEBUGSTEP == 0) {
#ifndef NOPROMPT
				do {
					printf("계속하려면 y를 입력해주세요 : ");
					scanf("%c", &select);
					fflush(stdin);
				} while (select != 'y');
#endif
			}
		}
#endif


		if ((A_isDead && B_isDead || numOfBlock == 999) || (A_isDead && B_score > A_score) || (B_isDead && A_score > B_score)) {
			printTetrisBoard(A_tetrisBoard, B_tetrisBoard, A_isDead, B_isDead, A_score, B_score);
			printf("블록 %d개에서 게임이 종료되었습니다.\n", numOfBlock + 1);
			if (A_score > B_score) { printf("A플레이어 승! B플레이어 패!\n"); }
			else if (B_score > A_score) { printf("A플레이어 패! B플레이어 승!\n"); }
			else { printf("무승부입니다.\n"); }
			break;
		}
		changeTwoToOne(A_tetrisBoard);
		changeTwoToOne(B_tetrisBoard);
		numOfBlock++;
	}
	scanf("%c", &select);
	return 0;
}

/* 초기 설정을 하는 함수 */
void initialize (char A_tetrisBoard[25][12], char B_tetrisBoard[25][12])
{
	int i;

	memset(A_tetrisBoard, 0, sizeof(char) * 25 * 12);
	memset(B_tetrisBoard, 0, sizeof(char) * 25 * 12);

	for (i = 4; i < 25; i++) {
		A_tetrisBoard[i][0] = 1;
		A_tetrisBoard[i][11] = 1;
		B_tetrisBoard[i][0] = 1;
		B_tetrisBoard[i][11] = 1;
	}
	for (i = 0; i < 12; i++) {
		A_tetrisBoard[24][i] = 1;
		B_tetrisBoard[24][i] = 1;
	}
}

/* 턴을 진행하는 함수 */
void playTurn (char tetrisBoard[25][12], char* isDead, int order, int thisBlock, int nextBlock, int stage, int numOfBlock, int* combo, int* score)
{
	char boardCopy[25][12];													//플레이어의 테트리스 판을 복사해두는 배열 (AI함수의 직접 접근을 막기 위함)
	int	 location, rotation,												//블록의 위치와 회전값
		 i, j;

	for (i = 0; i < 25; i++) {
		for (j = 0; j < 12; j++) {
			boardCopy[i][j] = tetrisBoard[i][j];
		}
	}

	if (stage >= 2) { nextBlock = -1; }										//2스테이지 이상 진행중이라면 nextBlock의 값을 -1로 바꾼다. (nextBlock을 알려주지 않음)

	if (order == 0) {
		printf("A Start ");
		A_Player(boardCopy, thisBlock, nextBlock, stage, &location, &rotation);
		printf("A End ");
	} else if (order == 1) {
		printf("B Start ");
		player_20190942(boardCopy, thisBlock, nextBlock, stage, &location, &rotation);
		printf("B End\n");
	}

	location++;

	correctError(thisBlock, &location, &rotation);
	placeBlockForUI(tetrisBoard, thisBlock, location, rotation);
	deleteCompletedLine(tetrisBoard, combo, score);
	checkGameEnd(tetrisBoard, isDead);
}

/* AI의 입력 오류를 수정하는 함수 */
void correctError (int thisBlock, int* location, int* rotation)
{
	/* 회전 값이 0 ~ 3을 벗어났을 경우 */
	if(*rotation < 0) {
		*rotation = 0;
	} else if(*rotation > 3) {
		*rotation = 3;
	}

	/* 위치 값이 게임 판을 벗어났을 경우 */
	if (*location < 1) {
		*location = 1;
	} else if (thisBlock == 0) {
		if ((*rotation == 0 || *rotation == 2) && *location > 7)
			*location = 7;

		else if ((*rotation == 1 || *rotation == 3) && *location > 10)
			*location = 10;

	} else if (thisBlock != 3 && (*rotation == 0 || *rotation == 2) && *location > 8) {
		*location = 8;
	} else if (*location > 9) {
		*location = 9;
	}
}

/* 블록을 테트리스 판에 놓는 함수 */
int placeBlock (char tetrisBoard[25][12], int thisBlock, int location, int rotation) {
	int i, j;

	/* 블록이 바닥에 부딪힐 때까지 한 줄씩 내린다.
	 * 바닥에 부딪혔다면 그 자리의 배열에 블록을 저장하고 함수를 종료한다.
	 * */
	location++;

	switch (thisBlock) {
	case 0:
		if (rotation == 0 || rotation == 2) {
			for (i = 0; i < 24; i++) {
				for (j = 0; j < 4; j++) {
					if (tetrisBoard[i + 1][location + j] == 1) {
						break;
					}
				}
				if (j != 4) {
					for (j = 0; j < 4; j++) {
						tetrisBoard[i][location + j] = 1;
					}
					return i;
				}
			}
		} else if (rotation == 1 || rotation == 3) {
			for (i = 0; i < 24; i++) {
				if (tetrisBoard[i + 4][location] == 1) {
					for (j = 0; j < 4; j++) {
						tetrisBoard[i + j][location] = 1;
					}
					return i;
				}
			}
		}
		break;

	case 1:
		if (rotation == 0) {
			for (i = 0; i < 24; i++) {
				for (j = 0; j < 3; j++) {
					if (tetrisBoard[i + 2][location + j] == 1) {
						break;
					}
				}
				if (j != 3) {
					for (j = 0; j < 3; j++) {
						tetrisBoard[i + 1][location + j] = 1;
					}
					tetrisBoard[i][location] = 1;
					return i;
				}
			}
		} else if (rotation == 1) {
			for (i = 0; i < 24; i++) {
				if (tetrisBoard[i + 3][location] == 1 || tetrisBoard[i + 1][location + 1] == 1) {
					for (j = 0; j < 3; j++) {
						tetrisBoard[i + j][location] = 1;
					}
					tetrisBoard[i][location + 1] = 1;
					return i;
				}
			}
		} else if (rotation == 2) {
			for (i = 0; i < 24; i++) {
				if (tetrisBoard[i + 1][location] == 1 || tetrisBoard[i + 1][location + 1] == 1 || tetrisBoard[i + 2][location + 2] == 1) {
					for (j = 0; j < 3; j++) {
						tetrisBoard[i][location + j] = 1;
					}
					tetrisBoard[i + 1][location + 2] = 1;
					return i;
				}
			}
		} else if (rotation == 3) {
			for (i = 0; i < 24; i++) {
				if (tetrisBoard[i + 3][location] == 1 || tetrisBoard[i + 3][location + 1] == 1) {
					for (j = 0; j < 3; j++) {
						tetrisBoard[i + j][location + 1] = 1;
					}
					tetrisBoard[i + 2][location] = 1;
					return i;
				}
			}
		}
		break;

	case 2:
		if (rotation == 0) {
			for (i = 0; i < 24; i++) {
				for (j = 0; j < 3; j++) {
					if (tetrisBoard[i + 2][location + j] == 1) {
						break;
					}
				}
				if (j != 3) {
					for (j = 0; j < 3; j++) {
						tetrisBoard[i + 1][location + j] = 1;
					}
					tetrisBoard[i][location + 2] = 1;
					return i;
				}
			}
		} else if (rotation == 1) {
			for (i = 0; i < 24; i++) {
				if (tetrisBoard[i + 3][location] == 1 || tetrisBoard[i + 3][location + 1] == 1) {
					for (j = 0; j < 3; j++) {
						tetrisBoard[i + j][location] = 1;
					}
					tetrisBoard[i + 2][location + 1] = 1;
					return i;
				}
			}
		} else if (rotation == 2) {
			for (i = 0; i < 24; i++) {
				if (tetrisBoard[i + 2][location] == 1 || tetrisBoard[i + 1][location + 1] == 1 || tetrisBoard[i + 1][location + 2] == 1) {
					for (j = 0; j < 3; j++) {
						tetrisBoard[i][location + j] = 1;
					}
					tetrisBoard[i + 1][location] = 1;
					return i;
				}
			}
		} else if (rotation == 3) {
			for (i = 0; i < 24; i++) {
				if (tetrisBoard[i + 1][location] == 1 || tetrisBoard[i + 3][location + 1] == 1) {
					for (j = 0; j < 3; j++) {
						tetrisBoard[i + j][location + 1] = 1;
					}
					tetrisBoard[i][location] = 1;
					return i;
				}
			}
		}
		break;

	case 3:
		for (i = 0; i < 24; i++) {
			if (tetrisBoard[i + 2][location] == 1 || tetrisBoard[i + 2][location + 1] == 1) {
				for (j = 0; j < 2; j++) {
					tetrisBoard[i + j][location] = 1;
					tetrisBoard[i + j][location + 1] = 1;
				}
				return i;
			}
		}
		break;

	case 4:
		if (rotation == 0 || rotation == 2) {
			for (i = 0; i < 24; i++) {
				if (tetrisBoard[i + 2][location] == 1 || tetrisBoard[i + 2][location + 1] == 1 || tetrisBoard[i + 1][location + 2] == 1) {
					for (j = 0; j < 2; j++) {
						tetrisBoard[i][location + j + 1] = 1;
						tetrisBoard[i + 1][location + j] = 1;
					}
					return i;
				}
			}
		} else if (rotation == 1 || rotation == 3) {
			for (i = 0; i < 24; i++) {
				if (tetrisBoard[i + 2][location] == 1 || tetrisBoard[i + 3][location + 1] == 1) {
					for (j = 0; j < 2; j++) {
						tetrisBoard[i + j][location] = 1;
						tetrisBoard[i + j + 1][location + 1] = 1;
					}
					return i;
				}
			}
		}
		break;

	case 5:
		if (rotation == 0) {
			for (i = 0; i < 24; i++) {
				for (j = 0; j < 3; j++) {
					if (tetrisBoard[i + 2][location + j] == 1) {
						break;
					}
				}
				if (j != 3) {
					for (j = 0; j < 3; j++) {
						tetrisBoard[i + 1][location + j] = 1;
					}
					tetrisBoard[i][location + 1] = 1;
					return i;
				}
			}
		} else if (rotation == 1) {
			for (i = 0; i < 24; i++) {
				if (tetrisBoard[i + 3][location] == 1 || tetrisBoard[i + 2][location + 1] == 1) {
					for (j = 0; j < 3; j++) {
						tetrisBoard[i + j][location] = 1;
					}
					tetrisBoard[i + 1][location + 1] = 1;
					return i;
				}
			}
		} else if (rotation == 2) {
			for (i = 0; i < 24; i++) {
				if (tetrisBoard[i + 1][location] == 1 || tetrisBoard[i + 2][location + 1] == 1 || tetrisBoard[i + 1][location + 2] == 1) {
					for (j = 0; j < 3; j++) {
						tetrisBoard[i][location + j] = 1;
					}
					tetrisBoard[i + 1][location + 1] = 1;
					return i;
				}
			}
		} else if (rotation == 3) {
			for (i = 0; i < 24; i++) {
				if (tetrisBoard[i + 2][location] == 1 || tetrisBoard[i + 3][location + 1] == 1) {
					for (j = 0; j < 3; j++) {
						tetrisBoard[i + j][location + 1] = 1;
					}
					tetrisBoard[i + 1][location] = 1;
					return i;
				}
			}
		}
		break;

	case 6:
		if (rotation == 0 || rotation == 2) {
			for (i = 0; i < 24; i++) {
				if (tetrisBoard[i + 1][location] == 1 || tetrisBoard[i + 2][location + 1] == 1 || tetrisBoard[i + 2][location + 2] == 1) {
					for (j = 0; j < 2; j++) {
						tetrisBoard[i][location + j] = 1;
						tetrisBoard[i + 1][location + j + 1] = 1;
					}
					return i;
				}
			}
		} else if (rotation == 1 || rotation == 3) {
			for (i = 0; i < 24; i++) {
				if (tetrisBoard[i + 3][location] == 1 || tetrisBoard[i + 2][location + 1] == 1) {
					for (j = 0; j < 2; j++) {
						tetrisBoard[i + j + 1][location] = 1;
						tetrisBoard[i + j][location + 1] = 1;
					}
					return i;
				}
			}
		}
		break;
	}
	return i;
}

/* 블록을 테트리스 판에 놓는 함수, 가장 마지막에 내링 블럭의 위치를 눈에 띄도록 함. */
int placeBlockForUI (char tetrisBoard[25][12], int thisBlock, int location, int rotation)
{
	int i, j;

	/* 블록이 바닥에 부딪힐 때까지 한 줄씩 내린다.
	 * 바닥에 부딪혔다면 그 자리의 배열에 블록을 저장하고 함수를 종료한다.
	 * */
	switch (thisBlock) {
	case 0:
		if (rotation == 0 || rotation == 2) {
			for (i = 0; i < 24; i++) {
				for (j = 0; j < 4; j++) {
					if (tetrisBoard[i + 1][location + j] == 1) {
						break;
					}
				}
				if (j != 4) {
					for (j = 0; j < 4; j++) {
						tetrisBoard[i][location + j] = 2;
					}
					return i;
				}
			}
		} else if (rotation == 1 || rotation == 3) {
			for (i = 0; i < 24; i++) {
				if (tetrisBoard[i + 4][location] == 1) {
					for (j = 0; j < 4; j++) {
						tetrisBoard[i + j][location] = 2;
					}
					return i;
				}
			}
		}
		break;

	case 1:
		if (rotation == 0) {
			for (i = 0; i < 24; i++) {
				for (j = 0; j < 3; j++) {
					if (tetrisBoard[i + 2][location + j] == 1) {
						break;
					}
				}
				if (j != 3) {
					for (j = 0; j < 3; j++) {
						tetrisBoard[i + 1][location + j] = 2;
					}
					tetrisBoard[i][location] = 2;
					return i;
				}
			}
		} else if (rotation == 1) {
			for (i = 0; i < 24; i++) {
				if (tetrisBoard[i + 3][location] == 1 || tetrisBoard[i + 1][location + 1] == 1) {
					for (j = 0; j < 3; j++) {
						tetrisBoard[i + j][location] = 2;
					}
					tetrisBoard[i][location + 1] = 2;
					return i;
				}
			}
		} else if (rotation == 2) {
			for (i = 0; i < 24; i++) {
				if (tetrisBoard[i + 1][location] == 1 || tetrisBoard[i + 1][location + 1] == 1 || tetrisBoard[i + 2][location + 2] == 1) {
					for (j = 0; j < 3; j++) {
						tetrisBoard[i][location + j] = 2;
					}
					tetrisBoard[i + 1][location + 2] = 2;
					return i;
				}
			}
		} else if (rotation == 3) {
			for (i = 0; i < 24; i++) {
				if (tetrisBoard[i + 3][location] == 1 || tetrisBoard[i + 3][location + 1] == 1) {
					for (j = 0; j < 3; j++) {
						tetrisBoard[i + j][location + 1] = 2;
					}
					tetrisBoard[i + 2][location] = 2;
					return i;
				}
			}
		}
		break;

	case 2:
		if (rotation == 0) {
			for (i = 0; i < 24; i++) {
				for (j = 0; j < 3; j++) {
					if (tetrisBoard[i + 2][location + j] == 1) {
						break;
					}
				}
				if (j != 3) {
					for (j = 0; j < 3; j++) {
						tetrisBoard[i + 1][location + j] = 2;
					}
					tetrisBoard[i][location + 2] = 2;
					return i;
				}
			}
		} else if (rotation == 1) {
			for (i = 0; i < 24; i++) {
				if (tetrisBoard[i + 3][location] == 1 || tetrisBoard[i + 3][location + 1] == 1) {
					for (j = 0; j < 3; j++) {
						tetrisBoard[i + j][location] = 2;
					}
					tetrisBoard[i + 2][location + 1] = 2;
					return i;
				}
			}
		} else if (rotation == 2) {
			for (i = 0; i < 24; i++) {
				if (tetrisBoard[i + 2][location] == 1 || tetrisBoard[i + 1][location + 1] == 1 || tetrisBoard[i + 1][location + 2] == 1) {
					for (j = 0; j < 3; j++) {
						tetrisBoard[i][location + j] = 2;
					}
					tetrisBoard[i + 1][location] = 2;
					return i;
				}
			}
		} else if (rotation == 3) {
			for (i = 0; i < 24; i++) {
				if (tetrisBoard[i + 1][location] == 1 || tetrisBoard[i + 3][location + 1] == 1) {
					for (j = 0; j < 3; j++) {
						tetrisBoard[i + j][location + 1] = 2;
					}
					tetrisBoard[i][location] = 2;
					return i;
				}
			}
		}
		break;

	case 3:
		for (i = 0; i < 24; i++) {
			if (tetrisBoard[i + 2][location] == 1 || tetrisBoard[i + 2][location + 1] == 1) {
				for (j = 0; j < 2; j++) {
					tetrisBoard[i + j][location] = 2;
					tetrisBoard[i + j][location + 1] = 2;
				}
				return i;
			}
		}
		break;

	case 4:
		if (rotation == 0 || rotation == 2) {
			for (i = 0; i < 24; i++) {
				if (tetrisBoard[i + 2][location] == 1 || tetrisBoard[i + 2][location + 1] == 1 || tetrisBoard[i + 1][location + 2] == 1) {
					for (j = 0; j < 2; j++) {
						tetrisBoard[i][location + j + 1] = 2;
						tetrisBoard[i + 1][location + j] = 2;
					}
					return i;
				}
			}
		} else if (rotation == 1 || rotation == 3) {
			for (i = 0; i < 24; i++) {
				if (tetrisBoard[i + 2][location] == 1 || tetrisBoard[i + 3][location + 1] == 1) {
					for (j = 0; j < 2; j++) {
						tetrisBoard[i + j][location] = 2;
						tetrisBoard[i + j + 1][location + 1] = 2;
					}
					return i;
				}
			}
		}
		break;

	case 5:
		if (rotation == 0) {
			for (i = 0; i < 24; i++) {
				for (j = 0; j < 3; j++) {
					if (tetrisBoard[i + 2][location + j] == 1) {
						break;
					}
				}
				if (j != 3) {
					for (j = 0; j < 3; j++) {
						tetrisBoard[i + 1][location + j] = 2;
					}
					tetrisBoard[i][location + 1] = 2;
					return i;
				}
			}
		} else if (rotation == 1) {
			for (i = 0; i < 24; i++) {
				if (tetrisBoard[i + 3][location] == 1 || tetrisBoard[i + 2][location + 1] == 1) {
					for (j = 0; j < 3; j++) {
						tetrisBoard[i + j][location] = 2;
					}
					tetrisBoard[i + 1][location + 1] = 2;
					return i;
				}
			}
		} else if (rotation == 2) {
			for (i = 0; i < 24; i++) {
				if (tetrisBoard[i + 1][location] == 1 || tetrisBoard[i + 2][location + 1] == 1 || tetrisBoard[i + 1][location + 2] == 1) {
					for (j = 0; j < 3; j++) {
						tetrisBoard[i][location + j] = 2;
					}
					tetrisBoard[i + 1][location + 1] = 2;
					return i;
				}
			}
		} else if (rotation == 3) {
			for (i = 0; i < 24; i++) {
				if (tetrisBoard[i + 2][location] == 1 || tetrisBoard[i + 3][location + 1] == 1) {
					for (j = 0; j < 3; j++) {
						tetrisBoard[i + j][location + 1] = 2;
					}
					tetrisBoard[i + 1][location] = 2;
					return i;
				}
			}
		}
		break;

	case 6:
		if (rotation == 0 || rotation == 2) {
			for (i = 0; i < 24; i++) {
				if (tetrisBoard[i + 1][location] == 1 || tetrisBoard[i + 2][location + 1] == 1 || tetrisBoard[i + 2][location + 2] == 1) {
					for (j = 0; j < 2; j++) {
						tetrisBoard[i][location + j] = 2;
						tetrisBoard[i + 1][location + j + 1] = 2;
					}
					return i;
				}
			}
		} else if (rotation == 1 || rotation == 3) {
			for (i = 0; i < 24; i++) {
				if (tetrisBoard[i + 3][location] == 1 || tetrisBoard[i + 2][location + 1] == 1) {
					for (j = 0; j < 2; j++) {
						tetrisBoard[i + j + 1][location] = 2;
						tetrisBoard[i + j][location + 1] = 2;
					}
					return i;
				}
			}
		}
		break;
	}
	return i;
}

/* 완성된 줄을 삭제하는 함수 */
void deleteCompletedLine (char tetrisBoard[25][12], int* combo, int* score)
{
	int numOfCompletedLine = 0,												//완성된 줄의 갯수
		plusScore = 0,														//추가 점수
		i, j, k, l;

	/* 한 줄이 완성되었는지 판별함 */
	for (i = 4; i < 24; i++) {
		for (j = 1; j < 11; j++) {
			if (tetrisBoard[i][j] != 1 && tetrisBoard[i][j] != 2) { break; }
		}
		if (j == 11) {
			numOfCompletedLine++;
			for (k = i; k > 0; k--) {
				for (l = 1; l < 11; l++) {
					tetrisBoard[k][l] = tetrisBoard[k-1][l];
				}
			}
			for (l = 1; l < 11; l++) {
				tetrisBoard[0][l] = 0;
			}
		}
	}

	if (numOfCompletedLine == 0) { *combo = 0; }							//한 줄도 완성하지 못했다면 콤보를 0으로 초기화한다.
	else { (*combo)++; }

	/* 점수 공식 : 현재 점수 += (이번 턴에 없앤 줄 수) * 콤보
	 * 기본적으로 한 줄은 1점이다.
	 * 한 번에 여러 줄을 없앴을 경우, 등차식으로 추가 점수가 주어진다.
	 * (예 : 2줄을 없앴을 경우 점수는 1 + 2 , 4줄을 없앴을 경우 점수는 1 + 2 + 3 + 4)
	 * 여러 턴동안 연속으로 줄을 없애 콤보가 생겼다면, 위에서 구한 점수에 콤보수를 곱한다.
	 * (예 : 2줄을 없앴고 콤보가 3이라면 점수는 (1 + 2) * 3)
	 * 이렇게 나온 점수를 기존 점수에 더한다.
	 * 요약 : 현재 점수 += (이번 턴에 없앤 줄 수)(이번 턴에 없앤 줄 수 + 1) / 2 * (콤보 수)
	 * */
	for (i = 0; i < numOfCompletedLine; i++) {
		plusScore += (i + 1);
	}
	plusScore *= (*combo);
	(*score) += plusScore;
}

/* 게임 종료 여부를 판별하는 함수 */
void checkGameEnd (char tetrisBoard[25][12], char* isDead)
{
	int i, j;

	/* 화면에 출력되지 않는 위 4줄에 한 칸이라도 블록이 차 있다면 게임 오버 */
	for (i = 0; i < 4; i++) {
		for (j = 1; j < 11; j++) {
			if (tetrisBoard[i][j] == 1 || tetrisBoard[i][j] == 2) { *isDead = true; }
		}
	}
}

/* 3스테이지의 룰을 수행하는 함수 */
void doThirdStageRule (char A_tetrisBoard[25][12], char B_tetrisBoard[25][12], int numOfBlock)
{
	int blank, i, j;

	if (numOfBlock % 5 != 0) { return; }

	blank = rand() % 10 + 1;
	for (i = 0; i < 23; i++) {
		for (j = 1; j < 11; j++) {
			A_tetrisBoard[i][j] = A_tetrisBoard[i + 1][j];
			B_tetrisBoard[i][j] = B_tetrisBoard[i + 1][j];
		}
	}
	for (j = 1; j < 11; j++) {
		A_tetrisBoard[23][j] = 1;
		B_tetrisBoard[23][j] = 1;
	}
	A_tetrisBoard[23][blank] = 0;
	B_tetrisBoard[23][blank] = 0;
}

/* 스테이지를 설정하는 함수 */
int setStage (int numOfBlock)
{
	if (numOfBlock < 400) { return 1; }
	else if (numOfBlock < 700) { return 2; }
	else if (numOfBlock < 1000) { return 3; }
	else { return 0; }
}

/* 랜덤하게 블록을 생성하는 함수 */
void spawnBlock (int blockBag[2][7], int numOfBlock)
{
	int block, i, j;

	if (numOfBlock % 7 != 0) { return; }

	for (i = 0; i < 7; i++) {
		blockBag[0][i] = blockBag[1][i];
	}

	for (i = 0; i < 7; i++) {
		block = rand() % 7;
		for (j = 0; j < i; j++) {
			if (block == blockBag[1][j]) {
				block = rand() % 7;
				j = -1;
				continue;
			}
		}
		blockBag[1][i] = block;
	}
	return;
}

/* 테트리스 판의 2를 1로 바꾸는 함수 */
void changeTwoToOne (char tetrisBoard[25][12]) {
	int i, j;

	for (i = 0; i < 25; i++) {
		for (j = 0; j < 12; j++) {
			if (tetrisBoard[i][j] == 2) {
				tetrisBoard[i][j] = 1;
			}
		}
	}
}

/* 화면에 테트리스 판을 출력하는 함수 */
void printTetrisBoard (char A_tetrisBoard[25][12], char B_tetrisBoard[25][12], char A_isDead, char B_isDead, int A_score, int B_score)
{
	int i, j;

	printf("      Player A              Player B\n");
	for (i = 4; i < 24; i++) {
		for (j = 1; j < 11; j++) {
			if (A_tetrisBoard[i][j] == 0) { printf("□"); }
			else if (A_tetrisBoard[i][j] == 1) { printf("■"); }
			else if (A_tetrisBoard[i][j] == 2) { printf("▩"); }
		}
		printf("  ");
		for (j = 1; j < 11; j++) {
			if (B_tetrisBoard[i][j] == 0) { printf("□"); }
			else if (B_tetrisBoard[i][j] == 1) { printf("■"); }
			else if (B_tetrisBoard[i][j] == 2) { printf("▩"); }
		}
		printf("\n");
	}
	printf("    Score : %5d     ", A_score);
	printf("    Score : %5d\n", B_score);
	if (A_isDead == 1) { printf("        Dead         "); }
	else { printf("                     "); }
	if (B_isDead == 1) { printf("        Dead\n"); }
	else { printf("\n"); }
}
