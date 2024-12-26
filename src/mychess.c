#include <stdio.h>
#include "../common/common.h"


#define RED	FB_COLOR(255,0,0)
#define ORANGE	FB_COLOR(255,165,0)
#define YELLOW	FB_COLOR(255,255,0)
#define GREEN	FB_COLOR(0,255,0)
#define CYAN	FB_COLOR(0,127,255)
#define BLUE	FB_COLOR(0,0,255)
#define PURPLE	FB_COLOR(139,0,255)
#define WHITE   FB_COLOR(255,255,255)
#define BLACK   FB_COLOR(0,0,0)
#define MYBLUE  FB_COLOR(0,120,192)
#define OK 1
#define LEAGL_INTERVAL 2000
#define REGRET_X	(SCREEN_WIDTH-60)
#define REGRET_Y	0
#define REGRET_W	60
#define REGRET_H	60
#define Welocme_Bottom_W 238
#define Welocme_Bottom_H 75
#define Bottom_x 355
#define Bottom_1_y 325
#define Bottom_2_y 456

int should_exit = 0;//�˳��ź�
// �洢����״̬�͵�ǰ�غ�,board_bak�洢����״̬��turn_bak�洢��ǰ�غϣ��췽��ڷ�����nextָ����һ������״̬��
struct bak {
	int board_bak[10][11];
	int turn_bak;
	struct bak* next;
};

typedef struct mypoint {
	int x;
	int y;
}point;

int board[10][11] = {
	[5] [1] = 1,[5][10] = -1,
	[4][1] = 2,[6][1] = 2,[4][10] = -2,[6][10] = -2,
	[3][1] = 3,[7][1] = 3,[3][10] = -3,[7][10] = -3,
	[2][1] = 4,[8][1] = 4,[2][10] = -4,[8][10] = -4,
	[1][1] = 5,[9][1] = 5,[1][10] = -5,[9][10] = -5,
	[2][3] = 6,[8][3] = 6,[2][8] = -6,[8][8] = -6,
	[1][4] = 7,[3][4] = 7,[5][4] = 7,[7][4] = 7,[9][4] = 7,
	[1][7] = -7,[3][7] = -7,[5][7] = -7,[7][7] = -7,[9][7] = -7,
};

void ini_board() {
	int mytemp[10][11] = {
	[5] [1] = 1,[5][10] = -1,
	[4][1] = 2,[6][1] = 2,[4][10] = -2,[6][10] = -2,
	[3][1] = 3,[7][1] = 3,[3][10] = -3,[7][10] = -3,
	[2][1] = 4,[8][1] = 4,[2][10] = -4,[8][10] = -4,
	[1][1] = 5,[9][1] = 5,[1][10] = -5,[9][10] = -5,
	[2][3] = 6,[8][3] = 6,[2][8] = -6,[8][8] = -6,
	[1][4] = 7,[3][4] = 7,[5][4] = 7,[7][4] = 7,[9][4] = 7,
	[1][7] = -7,[3][7] = -7,[5][7] = -7,[7][7] = -7,[9][7] = -7,
	};
	for (int i = 0; i < 10; i++) {
		for (int j = 0; j < 11; j++) {
			board[i][j] = mytemp[i][j];
		}
	}
}

static int touch_fd; // �������ļ�������
static int bluetooth_fd; // �����ļ�������


struct bak* latest_bak = NULL; // ָ�����µı���״̬��
struct bak* new_bak = NULL; // 

point global_select = { 0,0 };
point pre_global_select;
point board_pos; // ȫ�ֱ������洢��ǰѡ�е�����λ��
point pre_board_pos; // ȫ�ֱ������洢��һ��ѡ�е�����λ��
point my_ini;
point regame_1;
point regame_2;
point back_1;
point back_2;
int win = 0; // 1 0 -1 �ֱ�����췽ʤ��û�ֳ���ʤ�����ڷ�ʤ
int from_status; // �ǻ�ȡ��ʼλ�û����յ�λ��
int mode = 0; // ���뵱ǰģʽ��1Ϊ˫��ģʽ��0Ϊ����ģʽ
int turn = 1;//��¼�Ǻ췽�غϻ��Ǻڷ��غ�
int select_val = 0; // ѡ��״̬
int distance = 0;
int regret_trigger; // ���崥��
int state; //��ʾ״̬ 0��ӭ 1���� 2Э�齻�� 3����ҳ��
int ini_first;//��ʼ����ڷ�
int rec_ini_first;//���ܳ�ʼ��
int side;


point getdrawposition(point a) {
	point po;
	po.x = 215 + 66 * (a.x - 1) - 33;
	po.y = 32.5 + 66.5 * (a.y - 1) - 33;
	return po;
}


point getlogicalposition(point po) {
	point a;
	a.x = (int)(po.x - 215+33) / 66 + 1;
	a.y = (int)(po.y -32.5+33) / 66.5 + 1;
	return a;
}


void show_select(point a) {
	fb_image* img;
	point temp;
	img = fb_read_png_image("./img/select.png");
	temp = getdrawposition(a);
	fb_draw_image(temp.x + 8, temp.y + 8, img, 0);
	fb_update();
	fb_free_image(img);
	printf("*****************************************DEBUG*********************************************: turn: {%d}\n", turn);
	if (turn == 1) fb_draw_text(50, 50, "�췽����", 64, RED);
	else if (turn == -1) fb_draw_text(50, 50, "�ڷ�����", 64, BLACK);
}


void show_board() {
	printf("**************************DEBUG************************************: board[j_from][i_from],board[j_to][i_to]\n");
	for (int i = 1; i < 11; i++) {
		for (int j = 1; j < 10; j++) {
			printf("%d ", board[j][i]);
		}
		printf("\n");
	}
	printf("**************************DEBUG************************************: comeinto show_board\n");
	fb_image* img;
	img = fb_read_jpeg_image("./img/backround.jpg");
	fb_draw_image(0, 0, img, 0);
	fb_free_image(img);
	img = fb_read_png_image("./img/regame.png");
	fb_draw_image(38.25, 66, img, 0);
	fb_free_image(img);
	img = fb_read_png_image("./img/wantback.png");
	fb_draw_image(38.25, 333, img, 0);
	fb_free_image(img);
	//fb_draw_rect(REGRET_X, REGRET_Y, REGRET_W, REGRET_H, RED);
	fb_update();
	img = fb_read_jpeg_image("./test.jpg");
	fb_draw_image((SCREEN_WIDTH - img->pixel_w) / 2, 0, img, 0);
	fb_update();
	fb_free_image(img);
	for (int i = 1; i < 10; i++) {
		for (int j = 1; j < 11; j++) {
			point temp;
			point a;
			switch (board[i][j]) {
			case  1:
			{
				img = fb_read_png_image("./img/1.png");
				a.x = j; a.y = i;
				temp = getdrawposition(a);
				fb_draw_image(temp.x, temp.y, img, 0);
				fb_update();
				fb_free_image(img);
				break;
			}
			case  2:
			{
				img = fb_read_png_image("./img/2.png");
				a.x = j; a.y = i;
				temp = getdrawposition(a);
				fb_draw_image(temp.x, temp.y, img, 0);
				fb_update();
				fb_free_image(img);
				break;
			}
			case  3:
			{
				img = fb_read_png_image("./img/3.png");
				a.x = j; a.y = i;
				temp = getdrawposition(a);
				fb_draw_image(temp.x, temp.y, img, 0);
				fb_update();
				fb_free_image(img);
				break;
			}
			case  4:
			{
				img = fb_read_png_image("./img/4.png");
				a.x = j; a.y = i;
				temp = getdrawposition(a);
				fb_draw_image(temp.x, temp.y, img, 0);
				fb_update();
				fb_free_image(img);
				break;
			}
			case  5:
			{
				img = fb_read_png_image("./img/5.png");
				a.x = j; a.y = i;
				temp = getdrawposition(a);
				fb_draw_image(temp.x, temp.y, img, 0);
				fb_update();
				fb_free_image(img);
				break;
			}
			case  6:
			{
				img = fb_read_png_image("./img/6.png");
				a.x = j; a.y = i;
				temp = getdrawposition(a);
				fb_draw_image(temp.x, temp.y, img, 0);
				fb_update();
				fb_free_image(img);
				break;
			}
			case  7:
			{
				img = fb_read_png_image("./img/7.png");
				a.x = j; a.y = i;
				temp = getdrawposition(a);
				fb_draw_image(temp.x, temp.y, img, 0);
				fb_update();
				fb_free_image(img);
				break;
			}
			case  -1:
			{
				img = fb_read_png_image("./img/-1.png");
				a.x = j; a.y = i;
				temp = getdrawposition(a);
				fb_draw_image(temp.x, temp.y, img, 0);
				fb_update();
				fb_free_image(img);
				break;
			}
			case  -2:
			{
				img = fb_read_png_image("./img/-2.png");
				a.x = j; a.y = i;
				temp = getdrawposition(a);
				fb_draw_image(temp.x, temp.y, img, 0);
				fb_update();
				fb_free_image(img);
				break;
			}
			case  -3:
			{
				img = fb_read_png_image("./img/-3.png");
				a.x = j; a.y = i;
				temp = getdrawposition(a);
				fb_draw_image(temp.x, temp.y, img, 0);
				fb_update();
				fb_free_image(img);
				break;
			}
			case  -4:
			{
				img = fb_read_png_image("./img/-4.png");
				a.x = j; a.y = i;
				temp = getdrawposition(a);
				fb_draw_image(temp.x, temp.y, img, 0);
				fb_update();
				fb_free_image(img);
				break;
			}
			case  -5:
			{
				img = fb_read_png_image("./img/-5.png");
				a.x = j; a.y = i;
				temp = getdrawposition(a);
				fb_draw_image(temp.x, temp.y, img, 0);
				fb_update();
				fb_free_image(img);
				break;
			}
			case  -6:
			{
				img = fb_read_png_image("./img/-6.png");
				a.x = j; a.y = i;
				temp = getdrawposition(a);
				fb_draw_image(temp.x, temp.y, img, 0);
				fb_update();
				fb_free_image(img);
				break;
			}
			case  -7:
			{
				img = fb_read_png_image("./img/-7.png");
				a.x = j; a.y = i;
				temp = getdrawposition(a);
				fb_draw_image(temp.x, temp.y, img, 0);
				fb_update();
				fb_free_image(img);
				break;
			}
			default:
				break;
			}
		}
	}
	if (select_val) show_select(board_pos);
}


void show_end() {
	int x, y;
	fb_image* img;
	img = fb_read_png_image("./img/mask.png");
	printf("**************************DEBUG************************************: end_images\n");
	fb_draw_image(0, 0, img, 0);
	printf("**************************DEBUG************************************: end_images22222\n");
	fb_update();
	printf("**************************DEBUG************************************: end_images33333\n");
	fb_free_image(img);
	if (win == 1) 	img = fb_read_png_image("./img/win.png");
	else img = fb_read_png_image("./img/loss.png");
	x = ((1024 - 372) / 2 - 264) / 2;
	fb_draw_image(x, (600-226)/2, img, 0);
	fb_free_image(img);
	if (win == 1) 	img = fb_read_png_image("./img/loss.png");
	else img = fb_read_png_image("./img/win.png");
	fb_draw_image(1024 - 264 - x, (600 - 226) / 2, img, 0);
	fb_free_image(img);
	img = fb_read_png_image("./img/again.png");
	fb_draw_image(1024/2-372/2, 600/3 - 50, img, 0);
	fb_free_image(img);
	img = fb_read_png_image("./img/back_home.png");
	fb_draw_image(1024 / 2 - 372 / 2, 600*2/3 - 50, img, 0);
	fb_free_image(img);
	fb_update();
	ini_board();
}


void show_Welcome_Page() {
	fb_image* img;
	img = fb_read_jpeg_image("./img/welcome.jpg");
	printf("**************************DEBUG************************************: welcome_images\n");
	fb_draw_image(0, 0, img, 0);
	printf("**************************DEBUG************************************: welcome_images22222\n");
	fb_update();
	printf("**************************DEBUG************************************: welcome_images33333\n");
	fb_free_image(img);

}

//ʤ��
void check_win() {
	printf("**************************DEBUG************************************: comeinto check_win\n");
	for (int i = 1; i < 11; i++) // ���������ϵĺ�˧�ͺڽ��Ƿ���
	{
		for (int j = 1; j < 10; j++)
		{
			if (board[j][i] == 1)
				win = win + 1;
			if (board[j][i] == -1)
				win = win - 1;
		}
	}
	if (win == 1)
	{
		fb_image* red_win = fb_read_font_image("�췽ʤ����", 20, NULL);
		fb_draw_image(400, 350, red_win, RED);
		fb_update();
		fb_free_image(red_win);
		state = 3;
		show_end();
		should_exit = 1;
	}
	if (win == -1)
	{
		fb_image* black_win = fb_read_font_image("�ڷ�ʤ����", 20, NULL);
		fb_draw_image(400, 350, black_win, RED);
		fb_update();
		fb_free_image(black_win);
		state = 3;
		show_end();
		should_exit = 1;
	}
	printf("**************************DEBUG************************************: finish check_win, cout << {%d}\n", win);
}

// �жϺϷ���
int check_identity() {
	printf("**************************DEBUG************************************: comeinto check_identity\n");
	board_pos = getlogicalposition(global_select);
	// pre_board_pos = getlogicalposition(pre_global_select);
	int i_from = board_pos.x;
	int j_from = board_pos.y;
	printf("|||||||||||||||||************** DEBUG **************|||||||||||||||||: {j_from: %d, i_from: %d}\n", j_from, i_from);
	// �����Ϸ���
	if (j_from < 1 || j_from > 9 || i_from < 1 || i_from > 10) return !OK;
	int res = turn * board[j_from][i_from];
	printf("**************************DEBUG************************************: {res: %d}\n", res);
	if (from_status == 0 && res < 0) return !OK;
	printf("**************************DEBUG************************************: finish touch legal check {from_status: %d}\n", from_status);
	// ����Ϸ���
	if (from_status == 0) {
		if (board[j_from][i_from] == 0) return !OK;
		printf("**************************DEBUG************************************: {get i_from: %d, j_form: %d}\n", i_from, j_from);
		pre_board_pos.x = i_from;
		pre_board_pos.y = j_from;
		from_status = 1;
		select_val = 1;
		// show_select(board_pos);
		return OK;
	}
	else if (j_from == pre_board_pos.y && i_from == pre_board_pos.x) { //ȡ��ѡ��
		printf("**************************DEBUG************************************: comeinto ȡ��ѡ��\n");
		pre_board_pos.x = 0;
		pre_board_pos.y = 0;
		select_val = 0;
		from_status = 0;
		return OK;
	}
	else if (board[j_from][i_from] * turn > 0) return !OK; // ���ܳ��Լ�������
	else
	{
		printf("**************************DEBUG************************************: comeinto ����Ƿ�����������\n");
		int i_to = i_from, j_to = j_from;
		i_from = pre_board_pos.x;
		j_from = pre_board_pos.y;
		switch (abs(board[j_from][i_from])) {
		case  1://˧������------------------------------------------------------
		{
			printf("**************************DEBUG************************************: comeinto ˧������\n");
			if (j_to == j_from && board[j_to][i_to] == -1 * turn)
			{
				//printf("\nkiller 1\n");
				for (int flag = i_from + turn; turn * flag < turn * (i_to - turn); flag = flag + turn)
				{
					//printf("\n%d\n%d\n", j_from, flag);
					if (board[j_from][flag] != 0)
					{
						//printf("\nYou Dead!!!!!!!!!!!!\n");
						return !OK;
					}
				}
				break;
			}
			else
			{
				//printf("\nkiller 2\n");
				if ((i_to != i_from && j_to != j_from) || abs(j_to - j_from) > 1 || abs(i_to - i_from) > 1)
					return !OK;
				if (j_to < 4 || j_to > 6 || turn * i_to > turn * (int)(5.5 - turn * 2.5))
					return !OK;
				break;
			}
		}
		case  2://�˵�����------------------------------------------------------
		{
			printf("**************************DEBUG************************************: comeinto �˵�����\n");
			if (!(((j_to == 4 || j_to == 6) && (i_to == (int)(5.5 - turn * 4.5) || i_to == (int)(5.5 - turn * 2.5))) || (j_to == 5 && i_to == (int)(5.5 - turn * 3.5))))
				return !OK;
			if (abs(j_to - j_from) > 1 || abs(i_to - i_from) > 1)
				return !OK;
			break;
		}
		case  3://�������-------------------------------------------------------
		{
			printf("**************************DEBUG************************************: comeinto �������\n");
			if (turn * i_to > turn * (int)(5.5 - turn * 0.5))
				return !OK;
			if (abs(j_to - j_from) != 2 || abs(i_to - i_from) != 2)
				return !OK;
			break;
		}
		case  4://��������-------------------------------------------------------
		{
			printf("**************************DEBUG************************************: comeinto ��������\n");
			if (abs(j_to - j_from) == 2 && abs(i_to - i_from) == 1)
			{
				if (board[(j_to + j_from) / 2][i_from] != 0)
					return !OK;
			}
			if (abs(j_to - j_from) == 1 && abs(i_to - i_from) == 2)
			{
				if (board[j_from][(i_to + i_from) / 2] != 0)
					return !OK;
			}
			if (!((abs(j_to - j_from) == 2 && abs(i_to - i_from) == 1) || (abs(j_to - j_from) == 1 && abs(i_to - i_from) == 2)))
				return !OK;
			break;
		}
		case  5://��������--------------------------------------------------------
		{
			printf("**************************DEBUG************************************: comeinto ��������\n");
			if (j_to != j_from && i_to != i_from)
				return !OK;
			distance = 0;
			if (j_to == j_from)
			{
				distance = (i_to - i_from) / abs(i_to - i_from);
				for (int flag = i_from; distance * flag < distance * (i_to - distance); flag = flag + distance)
				{
					if (board[j_from][flag] != 0 && flag != i_from)
						return !OK;
				}
			}
			if (i_to == i_from)
			{
				distance = (j_to - j_from) / abs(j_to - j_from);
				for (int flag = j_from; distance * flag < distance * (j_to - distance); flag = flag + distance)
				{
					if (board[flag][i_from] != 0 && flag != j_from)
						return !OK;
				}
			}
			printf("**************************DEBUG************************************: ��������Ϸ�\n");
			break;
		}
		case  6://�ڵ�����-------------------------------------------------------
		{
			printf("**************************DEBUG************************************: comeinto �ڵ�����\n");
			if (j_to != j_from && i_to != i_from)
				return !OK;
			int distance = 0;
			int chesses = 0;
			if (j_to == j_from)
			{
				distance = (i_to - i_from) / abs(i_to - i_from);
				for (int flag = i_from; distance * flag <= distance * (i_to - distance); flag = flag + distance)
				{
					if (board[j_from][flag] != 0 && flag != i_from)
						chesses = chesses + 1;
				}
			}
			if (i_to == i_from)
			{
				distance = (j_to - j_from) / abs(j_to - j_from);
				for (int flag = j_from; distance * flag <= distance * (j_to - distance); flag = flag + distance)
				{
					if (board[flag][i_from] != 0 && flag != j_from)
						chesses = chesses + 1;
				}
			}
			//printf("%d\n",chesses);
			if (chesses > 1)
				return !OK;
			if (chesses == 1 && board[j_to][i_to] == 0)
				return !OK;
			if (chesses < 1 && turn * board[j_to][i_to] < 0)
				return !OK;
			break;
		}
		case  7://��������-----------------------------------------------------------
		{
			printf("**************************DEBUG************************************: comeinto ��������\n");
			if (turn * i_from <= turn * (int)(5.5 - 0.5 * turn))
			{
				printf("���ո�ȥ,û����\n");
				if (i_to - i_from != turn || j_to != j_from)
					return !OK;
			}
			if (turn * i_from >= turn * (int)(5.5 + 0.5 * turn))
			{
				printf("���ո�ȥ\n");
				if ((i_to != i_from && j_to != j_from) || abs(j_to - j_from) > 1 || abs(i_to - i_from) > 1 || turn * (i_to - i_from) < 0)
				{
					//printf("\n%d\n%d\n%d\n%d\n", j_to, i_to, j_from, i_from);
					return !OK;
				}
			}
			break;
		}
		}

		//������״�Թ�����
		new_bak = malloc(sizeof(struct bak));
		(*new_bak).next = latest_bak;
		memcpy((*new_bak).board_bak, board, sizeof(board));
		(*new_bak).turn_bak = turn;
		latest_bak = new_bak;
		new_bak = NULL;
		for (int i = 1; i < 11; i++) {
			for (int j = 1; j < 10; j++) {
				printf("%d ", board[j][i]);
			}
			printf("\n");
		}
		//�ƶ�����
		printf("**************************DEBUG************************************: comeinto �ƶ�����\n");
		printf("**************************DEBUG************************************: i_from=%d,j_from=%d,i_to=%d,j_to=%d\n", i_from, j_from, i_to, j_to);
		printf("**************************DEBUG************************************: board[j_from][i_from],board[j_to][i_to]\n");
		for (int i = 1; i < 11; i++) {
			for (int j = 1; j < 10; j++) {
				printf("%d ", board[j][i]);
			}
			printf("\n");
		}
		board[j_to][i_to] = board[j_from][i_from];
		board[j_from][i_from] = 0;
		from_status = 0;
		select_val = 0;
		//�غ�ˢ��
		turn = -1 * turn;
	}
	return OK;
}

void regert() {
	printf("**************************DEBUG************************************: comeinto regert\n");
	if (latest_bak == NULL) return;
	new_bak = latest_bak;
	latest_bak = (*latest_bak).next;
	memcpy(board, (*new_bak).board_bak, sizeof(board));
	turn = (*new_bak).turn_bak;
	free(new_bak);
	new_bak = NULL;
	regret_trigger = 0;
	show_board();
	printf("**************************DEBUG************************************: I will never regert any more hhhhhhhh  : )\n");
}

// ά�����
void maintain_roll() {
	printf("**************************DEBUG in maiintain************************************: board[j_from][i_from],board[j_to][i_to]\n");
	for (int i = 1; i < 11; i++) {
		for (int j = 1; j < 10; j++) {
			printf("%d ", board[j][i]);
		}
		printf("\n");
	}
	if (regret_trigger) {
		regert();
		regret_trigger = 0;
	}
	else {
		int res = check_identity();
		printf("-----------------------------DEBUG: res=%d\n", res);
		if (res == OK) show_board();
		check_win();
	}
	return;
}

// ���������������ݸ�ʽ��x y�����ö��ŷָ� e.g. 1037,888, ����ģ�������ܵ�0��0��˵������Ҫ����
point parser_bluetooth(char* buf) {
	point a;
	int t = 0;
	char* str = buf;
	while (*str != ',') {
		t = t * 10 + *str - '0';
		str++;
	}
	a.x = t;
	t = 0;
	str++;
	while (*str != '\0') {
		t = t * 10 + *str - '0';
		str++;
	}
	a.y = t;
	return a;
}

// ��װ�������ݰ�
char* pack_bluetooth(point a) {
	char* buf = (char*)malloc(20);
	sprintf(buf, "%d,%d", a.x, a.y); // ͨ��sprintfд��buf
	return buf;
}

static void bluetooth_tty_event_cb(int fd)
{
	if (state == 1) {
		char buf[128];
		int n;
		printf("**************************DEBUG************************************: comeinto RECEIVED LY MSS\n");
		n = myRead_nonblock(fd, buf, sizeof(buf) - 1);
		if (n <= 0) {
			printf("close bluetooth tty fd\n");
			task_delete_file(fd);
			close(fd);
			exit(0);
			return;
		}

		buf[n] = '\0';
		printf("bluetooth tty receive \"%s\"\n", buf);
		global_select = parser_bluetooth(buf);
		if (win == 0) maintain_roll();
	}
	else if (state == 2) {
		char buf[128];
		int n;
		printf("**************************DEBUG************************************: comeinto RECEIVED LY MSS\n");
		n = myRead_nonblock(fd, buf, sizeof(buf) - 1);
		buf[n] = '\0';
		printf("bluetooth tty receive \"%s\"\n", buf);
		my_ini = parser_bluetooth(buf);
		rec_ini_first = my_ini.x;
		if (((ini_first ^ rec_ini_first) & 1) != 1) {
			srand((unsigned int)ini_first);
			int random_number = rand() % 2 + 1; // rand() % 2 ����0��1��Ȼ���1�õ�1��2
			ini_first = random_number;
			my_ini.x = my_ini.y = ini_first;
			char* pkg = pack_bluetooth(my_ini);
			printf(pkg);
			myWrite_nonblock(bluetooth_fd, pkg, sizeof pkg);
			printf("%d,%d", ini_first, rec_ini_first);
		}
		else {
			should_exit = 1;
			side = ((ini_first & 1) == 1) ? -1 : 1;
			state = 1;
		}
	}
	return;
}

static int bluetooth_tty_init(const char* dev)
{
	int fd = open(dev, O_RDWR | O_NOCTTY | O_NONBLOCK); /*������ģʽ*/
	if (fd < 0) {
		printf("bluetooth_tty_init open %s error(%d): %s\n", dev, errno, strerror(errno));
		return -1;
	}
	return fd;
}

// ����⵽�����¼�ʱ�����ô˺���
static void touch_event_cb(int fd) {
	printf("**************************DEBUG************************************: comeinto touch_event_cb\n");

	int type, x, y, finger;
	type = touch_read(fd, &x, &y, &finger);
	// x = x * 800 / 1024;
	// y = y * SCREEN_HEIGHT / 600;
	if (state ==1) {
		printf("state1TOUCH_PRESS\n");
		switch (type)
		{
		case TOUCH_PRESS:
			if ((side == turn)||mode==0) {
				printf("TOUCH_PRESS:x=%d,y=%d,finger=%d\n", x, y, finger);
				pre_global_select = global_select;
				global_select.x = x;
				global_select.y = y;
				int delta_x = global_select.x - pre_global_select.x;
				int delta_y = global_select.y - pre_global_select.y;
				printf("******************************DEBUG: ****************************************delta_x=%d,delta_y=%d\n", delta_x, delta_y);
				if (delta_x * delta_x + delta_y * delta_y < LEAGL_INTERVAL) return;
				// ���崥��
				if ((x >= REGRET_X) && (x < REGRET_X + REGRET_W) && (y >= REGRET_Y) && (y < REGRET_Y + REGRET_H)) {
					regret_trigger = 1;
					printf("*************************************DEBUG: **************************** I am very regret 555555555555555 : (\n");
					point a;
					a.x = 0, a.y = 0;
					char* regert_pkg = pack_bluetooth(a);
					myWrite_nonblock(bluetooth_fd, regert_pkg, sizeof regert_pkg);
				}
				// �������ģʽ����������
				if (mode) {
					char* pkg = pack_bluetooth(global_select);
					printf(pkg);
					myWrite_nonblock(bluetooth_fd, pkg, sizeof pkg);
				}
				// �����̽��в���
				if (win == 0) maintain_roll();
			}
			break;
		case TOUCH_MOVE:
			printf("TOUCH_MOVE��x=%d,y=%d,finger=%d\n", x, y, finger);
			break;
		case TOUCH_RELEASE:
			printf("TOUCH_RELEASE:x=%d,y=%d,finger=%d\n", x, y, finger);
			break;
		case TOUCH_ERROR:
			printf("close touch fd\n");
			close(fd);
			task_delete_file(fd);
			break;
		default:
			return;
		}
	}
	else if (state == 0) {
		printf("state0TOUCH_PRESS\n");
		switch (type)
		{
		case TOUCH_PRESS:
		printf("TOUCH_PRESS:x=%d,y=%d,finger=%d\n", x, y, finger);
		global_select.x = x;
		global_select.y = y;
		if ((x >= Bottom_x) && (x < Bottom_x + Welocme_Bottom_W) && (y >= Bottom_1_y) && (y < Bottom_1_y + Welocme_Bottom_H)) {
			state = 2;
			point a;
			a.x = x, a.y = y;
			ini_first = x;
			char* regert_pkg = pack_bluetooth(a);
			myWrite_nonblock(bluetooth_fd, regert_pkg, sizeof regert_pkg);
			mode = 1;
		}
		else if ((x >= Bottom_x) && (x < Bottom_x + Welocme_Bottom_W) && (y >= Bottom_2_y) && (y < Bottom_2_y + Welocme_Bottom_H)) {
			state = 1;
			should_exit = 1;
			mode = 0;
		}
		break;
		default:
			return;
		}
	}
	if (state == 3) {
		printf("state3TOUCH_PRESS\n");
		switch (type)
		{
		case TOUCH_PRESS:
			printf("TOUCH_PRESS:x=%d,y=%d,finger=%d\n", x, y, finger);
			global_select.x = x;
			global_select.y = y;
			if ((x >= (1024 / 2 - 372 / 2)) && (x < (1024 / 2 - 372 / 2) + 372) && (y >= 600 / 3 - 50) && (y < 600 / 3 - 50 + 99)) {
				state = 1;
				turn = 1;
				should_exit = 1;
			}
			else if ((x >= (1024 / 2 - 372 / 2)) && (x < (1024 / 2 - 372 / 2) + 372) && (y >= 600 * 2 / 3 - 50) && (y < 600 * 2 / 3 - 50 + 99)) {
				state = 0;
				turn = 1;
				should_exit = 1;
			}
			break;
		case TOUCH_MOVE:
			printf("TOUCH_MOVE��x=%d,y=%d,finger=%d\n", x, y, finger);
			break;
		case TOUCH_RELEASE:
			printf("TOUCH_RELEASE:x=%d,y=%d,finger=%d\n", x, y, finger);
			break;
		case TOUCH_ERROR:
			printf("close touch fd\n");
			close(fd);
			task_delete_file(fd);
			break;
		default:
			return;
		}
	}
}

int main(int argc, char* argv[]) {
	state = 0;
	should_exit = 0;
	fb_init("/dev/fb0");
	// �򿪶�㴥���豸�ļ�, �����ļ�fd
	touch_fd = touch_init("/dev/input/event2");
	//��������, ��touch_fd�ļ��ɶ�ʱ, ���Զ�����touch_event_cb����
	task_add_file(touch_fd, touch_event_cb);
state_0:
	show_Welcome_Page();
	/*
	bluetooth_fd = bluetooth_tty_init("/dev/rfcomm0");
	printf("*************************************DEBUG: **************************** bluetooth_tty_init \n");
	if (bluetooth_fd == -1) return 0;
	task_add_file(bluetooth_fd, bluetooth_tty_event_cb);*/

	printf("�����뵱ǰ��������Ӫ, 1Ϊ�췽, -1Ϊ�ڷ�\n");
	printf("�뱣֤��̨��������Ӫ��ͬ\n");
	should_exit = 0;
	task_loop(); // ��������ѭ��
state_1:

	global_select.x = 0; global_select.y = 0;
	pre_global_select.x = 0; pre_global_select.y = 0;
	show_board();
	show_select(global_select);
	should_exit = 0;
	task_loop(); // ��������ѭ��
	should_exit = 0;
	task_loop(); // ��������ѭ��
	if (state == 0) goto state_0;
	else goto state_1;
	return 0;
}