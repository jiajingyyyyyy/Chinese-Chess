#include "common.h"
#include <sys/ioctl.h>
#include <linux/fb.h>
#include <linux/kd.h>
#include <linux/vt.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/mman.h>
#include <string.h>

static int LCD_FB_FD;
static int *LCD_FB_BUF = NULL;
static int DRAW_BUF[SCREEN_WIDTH*SCREEN_HEIGHT];

static struct area {
	int x1, x2, y1, y2;
} update_area = {0,0,0,0};

#define AREA_SET_EMPTY(pa) do {\
	(pa)->x1 = SCREEN_WIDTH;\
	(pa)->x2 = 0;\
	(pa)->y1 = SCREEN_HEIGHT;\
	(pa)->y2 = 0;\
} while(0)

void fb_init(char *dev)
{
	int fd;
	struct fb_fix_screeninfo fb_fix;
	struct fb_var_screeninfo fb_var;

	if(LCD_FB_BUF != NULL) return; /*already done*/

	//进入终端图形模式
	fd = open("/dev/tty0",O_RDWR,0);
	ioctl(fd, KDSETMODE, KD_GRAPHICS);
	close(fd);

	//First: Open the device
	if((fd = open(dev, O_RDWR)) < 0){
		printf("Unable to open framebuffer %s, errno = %d\n", dev, errno);
		return;
	}
	if(ioctl(fd, FBIOGET_FSCREENINFO, &fb_fix) < 0){
		printf("Unable to FBIOGET_FSCREENINFO %s\n", dev);
		return;
	}
	if(ioctl(fd, FBIOGET_VSCREENINFO, &fb_var) < 0){
		printf("Unable to FBIOGET_VSCREENINFO %s\n", dev);
		return;
	}

	printf("framebuffer info: bits_per_pixel=%u,size=(%d,%d),virtual_pos_size=(%d,%d)(%d,%d),line_length=%u,smem_len=%u\n",
		fb_var.bits_per_pixel, fb_var.xres, fb_var.yres, fb_var.xoffset, fb_var.yoffset,
		fb_var.xres_virtual, fb_var.yres_virtual, fb_fix.line_length, fb_fix.smem_len);

	//Second: mmap
	void *addr = mmap(NULL, fb_fix.smem_len, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
	if(addr == (void *)-1){
		printf("failed to mmap memory for framebuffer.\n");
		return;
	}

	if((fb_var.xoffset != 0) ||(fb_var.yoffset != 0))
	{
		fb_var.xoffset = 0;
		fb_var.yoffset = 0;
		if(ioctl(fd, FBIOPAN_DISPLAY, &fb_var) < 0) {
			printf("FBIOPAN_DISPLAY framebuffer failed\n");
		}
	}

	LCD_FB_FD = fd;
	LCD_FB_BUF = addr;

	//set empty
	AREA_SET_EMPTY(&update_area);
	return;
}

static void _copy_area(int *dst, int *src, struct area *pa)
{
	int x, y, w, h;
	x = pa->x1; w = pa->x2-x;
	y = pa->y1; h = pa->y2-y;
	src += y*SCREEN_WIDTH + x;
	dst += y*SCREEN_WIDTH + x;
	while(h-- > 0){
		memcpy(dst, src, w*4);
		src += SCREEN_WIDTH;
		dst += SCREEN_WIDTH;
	}
}

static int _check_area(struct area *pa)
{
	if(pa->x2 == 0) return 0; //is empty

	if(pa->x1 < 0) pa->x1 = 0;
	if(pa->x2 > SCREEN_WIDTH) pa->x2 = SCREEN_WIDTH;
	if(pa->y1 < 0) pa->y1 = 0;
	if(pa->y2 > SCREEN_HEIGHT) pa->y2 = SCREEN_HEIGHT;

	if((pa->x2 > pa->x1) && (pa->y2 > pa->y1))
		return 1; //no empty

	//set empty
	AREA_SET_EMPTY(pa);
	return 0;
}

void fb_update(void)
{
	if(_check_area(&update_area) == 0) return; //is empty
	_copy_area(LCD_FB_BUF, DRAW_BUF, &update_area);
	AREA_SET_EMPTY(&update_area); //set empty
	return;
}

/*======================================================================*/

static void * _begin_draw(int x, int y, int w, int h)
{
	int x2 = x+w;
	int y2 = y+h;
	if(update_area.x1 > x) update_area.x1 = x;
	if(update_area.y1 > y) update_area.y1 = y;
	if(update_area.x2 < x2) update_area.x2 = x2;
	if(update_area.y2 < y2) update_area.y2 = y2;
	return DRAW_BUF;
}

void fb_draw_pixel(int x, int y, int color)
{
	if(x<0 || y<0 || x>=SCREEN_WIDTH || y>=SCREEN_HEIGHT) return;
	int *buf = _begin_draw(x,y,1,1);
/*---------------------------------------------------*/
	*(buf + y*SCREEN_WIDTH + x) = color;
/*---------------------------------------------------*/
	return;
}

void fb_draw_rect(int x, int y, int w, int h, int color)
{
	if(x < 0) { w += x; x = 0;}
	if(x+w > SCREEN_WIDTH) { w = SCREEN_WIDTH-x;}
	if(y < 0) { h += y; y = 0;}
	if(y+h >SCREEN_HEIGHT) { h = SCREEN_HEIGHT-y;}
	if(w<=0 || h<=0) return;
	int *buf = _begin_draw(x,y,w,h);
/*---------------------------------------------------*/
	for(int i=0;i<h;i++)
	{
		for(int j=0;j<w;j++)
		{
			*(buf + (y+i) * SCREEN_WIDTH + x+j) = color;
		}
	}

/*---------------------------------------------------*/
	return;
}

void fb_draw_line(int x1, int y1, int x2, int y2, int color)
{
	// 确定绘制区域的范围
	int x_min = x1 < x2 ? x1 : x2;
	int y_min = y1 < y2 ? y1 : y2;
	int x_max = x1 > x2 ? x1 : x2;
	int y_max = y1 > y2 ? y1 : y2;
	int* buf = _begin_draw(x_min, y_min, x_max - x_min + 1, y_max - y_min + 1);

	// 处理垂直线的情况
	if (x1 == x2) {
		for (int y = y_min; y <= y_max; y++) {
			*(buf + y * SCREEN_WIDTH + x1) = color;
		}
		return;
	}

	// 处理水平线的情况
	if (y1 == y2) {
		for (int x = x_min; x <= x_max; x++) {
			*(buf + y1 * SCREEN_WIDTH + x) = color;
		}
		return;
	}

	// 判断斜率绝对值：|斜率| < 1 时按 x 遍历；否则按 y 遍历
	if (abs(y2 - y1) < abs(x2 - x1)) {
		// 按 x 遍历
		if (x1 < x2) { // 从左到右
			for (int x = x1; x <= x2; x++) {
				int y = (y2 - y1) * (x - x1) / (x2 - x1) + y1;
				*(buf + y * SCREEN_WIDTH + x) = color;
			}
		}
		else { // 从右到左
			for (int x = x1; x >= x2; x--) {
				int y = (y2 - y1) * (x - x1) / (x2 - x1) + y1;
				*(buf + y * SCREEN_WIDTH + x) = color;
			}
		}
	}
	else {
		// 按 y 遍历
		if (y1 < y2) { // 从上到下
			for (int y = y1; y <= y2; y++) {
				int x = (x2 - x1) * (y - y1) / (y2 - y1) + x1;
				*(buf + y * SCREEN_WIDTH + x) = color;
			}
		}
		else { // 从下到上
			for (int y = y1; y >= y2; y--) {
				int x = (x2 - x1) * (y - y1) / (y2 - y1) + x1;
				*(buf + y * SCREEN_WIDTH + x) = color;
			}
		}
	}
	return;
}

void fb_draw_image(int x, int y, fb_image *image, int color) {
    if (image == NULL)
        return;

    int ix = 0;	 //image x
    int iy = 0;	 //image y
    int w = image->pixel_w;	 //draw width
    int h = image->pixel_h;	 //draw height

    if (x < 0) {
        w += x;
        ix -= x;
        x = 0;
    }
    if (y < 0) {
        h += y;
        iy -= y;
        y = 0;
    }

    if (x + w > SCREEN_WIDTH) {
        w = SCREEN_WIDTH - x;
    }
    if (y + h > SCREEN_HEIGHT) {
        h = SCREEN_HEIGHT - y;
    }
    if ((w <= 0) || (h <= 0))
        return;

    int *buf = _begin_draw(x, y, w, h);
    int *dst = buf + y * SCREEN_WIDTH + x;
    // char *src;

    if (image->color_type == FB_COLOR_RGB_8880) /*lab3: jpg*/
    {
        int *jpg_src = (int *)image->content + iy * image->pixel_w + ix;
        int len = 4 * w;
        for (int i = 0; i < h; ++i) {
			// if(i % 10 == 0) continue;
            // for (int j = 0; j < w; ++j) {
            // 	dst[i * SCREEN_WIDTH + j] = jpg_src[i * image->pixel_w + j];
            // }
            memcpy(dst + i * SCREEN_WIDTH, jpg_src + i * image->pixel_w, len);
        }
        return;
    } else if (image->color_type == FB_COLOR_RGBA_8888) /*lab3: png*/
	{
	    int x0, y0, x3, y3;
	    unsigned char alpha;
	    char *show_color, *temp;
	    
	    // 提前准备好常用的数据
	    char r, g, b;

	    
	    for (y0 = y, y3 = iy; y0 < y + h; y0++, y3++) 
	    {
			// if(y0 % 10 == 0) continue;
		for (x0 = x, x3 = ix; x0 < x + w; x0++, x3++) 
		{
		    show_color = (char *)(buf + y0 * SCREEN_WIDTH + x0);
		    temp = image->content + y3 * image->pixel_w * 4 + x3 * 4;
		    alpha = temp[3];

		    // 提前取出颜色分量
		    r = temp[0];
		    g = temp[1];
		    b = temp[2];

		    // 直接使用 if 来避免 switch 的开销
		    if (alpha == 0)
		    {
		        continue;  // 跳过
		    }
		    else if (alpha == 255)
		    {
		        // 完全不透明，直接拷贝颜色
		        show_color[0] = r;
		        show_color[1] = g;
		        show_color[2] = b;
		    }
		    else
		    {
		        // 半透明，根据 alpha 值合成颜色
		        show_color[0] += (((r - show_color[0]) * alpha) >> 8);
		        show_color[1] += (((g - show_color[1]) * alpha) >> 8);
		        show_color[2] += (((b - show_color[2]) * alpha) >> 8);
		    }
		}
	    }
	    return;
	}
	else if (image->color_type == FB_COLOR_ALPHA_8) /*lab3: font*/
	{
	int x0, y0, x3, y3;
	unsigned char alpha;
	char *show_color, *temp;
	for (y0 = y, y3 = iy; y0 < y + h; y0++, y3++)
	{
		for (x0 = x, x3 = ix; x0 < x + w; x0++, x3++)
		{
			show_color = (char *)(buf + y0 * SCREEN_WIDTH + x0);
			temp = image->content + y3 * image->pixel_w + x3;
			alpha = *temp;
			switch (alpha)
			{
			case 0:
				break;
			case 255:
				show_color[0] = (color & 0xff);
				show_color[1] = (color & 0xff00) >> 8;
				show_color[2] = (color & 0xff0000) >> 16;
				break;
			default:
				show_color[0] += ((((color & 0xff) - show_color[0]) * alpha) >> 8);
				show_color[1] += (((((color & 0xff00) >> 8) - show_color[1]) * alpha) >> 8);
				show_color[2] += (((((color & 0xff0000) >> 16) - show_color[2]) * alpha) >> 8);
			}
		}
	}
	return;
	}
    /*---------------------------------------------------------------*/
    return;
}
void fb_draw_border(int x, int y, int w, int h, int color)
{
	if(w<=0 || h<=0) return;
	fb_draw_rect(x, y, w, 1, color);
	if(h > 1) {
		fb_draw_rect(x, y+h-1, w, 1, color);
		fb_draw_rect(x, y+1, 1, h-2, color);
		if(w > 1) fb_draw_rect(x+w-1, y+1, 1, h-2, color);
	}
}

void fb_draw_circle(int x, int y, int r, int color) {
	printf("fb_draw_circle: x=%d, y=%d, r=%d, color=%d\n", x, y, r, color);
	// 特判
    x = x < 0 ? 0 : x;
    x = x >= SCREEN_WIDTH ? SCREEN_WIDTH - 1 : x;
    y = y < 0 ? 0 : y;
    y = y >= SCREEN_HEIGHT ? SCREEN_HEIGHT - 1 : y;
    int x_lower_bound = x - r < 0 ? 0 : x - r;
    int x_upper_bound = x + r >= SCREEN_WIDTH ? SCREEN_WIDTH - 1 : x + r;
    int y_lower_bound = y - r < 0 ? 0 : y - r;
    int y_upper_bound = y + r >= SCREEN_HEIGHT ? SCREEN_HEIGHT - 1 : y + r;
    for (int i = y_lower_bound; i <= y_upper_bound; ++i) {
        for (int j = x_lower_bound; j <= x_upper_bound; ++j) {
			// int tmp_x = x * 0.8;
            int delta_x = j - x;
            int delta_y = i - y;
			// delta_x *= 0.8;
            if (delta_x * delta_x + delta_y * delta_y <= r * r) {
                fb_draw_pixel(j, i, color);
            }
        }
    }
}



/** draw a text string **/
void fb_draw_text(int x, int y, char *text, int font_size, int color)
{
	fb_image *img;
	fb_font_info info;
	int i=0;
	int len = strlen(text);
	while(i < len)
	{
		img = fb_read_font_image(text+i, font_size, &info);
		if(img == NULL) break;
		fb_draw_image(x+info.left, y-info.top, img, color);
		fb_free_image(img);

		x += info.advance_x;
		i += info.bytes;
	}
	return;
}
