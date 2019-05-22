#include <stdio.h>
#include <stdlib.h>
//#include <termio.h>
//#include <unistd.h>
#include <time.h>

int i = -1, j, k, stage = 0, clear_num, time_num = 0, undo_reset_num = 0, first_com_error = 0;
int box_num = 0, boxput_num = 0, err_num = 0;
int pl_x, pl_y, game_exit = 0, save_stage, save_count = 0, dl_x, dl_y;
int map_end[5][1], end_cnt;
double start_time[5], finish_time[5], diff_time[5] = { 0 };
char ch = 0, game_act = 0, get;
char map_current[5][30][30], map_load[30][30];
char map_file[5][30][30] = { };
char undo[5][30][30];
char name[10];

void getname();
void getmap();
void checkmap();
void showmap();
void showcommand();
void showname();
void getkey();
void time_set();
void pl_move();
void other_act();
void undo_save();
void undo_reset();
void clear();
void after_game();

int getch(void) {	//문자열을 입력받는 함수
	int ch;
	/*
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
		*/

	return ch;
}


int main(void) {
	getname();
	getmap();
	checkmap();
	while (game_exit != 1) {
		system("clear");
		showname();
		showmap();
		showcommand();
		time_set();
		get = getch();
		pl_move();
		other_act();
		clear();
	}
	after_game();
	showcommand();

	return 0;
}



void getname() {	//이름을 입력받는 함수
	system("clear");
	printf("Start....\n");
	printf("input name : ");
	scanf("%s", name);
}

void getmap() {	//map.txt에서 맵 불러와서 저장
	FILE *file = fopen("map.txt", "r");

	while (ch != 'e') {
		for (k = 0; k < 30; k++) {
			fscanf(file, "%c", &ch);
			if (ch == 'm') {
				i++;
				j = 0;
				k = -1;
				continue;
			}
			if (ch == 'a' || ch == 'p') {
				k--;
				continue;
			}
			if (ch == 'e') {
				break;
			}
			if (ch == '\n') {
				j++;
				k = -1;
				continue;
			}
			else {
				switch (ch) {
				case ' ':	//공백일 경우 0으로 저장
					map_file[i][j][k] = 0;
					break;
				case '#':	//벽일 경우 1로 저장
					map_file[i][j][k] = 1;
					break;
				case '$':	//박스일 경우 2로 저장
					map_file[i][j][k] = 2;
					break;
				case 'O':	//박스 놓는곳일 경우 4로 저장
					map_file[i][j][k] = 4;
					break;
				case '@':	//플레이어일 경우 5로 저장
					map_file[i][j][k] = 5;
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
				map_current[i][j][k] = map_file[i][j][k];
			}
		}
	}

	for (i = 0; i < 5; i++) {
		for (j = 1; j < 30; j++) {
			end_cnt = 0;
			for (k = 0; k < 30; k++) {
				if (map_file[i][j][k] == 0) {
					end_cnt++;
				}
			}
			if (end_cnt == 30) {
				map_end[i][0] = j;
				break;
			}
		}
	}
	fclose(file);
}

void checkmap() {
	for (i = 0; i < 5; i++) {
		for (j = 0; j < 30; j++) {
			for (k = 0; k < 30; k++) {
				if (map_file[i][j][k] == 2)
					box_num++;
				if (map_file[i][j][k] == 4)
					boxput_num++;
			}
		}
		if (box_num != boxput_num) {
			game_exit = 1;
			err_num = 1;
		}
		box_num = 0;
		boxput_num = 0;
	}
}

void showname() {

	printf("    Hello ");
	for (i = 0; i < 20; i++) {
		printf("%c", name[i]);
	}
	printf("\n");
}

void showmap() {
	for (j = 0; j < map_end[stage][0]; j++) {
		for (k = 0; k < 30; k++) {
			switch (map_current[stage][j][k]) {
			case 0:	//0일 경우 공백으로 표시
				printf(" ");
				break;
			case 1:	//1일 경우 벽으로 표시
				printf("#");
				break;
			case 2:	//2일 경우 박스로 표시
				printf("$");
				break;
			case 4:	//4일 경우 박스놓는 곳으로 표시
				printf("O");
				break;
			case 5:	//플레이어일 경우 5로 표시
				printf("@");
				pl_x = k;
				pl_y = j;
				break;
			default:
				break;
			}
		}
		printf("\n");
	}
}

void showcommand() {
	if (first_com_error == 0) {
		undo_reset();
	}
	else {
		printf("(Command) %c", game_act);
	}
	first_com_error = 1;
}

void time_set() {
	if (time_num == 0) {
		start_time[stage] = clock();
		time_num = 1;
	}
}

void pl_move() {
	if (get != 'u') {
		undo_save();
	}
	switch (get) {
	case 'l':
		dl_x = 1, dl_y = 0;
		if (map_file[stage][pl_y][pl_x] == 4) {	//있던 자리가 박스 놓는 위치일 경우
			if (map_current[stage][pl_y + dl_y][pl_x + dl_x] == 2) {	//오른쪽이 박스일 경우
				if (map_current[stage][pl_y + 2 * dl_y][pl_x + 2 * dl_x] != 1 && map_current[stage][pl_y + 2 * dl_y][pl_x + 2 * dl_x] != 2) {//건너편이 박스랑 벽이 아닐 경우
					map_current[stage][pl_y + dl_y][pl_x + dl_x] = 5;	//플레이어 오른쪽 이동
					map_current[stage][pl_y + 2 * dl_y][pl_x + 2 * dl_x] = 2; //건너편은 박스
					map_current[stage][pl_y][pl_x] = 4;	//있던 자리는 박스 놓는 위치로 바꿈
				}
				else {	//건너편이 박스나 벽일 경우
					break;
				}
			}
			else if (map_current[stage][pl_y + dl_y][pl_x + dl_x] == 1) {	//오른쪽이 벽일 경우
				break;
			}
			else if (map_current[stage][pl_y + dl_y][pl_x + dl_x] == 0 || map_current[stage][pl_y + dl_y][pl_x + dl_x] == 4) {	//오른쪽이 공백이거나 박스 놓는곳일 경우에
				map_current[stage][pl_y + dl_y][pl_x + dl_x] = 5;	//플레이어 오른쪽 이동
				map_current[stage][pl_y][pl_x] = 4;	//있던 자리는 박스 놓는 위치로 바꿈
			}
		}
		else {	//있던 자리가 공백일 경우
			if (map_current[stage][pl_y + dl_y][pl_x + dl_x] == 2) {	//오른쪽이 박스일 경우
				if (map_current[stage][pl_y + 2 * dl_y][pl_x + 2 * dl_x] != 1 && map_current[stage][pl_y + 2 * dl_y][pl_x + 2 * dl_x] != 2) {//건너편이 박스랑 벽이 아닐 경우
					map_current[stage][pl_y + dl_y][pl_x + dl_x] = 5;	//플레이어 오른쪽 이동
					map_current[stage][pl_y + 2 * dl_y][pl_x + 2 * dl_x] = 2; //건너편 박스
					map_current[stage][pl_y][pl_x] = 0;	//있던 자리는 공백
				}
				else {	//건너편이 박스나 벽일 경우
					break;
				}
			}
			else if (map_current[stage][pl_y + dl_y][pl_x + dl_x] == 1) {	//오른쪽이 벽일 경우
				break;
			}
			else if (map_current[stage][pl_y + dl_y][pl_x + dl_x] == 0 || map_current[stage][pl_y + dl_y][pl_x + dl_x] == 4) {	//오른쪽이 공백이거나 박스 놓는곳일 경우에
				map_current[stage][pl_y + dl_y][pl_x + dl_x] = 5;	//플레이어 오른쪽 이동
				map_current[stage][pl_y][pl_x] = 0;	//있던 자리는 박스 놓는 공백
			}
		}
		break;
	case 'h':
		dl_x = -1, dl_y = 0;
		if (map_file[stage][pl_y][pl_x] == 4) {	//있던 자리가 박스 놓는 위치일 경우
			if (map_current[stage][pl_y + dl_y][pl_x + dl_x] == 2) {	//왼쪽이 박스일 경우
				if (map_current[stage][pl_y + 2 * dl_y][pl_x + 2 * dl_x] != 1 && map_current[stage][pl_y + 2 * dl_y][pl_x + 2 * dl_x] != 2) {//건너편이 박스랑 벽이 아닐 경우
					map_current[stage][pl_y + dl_y][pl_x + dl_x] = 5;	//플레이어 왼쪽 이동
					map_current[stage][pl_y + 2 * dl_y][pl_x + 2 * dl_x] = 2; //건너편 박스
					map_current[stage][pl_y][pl_x] = 4;	//있던 자리는 박스 놓는 위치로 바꿈
				}
				else {	//건너편이 박스나 벽일 경우
					break;
				}
			}
			else if (map_current[stage][pl_y + dl_y][pl_x + dl_x] == 1) {	//왼쪽이 벽일 경우
				break;
			}
			else if (map_current[stage][pl_y + dl_y][pl_x + dl_x] == 0 || map_current[stage][pl_y + dl_y][pl_x + dl_x] == 4) {	//왼쪽이 공백이거나 박스 놓는곳일 경우에
				map_current[stage][pl_y + dl_y][pl_x + dl_x] = 5;	//플레이어 왼쪽 이동
				map_current[stage][pl_y][pl_x] = 4;	//있던 자리는 박스 놓는 위치로 바꿈
			}
		}
		else {	//있던 자리가 공백일 경우
			if (map_current[stage][pl_y + dl_y][pl_x + dl_x] == 2) {	//왼쪽이 박스일 경우
				if (map_current[stage][pl_y + 2 * dl_y][pl_x + 2 * dl_x] != 1 && map_current[stage][pl_y + 2 * dl_y][pl_x + 2 * dl_x] != 2) {	//건너편이 박스랑 벽이 아닐 경우
					map_current[stage][pl_y + dl_y][pl_x + dl_x] = 5;	//플레이어 왼쪽 이동
					map_current[stage][pl_y + 2 * dl_y][pl_x + 2 * dl_x] = 2; //건너편 박스
					map_current[stage][pl_y][pl_x] = 0;	//있던 자리는 공백
				}
				else {	//건너편이 박스나 벽일 경우
					break;
				}
			}
			else if (map_current[stage][pl_y + dl_y][pl_x + dl_x] == 1) {	//오른쪽이 벽일 경우
				break;
			}
			else if (map_current[stage][pl_y + dl_y][pl_x + dl_x] == 0 || map_current[stage][pl_y + dl_y][pl_x + dl_x] == 4) {	//건너편이 박스랑 벽이 아닐 경우
				map_current[stage][pl_y + dl_y][pl_x + dl_x] = 5;	//플레이어 왼쪽 이동
				map_current[stage][pl_y][pl_x] = 0;	//있던 자리는 공백
			}
		}
		break;
	case 'j':
		dl_x = 0, dl_y = 1;
		if (map_file[stage][pl_y][pl_x] == 4) {	//있던 자리가 박스 놓는 위치일 경우
			if (map_current[stage][pl_y + dl_y][pl_x + dl_x] == 2) {	//아래쪽이 박스일 경우
				if (map_current[stage][pl_y + 2 * dl_y][pl_x + 2 * dl_x] != 1 && map_current[stage][pl_y + 2 * dl_y][pl_x + 2 * dl_x] != 2) {//건너편이 박스랑 벽이 아닐 경우
					map_current[stage][pl_y + dl_y][pl_x + dl_x] = 5;	//플레이어 아래쪽 이동
					map_current[stage][pl_y + 2 * dl_y][pl_x + 2 * dl_x] = 2; //건너편 박스
					map_current[stage][pl_y][pl_x] = 4;	//있던 자리는 박스 놓는 위치로 바꿈
				}
				else {	//건너편이 박스나 벽일 경우
					break;
				}
			}
			else if (map_current[stage][pl_y + dl_y][pl_x + dl_x] == 1) {	//오른쪽이 벽일 경우
				break;
			}
			else if (map_current[stage][pl_y + dl_y][pl_x + dl_x] == 0 || map_current[stage][pl_y + dl_y][pl_x + dl_x] == 4) {	//아래쪽이 공백이거나 박스 놓는곳일 경우에
				map_current[stage][pl_y + dl_y][pl_x + dl_x] = 5;	//플레이어 아래쪽 이동
				map_current[stage][pl_y][pl_x] = 4;	//있던 자리는 박스 놓는 위치로 바꿈
			}
		}
		else {	//있던 자리가 공백일 경우
			if (map_current[stage][pl_y + dl_y][pl_x + dl_x] == 2) {	//아래쪽이 박스일 경우
				if (map_current[stage][pl_y + 2 * dl_y][pl_x + 2 * dl_x] != 1 && map_current[stage][pl_y + 2 * dl_y][pl_x + 2 * dl_x] != 2) {//건너편이 박스랑 벽이 아닐 경우
					map_current[stage][pl_y + dl_y][pl_x + dl_x] = 5;	//플레이어 아래쪽 이동
					map_current[stage][pl_y + 2 * dl_y][pl_x + 2 * dl_x] = 2; //건너편 박스
					map_current[stage][pl_y][pl_x] = 0;	//있던 자리는 공백
				}
				else {	//건너편이 박스나 벽일 경우
					break;
				}
			}
			else if (map_current[stage][pl_y + dl_y][pl_x + dl_x] == 1) {	//아래쪽이 벽일 경우
				break;
			}
			else if (map_current[stage][pl_y + dl_y][pl_x + dl_x] == 0 || map_current[stage][pl_y + dl_y][pl_x + dl_x] == 4) {	//아래쪽이 공백이거나 박스 놓는곳일 경우에
				map_current[stage][pl_y + dl_y][pl_x + dl_x] = 5;	//플레이어 아래쪽 이동
				map_current[stage][pl_y][pl_x] = 0;	//있던 자리는 공백
			}
		}
		break;
	case 'k':
		dl_x = 0, dl_y = -1;
		if (map_file[stage][pl_y][pl_x] == 4) {	//있던 자리가 박스 놓는 위치일 경우
			if (map_current[stage][pl_y + dl_y][pl_x + dl_x] == 2) {	//위쪽이 박스일 경우
				if (map_current[stage][pl_y + 2 * dl_y][pl_x + 2 * dl_x] != 1 && map_current[stage][pl_y + 2 * dl_y][pl_x + 2 * dl_x] != 2) {//건너편이 박스랑 벽이 아닐 경우
					map_current[stage][pl_y + dl_y][pl_x + dl_x] = 5;	//플레이어 위쪽 이동
					map_current[stage][pl_y + 2 * dl_y][pl_x + 2 * dl_x] = 2; //건너편 박스
					map_current[stage][pl_y][pl_x] = 4;	//있던 자리는 박스 놓는 위치로 바꿈
				}
				else {
					break;
				}
			}
			else if (map_current[stage][pl_y + dl_y][pl_x + dl_x] == 1) {	//오른쪽이 벽일 경우
				break;
			}
			else if (map_current[stage][pl_y + dl_y][pl_x + dl_x] == 0 || map_current[stage][pl_y + dl_y][pl_x + dl_x] == 4) {	//위쪽이 공백이거나 박스 놓는곳일 경우에
				map_current[stage][pl_y + dl_y][pl_x + dl_x] = 5;	//플레이어 위쪽 이동
				map_current[stage][pl_y][pl_x] = 4;	//있던 자리는 박스 놓는 위치로 바꿈
			}
		}
		else {	//있던 자리가 공백일 경우
			if (map_current[stage][pl_y + dl_y][pl_x + dl_x] == 2) {	//위쪽이 박스일 경우
				if (map_current[stage][pl_y + 2 * dl_y][pl_x + 2 * dl_x] != 1 && map_current[stage][pl_y + 2 * dl_y][pl_x + 2 * dl_x] != 2) {	//건너편이 박스랑 벽이 아닐 경우
					map_current[stage][pl_y + dl_y][pl_x + dl_x] = 5;	//플레이어 위쪽 이동
					map_current[stage][pl_y + 2 * dl_y][pl_x + 2 * dl_x] = 2; //건너편 박스
					map_current[stage][pl_y][pl_x] = 0;
				}
				else {	//건너편이 박스나 벽이 경우
					break;
				}
			}
			else if (map_current[stage][pl_y + dl_y][pl_x + dl_x] == 1) {	//위쪽이 벽일 경우
				break;
			}
			else if (map_current[stage][pl_y + dl_y][pl_x + dl_x] == 0 || map_current[stage][pl_y + dl_y][pl_x + dl_x] == 4) {	//위쪽이 공백이거나 박스 놓는곳일 경우에
				map_current[stage][pl_y + dl_y][pl_x + dl_x] = 5;	//플레이어 위쪽 이동
				map_current[stage][pl_y][pl_x] = 0;	//있던 자리는 공백
			}
		}
		break;
	default:
		break;
	}
	game_act = ' ';
}

void other_act() {
	switch (get) {
	case 'r':	//현재 맵 처음부터 다시 시작
		for (j = 0; j < 30; j++) {
			for (k = 0; k < 30; k++) {
				map_current[stage][j][k] = map_file[stage][j][k];
			}
		}
		game_act = 'r';
		undo_reset_num = 0;
		undo_reset();
		break;
	case 'n':	//첫번째 맵부터 다시 시작
		stage = 0;
		for (i = 0; i < 5; i++) {
			for (j = 0; j < 30; j++) {
				for (k = 0; k < 30; k++) {
					map_current[i][j][k] = map_file[i][j][k];
				}
			}
		}
		game_act = 'n';
		undo_reset_num = 0;
		undo_reset();
		time_num = 0;

		break;
	case 'e':	//게임 종료, 종료하기전에 저장해야 함
		if (save_count == 1) {
			game_exit = 1;
		}
		else {
			printf("e\n종료하기 전에 저장해야 합니다\n");
			//sleep(1);
		}
		game_act = 'e';
		break;
	case 's': {	//현재 상태를 파일에 저장	
		FILE *f = fopen("sokoban.txt", "w");
		for (j = 0; j < 30; j++) {
			for (k = 0; k < 30; k++) {
				fprintf(f, "%d", map_current[stage][j][k]);
			}
		}
		fprintf(f, "%d", stage);
		save_count = 1;
		fclose(f);
		game_act = 's';
		break;
	}
	case 'd':	//명령 내용 보여주기
		system("clear");
		showname();
		printf("\n");
		printf("h(왼쪽), j(아래), k(위), l(오른쪽)\n");
		printf("u(undo)\n");
		printf("r(replay)\n");
		printf("n(new)\n");
		printf("e(exit)\n");
		printf("s(save)\n");
		printf("f(file load)\n");
		printf("d(display help)\n");
		printf("t(top)\n\n");
		game_act = 'd';
		showcommand();
		game_act = ' ';
		getch();

		break;
	case 'f': {	//저장된 내용을 불러오기
		FILE *f = fopen("sokoban.txt", "r");
		for (j = 0; j < 30; j++) {
			for (k = 0; k < 30; k++) {
				fscanf(f, "%c", &ch);
				map_load[j][k] = ch - 48;
			}
		}
		fscanf(f, "%c", &save_stage);
		stage = save_stage - 48;
		for (j = 0; j < 30; j++) {
			for (k = 0; k < 30; k++) {
				map_current[stage][j][k] = map_load[j][k];
			}
		}
		fclose(f);
		game_act = 'f';
		break;
	}
	case 't':	//게임 순위 보여주기
		system("clear");
		showname();
		for (i = 0; i < 5; i++) {
			printf("%.1f\n", diff_time[stage]);
		}
		game_act = 't';
		showcommand();
		game_act = ' ';
		getch();
		break;
	case 'u':	//예전 상태로 돌아가기, 최대 5번까지
		for (j = 0; j < 30; j++) {
			for (k = 0; k < 30; k++) {
				map_current[stage][j][k] = undo[0][j][k];
			}
		}
		for (i = 0; i < 4; i++) {
			for (j = 0; j < 30; j++) {
				for (k = 0; k < 30; k++) {
					undo[i][j][k] = undo[i + 1][j][k];
				}
			}
		}
		game_act = 'u';
		break;
	default:
		break;
	}
}

void clear() {	//게임을 클리어 했는지 판단하는 함수
	clear_num = 0;
	for (j = 0; j < 30; j++) {	//박스 놓는 곳이 있는지 판단하기
		for (k = 0; k < 30; k++) {
			if (map_current[stage][j][k] == 4) {
				clear_num++;
			}
		}
	}
	if (stage != 5 && clear_num == 0 && map_file[stage][pl_y + dl_y][pl_x + dl_x] != 4) {	//박스 놓는 곳이 없고 플레이어 위치가 박스 놓는 곳이 아니라면 클리어
		finish_time[stage] = clock();
		diff_time[stage] = (finish_time[stage] - start_time[stage]) * 2.5 / CLOCKS_PER_SEC;
		printf("걸린시간: %.1f초", diff_time[stage]);
		stage++;
		undo_reset_num = 0;
		time_num = 0;
		getch();
	}
	if (stage == 5) {
		game_exit = 1;
	}
}

void undo_save() {	//undo를 5개 저장하게 해주는 함수
	for (i = 4; i > 0; i--) {
		for (j = 0; j < 30; j++) {
			for (k = 0; k < 30; k++) {
				undo[i][j][k] = undo[i - 1][j][k];
			}
		}
	}
	for (j = 0; j < 30; j++) {
		for (k = 0; k < 30; k++) {
			undo[0][j][k] = map_current[stage][j][k];
		}
	}
}

void undo_reset() {	//초반에 undo 눌렀을때 안움직이게 해주고 r, n눌렀을때 undo 초기화
	if (undo_reset_num == 0) {
		for (i = 0; i < 5; i++) {
			for (j = 0; j < 30; j++) {
				for (k = 0; k < 30; k++) {
					undo[i][j][k] = map_file[stage][j][k];
				}
			}
		}
	}
	undo_reset_num = 1;
}

void after_game() {
	if (err_num == 1) {
		system("clear");
		printf("\n맵 파일에서 박스개수와 보관장수 개수가 같지 않습니다\n\n\n");
		printf("(Command) %c", game_act);
	}
	else if (stage < 5) {
		system("clear");
		printf("\nSee YOU %s....\n\n\n", name);
	}
	else if (stage == 5) {
		system("clear");
		printf("\n게임을 클리어하셨습니다! %s님축하합니다~\n\n\n", name);
	}
}