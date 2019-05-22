#include <stdio.h>
#include <stdlib.h>
#include <termio.h>



int i, j, k, move_cnt, level, box_cnt, goal_cnt;
int undo_cnt;
int map_line[5];
char cmd;
char undo[5][30][30];
char map_data[5][30][30];
char map_cur[5][30][30], map_tmp[30][30];
char usr_name[10];




void game_start();
void get_map();
void checkmap();
void undo_update();
void undo_reset();
void is_complete();
void game_end();
void move();
void command();
void show_cmd();

int getch() {

	int ch;

	struct termios buf;

	struct termios save;

	tcgetattr(0, &save);

	buf = save;


	buf.c_lflag &= ~(ICANON | ECHO);
	buf.c_cc[VMIN] = 1;
	buf.c_cc[VTIME] = 0;

	tcsetattr(0, TCSAFLUSH, &buf);

	ch = getchar();

	tcsetattr(0, TCSAFLUSH, &save);

	return ch;
}

int main()
{
	gamestart();
	get_map();
	while (1) {

	}


	return 0;
}

void gamestart() { //이름을 입력받는 함수
		
		system("clear");
		printf("Start...\n");
		printf("input name : ");
		scanf("%s", usr_name);
	
}

void get_map() {
	char c = 'a';
	i = -1;
	j = 0;
	k = 0;
	FILE *fp = fopen("map.txt", "r");

	while (c != 'e') {
		for (k = 0; k < 30; k++) {
			fscanf(fp, "%c", &c);
			if ((c >= 48 && c <= 57)) {
				i++;
				j = 0;
				k = -1;
				continue;
			}
			if (c == 'e') {
				break;
			}
			if (c == '\n') {
				j++;
				k = -1;
				continue;
			}
			else {
				switch (c) {
				case '.':	//점일 경우 0으로 저장
					map_data[i][j][k] = 0;
					break;
				case '#':	//벽일 경우 1로 저장
					map_data[i][j][k] = 1;
					break;
				case '$':	//박스일 경우 2로 저장
					map_data[i][j][k] = 2;
					break;
				case 'O':	//박스 놓는곳일 경우 4로 저장
					map_data[i][j][k] = 4;
					break;
				case '@':	//플레이어일 경우 5로 저장
					map_data[i][j][k] = 5;
					break;
				default:
					break;
				}

			}
		}
	}
	for (i = 0; i < 5; i++) {
		for (j = 0; j < 30; j++) {
			for (k = 0; k < 30; k++) {
				map_cur[i][j][k] = map_data[i][j][k];
			}
		}
	}

	for (i = 0; i < 5; i++) {
		for (j = 1; j < 30; j++) {
			int cnt = 0;
			for (k = 0; k < 30; k++) {
				if (map_data[i][j][k] == 0) {
					cnt++;
				}
			}
			if (cnt == 30) {
				map_line[5] = j;
				break;
			}
		}
	}
	fclose(fp);
}
