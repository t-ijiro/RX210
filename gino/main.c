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

#include <machine.h>
#include <stdint.h>
#include <string.h>
#include "iodefine.h"
#include "lcd_lib4.h"
#include "onkai.h"
#include "hardwareInit.h"
#include "matrix.h"
#include "rotary.h"
#include "sw.h"

#define CHAT_PERIOD_MS 50

static const uint8_t arrow[8][8] = {
		{0x08,0x18,0x38,0x7f,0x7f,0x38,0x18,0x08}, //上
		{0x18,0x18,0x18,0xff,0x7e,0x3c,0x18,0x00}, //右
		{0x10,0x18,0x1c,0xfe,0xfe,0x1c,0x18,0x08}, //下
		{0x00,0x18,0x3c,0x7e,0xff,0x18,0x18,0x00}, //左
		{0x00,0x7e,0x3e,0x1e,0x3e,0x76,0xe2,0xc0}, //左下
		{0x00,0x7e,0x7c,0x78,0x7c,0x6e,0x46,0x00}, //左上
		{0x00,0x62,0x76,0x3e,0x1e,0x3e,0x7e,0x00}, //右下
		{0x03,0x47,0x6e,0x7c,0x78,0x7c,0x7e,0x00}  //右上
};

static uint8_t mode;
static uint8_t pre_mode;
volatile uint64_t time_1m_count;
volatile uint32_t btime;

// sw
static sw_t sw[4];
// mode1
static uint8_t sx = 0;
// mode2
static uint32_t beep_time = 100;
// mode3
volatile int64_t temp_total;
volatile uint8_t temp_meas_cnt;
// mode4
static rotary_t rotary;
static rotary_click_t rotary_click_dir;
static pixel_t color = pixel_red;
static pixel_t colors[3] = {pixel_green, pixel_orange, pixel_red};
static uint8_t color_id = 0;
static uint8_t arrow_id = 0;

uint64_t millis(void)
{
	return time_1m_count;
}

void init_BUZZER()
{
	SYSTEM.PRCR.WORD = 0x0A502;
	MSTP(MTU0) = 0;
	SYSTEM.PRCR.WORD = 0x0A500;

	PORT3.PDR.BIT.B4 = 1;
	PORT3.PMR.BIT.B4 = 1;
	MPC.PWPR.BIT.B0WI = 0;
	MPC.PWPR.BIT.PFSWE = 1;
	MPC.P34PFS.BIT.PSEL = 1;
	MPC.PWPR.BIT.PFSWE = 0;

	MTU.TSTR.BIT.CST0 = 0x00;
	MTU0.TCR.BIT.TPSC = 0x01;
	MTU0.TCR.BIT.CCLR = 0x01;
	MTU0.TMDR.BIT.MD = 0x02;

	MTU0.TIORH.BIT.IOA = 0x06;
	MTU0.TIORH.BIT.IOB = 0x05;
	MTU0.TCNT = 0;
}

void init_AD(void)
{
	SYSTEM.PRCR.WORD = 0xa502; // プロテクト解除
	MSTP(S12AD)= 0; 		   // A/Dコンバータ モジュールスタンバイ解除
	SYSTEM.PRCR.WORD = 0xa500; // プロテクト設定
	PORT4.PMR.BYTE = 0x0F;	   //P40-P43を周辺機能として使用
	S12AD.ADCSR.BIT.ADIE = 0;  // 割り込みを使用する
	S12AD.ADANSA.BIT.ANSA0 = 1;// アナログ入力チャネル選択AN000
	S12AD.ADANSA.BIT.ANSA1 = 1;// アナログ入力チャネル選択AN001(3軸加速度センサ)
	S12AD.ADANSA.BIT.ANSA2 = 1;// アナログ入力チャネル選択AN002(3軸加速度センサ)
	S12AD.ADANSA.BIT.ANSA3 = 1;// アナログ入カチャネル選択AN003(3軸加速度センサ)
	S12AD.ADCSR.BIT.ADCS = 0;  // シングルスキャンモード
	MPC.PWPR.BIT.B0WI = 0;     // PFSWE書き込み可
	MPC.PWPR.BIT.PFSWE = 1;    // PFSレジスタへの書き込み可
	MPC.P40PFS.BIT.ASEL = 1;   // P40をAN000として使用
	MPC.P41PFS.BIT.ASEL = 1;   // P41をAN001として使用
	MPC.P42PFS.BIT.ASEL = 1;   // P42をAN002として使用
	MPC.P43PFS.BIT.ASEL = 1;   // P43をAN003として使用
	MPC.PWPR.BIT.PFSWE = 0;    // PFSレジスタへの書き込み禁止
	IEN(S12AD,S12ADI0) = 1;    //割込み要求の許可
	IPR(S12AD,S12ADI0) = 1;	   //割込み要因プライオリティの設定
}

void ad_start()
{
	S12AD.ADCSR.BIT.ADST = 1;
}

void matrix_set_arrow(uint8_t arrow_id, pixel_t color)
{
	uint8_t x, y;

	for(y = 0; y < 8; y++)
	{
		for(x = 0; x < 8; x++)
		{
			if(arrow[arrow_id][x] & (1 << y))
			{
				matrix_write(x, y, color);
			}
			else
			{
				matrix_write(x, y, pixel_off);
			}
		}
	}
}

void beep(uint32_t tone, uint32_t interval)
{
	if (tone)
	{
		MTU.TSTR.BIT.CST0 = 0;
		MTU0.TGRA = tone;
		MTU0.TGRB = tone / 2;
		MTU.TSTR.BIT.CST0 = 1;
	}
	else
	{
		MTU.TSTR.BIT.CST0 = 0;
	}

	btime = interval;
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
	uint8_t sw_flag = 0x00;

	init_CLK();
	init_LCD();
	init_BUZZER();
	init_CMT0(1, 1);  // 1m秒  タイマースタート
	init_CMT1(10, 0); // 10m秒 タイマーストップ
	init_CMT2(2, 0);  // 2m秒  タイマーストップ
	init_AD();
	sw_init();
	matrix_init();
	rotary_init();
	setpsw_i();

	lcd_puts("Technical Test");
	lcd_xy(16, 2);
	lcd_put(0xFF);
	flush_lcd();
	
	for(i = 0; i < 4; i++)
	{
		sw[i].debounce_t = millis();
		sw[i].cur = SW_OFF;
		sw[i].pre = SW_OFF;
		sw[i].stable = SW_OFF;
	}
	
	rotary = rotary_get_instance(0, 0);

	while(1)
	{
		for(i = 0; i < 4; i++)
		{   
			uint8_t sw_id = i + 5; // 5 ~ 8
			sw[i].cur = sw_read(sw_id);
			if(sw[i].pre != sw[i].cur)
			{
				sw[i].debounce_t = millis();
				sw[i].pre = sw[i].cur;
			}

			if((millis() - sw[i].debounce_t >= CHAT_PERIOD_MS) && (sw[i].stable != sw[i].cur))
			{
				if(sw[i].stable == SW_ON && sw[i].cur == SW_OFF)
				{
					// sw8..0x80 sw7..0x40 sw6..0x20 sw5..0x10
					sw_flag |= 1 << (sw_id - 1);
				}
				sw[i].stable = sw[i].cur;
			}
		}
		
		if(sw_flag & 0x10)
		{
			mode = 1 + mode % 4;
			sw_flag &= ~0x10;
		}

		switch(mode)
		{
		case 1:
			if(mode != pre_mode)
			{
				sx = 16;
				lcd_clear();
				lcd_puts("mode1");
				lcd_xy(sx, 2);
				lcd_put(0xFF);
				flush_lcd();
				PORTE.PODR.BYTE = 0x00;
				CMT.CMSTR1.BIT.STR2 = 0;
			}

            if(sw_flag & 0x80)
            {
                if(1 < sx)
                {
                    lcd_xy(sx, 2);
                    lcd_put(' ');
                    sx--;
                    lcd_xy(sx, 2);
                    lcd_put(0xFF);
                    flush_lcd();
                }
				sw_flag &= ~0x80;
             }
              
			if(sw_flag & 0x40)
			{
				if(sx < 16)
				{
					lcd_xy(sx, 2);
					lcd_put(' ');
					sx++;
					lcd_xy(sx, 2);
					lcd_put(0xFF);
					flush_lcd();
				}
				sw_flag &= ~0x40;
			}
			break;
		case 2:
			if(mode != pre_mode)
			{
				beep_time = 100;
				lcd_clear();
				lcd_puts("mode2");
				lcd_xy(1, 2);
				lcd_puts("beep time:");
				lcd_dataout(beep_time / 1000);
				lcd_put('.');
				lcd_dataout(beep_time / 100 % 10);
				lcd_put('s');
				flush_lcd();
			}

            if(sw_flag & 0x80)
            {
				if(100 < beep_time)
				{
					beep_time -= 100;
					lcd_xy(strlen("beep time:") + 1, 2);
					lcd_dataout(beep_time / 1000);
					lcd_put('.');
					lcd_dataout(beep_time / 100 % 10);
					lcd_put('s');
					flush_lcd();
				}
				sw_flag &= ~0x80;
            }
	
			if(sw_flag & 0x40)
			{
				beep(RA1, beep_time);
				sw_flag &= ~0x40;
			}

			if(sw_flag & 0x20)
			{
				if(beep_time < 2000)
				{
					beep_time += 100;
					lcd_xy(strlen("beep time:") + 1, 2);
					lcd_dataout(beep_time / 1000);
					lcd_put('.');
					lcd_dataout(beep_time / 100 % 10);
					lcd_put('s');
					flush_lcd();
				}
				sw_flag &= ~0x20;
			}
			break;
		case 3:
			if(mode != pre_mode)
			{
				MTU.TSTR.BIT.CST0 = 0;
				lcd_clear();
				lcd_puts("mode3");
				lcd_xy(1, 2);
				lcd_puts("temperature:");
				ad_start();
				while(S12AD.ADCSR.BIT.ADST == 1)
					;
				lcd_dataout((S12AD.ADDR0*4700/4096/2-600)/10);
				lcd_put(0xDF);
				lcd_put(0x43);
				flush_lcd();
				temp_total=0;
			    temp_meas_cnt = 0;
				S12AD.ADCSR.BIT.ADIE = 1; // ここからは割り込み
				CMT.CMSTR0.BIT.STR1 = 1;
			}
			break;
		case 4:
			if(mode != pre_mode)
			{
				CMT.CMSTR0.BIT.STR1 = 0;
				CMT.CMSTR1.BIT.STR2 = 1;
				rotary_clear(&rotary);
				lcd_clear();
				lcd_puts("mode4");
				lcd_xy(1, 2);
				lcd_puts("matrixLED");
				flush_lcd();
				arrow_id = 0;
				color_id = 2;
			    color = colors[color_id];
				matrix_set_arrow(arrow_id, color);
				matrix_flush(BUFF_CLEAR);
			}

            if(sw_flag & 0x80)
            {
				color_id = (color_id + 1) % 3;
				color = colors[color_id];
				matrix_set_arrow(arrow_id, color);
				matrix_flush(BUFF_CLEAR);
				sw_flag &= ~0x80;
            }

			rotary_record_new(&rotary);
			rotary_click_dir = rotary_get_click_dir(rotary_calc_delta(&rotary));
			if(rotary_click_dir != CLICK_IDLE)
			{
				if(rotary_click_dir == CLICK_LEFT)
				{
					arrow_id = (arrow_id + 3) % 4;
				}
				else if(rotary_click_dir == CLICK_RIGHT)
				{
					arrow_id = (arrow_id + 1) % 4;
				}

				matrix_set_arrow(arrow_id, color);
				matrix_flush(BUFF_CLEAR);
				rotary_record_old(&rotary);
			}
			break;
		default:
			break;
		}
		pre_mode = mode;
	}
}

#ifdef __cplusplus
void abort(void)
{

}
#endif
