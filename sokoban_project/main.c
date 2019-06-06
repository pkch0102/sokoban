#include <stdio.h>
#include <stdlib.h>
#include <termio.h>
#include <stdbool.h>

#define Y 30 // �� �ִ� Y ��
#define X 30 // �� �ִ� X ��
#define STAGE 5 // �ִ� �������� ��

char data_map[STAGE][31][31], undo_map[STAGE][31][31], map[STAGE][31][31];
char usrName[11], rank_name[STAGE][5][11];
int maxi[5];
int maxj[5];
int goal_loc[5][5][5], rank_cnt[STAGE][5];
int box_count[STAGE], goal_count[STAGE];
int stage = -1, undo_cnt = 5, move_cnt = 0, save_cnt = 0;
int player_x[5], player_y[5], origin_player_x[5], origin_player_y[5];
int d_flg, t_flg, record;

void get_map();
void show_Map();
void clearMap();
void save();
void FileLoad();
void Move();
void get_name();
void set_undo();
void Undo();
void SaveTop();
void New();
void Replay();
void Option(char key);
bool StageClear();
void Display();
void Top(int Top_num);
int getch(void);

int main()
{

	// �̸� �Է�
	get_name();
	// �� �ε�
	get_map();

	// ���� ����
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
		// ���� �϶� ���ο� ��������
		if (ch <= '5'&& ch >= '1')
		{
			stage++;
			y = -1;
			continue;
		}
		// e �϶� ����
		if (ch == 'e')
		{
			break;
		}


		if (ch == '@')

		{
			player_x[stage] = x;
			player_y[stage] = y;

			origin_player_x[stage] = x;
			origin_player_y[stage] = y;

		}

		// ���� ���� ī��Ʈ
		if (ch == '$')
		{
			box_count[stage]++;
		}

		// O ���� ī��Ʈ
		if (ch == 'O')
		{
			goal_count[stage]++;
		}

		if (ch == '\n')
		{
			y++;
			x = 0;
		}
		// �� ����
		else
		{
			if (ch == '.') {
				ch = ' ';
			}
			map[stage][y][x] = ch;

			x++;
		}
	}
	// �ʱ� �� ����
	for (int i = 0; i < STAGE; i++)
	{
		for (int j = 0; j < Y; j++)
		{
			for (int k = 0; k < X; k++)
			{
				data_map[i][j][k] = map[i][j][k];
			}
		}
	}

	// $������ O������ �ٸ��� ����
	for (int i = 0; i < STAGE; i++)
	{
		if (box_count[i] != goal_count[i])
		{
			printf("ERROR : �ڽ� ������ ���� ������ ������ �ٸ��ϴ�.\n");
			return;
		}
	}

	// �������� �ʱ� ��
	stage = 0;

	fclose(fp);


}

void show_Map()
{
	StageClear();

	printf("    Hello %s\n\n", usrName);

	// �� ���
	for (int i = 0; i < Y; i++)
	{
		for (int j = 0; j < X; j++)
		{
			printf("%c", map[stage][i][j]);
		}
		printf("\n");
	}
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

	// sokoban.txt�� w ���� ����
	save = fopen("sokoban.txt", "w");

	// username, stage, move_count����
	fprintf(save, "%s\n", usrName);
	fprintf(save, "%d\n", stage);
	fprintf(save, "%d\n", move_cnt);
	fprintf(save, "%d\n", undo_cnt);

	i = stage;
	// ���� map ���� ����
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

	// sokoban.txt�� r ���� ����
	fileload = fopen("sokoban.txt", "r");
	// �ε��� ������ �� ������ ���, ���α׷� �����ϱ�
	if (fileload == NULL)
	{
		printf("\n\n\nLoad File Doesn't Exist.\n\n");

		exit(1);
	}

	fscanf(fileload, "%s\n", usrName);
	fscanf(fileload, "%d\n%d\n%d", &stage, &move_cnt, &undo_cnt);
	fscanf(fileload, "%c", &ch);

	// ������ ���κб��� ���Ͽ� �ִ� ���� �б�
	while (fscanf(fileload, "%c", &ch) != EOF)
	{
		// ch�� '\n' ������ ���, x��ǥ 0���� �ʱ�ȭ, line�� y��ǥ 1�� ����
		if (ch == '\n')
		{
			line++;
			load_x = 0;
			load_y++;
		}
		// ��ǥ �ʱ�ȭ�ϰ� �����Ų Undo ���� ���� �ʱ� ���� ���߱�
		// �о���� ���ڸ� �����ϰ� x��ǥ 1�� ����
		else
		{
			map[stage][load_y][load_x] = ch;
			load_x++;
		}

		// �÷��̾� ��ġ ��ǥ
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

	// usrName[i]�� �������� �ʱ�ȭ
	for (int i = 0; i < 11; i++)
	{
		usrName[i] = ' ';
	}

	while (1)
	{
		flag = true;
		length = 0;
		printf("input name : ");
		scanf("%s", usrName);

		// username[i]�� ���� ������ Ȥ�� '\0'�� ���� �ƴ� ���
		for (int i = 0; i < 11; i++)
		{
			if (!(('a' <= usrName[i] && usrName[i] <= 'z') || ('A' <= usrName[i] && usrName[i] <= 'Z')) && (usrName[i] != '\0'))
			{
				flag = false;
			}

			if (usrName[i] == '\0')
				break;
		}

		// username[length]�� ���� '\0'�� ������ �ݺ�
		while (usrName[length] != '\0')
		{
			if (length > 9) // usrName�� ���ڰ� 10���� �ʰ����� ���
				flag = false;

			length++;
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
	char key;
	int dx = 0, dy = 0;

	printf("(Command) ");

	// Ű �Է�
	key = getch();

	switch (key)
	{
		//  ���� �̵�
	case 'h':
	case 'H':
		dx = -1;
		break;

		// �Ʒ��� �̵�
	case 'j':
	case 'J':
		dy = 1;
		break;

		// ���� �̵�
	case 'k':
	case 'K':
		dy = -1;
		break;

		// ������ �̵�
	case 'l':
	case 'L':
		dx = 1;
		break;
	}
	// h, j, k, l �̿��� ����� ���� ��� Option() �Լ��� ���Ͽ� �Է¹���
	if (((((((key != 'h' && key != 'j') && key != 'k') && key != 'l') && key != 'H') && key != 'J') && key != 'K') && key != 'L')
	{
		Option(key);
	}
	// �浹 üũ
	if (map[stage][player_y[stage] + dy][player_x[stage] + dx] == '#') // �տ� '#'���� ������
	{
		dx = 0; // �������� ����
		dy = 0;
	}
	if (map[stage][player_y[stage] + dy][player_x[stage] + dx] == '$') // '$'���ڸ� �����ٸ�
	{
		if (map[stage][player_y[stage] + 2 * dy][player_x[stage] + 2 * dx] == ' ') // �� ���� �����̶��
		{
			set_undo(); // ��� �� ����
			map[stage][player_y[stage] + dy][player_x[stage] + dx] = '@'; // ������ �� �÷��̾� ��ǥ ����
			map[stage][player_y[stage] + 2 * dy][player_x[stage] + 2 * dx] = '$'; // ������ �� ���� ��ǥ ����
		}
		else if (map[stage][player_y[stage] + 2 * dy][player_x[stage] + 2 * dx] == '#' || map[stage][player_y[stage] + 2 * dy][player_x[stage] + 2 * dx] == '$') // �÷��̾ ���ڸ� �а� ������ ���� ���� # �̰ų� $ �̶��
		{
			dx = 0; // �������� ����
			dy = 0;
		}
	}

	if (map[stage][player_y[stage] + dy][player_x[stage] + dx] == ' ') // �÷��̾ �����δٸ�
	{
		set_undo(); // ��� �� ����
	}

	if (map[stage][player_y[stage] + dy][player_x[stage] + dx] == 'O') // 'O'�� �����ٸ�
	{
		set_undo(); // ��� �� ����
	}

	if (map[stage][player_y[stage] + 2 * dy][player_x[stage] + 2 * dx] == 'O' &&map[stage][player_y[stage] + dy][player_x[stage] + dx] == '$') // ���ڸ� �а� �ְ�, ���� �տ� 'O'�� �ִٸ�
	{
		set_undo(); // ��� �� ����
		map[stage][player_y[stage] + 2 * dy][player_x[stage] + 2 * dx] = '$'; // ���ڸ� ������ ������
	}

	map[stage][player_y[stage]][player_x[stage]] = ' '; // �÷��̾��� �� ��ġ�� ������

	if (data_map[stage][player_y[stage]][player_x[stage]] == 'O') // ���� �ʿ��� 'O'������
	{
		map[stage][player_y[stage]][player_x[stage]] = 'O'; // 'O' ��� ����
	}

	player_x[stage] += dx; // �÷��̾� ��ǥ ����
	player_y[stage] += dy; // �÷��̾� ��ǥ ����
	map[stage][player_y[stage]][player_x[stage]] = '@'; // @�� �÷��̾� ��ǥ ����

	if (!(dx == 0 && dy == 0)) // �������� �ִٸ�
	{
		move_cnt++; // move_count�� �ø���
	}
}

void set_undo()
{
	int i, j, k = 0;
	// ��� �� ����Ƚ���� 5���� �Ѿ������
	if (save_cnt >= 5)
	{
		for (i = 1; i < STAGE; i++)
		{
			for (j = 0; j < Y; j++)
			{
				for (k = 0; k < X; k++)
				{
					undo_map[i - 1][j][k] = undo_map[i][j][k]; // ������ ��� ������� ����
				}
			}
		}
		save_cnt--;
	}
	// ��θ� ���� �� ����
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
		return; // ��� ī��Ʈ�� 0�̰ų� ���� ī��Ʈ�� 0�ε� ��� ���� Ƚ���� ���� ���� ��� ��θ� �������� ����

	undo_cnt--;
	save_cnt--;

	for (int i = 0; i < Y; i++)
	{
		for (int j = 0; j < X; j++)
		{
			map[stage][i][j] = undo_map[save_cnt][i][j]; // ���� ���� ��θʿ��� ������ ������ ��ü��
		}
	}
	// @�� ��ǥ���� ������
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

	player_y[stage] = undo_y; // �÷��̾� y��ǥ ����
	player_x[stage] = undo_x; // �÷��̾� x��ǥ ����
}

bool StageClear()
{
	int count = 0;
	bool flag = false;

	for (int y = 0; y < Y; y++)
	{
		for (int x = 0; x < X; x++)
		{
			if (data_map[stage][y][x] == 'O' && map[stage][y][x] == '$') // ���� ���� O�� ��ǥ�� ���� ���� $�� ��ǥ�� ���� ���
			{
				count++; // count���� ����
			}
		}
	}

	if (count == goal_count[stage]) // count ���� �������� Ŭ��� ���� ���� ���� ��
	{
		SaveTop(); //���� ���
		stage++; // �������� ����
		flag = true; // flag�� true��
		move_cnt = 0; //���� �ʱ�ȭ
	}

	if (stage >= 5) // 5�������� ���� Ŭ������ ���
	{
		printf("\n\nCongratulations !\n"); // ���� ��������
		printf("\nAll Stage Clear !\n\n");
		exit(0); // ���α׷� ����
	}

	if (flag) // ���������� �ö󰡸� ���������� �ʱ�ȭ��Ŵ
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

		save_cnt = 0; // ���������� �ö󰡸� ���̺� ī��Ʈ �ʱ�ȭ
		undo_cnt = 5; // ���������� �ö󰡸�  ��� ī��Ʈ �ʱ�ȭ
		move_cnt = 0; // ���������� �ö󰡸� ���� ī��Ʈ �ʱ�ȭ
	}

	return flag;
}

void Option(char key)
{
	char enter;
	int Top_i;

	// h(H), j(J), k(K), l(L) �� ������ Ű ����ϱ�
	if (((((((key != 'h' && key != 'j') && key != 'k') && key != 'l') && key != 'H') && key != 'J') && key != 'K') && key != 'L')
		printf("%c", key);


	switch (key)
	{
	case 's':
	case 'S':
		save(); // s(S) Ű�� ������ �� Save() ��� ����
		break;

	case 'f':
	case 'F':
		FileLoad(); // f(F) Ű�� ������ �� FileLoad() ��� ����
		break;

	case 'd':
	case 'D':
		if (d_flg == 0)
		{
			d_flg = 1;
			Display();
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
		if (t_flg == 0)
		{
			t_flg = 1;
		}
		enter = getch();
		if (enter == '\n')
			Top_i = 0; //t�� �Է�������
		else {
			enter = getch();
			printf(" ");
			switch (enter)
			{
				// enter ���� 1, 2, 3, 4, 5�� ��� ������ �������� Top ���
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

				// enter ���� '\n' ������ ��� ��ü Top ���
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
				Top(0); //����ü ��ŷ����� ������
				break;
			}
			if (Top_i == -1)
			{
				break;
			}
			if (getch() == '\n' && Top_i != 0 && Top_i != -1)
			{
				Top(Top_i); //�ش� ���� ��ŷ����� ������
				break;
			}
		}
		break;

	case 'e':
	case 'E':
		clearMap();
		printf("\n\n\nSEE YOU %s . . . .\n\n\n", usrName);
		save(); // ���� �����ϱ� ���� �����ϱ�
		exit(0);
		break;

	case 'u':
	case 'U'://u, U �� �Է½� ��� ��� ����
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

	//�Է�
	while (1)
	{
		for (i = 0; i < STAGE; i++)
		{
			for (j = 0; j < 5; j++)
			{
				fscanf(fp, "%s %d\n", name, &record);
				for (k = 0; k < 11; k++)
				{
					if (name[0] == '.' && name[1] == '.' && name[2] == '.' && name[3] == '\0')
					{
						rank_name[i][j][0] = ' ';
						rank_name[i][j][1] = ' ';
						rank_name[i][j][2] = '\0';
						break; //�̸��� 0.0 ����Ʈ ������ �Ǿ������� �迭�� "  "���� �����Ѵ�
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
			break; //������ ������ ���̻� �ݺ����� �ʴ´�.
		}
	}

	//���ο� ���� ���� �ԷµǸ� ũ�⸦ ���� �迭�� �ִ´�.
	for (i = 0; i < 4; i++)
	{
		//���ο� �������� 0~3���� ������ ���̿� ���ų� 0�������� ���� ���� Ŭ��
		if (rank_cnt[stage][i] > move_cnt || rank_cnt[stage][i] == 0)
		{
			//���ο� �������� ���ڸ��� ���� ���� �ڷ� ���� �̷��
			for (j = 0; j < 4 - i; j++)
			{
				rank_cnt[stage][4 - j] = rank_cnt[stage][3 - j];
				for (k = 0; k < 11; k++)
				{
					rank_name[stage][4 - j][k] = rank_name[stage][3 - j][k];
				}
			}
			for (k = 0; k < 11; k++)  //���ο� ������ �Է�
			{
				rank_name[stage][i][k] = usrName[k];
			}
			rank_cnt[stage][i] = move_cnt;
			break;
		}
	}
	//���ο� ������ 4��°���϶�
	if (rank_cnt[stage][i] > move_cnt || rank_cnt[stage][i] == 0)
	{
		rank_cnt[stage][i] = move_cnt;

		for (k = 0; k < 11; k++)
		{
			rank_name[stage][i][k] = usrName[k];
		}
	}

	fclose(fp);

	//��ŷ ���Ͽ� ����
	fp = fopen("ranking.txt", "w");

	for (i = 0; i < 5; i++)
	{
		for (j = 0; j < 5; j++)
		{
			if (rank_name[i][j][0] == ' ' && rank_name[i][j][1] == ' ' && rank_name[i][j][2] == '\0')
			{
				fprintf(fp, "... ", rank_name[i][j]); //�̸��� "  "�̸� ... ����Ʈ ������ ����
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

	//�̸��� ����� �о���δ�.
	while (1)
	{
		for (i = 0; i < STAGE; i++)
		{
			for (j = 0; j < 5; j++)
			{
				fscanf(fp, "%s %d\n", &name, &record);
				for (k = 0; k < 11; k++)
				{
					if (name[0] == '.' && name[1] == '.' && name[2] == '.')
					{
						rank_name[i][j][0] = ' ';
						rank_name[i][j][1] = ' ';
						rank_name[i][j][2] = '\0';
						break; //�̸��� 0.0 ����Ʈ ������ �Ǿ������� �迭�� "  "���� �����Ѵ�
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
			break; //������ ������ ���̻� �ݺ����� �ʴ´�.
		}
	}

	int s = 0; //�̸��� ����Ǿ��ִ����� ������ ����ϱ����� ����

	//Top_i�� ���� ���
	while (1)
	{
		clearMap(); //0�϶� ������ ��� ���
		if (Top_num == 0)
		{
			for (i = 0; i < STAGE; i++)
			{
				printf("map %d\n\n", i + 1);
				for (j = 0; j < 5; j++)
				{
					for (k = 0; k < 11; k++)
					{
						if (rank_name[i][j][0] == ' ' && rank_name[i][j][1] == ' ' && rank_name[i][j][2] == '\0')
						{
							s = 0;
							break; //�̸��� "  "���� ����� ������ ������� �ʰ� ��
						}
						else
						{
							s = 1;
							break;
						}
					}
					if (s == 1) //����� ������� ���
					{
						printf("%s  ", rank_name[i][j]);
						printf("%d", rank_cnt[i][j]);
					}
					printf("\n");
				}
			}
		}
		if (Top_num != 0)
		{
			i = Top_num - 1;
			printf("map %d\n\n", i + 1);
			for (j = 0; j < 5; j++)
			{
				for (k = 0; k < 11; k++)
				{
					if (rank_name[i][j][0] == ' ' && rank_name[i][j][1] == ' ' && rank_name[i][j][2] == '\0')
					{
						s = 0;
						break; //�̸��� "  "���� ����� ������ ������� �ʰ� ��
					}
					else
					{
						s = 1;
						break;
					}
				}
				if (s == 1) //����� ������� ���
				{
					printf("%s  ", rank_name[i][j]);
					printf("%d", rank_cnt[i][j]);
				}
				printf("\n");
			}
		}
		printf("�������� ����Ű�� �����ÿ�."); //����� ���� ���¸鼭 t,T �� ������� ����������.
		if (getch() == '\n' && t_flg == 1)
		{
			t_flg = 0;
			break;
		}
	}
	return;

}

void Display()
{
	while (1)
	{
		clearMap();
		printf("h : �������� �̵�, j : �Ʒ��� �̵�, k : ���� �̵�, l : ���������� �̵�\n");
		printf("u : �����̱� �� ���·� �̵��Ѵ�. (�ִ� 5�� ����)\n");
		printf("r : ���� ���� ó������ �ٽý����Ѵ�.\n");
		printf("n : ù ��° �ʺ��� �ٽ� ����\n");
		printf("e : ��������\n");
		printf("s : ���� ����\n");
		printf("f : ������ �̾�Ѵ�.\n");
		printf("d : ��ɳ����� �����ش�.\n");
		printf("t : ���� ������ �����ش�.\n");
		printf("�������� ����Ű�� �����ÿ�.\n");
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
	//�ʱ�ʰ� �ʱ��÷��̾���ġ �ҷ�����
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
	stage = 0; //1���������� ����

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
	//�ʱ�ʰ� �ʱ��÷��̾���ġ �ҷ�����
	for (i = 0; i < STAGE; i++)
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