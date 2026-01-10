// matrix.c
// Created on : 2025/12/13
// Author : T.Ijiro

#include <stdint.h>
#include <string.h>
#include "iodefine.h"
#include "matrix.h"

// 74HC595への出力ピン
#define SERIAL_PIN PORT1.PODR.BIT.B5
#define CLOCK_PIN  PORT1.PODR.BIT.B6
#define LATCH_PIN  PORT1.PODR.BIT.B7

// ダイナミック点灯制御ポート
#define DYNAMIC_PORT PORTE.PODR.BYTE

// ダブルバッファ
static uint16_t buffer[2][MATRIX_WIDTH] = {{0x0000}};

// 描画バッファへのポインタ
static volatile uint16_t *back  = buffer[0];

// 表示バッファへのポインタ
static volatile uint16_t *front = buffer[1];

// 入出力初期化
void matrix_init(void)
{
    // 使用ピンを全て出力に設定
    PORT1.PDR.BYTE = 0xE0;
    PORTE.PDR.BYTE = 0xFF;
    
    CLOCK_PIN    = 0;
    LATCH_PIN    = 0;
    DYNAMIC_PORT = 0x00;
}

// 描画バッファの指定座標に色を書き込む
void matrix_write(uint8_t x, uint8_t y, pixel_t c)
{
    back[x] &= ~((1 << (y + 8)) | (1 << y));

    if(c & pixel_red)
    {
        back[x] |= (1 << (y + 8));
    }

    if(c & pixel_green)
    {
        back[x] |= (1 << y);
    }
}

// 描画バッファの指定座標の色を読み込む
pixel_t matrix_read(uint8_t x, uint8_t y)
{
    pixel_t c = pixel_off;
    
    if(back[x] & (1 << (y + 8)))
    {
        c |= pixel_red;
    }

    if(back[x] & (1 << y))
    {
        c |= pixel_green;
    }

    return c;
}

// 描画バッファ全削除
void matrix_clear(void)
{
    uint8_t x;

	for(x = 0; x < MATRIX_WIDTH; x++)
	{
        back[x] = 0x0000;
	}
}

// アスキー文字使用時
#if MATRIX_USE_ASCII
#define FONT_WIDTH 8 // アルファベット１文字分のデータ幅
#define SCROLL_TEXT_SIZE 32 // スクロール文字列の文字数
#define SCROLL_BUF_SIZE (SCROLL_TEXT_SIZE * FONT_WIDTH)

// A-Zフォントデータ
static const uint8_t ucALPHABET[26][8]={
	{0x00,0x00,0x1f,0x64,0x64,0x1f,0x00,0x00},
	{0x00,0x00,0x7f,0x49,0x49,0x36,0x00,0x00},
	{0x00,0x00,0x3e,0x41,0x41,0x22,0x00,0x00},
	{0x00,0x00,0x7f,0x41,0x41,0x3e,0x00,0x00},
	{0x00,0x00,0x7f,0x49,0x49,0x41,0x00,0x00},
	{0x00,0x00,0x7f,0x48,0x48,0x40,0x00,0x00},
	{0x00,0x00,0x3e,0x41,0x49,0x2f,0x00,0x00},
	{0x00,0x00,0x7f,0x08,0x08,0x7f,0x00,0x00},
	{0x00,0x00,0x00,0x41,0x7f,0x41,0x00,0x00},
	{0x00,0x00,0x02,0x01,0x01,0x7e,0x00,0x00},
	{0x00,0x00,0x7f,0x08,0x14,0x63,0x00,0x00},
	{0x00,0x00,0x7f,0x01,0x01,0x01,0x00,0x00},
	{0x00,0x7f,0x40,0x20,0x18,0x20,0x40,0x7f},
	{0x00,0x7f,0x20,0x10,0x08,0x04,0x02,0x7f},
	{0x00,0x00,0x3e,0x41,0x41,0x3e,0x00,0x00},
	{0x00,0x00,0x7f,0x44,0x44,0x38,0x00,0x00},
	{0x00,0x3e,0x45,0x45,0x43,0x3e,0x01,0x00},
	{0x00,0x00,0x7f,0x48,0x48,0x36,0x01,0x00},
	{0x00,0x00,0x32,0x49,0x49,0x26,0x00,0x00},
	{0x00,0x20,0x20,0x20,0x3f,0x20,0x20,0x20},
	{0x00,0x7e,0x01,0x01,0x01,0x01,0x7e,0x00},
	{0x00,0x00,0x70,0x0c,0x03,0x0c,0x70,0x00},
	{0x00,0x3e,0x01,0x06,0x18,0x06,0x01,0x3e},
	{0x00,0x41,0x22,0x14,0x08,0x14,0x22,0x41},
	{0x00,0x40,0x20,0x10,0x0f,0x10,0x20,0x40},
	{0x00,0x00,0x43,0x45,0x59,0x61,0x00,0x00}
};

// スクロール文字列管理構造体
typedef struct{
    uint8_t text[SCROLL_BUF_SIZE];
    int32_t length;
    int32_t position;
    pixel_t fg_color;
    pixel_t bg_color;
}scroll_text_t;

// スクロール文字列管理変数
static scroll_text_t scroll_text = {
    {0x00},
    0,
    0,
    pixel_off,
    pixel_off
};

// １文字を描画バッファに書き込む
// ch: 描画する文字 (A-Zのみ対応)
void matrix_put_char(char ch, pixel_t fg, pixel_t bg)
{
    uint8_t x, y;

    if(ch < 'A' || ch > 'Z')
    {
        return;
    }
    
    for(x = 0; x < FONT_WIDTH; x++)
    {
        for(y = 0; y < MATRIX_HEIGHT; y++)
        {
            if(ucALPHABET[ch - 'A'][x] & (1 << y))
            {
                matrix_write(x, y, fg);
            }
            else
            {
                matrix_write(x, y, bg);
            }
        }
    }
}

// スクロール文字列を設定
// text: 表示する文字列 (A-Zのみ対応)
// 最大文字数はSCROLL_TEXT_SIZE文字まで
void matrix_set_scroll_text(const char *text)
{
    scroll_text.position = 0;
    scroll_text.length = 0;

    while(*text != '\0' && scroll_text.length < SCROLL_BUF_SIZE)
    {
        char ch = *text++;

        if('A' <= ch && ch <= 'Z')
        {
            memmove(&scroll_text.text[scroll_text.length], ucALPHABET[ch - 'A'], FONT_WIDTH);
        }
        else
        {
            // 未対応文字は空白扱い
            memset(&scroll_text.text[scroll_text.length], 0x00, FONT_WIDTH);
        }

        scroll_text.length += FONT_WIDTH;
    }
}

// スクロール文字列の前景色・背景色を設定
void matrix_set_scroll_colors(pixel_t fg, pixel_t bg)
{
    scroll_text.fg_color = fg;
    scroll_text.bg_color = bg;
}

// スクロール文字列を指定した方向に１つずらす
// 左：'l'  右：'r'
void matrix_scroll_text(char dir)
{
    uint8_t x, y;
	
    switch(dir)
    {
        case 'l':
            scroll_text.position++;

            if(scroll_text.length <= scroll_text.position)
            {
                scroll_text.position = 0;
            }

            break;
        case 'r':
            scroll_text.position--;

            if(scroll_text.position < 0)
            {
                scroll_text.position = scroll_text.length - 1;
            }       

            break;
        default:
            break;
    }
    
    for(y = 0; y < MATRIX_HEIGHT; y++)
    {
        for(x = 0; x < FONT_WIDTH; x++)
        {
			uint16_t idx = (scroll_text.position + x) % scroll_text.length;
            
			if(scroll_text.text[idx] & (1 << y))
            {
                matrix_write(x, y, scroll_text.fg_color);
			}
			else
			{
                matrix_write(x, y, scroll_text.bg_color);
			}
		}
	}
}
#endif /* MATRIX_USE_ASCII */

// 描画バッファを外部バッファにコピー
void matrix_copy(uint16_t dst[MATRIX_WIDTH])
{
    uint8_t x;
    
    for(x = 0; x < MATRIX_WIDTH; x++)
    {
        dst[x] = back[x];  
    }
}

// 外部バッファを描画バッファにコピー
void matrix_paste(const uint16_t src[MATRIX_WIDTH])
{
    uint8_t x;
    
    for(x = 0; x < MATRIX_WIDTH; x++)
    {
        back[x] = src[x];  
    }
}

// flush内部処理用
static void v_matrix_paste(volatile const uint16_t src[MATRIX_WIDTH])
{
    uint8_t x;
    
    for(x = 0; x < MATRIX_WIDTH; x++)
    {
        back[x] = src[x];  
    }
}

// 描画バッファと表示バッファを入れ替える
// option = HANDLE_BUFF_INHERIT で描画バッファ内容を保持
// option = HANDLE_BUFF_CLEAR   で描画バッファ内容を破棄
void matrix_flush(handle_buff_t option)
{
    volatile uint16_t *tmp = front;
    front = back;
    back  = tmp;

    switch(option)
    {
        case HANDLE_BUFF_INHERIT:
            v_matrix_paste(front);
            break;
        case HANDLE_BUFF_CLEAR:
            matrix_clear();
            break;
        default :
            // 描画バッファの状態は不定
            break;
    }
}

// 指定列のマトリックスLED送信用16bitデータを取得
uint16_t matrix_get_data(uint8_t x)
{
    return front[x];
}

// 16bitデータをマトリックスLEDの指定列に出力
// data のビット配置:
// bit[15:8] : 指定列の赤LEDの点灯パターン
// bit[7:0]  : 指定列の緑LEDの点灯パターン
// 例 : 10101111 01011111 = 赤緑赤緑橙橙橙橙
void matrix_out(uint8_t x, uint16_t data)
{
    uint8_t shift;
    
    for(shift = 0; shift < 16; shift++) 
    {
        if(data & (1 << shift)) 
        {
            SERIAL_PIN = 0;
        } 
        else 
        {
            SERIAL_PIN = 1;
        }

        CLOCK_PIN = 1;
        CLOCK_PIN = 0;
    }
    
    DYNAMIC_PORT = 0x00;

    LATCH_PIN = 1;
    LATCH_PIN = 0;

    DYNAMIC_PORT = 1 << x;
}