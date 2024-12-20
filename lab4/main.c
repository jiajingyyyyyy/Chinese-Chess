#include <stdio.h>
#include "../common/common.h"

#define MAX_FINGER_NUM 6
#define One FB_COLOR(224, 0, 0)
#define Two FB_COLOR(128, 224, 0)
#define Three FB_COLOR(128, 128, 224)
#define Four FB_COLOR(224, 224, 224)
#define Five FB_COLOR(0, 127, 255)
#define Six FB_COLOR(0, 0, 224)
#define POINT_RADIUS 30
const int color_range[] = {
	One,
	Two,
	Three,
	Four,
	Five,
	Six,
};

#define COLOR_BACKGROUND FB_COLOR(0xff, 0xff, 0xff) // white

static int touch_fd;
static int radius[5] = {45, 47, 49, 51, 53};
static struct old_pos
{
	int x;
	int y;
} old[5];

static void touch_event_cb(int fd)
{
	int type, x, y, finger, color;
	type = touch_read(fd, &x, &y, &finger);
	x = x * 800 / 1024;
	y = y * SCREEN_HEIGHT / 600;
	int delta_x, delta_y;
	delta_x = x - old[finger].x;
	delta_y = y - old[finger].y;
	switch (type)
	{
	case TOUCH_PRESS:
		printf("TOUCH_PRESS:x=%d,y=%d,finger=%d\n", x, y, finger);
		switch (finger)
		{
		case 0:
			color = One;
			break;
		case 1:
			color = Two;
			break;
		case 2:
			color = Three;
			break;
		case 3:
			color = Four;
			break;
		case 4:
			color = Five;
			break;
		default:
			break;
		}
		
		fb_draw_circle(x, y, POINT_RADIUS, color);
		old[finger].x = x;
		old[finger].y = y;
		break;
	case TOUCH_MOVE:
		printf("TOUCH_MOVE：x=%d,y=%d,finger=%d\n", x, y, finger);
		switch (finger)
		{
		case 0:
			color = One;
			break;
		case 1:
			color = Two;
			break;
		case 2:
			color = Three;
			break;
		case 3:
			color = Four;
			break;
		case 4:
			color = Five;
			break;
		default:
			break;
		}
		fb_draw_circle(old[finger].x, old[finger].y, POINT_RADIUS + 20, COLOR_BACKGROUND);
		fb_draw_circle(x + 0.1*delta_x, y + 0.1*delta_y, POINT_RADIUS, color);
		old[finger].x = x;
		old[finger].y = y;
		break;
	case TOUCH_RELEASE:
		printf("TOUCH_RELEASE:x=%d,y=%d,finger=%d\n", x, y, finger);
		fb_draw_circle(x, y, POINT_RADIUS + 2, COLOR_BACKGROUND);
		break;
	case TOUCH_ERROR:
		printf("close touch fd\n");
		close(fd);
		task_delete_file(fd);
		break;
	default:
		return;
	}
	fb_update();
	return;
}

int main(int argc, char *argv[])
{
	fb_init("/dev/fb0");
	fb_draw_rect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, COLOR_BACKGROUND);
	fb_update();

	// 打开多点触摸设备文件, 返回文件fd
	touch_fd = touch_init("/dev/input/event2");
	// 添加任务, 当touch_fd文件可读时, 会自动调用touch_event_cb函数
	task_add_file(touch_fd, touch_event_cb);

	task_loop(); // 进入任务循环
	return 0;
}