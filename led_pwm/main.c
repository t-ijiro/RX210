/***********************************************************************/
/*                                                                     */
/*  FILE        : Main.c                                   */
/*  DATE        :Tue, Oct 31, 2006                                     */
/*  DESCRIPTION :Main Program                                          */
/*  CPU TYPE    :                                                      */
/*                                                                     */
/*  NOTE:THIS IS A TYPICAL EXAMPLE.                                    */
/*                                                                     */
/***********************************************************************/
//#include "typedefine.h"
#ifdef __cplusplus
//#include <ios>                        // Remove the comment when you use ios
//_SINT ios_base::Init::init_cnt;       // Remove the comment when you use ios
#endif

#include <stdint.h>
#include "iodefine.h"
#include "hardwareInit.h"
#include "matrix.h"

#define CYCLE 64

typedef struct
{
	uint8_t count;
	uint8_t threshold;
	pixel_color_t color;
	uint8_t  mode :1;
}PWM_t;

PWM_t pixel[8][8];

void pwm_init(void)
{
	uint8_t x, y;

	for(y = 0; y < 8; y++)
	{
		for(x = 0; x < 8; x++)
		{
			PWM_t *p = &pixel[y][x];

			p->count = CYCLE - (y + 1) * (x + 1);
			p->threshold = (y + 1) * (x + 1);
			p->color = pixel_red;
			p->mode = 1;
		}
	}
}

void pwm_update(uint8_t x, uint8_t y)
{
	PWM_t *p = &pixel[y][x];

	if(p->count == p->threshold)
	{
		matrix_write(x, y, !p->mode ? p->color : pixel_off);
		matrix_flush(BUFF_KEEP);
	}

	if(p->count == CYCLE)
	{
		p->count = 0;
		p->threshold--;

		if(p->threshold == 0)
		{
			p->threshold = CYCLE;
			p->mode ^= 1;

			if(p->mode == 0)
			{
				p->color = 1 + p->color % 2;
			}
		}

		matrix_write(x, y, p->mode ? p->color : pixel_off);
		matrix_flush(BUFF_KEEP);
	}

	p->count++;
}

void main(void);
#ifdef __cplusplus
extern "C" {
void abort(void);
}
#endif

void main(void)
{
	uint8_t x, y;

	init_CLK();
	init_CMT0(1u, 40u); // 1ms / 40 = 25us = 40kHz
	matrix_init();
	pwm_init();

	while(1)
	{
		for(y = 0; y < 8; y++)
		{
			for(x = 0; x < 8; x++)
			{
				pwm_update(x, y);
			}
		}
	}
}

#ifdef __cplusplus
void abort(void)
{

}
#endif
