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
#define OK 1
#define LEAGL_INTERVAL 2000
#define REGRET_X	(SCREEN_WIDTH-60)
#define REGRET_Y	0
#define REGRET_W	60
#define REGRET_H	60

// 存储棋盘状态和当前回合,board_bak存储棋盘状态，turn_bak存储当前回合（红方或黑方），next指向下一个备份状态。
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

static int touch_fd; // 触摸屏文件描述符
static int bluetooth_fd; // 蓝牙文件描述符

struct bak* latest_bak = NULL; // 指向最新的备份状态。
struct bak* new_bak = NULL; // 

point global_select={0,0};
point pre_global_select;
point board_pos; // 全局变量，存储当前选中的棋子位置
point pre_board_pos; // 全局变量，存储上一次选中的棋子位置
int win = 0; // 1 0 -1 分别代表红方胜，没分出来胜负，黑方胜
int from_status; // 是获取起始位置还是终点位置
int mode = 0; // 输入当前模式，1为双机模式，0为单机模式
int turn = 1;//记录是红方回合还是黑方回合
int select_val = 0; // 选中状态
int distance = 0; 
int regret_trigger; // 悔棋触发

point getdrawposition(point a) {
	point po;
	po.x = 215 + 66 * (a.x - 1) - 33;
	po.y = 32.5 + 66.5 * (a.y - 1) - 33;
	return po;
}
point getlogicalposition(point po) {
    point a;
    a.x = (int) (po.x - 182) / 66 + 1;
    a.y = (int) (po.y + 0.5) / 66.5 + 1;
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
	if(turn == 1) fb_draw_text(50,50,"红方出棋",64,RED);
	else if(turn == -1) fb_draw_text(50,50,"黑方出棋",64,BLACK);
}

void show_board() {

	  printf("**************************DEBUG************************************: board[j_from][i_from],board[j_to][i_to]\n");
	  for(int i = 1; i < 11; i++) {
		  for(int j = 1; j < 10; j++) {
			  printf("%d ", board[j][i]);
		  }
		  printf("\n");
	  }	
	printf("**************************DEBUG************************************: comeinto show_board\n");
	fb_image* img;
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
	if(select_val) show_select(board_pos);
}

//胜负
void check_win() {
	printf("**************************DEBUG************************************: comeinto check_win\n");
	for(int i = 1; i < 11; i++) // 看看棋盘上的红帅和黑将是否还在
	{
		for(int j = 1; j < 10; j++)
		{
			if(board[j][i] == 1)
			win = win + 1;
			if(board[j][i] == -1)
			win = win - 1;
		}
	} 
	if(win == 1)
	{
		fb_image * red_win = fb_read_font_image("红方胜利！", 20, NULL);
		fb_draw_image(400,350,red_win,RED);
		fb_update();
		fb_free_image(red_win);
	}
	if(win == -1)
	{
		fb_image * black_win = fb_read_font_image("黑方胜利！", 20, NULL);	
		fb_draw_image(400,350,black_win,RED);
		fb_update();
		fb_free_image(black_win);
	}
	printf("**************************DEBUG************************************: finish check_win, cout << {%d}\n", win);
}

// 判断合法性
int check_identity() {
	printf("**************************DEBUG************************************: comeinto check_identity\n");
	board_pos = getlogicalposition(global_select);
	// pre_board_pos = getlogicalposition(pre_global_select);
	int i_from = board_pos.x;
	int j_from = board_pos.y;
	printf("|||||||||||||||||************** DEBUG **************|||||||||||||||||: {j_from: %d, i_from: %d}\n", j_from, i_from);
	// 触摸合法性
	if(j_from < 1 || j_from > 9 || i_from < 1 || i_from > 10) return !OK;
	int res = turn * board[j_from][i_from];
	printf("**************************DEBUG************************************: {res: %d}\n", res);
	if(from_status == 0 && res < 0) return !OK;
	printf("**************************DEBUG************************************: finish touch legal check {from_status: %d}\n", from_status);
	// 走棋合法性
	if (from_status == 0) {
		if(board[j_from][i_from] == 0) return !OK;
		printf("**************************DEBUG************************************: {get i_from: %d, j_form: %d}\n", i_from, j_from);
		pre_board_pos.x = i_from;
		pre_board_pos.y = j_from;
		from_status = 1;
		select_val = 1;
		// show_select(board_pos);
		return OK;
	} 
	else if (j_from == pre_board_pos.y && i_from == pre_board_pos.x) { //取消选中
		printf("**************************DEBUG************************************: comeinto 取消选中\n");
		pre_board_pos.x = 0;
		pre_board_pos.y = 0;
		select_val = 0;
		from_status = 0;
		return OK;
	}
	else if(board[j_from][i_from] * turn > 0) return !OK; // 不能吃自己的棋子
	else 
	{
		printf("**************************DEBUG************************************: comeinto 检查是否符合象棋规则\n");
		int i_to = i_from, j_to = j_from;
		i_from = pre_board_pos.x;
		j_from = pre_board_pos.y;
		switch (abs(board[j_from][i_from])) {
		case  1://帅的走棋------------------------------------------------------
          {
			printf("**************************DEBUG************************************: comeinto 帅的走棋\n");
            if( j_to == j_from && board[j_to][i_to] == -1*turn )
              {
                //printf("\nkiller 1\n");
                for(int flag = i_from + turn ; turn*flag < turn*(i_to - turn) ; flag = flag + turn)
                  {
                    //printf("\n%d\n%d\n", j_from, flag);
                    if(board[j_from][flag] != 0 )
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
                if( (i_to != i_from && j_to != j_from) || abs(j_to-j_from) > 1 || abs(i_to-i_from) > 1 )
                  return !OK;
                if( j_to < 4 || j_to > 6 || turn*i_to > turn*(int)(5.5-turn*2.5) )
                  return !OK;
                break;
              }
          }
        case  2://仕的走棋------------------------------------------------------
          {
			printf("**************************DEBUG************************************: comeinto 仕的走棋\n");
            if( !(((j_to == 4 || j_to == 6) && (i_to == (int)(5.5-turn*4.5) || i_to == (int)(5.5-turn*2.5))) || ( j_to == 5 && i_to == (int)(5.5-turn*3.5))))
              return !OK;
            if( abs(j_to-j_from) > 1 || abs(i_to-i_from) > 1)
              return !OK;
            break;
          }
        case  3://相的走棋-------------------------------------------------------
          {
			printf("**************************DEBUG************************************: comeinto 相的走棋\n");
            if( turn*i_to > turn*(int)(5.5-turn*0.5) )
              return !OK;
            if( abs(j_to-j_from) != 2 || abs(i_to-i_from) != 2 )
              return !OK;
            break;
          }
        case  4://马的走棋-------------------------------------------------------
          {
			printf("**************************DEBUG************************************: comeinto 马的走棋\n");
            if( abs(j_to-j_from) == 2 && abs(i_to-i_from) == 1)
              {
                if(board[(j_to+j_from)/2][i_from] != 0)
                  return !OK;
              }
            if( abs(j_to-j_from) == 1 && abs(i_to-i_from) == 2)
              {
                if(board[j_from][(i_to+i_from)/2] != 0)
                  return !OK;
              }
            if(!((abs(j_to-j_from) == 2 && abs(i_to-i_from) == 1) || (abs(j_to-j_from) == 1 && abs(i_to-i_from) == 2 )))
              return !OK;
            break;
          }
        case  5://车的走棋--------------------------------------------------------
          {
			printf("**************************DEBUG************************************: comeinto 车的走棋\n");
            if( j_to != j_from && i_to != i_from )
              return !OK;
            distance = 0;
            if( j_to == j_from )
              {
                distance = (i_to-i_from)/abs(i_to-i_from);
                for(int flag = i_from ; distance*flag < distance*(i_to - distance) ; flag = flag + distance)
                  {
                    if(board[j_from][flag] != 0 && flag != i_from)
                      return !OK;
                  }
              }
            if( i_to == i_from )
              {
                distance = (j_to-j_from)/abs(j_to-j_from);
                for(int flag = j_from ; distance*flag < distance*(j_to - distance) ; flag = flag + distance)
                  {
                    if(board[flag][i_from] != 0 && flag != j_from)
                      return !OK;
                  }
              }
			printf("**************************DEBUG************************************: 车的走棋合法\n");
            break;
          }
        case  6://炮的走棋-------------------------------------------------------
          {
			printf("**************************DEBUG************************************: comeinto 炮的走棋\n");
            if( j_to != j_from && i_to != i_from )
              return !OK;
            int distance = 0;
            int chesses = 0;
            if( j_to == j_from )
              {
                distance = (i_to-i_from)/abs(i_to-i_from);
                for(int flag = i_from ; distance*flag <= distance*(i_to - distance) ; flag = flag + distance)
                  {
                    if(board[j_from][flag] != 0 && flag != i_from)
                      chesses = chesses + 1;
                  }
              }
            if( i_to == i_from )
              {
                distance = (j_to-j_from)/abs(j_to-j_from);
                for(int flag = j_from ; distance*flag <= distance*(j_to - distance) ; flag = flag + distance)
                  {
                    if(board[flag][i_from] != 0 && flag != j_from)
                      chesses = chesses + 1;
                  }
              }
            //printf("%d\n",chesses);
            if( chesses > 1 )
              return !OK;
            if( chesses == 1 && board[j_to][i_to] == 0 )
              return !OK;
            if( chesses < 1 && turn*board[j_to][i_to] < 0 )
              return !OK;
            break;
          }
        case  7://兵的走棋-----------------------------------------------------------
          {
			printf("**************************DEBUG************************************: comeinto 兵的走棋\n");
            if( turn*i_from <= turn*(int)(5.5-0.5*turn))
              {                
				printf("我勒个去,没过河\n");
				if( i_to - i_from != turn || j_to != j_from )
                  return !OK;
              }
            if( turn*i_from >= turn*(int)(5.5+0.5*turn))
              {
				printf("我勒个去\n");
                if( (i_to != i_from && j_to != j_from) || abs(j_to-j_from) > 1 || abs(i_to-i_from) > 1 || turn*(i_to-i_from) < 0 )
                  {
                    //printf("\n%d\n%d\n%d\n%d\n", j_to, i_to, j_from, i_from);
                    return !OK;
                  }
              }
            break;
          }
		}

		//备份现状以供悔棋
		new_bak = malloc( sizeof( struct bak ) );
		(*new_bak).next = latest_bak;
		memcpy( (*new_bak).board_bak, board, sizeof(board) );
		(*new_bak).turn_bak = turn;
		latest_bak = new_bak;
		new_bak = NULL;
		for(int i = 1; i < 11; i++) {
			for(int j = 1; j < 10; j++) {
				printf("%d ", board[j][i]);
			}
			printf("\n");
		}	
		//移动棋子
		printf("**************************DEBUG************************************: comeinto 移动棋子\n");
		printf("**************************DEBUG************************************: i_from=%d,j_from=%d,i_to=%d,j_to=%d\n", i_from, j_from, i_to, j_to);
		printf("**************************DEBUG************************************: board[j_from][i_from],board[j_to][i_to]\n");
		for(int i = 1; i < 11; i++) {
			for(int j = 1; j < 10; j++) {
				printf("%d ", board[j][i]);
			}
			printf("\n");
		}	
		board[j_to][i_to] = board[j_from][i_from];
		board[j_from][i_from] = 0;
		from_status = 0;
		select_val = 0;
		//回合刷新
		turn = -1*turn;
	}
	return OK;
}

void regert() {
	printf("**************************DEBUG************************************: comeinto regert\n");
	if(latest_bak == NULL) return;
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

// 维护棋局
void maintain_roll() {
	printf("**************************DEBUG in maiintain************************************: board[j_from][i_from],board[j_to][i_to]\n");
	  for(int i = 1; i < 11; i++) {
		  for(int j = 1; j < 10; j++) {
			  printf("%d ", board[j][i]);
		  }
		  printf("\n");
	  }	
	if(regret_trigger) {
		regert();
		regret_trigger = 0;
	}
	else {
		int res = check_identity();
		printf("-----------------------------DEBUG: res=%d\n", res);
		if(res == OK) show_board();
		check_win();
	}
	return ;
}
// 定义蓝牙传输数据格式，x y坐标用逗号分隔 e.g. 1037,888, 特殊的，如果接受到0，0，说明对面要悔棋
point parser_bluetooth(char* buf) {
	for(int i = 0; i < sizeof buf; i++) { // 检查数据合法性，但没有校验位，应该不会出现问题
		if(buf[i] > '9' || buf[i] < '0' || buf[i] != ',') return (point){-1, -1};
	}
	point a;
	int t = 0;
	while(*buf != ',') t = t*10 + *buf - '0';
	a.x = t;
	t = 0;
	buf++;
	while(*buf != '\0') t = t*10 + *buf - '0';
	a.y = t;
	return a;
}

// 封装蓝牙数据包
char* pack_bluetooth(point a) {
	char* buf = (char *) malloc(20);
	sprintf(buf, "%d,%d", a.x, a.y); // 通过sprintf写入buf
	return buf;
}

static void bluetooth_tty_event_cb(int fd)
{
	char buf[128];
	int n;

	n = myRead_nonblock(fd, buf, sizeof(buf)-1);
	if(n <= 0) {
		printf("close bluetooth tty fd\n");
		//task_delete_file(fd);
		//close(fd);
		exit(0);
		return;
	}

	buf[n] = '\0';
	printf("bluetooth tty receive \"%s\"\n", buf);
	global_select = parser_bluetooth(buf);
	if(win == 0) maintain_roll();
	return;
}

static int bluetooth_tty_init(const char *dev)
{
	int fd = open(dev, O_RDWR|O_NOCTTY|O_NONBLOCK); /*非阻塞模式*/
	if(fd < 0){
		printf("bluetooth_tty_init open %s error(%d): %s\n", dev, errno, strerror(errno));
		return -1;
	}
	return fd;
}

// 当监测到触摸事件时，调用此函数
static void touch_event_cb(int fd) {
	printf("**************************DEBUG************************************: comeinto touch_event_cb\n");
	
  int type, x, y, finger;
	type = touch_read(fd, &x, &y, &finger);
	// x = x * 800 / 1024;
	// y = y * SCREEN_HEIGHT / 600;
  switch (type)
	{
	case TOUCH_PRESS:
		printf("TOUCH_PRESS:x=%d,y=%d,finger=%d\n", x, y, finger);
		pre_global_select = global_select;
		global_select.x = x;
		global_select.y = y;
		int delta_x = global_select.x - pre_global_select.x;
		int delta_y = global_select.y - pre_global_select.y;
		printf("******************************DEBUG: ****************************************delta_x=%d,delta_y=%d\n", delta_x, delta_y);
		if(delta_x * delta_x + delta_y * delta_y < LEAGL_INTERVAL) return;
		// 悔棋触发
		if((x>=REGRET_X)&&(x<REGRET_X+REGRET_W)&&(y>=REGRET_Y)&&(y<REGRET_Y+REGRET_H)) {
			regret_trigger = 1;
			printf("*************************************DEBUG: **************************** I am very regret 555555555555555 : (\n");
			point a;
			a.x = 0, a.y = 0;
			char *regert_pkg = pack_bluetooth(a);
			myWrite_nonblock(bluetooth_fd, regert_pkg, sizeof regert_pkg);
		}
		// 如果联机模式，发送数据
		if(mode) {
			char *pkg = pack_bluetooth(global_select);
			myWrite_nonblock(bluetooth_fd, pkg, sizeof pkg);
		}
		// 对棋盘进行操作
		if(win == 0) maintain_roll();
		break;
	case TOUCH_MOVE:
		printf("TOUCH_MOVE：x=%d,y=%d,finger=%d\n", x, y, finger);
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

int main(int argc, char* argv[]) {
	printf("请选择模式, 1为双机模式, 0为单机模式\n");
	scanf("%d", &mode);
	if(mode) {
		// 双机模式下，输入该主机的阵营，1为红方，-1为黑方，默认红方先出棋
		printf("请输入当前主机的阵营, 1为红方, -1为黑方\n");
		printf("请保证两台主机的阵营不同\n");
		scanf("%d", &turn);
	}
	global_select.x = 0; global_select.y = 0;
	pre_global_select.x = 0; pre_global_select.y = 0;
	fb_init("/dev/fb0");
	// 打开多点触摸设备文件, 返回文件fd
	touch_fd = touch_init("/dev/input/event2");
	fb_draw_rect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, WHITE);
	fb_draw_rect(REGRET_X, REGRET_Y, REGRET_W, REGRET_H, RED);
	fb_update();
	fb_image* img;
	img = fb_read_jpeg_image("./test.jpg");
	fb_draw_image((SCREEN_WIDTH - img->pixel_w) / 2, 0, img, 0);
	fb_update();
	fb_free_image(img);
	show_board();
	show_select(global_select);
	// 添加任务, 当touch_fd文件可读时, 会自动调用touch_event_cb函数
	task_add_file(touch_fd, touch_event_cb);
	if(mode) {
		bluetooth_fd = bluetooth_tty_init("/dev/rfcomm0");
		if(bluetooth_fd == -1) return 0;
		task_add_file(bluetooth_fd, bluetooth_tty_event_cb);
	}

	task_loop(); // 进入任务循环
	return 0;
}
