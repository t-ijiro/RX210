//#include "typedefine.h"
#ifdef __cplusplus
//#include <ios>                        // Remove the comment when you use ios
//_SINT ios_base::Init::init_cnt;       // Remove the comment when you use ios
#endif

#include <stdint.h>
#include <stdbool.h>
#include <machine.h>
#include "iodefine.h"
#include "hardwareInit.h"
#include "timer_event_t.h"
#include "period.h"
#include "matrix.h"

#define NUM_PENCIL 3
#define NUM_COLOR  3

volatile timer_event_t timer_event_flag;

struct Pencil{
	uint8_t x;
	uint8_t y;
	pixel_color_t color;
    uint8_t delay;
	bool active;
};

void pencil_boot(struct Pencil *pen)
{
	if(--pen->delay == 0)
	{
		pen->active = true;
	}
}

void pencil_update(struct Pencil *pen)
{
	pen->x = (pen->x + 1) % MATRIX_WIDTH;

	if(pen->x == 0)
	{
		pen->y = (pen->y + 1) % MATRIX_HEIGHT;

		if(pen->y == 0)
		{
			pen->color = 1 + pen->color % NUM_COLOR;
		}
	}
}

void main(void);
#ifdef __cplusplus
extern "C" {
void abort(void);
}
#endif

void main(void)
{
	uint8_t i;
	
	uint32_t counter_dynamic   = PERIOD_DYNAMIC_MS;
	uint32_t counter_gradation = PERIOD_GRADATION_MS;
	uint32_t counter_scroll    = PERIOD_SCROLL_MS;
 
	uint8_t vert_cnt = 0;
	
	struct Pencil pencil[NUM_PENCIL] =
	{
		{0, 0, pixel_red,    1,  false},
		{0, 0, pixel_green,  21, false},
		{0, 0, pixel_orange, 42, false}
	};
	
	uint16_t screen_buf[MATRIX_WIDTH];
	
	init_CLK();
	init_CMT0(1u);  // 1ms周期
	matrix_init();

	matrix_set_scroll_text("HELLO WORLD");
	matrix_set_scroll_colors(pixel_red, pixel_off);

	while(1)
	{
		// ************************************************************
		// ソフトウェアタイマー生成処理
		// ************************************************************
		if(timer_event_flag & TASK_GEN_SOFTWARE_TIMER)
		{
			if(--counter_dynamic == 0)
			{
				timer_event_flag |= TASK_DYNAMIC;
				counter_dynamic = PERIOD_DYNAMIC_MS;
			}

			if(--counter_gradation == 0)
			{
				timer_event_flag |= TASK_GRADATION;
				counter_gradation = PERIOD_GRADATION_MS;
			}

			if(--counter_scroll == 0)
			{
				timer_event_flag |= TASK_SCROLL;
				counter_scroll = PERIOD_SCROLL_MS;
			}

			timer_event_flag &= ~TASK_GEN_SOFTWARE_TIMER;
		}
		
		// ************************************************************
		// ダイナミック点灯処理
		// ************************************************************
		if(timer_event_flag & TASK_DYNAMIC)
		{
			uint16_t vert_data;
			
			vert_cnt = (vert_cnt + 1) % MATRIX_WIDTH;

			vert_data = matrix_get_data(vert_cnt);

			matrix_out(vert_cnt, vert_data);

			timer_event_flag &= ~TASK_DYNAMIC;
		}
		
		// ************************************************************
		// グラデーション処理
		// ************************************************************
		if(timer_event_flag & TASK_GRADATION)
		{
			for(i = 0; i < NUM_PENCIL; i++)
			{
				if(!pencil[i].active)
				{
					pencil_boot(&pencil[i]);
					continue;
				}

				if(matrix_read(pencil[i].x, pencil[i].y) != pixel_off)
				{
					matrix_write(pencil[i].x, pencil[i].y, pencil[i].color);
				}

				pencil_update(&pencil[i]);
			}
			
			matrix_copy(screen_buf);
			matrix_flush();
			matrix_paste(screen_buf);

			timer_event_flag &= ~TASK_GRADATION;
		}

		// ************************************************************
		// スクロール処理
		// ************************************************************
		if(timer_event_flag & TASK_SCROLL)
		{
			// グラデーション処理で上書きするのでflushはしない
			matrix_scroll_text('l');
			
			timer_event_flag &= ~TASK_SCROLL;
		}
	}
}

#ifdef __cplusplus
void abort(void)
{

}
#endif
