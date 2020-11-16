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
	char A_tetrisBoard[25][12], B_tetrisBoard[25][12],						//�� �÷��̾��� ������. ���� ũ�⺸�� ū ������ ����� ��ġ��Ű�� ���������� �Ǻ��ϴ� ���� ���Ǽ��� ����.
		 A_isDead = false, B_isDead = false,								//�� �÷��̾��� ���� ����
		 select;
	int  thisBlock, nextBlock, numOfBlock = 0,								//���� ���, ���� ���, ���ݱ��� �� ����� ����
		 blockBag[2][7],													//�ֱ� 7���� ���
		 stage,																//���� ��������
		 A_combo = 0, B_combo = 0,											//�� �÷��̾��� �޺���
		 A_score = 0, B_score = 0;											//�� �÷��̾��� ����

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

		if (stage == 3) { doThirdStageRule(A_tetrisBoard, B_tetrisBoard, numOfBlock); }			//3���������� ��� 3��° ��Ģ�� �����Ѵ�
		if (!A_isDead) { playTurn(A_tetrisBoard, &A_isDead, 0, thisBlock, nextBlock, stage, numOfBlock, &A_combo, &A_score); }
		if (!B_isDead) { playTurn(B_tetrisBoard, &B_isDead, 1, thisBlock, nextBlock, stage, numOfBlock, &B_combo, &B_score); }

/* DEBUG ����� ���, ȭ�鿡 ���� �������� �� �÷��̾��� ��Ʈ���� �� ���¸� ����Ѵ�. */
#ifdef _DEBUG_
		Sleep(100);
		if ((numOfBlock + 1) % DISPLAYSTEP == 0) {
			printTetrisBoard(A_tetrisBoard, B_tetrisBoard, A_isDead, B_isDead, A_score, B_score);
			printf("���� ���� ��Ȳ : ��� %d�� ������\n", numOfBlock + 1);
			printf("���� ��� �� : %d\n", nextBlock);
			if ((numOfBlock + 1) % DEBUGSTEP == 0) {
#ifndef NOPROMPT
				do {
					printf("����Ϸ��� y�� �Է����ּ��� : ");
					scanf("%c", &select);
					fflush(stdin);
				} while (select != 'y');
#endif
			}
		}
#endif


		if ((A_isDead && B_isDead || numOfBlock == 999) || (A_isDead && B_score > A_score) || (B_isDead && A_score > B_score)) {
			printTetrisBoard(A_tetrisBoard, B_tetrisBoard, A_isDead, B_isDead, A_score, B_score);
			printf("��� %d������ ������ ����Ǿ����ϴ�.\n", numOfBlock + 1);
			if (A_score > B_score) { printf("A�÷��̾� ��! B�÷��̾� ��!\n"); }
			else if (B_score > A_score) { printf("A�÷��̾� ��! B�÷��̾� ��!\n"); }
			else { printf("���º��Դϴ�.\n"); }
			break;
		}
		changeTwoToOne(A_tetrisBoard);
		changeTwoToOne(B_tetrisBoard);
		numOfBlock++;
	}
	scanf("%c", &select);
	return 0;
}

/* �ʱ� ������ �ϴ� �Լ� */
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

/* ���� �����ϴ� �Լ� */
void playTurn (char tetrisBoard[25][12], char* isDead, int order, int thisBlock, int nextBlock, int stage, int numOfBlock, int* combo, int* score)
{
	char boardCopy[25][12];													//�÷��̾��� ��Ʈ���� ���� �����صδ� �迭 (AI�Լ��� ���� ������ ���� ����)
	int	 location, rotation,												//����� ��ġ�� ȸ����
		 i, j;

	for (i = 0; i < 25; i++) {
		for (j = 0; j < 12; j++) {
			boardCopy[i][j] = tetrisBoard[i][j];
		}
	}

	if (stage >= 2) { nextBlock = -1; }										//2�������� �̻� �������̶�� nextBlock�� ���� -1�� �ٲ۴�. (nextBlock�� �˷����� ����)

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

/* AI�� �Է� ������ �����ϴ� �Լ� */
void correctError (int thisBlock, int* location, int* rotation)
{
	/* ȸ�� ���� 0 ~ 3�� ����� ��� */
	if(*rotation < 0) {
		*rotation = 0;
	} else if(*rotation > 3) {
		*rotation = 3;
	}

	/* ��ġ ���� ���� ���� ����� ��� */
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

/* ����� ��Ʈ���� �ǿ� ���� �Լ� */
int placeBlock (char tetrisBoard[25][12], int thisBlock, int location, int rotation) {
	int i, j;

	/* ����� �ٴڿ� �ε��� ������ �� �پ� ������.
	 * �ٴڿ� �ε����ٸ� �� �ڸ��� �迭�� ����� �����ϰ� �Լ��� �����Ѵ�.
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

/* ����� ��Ʈ���� �ǿ� ���� �Լ�, ���� �������� ���� ���� ��ġ�� ���� �絵�� ��. */
int placeBlockForUI (char tetrisBoard[25][12], int thisBlock, int location, int rotation)
{
	int i, j;

	/* ����� �ٴڿ� �ε��� ������ �� �پ� ������.
	 * �ٴڿ� �ε����ٸ� �� �ڸ��� �迭�� ����� �����ϰ� �Լ��� �����Ѵ�.
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

/* �ϼ��� ���� �����ϴ� �Լ� */
void deleteCompletedLine (char tetrisBoard[25][12], int* combo, int* score)
{
	int numOfCompletedLine = 0,												//�ϼ��� ���� ����
		plusScore = 0,														//�߰� ����
		i, j, k, l;

	/* �� ���� �ϼ��Ǿ����� �Ǻ��� */
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

	if (numOfCompletedLine == 0) { *combo = 0; }							//�� �ٵ� �ϼ����� ���ߴٸ� �޺��� 0���� �ʱ�ȭ�Ѵ�.
	else { (*combo)++; }

	/* ���� ���� : ���� ���� += (�̹� �Ͽ� ���� �� ��) * �޺�
	 * �⺻������ �� ���� 1���̴�.
	 * �� ���� ���� ���� ������ ���, ���������� �߰� ������ �־�����.
	 * (�� : 2���� ������ ��� ������ 1 + 2 , 4���� ������ ��� ������ 1 + 2 + 3 + 4)
	 * ���� �ϵ��� �������� ���� ���� �޺��� ����ٸ�, ������ ���� ������ �޺����� ���Ѵ�.
	 * (�� : 2���� ���ݰ� �޺��� 3�̶�� ������ (1 + 2) * 3)
	 * �̷��� ���� ������ ���� ������ ���Ѵ�.
	 * ��� : ���� ���� += (�̹� �Ͽ� ���� �� ��)(�̹� �Ͽ� ���� �� �� + 1) / 2 * (�޺� ��)
	 * */
	for (i = 0; i < numOfCompletedLine; i++) {
		plusScore += (i + 1);
	}
	plusScore *= (*combo);
	(*score) += plusScore;
}

/* ���� ���� ���θ� �Ǻ��ϴ� �Լ� */
void checkGameEnd (char tetrisBoard[25][12], char* isDead)
{
	int i, j;

	/* ȭ�鿡 ��µ��� �ʴ� �� 4�ٿ� �� ĭ�̶� ����� �� �ִٸ� ���� ���� */
	for (i = 0; i < 4; i++) {
		for (j = 1; j < 11; j++) {
			if (tetrisBoard[i][j] == 1 || tetrisBoard[i][j] == 2) { *isDead = true; }
		}
	}
}

/* 3���������� ���� �����ϴ� �Լ� */
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

/* ���������� �����ϴ� �Լ� */
int setStage (int numOfBlock)
{
	if (numOfBlock < 400) { return 1; }
	else if (numOfBlock < 700) { return 2; }
	else if (numOfBlock < 1000) { return 3; }
	else { return 0; }
}

/* �����ϰ� ����� �����ϴ� �Լ� */
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

/* ��Ʈ���� ���� 2�� 1�� �ٲٴ� �Լ� */
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

/* ȭ�鿡 ��Ʈ���� ���� ����ϴ� �Լ� */
void printTetrisBoard (char A_tetrisBoard[25][12], char B_tetrisBoard[25][12], char A_isDead, char B_isDead, int A_score, int B_score)
{
	int i, j;

	printf("      Player A              Player B\n");
	for (i = 4; i < 24; i++) {
		for (j = 1; j < 11; j++) {
			if (A_tetrisBoard[i][j] == 0) { printf("��"); }
			else if (A_tetrisBoard[i][j] == 1) { printf("��"); }
			else if (A_tetrisBoard[i][j] == 2) { printf("��"); }
		}
		printf("  ");
		for (j = 1; j < 11; j++) {
			if (B_tetrisBoard[i][j] == 0) { printf("��"); }
			else if (B_tetrisBoard[i][j] == 1) { printf("��"); }
			else if (B_tetrisBoard[i][j] == 2) { printf("��"); }
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
