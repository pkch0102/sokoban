#include <stdio.h>
#include <stdlib.h>
#include <termio.h>
#include <stdbool.h>
#include <unistd.h>

#define Y 30 // 맵 최대 Y 값
#define X 30 // 맵 최대 X 값
#define STAGE 5 // 최대 스테이지 값

char data_map[STAGE][31][31], undo_map[STAGE][31][31], map[STAGE][31][31];//왼쪽부터 파일에서 맵을 받아와서 저장하는 배열, undo를 사용할때 쓸 배열, 게임플레이에 관여할 배열
char usrName[11], rank_name[STAGE][5][11];//유저의 이름, 순위권 플레이어의 이름
int maxi[5];
int maxj[5];
int goal_loc[5][5][5], rank_cnt[STAGE][5];//스테이지별 창고의 위치, 순위권플레이어의 점수
int box_count[STAGE], goal_count[STAGE];//금의 개수와 창고의 개수
int stage = -1, undo_cnt = 5, move_cnt = 0, save_cnt = 0;//스테이지 초기 값,  얼마나 undo를 많이 했는가, 움직인 횟수, undo맵 저장 횟수
int player_x[5], player_y[5], origin_player_x[5], origin_player_y[5];//현재 플레이어의 좌표와 초기 플레이어의 좌표
int d_flg, t_flg, record;//dispaly기능이 실행되고 있는가, top기능이 실행되고 있는가, 순위권 플레이어의 점수를 받아올 때 필요한 임시 변수
int master_key;//입력한 커맨드 출력에 필요한 변수

int max_stage = 0;
void get_map();//맵 파일 읽어오기
void show_Map();//맵 보여주기
void clearMap();//화면 삭제
void save();//맵 저장 기능
void FileLoad();//저장된 맵 불러오기
void Move();//플레이어의 움직임
void get_name();//유저의 이름 받기
void set_undo();//undo맵 업데이트
void Undo();//undo기능
void SaveTop();//ranking.txt에서 데이터를 읽어와 rank_cnt와 rank_name 업데이트 및 다시 ranking.txt업데이트
void New();//1스테이지 부터 시작
void Replay();//스테이지 다시시작
void Other_Command(char key);//움직임이외에 커맨드 제어
bool StageClear();//스테이지를 클리어 했는가
void Display_help();//display help기능
void Top(int Top_num);//순위를 보여준다
int getch(void);//커맨드 입력

int main()
{

	// 이름 입력
	get_name();
	// 맵 로드
	get_map();

	// 게임 진행
	while (1)
	{
		clearMap();
		show_Map();
		Move();
	}
	return 0;
}

int getch(void) {

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

void get_map() {
	FILE *fp;
	char ch;
	int x = 0, y = 0;

	fp = fopen("map.txt", "r");

	while ((fscanf(fp, "%c", &ch)) != EOF)
	{
		// 숫자 일때 새로운 스테이지
		if (ch <= '5'&& ch >= '1')
		{
			stage++;
			y = -1;
			continue;
		}
		// e 일때 종료
		if (ch == 'e')
		{
			max_stage = stage;
			break;
		}


		if (ch == '@')

		{
			player_x[stage] = x;
			player_y[stage] = y;

			origin_player_x[stage] = x;
			origin_player_y[stage] = y;

		}

		// 상자 개수 카운트
		if (ch == '$')
		{
			box_count[stage]++;
		}

		// 창고 개수 카운트
		if (ch == 'O')
		{
			goal_count[stage]++;
		}

		if (ch == '\n')
		{
			y++;
			x = 0;
		}
		// 맵 저장
		else
		{
			if (ch == '.') {
				ch = ' ';
			}
			map[stage][y][x] = ch;

			x++;
		}
	}
	// 초기 맵 저장
	for (int i = 0; i < max_stage; i++)
	{
		for (int j = 0; j < Y; j++)
		{
			for (int k = 0; k < X; k++)
			{
				data_map[i][j][k] = map[i][j][k];
			}
		}
	}

	// $개수와 O개수가 다르면 종료
	for (int i = 0; i < max_stage; i++)
	{
		if (box_count[i] != goal_count[i])
		{
			printf("박스 개수와 도착 지점의 개수가 다릅니다.\n");
			return;
		}
	}

	// 스테이지 초기 값
	stage = 0;

	fclose(fp);


}

void show_Map()
{
	StageClear();

	printf("\tHello %s\n\n", usrName);

	// 맵 출력
	for (int i = 0; i < Y; i++)
	{
		for (int j = 0; j < X; j++)
		{
			printf("%c", map[stage][i][j]);
		}
		printf("\n");
	}
	if (master_key == 'd' || master_key == 'u' || master_key == 'e')
	printf("(Command) %c",master_key);
	else if(master_key == 't')
		printf("(Command)")
	else {
		printf("(Command) \n");
	}
	master_key = ' ';
}


void clearMap()
{

	system("clear");
	system("clear");
}

void save()
{
	FILE *save;
	int i;

	// sokoban.txt를 w 모드로 열기
	save = fopen("sokoban.txt", "w");

	// username, stage, move_count저장
	fprintf(save, "%s\n", usrName);
	fprintf(save, "%d\n", stage);
	fprintf(save, "%d\n", move_cnt);
	fprintf(save, "%d\n", undo_cnt);

	i = stage;
	// 현재 map 상태 저장
	for (int j = 0; j < Y; j++)
	{
		for (int k = 0; k < X; k++)
		{
			fprintf(save, "%c", map[i][j][k]);
		}
		fprintf(save, "\n");
	}


	fclose(save);
}


void FileLoad()
{
	FILE *fileload;
	char ch;
	int data_map[Y][X];
	int load_x = 0, load_y = 0, load_z;
	int line = 0;

	// sokoban.txt를 r 모드로 열기
	fileload = fopen("sokoban.txt", "r");
	// 로드한 파일이 빈 파일일 경우, 프로그램 종료하기
	if (fileload == NULL)
	{
		printf("\n\n\nLoad File Doesn't Exist.\n\n");

		exit(1);
	}

	fscanf(fileload, "%s\n", usrName);
	fscanf(fileload, "%d\n%d\n%d", &stage, &move_cnt, &undo_cnt);
	fscanf(fileload, "%c", &ch);

	// 파일의 끝부분까지 파일에 있는 내용 읽기
	while (fscanf(fileload, "%c", &ch) != EOF)
	{
		// ch가 '\n' 개행일 경우, x좌표 0으로 초기화, line과 y좌표 1씩 증가
		if (ch == '\n')
		{
			line++;
			load_x = 0;
			load_y++;
		}
		// 좌표 초기화하고 저장시킨 Undo 맵을 읽지 않기 위해 멈추기
		// 읽어들인 문자를 대입하고 x좌표 1씩 증가
		else
		{
			map[stage][load_y][load_x] = ch;
			load_x++;
		}

		// 플레이어 위치 좌표
		if (ch == '@')
		{
			player_x[stage] = load_x - 1;
			player_y[stage] = load_y;
		}
	}


	printf("\n");
	for (int a = 0; a < 5; a++)
	{
		for (int b = 0; b < Y; b++)
		{
			for (int c = 0; c < X; c++)
			{
				printf("%c", undo_map[a][b][c]);
			}
			printf("\n");
		}
	}

	fclose(fileload);
}

void get_name()
{
	bool flag;
	int i = 0;
	int length;

	printf("Start....\n");

	// usrName[i]를 공백으로 초기화
	for (int i = 0; i < 11; i++)
	{
		usrName[i] = ' ';
	}

	while (1)
	{
		flag = true;
		len = 0;
		printf("input name : ");
		scanf("%s", usrName);

		// usrName[i]의 값이 영문자 혹은 '\0'의 값이 아닐 경우
		for (int i = 0; i < 11; i++)
		{
			if (!(('a' <= usrName[i] && usrName[i] <= 'z') || ('A' <= usrName[i] && usrName[i] <= 'Z')) && (usrName[i] != '\0'))
			{
				flag = false;
			}

			if (usrName[i] == '\0')
				break;
		}

		// usrName[len]의 값이 '\0'일 때까지 반복
		while (usrName[len] != '\0')
		{
			if (len > 9) // usrName의 글자가 10문자 초과했을 경우
				flag = false;

			len++;
		}

		if (flag)
			break;

		printf("Name Must Be In English And Do Not Exceed 10 Letters.\n");
	}

	getch();
	clearMap();
}

void Move()
{
	char key = ' ';
	int dx = 0, dy = 0;

	// 키 입력
	key = getch();
	clearMap();
	show_Map();
	// h, j, k, l 이외의 명령이 들어올 경우 Other_Command() 함수를 통하여 입력받음
	if (((((((key != 'h' && key != 'j') && key != 'k') && key != 'l') && key != 'H') && key != 'J') && key != 'K') && key != 'L')
	{
		master_key = key;
		Other_Command(key);
		return;
	}
	switch (key)
	{
		//  왼쪽 이동
	case 'h':
	case 'H':
		dx = -1;
		break;

		// 아래쪽 이동
	case 'j':
	case 'J':
		dy = 1;
		break;

		// 위쪽 이동
	case 'k':
	case 'K':
		dy = -1;
		break;

		// 오른쪽 이동
	case 'l':
	case 'L':
		dx = 1;
		break;
	}
	
	// 충돌 체크
	if (map[stage][player_y[stage] + dy][player_x[stage] + dx] == '#') // 앞에 '#'벽이 있으면
	{
		dx = 0; // 움직이지 않음
		dy = 0;
	}
	if (map[stage][player_y[stage] + dy][player_x[stage] + dx] == '$') // '$'상자를 만나면
	{
		if (map[stage][player_y[stage] + 2 * dy][player_x[stage] + 2 * dx] == ' ') // 그 앞이 공백이라면
		{
			set_undo(); // 언두 맵 저장
			map[stage][player_y[stage] + dy][player_x[stage] + dx] = '@'; // 움직인 후 플레이어 좌표 저장
			map[stage][player_y[stage] + 2 * dy][player_x[stage] + 2 * dx] = '$'; // 움직인 후 상자 좌표 저장
		}
		else if (map[stage][player_y[stage] + 2 * dy][player_x[stage] + 2 * dx] == '#' || map[stage][player_y[stage] + 2 * dy][player_x[stage] + 2 * dx] == '$') // 플레이어가 상자를 밀고 있을때 상자 앞이 # 이거나 $ 이라면
		{
			dx = 0; // 움직이지 않음
			dy = 0;
		}
	}

	if (map[stage][player_y[stage] + dy][player_x[stage] + dx] == ' ') // 플레이어가 움직인다면
	{
		set_undo(); // 언두 맵 저장
	}

	if (map[stage][player_y[stage] + dy][player_x[stage] + dx] == 'O') // 'O'를 만난다면
	{
		set_undo(); // 언두 맵 저장
	}

	if (map[stage][player_y[stage] + 2 * dy][player_x[stage] + 2 * dx] == 'O' &&map[stage][player_y[stage] + dy][player_x[stage] + dx] == '$') // 상자를 밀고 있고, 상자 앞에 'O'가 있다면
	{
		set_undo(); // 언두 맵 저장
		map[stage][player_y[stage] + 2 * dy][player_x[stage] + 2 * dx] = '$'; // 상자를 앞으로 움직임
	}

	map[stage][player_y[stage]][player_x[stage]] = ' '; // 플레이어의 전 위치를 지워줌

	if (data_map[stage][player_y[stage]][player_x[stage]] == 'O') // 원래 맵에서 'O'였으면
	{
		map[stage][player_y[stage]][player_x[stage]] = 'O'; // 'O' 계속 유지
	}

	player_x[stage] += dx; // 플레이어 좌표 설정
	player_y[stage] += dy; // 플레이어 좌표 설정
	map[stage][player_y[stage]][player_x[stage]] = '@'; // @에 플레이어 좌표 대입

	if (!(dx == 0 && dy == 0)) // 움직임이 있다면
	{
		move_cnt++; // move_cnt를 올린다
	}
}

void set_undo()
{
	int i, j, k = 0;
	// 언두 맵 저장횟수가 5번이 넘었을경우
	if (save_cnt >= 5)
	{
		for (i = 1; i < STAGE; i++)
		{
			for (j = 0; j < Y; j++)
			{
				for (k = 0; k < X; k++)
				{
					undo_map[i - 1][j][k] = undo_map[i][j][k]; // 마지막 언두 저장맵을 버림
				}
			}
		}
		save_cnt--;
	}
	// 언두를 위한 맵 저장
	for (j = 0; j < Y; j++)
	{
		for (k = 0; k < X; k++)
		{
			undo_map[save_cnt][j][k] = map[stage][j][k];
		}
	}
	save_cnt++;
}

void Undo()
{
	int undo_x = 0, undo_y = 0;

	if ((undo_cnt < 1) || ((5 - undo_cnt) >= move_cnt))
		return; // 언두 카운트가 0이거나 무브 카운트가 0인데 언두 가능 횟수가 남아 있을 경우 언두를 실행하지 않음

	undo_cnt--;
	save_cnt--;

	for (int i = 0; i < Y; i++)
	{
		for (int j = 0; j < X; j++)
		{
			map[stage][i][j] = undo_map[save_cnt][i][j]; // 현재 맵을 언두맵에서 저장한 맵으로 대체함
		}
	}
	// @의 좌표값을 저장함
	for (int i = 0; i < Y; i++)
	{
		for (int j = 0; j < X; j++)
		{
			if (map[stage][i][j] == '@')
			{
				undo_x = j;
				undo_y = i;
			}
		}
	}
	move_cnt++//움직임 횟수 증가
	player_y[stage] = undo_y; // 플레이어 y좌표 조정
	player_x[stage] = undo_x; // 플레이어 x좌표 조정
}

bool StageClear()
{
	int count = 0;
	bool flag = false;

	for (int y = 0; y < Y; y++)
	{
		for (int x = 0; x < X; x++)
		{
			if (data_map[stage][y][x] == 'O' && map[stage][y][x] == '$') // 원래 맵의 O의 좌표와 현재 맵의 $의 좌표가 같을 경우
			{
				count++; // count값이 증가
			}
		}
	}

	if (count == goal_count[stage]) // count 값이 스테이지 클리어를 위한 값과 같을 때
	{
		SaveTop(); //점수 기록
		stage++; // 스테이지 증가
		flag = true; // flag에 true값
		move_cnt = 0; //점수 초기화
	}

	if (stage >= max_stage) // 5스테이지 까지 클리어할 경우
	{
		printf("\n\nCongratulations !\n"); // 축하 문구전송
		printf("\nAll Stage Clear !\n\n");
		exit(0); // 프로그램 종료
	}

	if (flag) // 스테이지가 올라가면 언두저장맵을 초기화시킴
	{
		for (int k = 0; k < STAGE; k++)
		{
			for (int i = 0; i < Y; i++)
			{
				for (int j = 0; j < X; j++)
				{
					undo_map[k][i][j] = ' ';
				}
			}
		}

		save_cnt = 0; // 스테이지가 올라가면 세이브 카운트 초기화
		undo_cnt = 5; // 스테이지가 올라가면  언두 카운트 초기화
		move_cnt = 0; // 스테이지가 올라가면 무브 카운트 초기화
	}

	return flag;
}

void Other_Command(char key)
{
	char enter;
	int Top_i;

	// h(H), j(J), k(K), l(L) 를 제외한 키 출력하기
	

	switch (key)
	{
	case 's':
	case 'S':
		save(); // s(S) 키를 눌렀을 때 Save() 기능 실행
		break;

	case 'f':
	case 'F':
		FileLoad(); // f(F) 키를 눌렀을 때 FileLoad() 기능 실행
		break;

	case 'd':
	case 'D':
		if (d_flg == 0)
		{
			d_flg = 1;
			Display_help();
		}
		break;

	case 'r':
	case 'R':
		Replay();
		break;

	case 'n':
	case 'N':
		New();
		break;


	case 't':
	case 'T':
		printf("t");
		if (t_flg == 0)
		{
			t_flg = 1;
		}
		enter = getch();
		if (enter == '\n')
			Top_i = 0; //t만 입력했을때
		else {
			enter = getch();
			printf(" ");
			switch (enter)
			{
				// enter 값이 1, 2, 3, 4, 5일 경우 각각의 스테이지 Top 출력
			case '1':
				printf("1");
				Top_i = 1;
				break;

			case '2':
				printf("2");
				Top_i = 2;
				break;

			case '3':
				printf("3");
				Top_i = 3;
				break;

			case '4':
				printf("4");
				Top_i = 4;
				break;

			case '5':
				printf("5");
				Top_i = 5;
				break;

				// enter 값이 '\n' 개행일 경우 전체 Top 출력
			case '\n':

				break;

			default:
				Top_i = -1;
				printf("\n-----------------------------------\n\n       Command Doesn't Exit.\n\n-----------------------------------\n");
				break;
			}
		}

		while (1)
		{
			if (Top_i == 0)
			{
				Top(0); //맵전체 랭킹기록을 보여줌
				break;
			}
			if (Top_i == -1)
			{
				break;
			}
			if (getch() == '\n' && Top_i != 0 && Top_i != -1)
			{
				Top(Top_i); //해당 맵의 랭킹기록을 보여줌
				break;
			}
		}
		break;

	case 'e':
	case 'E':
		clearMap();
		printf("\n\n\nSEE YOU %s . . . .\n\n\n", usrName);
		save(); // 게임 종료하기 전에 저장하기
		exit(0);
		break;

	case 'u':
	case 'U'://u, U 가 입력시 언두 기능 실행
		Undo();
		break;

	default:
		if (enter == '\n')
			printf("\n-----------------------------------\n\n       Command Doesn't Exist.\n\n-----------------------------------\n");
		break;
	}
}

void SaveTop()
{
	FILE *fp;
	char name[11];
	char a;
	int record;
	int i, j, k;

	fp = fopen("ranking.txt", "r");

	//입력
	while (1)
	{
		for (i = 0; i < STAGE; i++)
		{
			for (j = 0; j < 5; j++)
			{
				fscanf(fp, "%s %d\n", name, &record);
				for (k = 0; k < 11; k++)
				{
					if ((name[0] == '.' && name[1] == '.' && name[2] == '.' && name[3] == '\0') || (name[0] == '.' && name[1] == '.' && name[2] == '\0'))
					{
						rank_name[i][j][0] = ' ';
						rank_name[i][j][1] = ' ';
						rank_name[i][j][2] = '\0';
						break; //이름이 ... 디폴트 값으로 되어있으면 배열에 "  "으로 저장한다
					}
					if (name[k] == '\0')
					{
						break;
					}
					rank_name[i][j][k] = name[k];
				}
				rank_cnt[i][j] = record;
			}
		}
		if ((fscanf(fp, "%c", &a)) == EOF)
		{
			break; //파일이 끝나면 더이상 반복하지 않는다.
		}
	}

	//새로운 점수 값이 입력되면 크기를 비교해 배열에 넣는다.
	for (i = 0; i < 4; i++)
	{
		//새로운 점수값이 0~3번 점수들 사이에 들어가거나 0을제외한 값중 가장 클때
		if (rank_cnt[stage][i] > move_cnt || rank_cnt[stage][i] == 0)
		{
			//새로운 점수값이 들어갈자리를 비우기 위해 뒤로 값을 미룬다
			for (j = 0; j < 4 - i; j++)
			{
				rank_cnt[stage][4 - j] = rank_cnt[stage][3 - j];
				for (k = 0; k < 11; k++)
				{
					rank_name[stage][4 - j][k] = rank_name[stage][3 - j][k];
				}
			}
			for (k = 0; k < 11; k++)  //새로운 점수값 입력
			{
				rank_name[stage][i][k] = usrName[k];
			}
			rank_cnt[stage][i] = move_cnt;
			break;
		}
	}
	//새로운 점수가 4번째값일때
	if (rank_cnt[stage][i] > move_cnt || rank_cnt[stage][i] == 0)
	{
		rank_cnt[stage][i] = move_cnt;

		for (k = 0; k < 11; k++)
		{
			rank_name[stage][i][k] = usrName[k];
		}
	}

	fclose(fp);

	//랭킹 파일에 저장
	fp = fopen("ranking.txt", "w");

	for (i = 0; i < 5; i++)
	{
		for (j = 0; j < 5; j++)
		{
			if (rank_name[i][j][0] == ' ' && rank_name[i][j][1] == ' ' && rank_name[i][j][2] == '\0')
			{
				fprintf(fp, "... ", rank_name[i][j]); //이름이 "  "이면 ... 디폴트 값으로 저장
			}
			else
			{
				fprintf(fp, "%s ", rank_name[i][j]);
			}
			fprintf(fp, "%d", rank_cnt[i][j]);
		}
	}

	fclose(fp);

}

void Top(int Top_num)
{
	FILE *fp;
	char name[11];
	char a;
	int record;
	int i, j, k;

	fp = fopen("ranking.txt", "r");

	//이름과 기록을 읽어들인다.
	while (1)
	{
		for (i = 0; i < STAGE; i++)
		{
			for (j = 0; j < 5; j++)
			{
				fscanf(fp, "%s %d\n", &name, &record);
				for (k = 0; k < 11; k++)
				{
					if ((name[0] == '.' && name[1] == '.' && name[2] == '.') || (name[0] == '.' && name[1] == '.'))
					{
						rank_name[i][j][0] = ' ';
						rank_name[i][j][1] = ' ';
						rank_name[i][j][2] = '\0';
						break; //이름이 ... 디폴트 값으로 되어있으면 배열에 "  "으로 저장한다
					}
					if (name[k] == '\0')
					{
						break;
					}
					rank_name[i][j][k] = name[k];
				}
				rank_cnt[i][j] = record;
			}
		}
		if ((fscanf(fp, "%c", &a)) == EOF)
		{
			break; //파일이 끝나면 더이상 반복하지 않는다.
		}
	}

	int s = 0; //이름이 저장되어있는지의 유무로 출력하기위한 변수

	//Top_i에 따라 출력
	while (1)
	{
		clearMap(); //0일때 모든맵의 기록 출력
		if (Top_num == 0)
		{
			for (i = 0; i < STAGE; i++)
			{
				printf("map %d\n", i + 1);
				for (j = 0; j < 5; j++)
				{
					for (k = 0; k < 11; k++)
					{
						if (rank_name[i][j][0] == ' ' && rank_name[i][j][1] == ' ' && rank_name[i][j][2] == '\0')
						{
							s = 0;
							break; //이름이 "  "으로 기록이 없으면 출력하지 않게 함
						}
						else
						{
							s = 1;
							break;
						}
					}
					if (s == 1) //기록이 있을경우 출력
					{
						printf("%s  ", rank_name[i][j]);
						printf("%d", rank_cnt[i][j]);
					}
					printf("\n");
				}
			}
			printf("\n\n(Command) t");
		}
		if (Top_num != 0)
		{
			i = Top_num - 1;
			printf("map %d\n", i + 1);
			for (j = 0; j < 5; j++)
			{
				for (k = 0; k < 11; k++)
				{
					if (rank_name[i][j][0] == ' ' && rank_name[i][j][1] == ' ' && rank_name[i][j][2] == '\0')
					{
						s = 0;
						break; //이름이 "  "으로 기록이 없으면 출력하지 않게 함
					}
					else
					{
						s = 1;
						break;
					}
				}
				if (s == 1) //기록이 있을경우 출력
				{
					printf("%s  ", rank_name[i][j]);
					printf("%d", rank_cnt[i][j]);
				}
				printf("\n");
			}
			printf("\n\n(Command) t %d",i + 1);
		}
		printf("\n나가려면 엔터키를 누르시오."); //출력후 들어온 상태면서 t,T 를 누를경우 빠져나간다.
		if (getch() == '\n' && t_flg == 1)
		{
			t_flg = 0;
			break;
		}
	}
	return;

}

void Display_help()
{
	clearMap();
	while (1)
	{
		clearMap();
		printf("\tHello %s\n", usrName);
		printf("h : 왼쪽으로 이동, j : 아래로 이동, k : 위로 이동, l : 오른쪽으로 이동\n");
		printf("u : 움직이기 전 상태로 이동한다. (최대 5번 가능)\n");
		printf("r : 현재 맵을 처음부터 다시시작한다.\n");
		printf("n : 첫 번째 맵부터 다시 시작\n");
		printf("e : 게임종료\n");
		printf("s : 게임 저장\n");
		printf("f : 게임을 이어서한다.\n");
		printf("d : 명령내용을 보여준다.\n");
		printf("t : 게임 순위를 보여준다.\n");
		printf("나가려면 엔터키를 누르시오.\n");
		printf("\n\n(Command) d");
		if (getch() == '\n' & d_flg == 1)
		{
			d_flg = 0;
			break;
		}
	}
	return;
}

void Replay()
{
	int stage_tmp = stage;
	clearMap();
	int i, j, k;

	for (int k = 0; k < STAGE; k++)
	{
		for (int i = 0; i < Y; i++)
		{
			for (int j = 0; j < X; j++)
			{
				undo_map[k][i][j] = ' ';
			}
		}
	}

	move_cnt = 0;
	undo_cnt = 5;
	save_cnt = 0;
	//초기맵과 초기플레이어위치 불러오기
	i = stage_tmp;
	for (j = 0; j < Y; j++)
	{
		for (k = 0; k < X; k++)
		{
			map[i][j][k] = data_map[i][j][k];
		}
	}
	player_x[i] = origin_player_x[i];
	player_y[i] = origin_player_y[i];
}

void New()
{

	clearMap();
	stage = 0; //1스테이지로 변경

	int i, j, k;

	for (int k = 0; k < max_stage; k++)
	{
		for (int i = 0; i < Y; i++)
		{
			for (int j = 0; j < X; j++)
			{
				undo_map[k][i][j] = ' ';
			}
		}
	}

	move_cnt = 0;
	undo_cnt = 5;
	save_cnt = 0;
	//초기맵과 초기플레이어위치 불러오기
	for (i = 0; i < max_stage; i++)
	{
		for (j = 0; j < Y; j++)
		{
			for (k = 0; k < X; k++)
			{
				map[i][j][k] = data_map[i][j][k];
			}
		}
		player_x[i] = origin_player_x[i];
		player_y[i] = origin_player_y[i];
	}
}
