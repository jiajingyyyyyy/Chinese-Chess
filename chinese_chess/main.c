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

point select;

point getdrawposition(point a) {
	point po;
	po.x = 215 + 66 * (a.x - 1) - 33;
	po.y = 32.5 + 66.5 * (a.y - 1) - 33;
	return po;
}
point getposition(point a) {
	point po;
	po.x = 215 + 66 * (a.x - 1) - 33;
	po.y = 32.5 + 66.5 * (a.y - 1) - 33;
	return po;
}

void show_select(point a) {
	fb_image* img;
	point temp;
	img = fb_read_png_image("./img/select.png");
	temp = getdrawposition(a);
	fb_draw_image(temp.x + 8, temp.y + 8, img, 0);
	fb_update();
	fb_free_image(img);
}

void show_board() {
	fb_image* img;
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
}

int main(int argc, char* argv[]) {
	select.x = 0; select.y = 0;
	fb_init("/dev/fb0");
	fb_draw_rect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, BLACK);
	fb_update();
	fb_image* img;
	img = fb_read_jpeg_image("./test.jpg");
	fb_draw_image((SCREEN_WIDTH - img->pixel_w) / 2, 0, img, 0);
	fb_update();
	fb_free_image(img);
	show_board();
	show_select(select);
	return 0;
}
