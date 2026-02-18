/* 
 * matrix.c
 * e2417105
 */

#include <stdint.h>
#include <string.h>
#include "iodefine.h"
#include "matrix.h"

#define SERIAL_PIN PORT1.PODR.BIT.B5
#define CLOCK_PIN  PORT1.PODR.BIT.B6
#define LATCH_PIN  PORT1.PODR.BIT.B7
#define DYNAMIC_PORT PORTE.PODR.BYTE
#define MATRIX_WIDTH  8
#define MATRIX_HEIGHT 8

static uint16_t buffer[2][MATRIX_WIDTH] = {{0x0000}};
static uint16_t *back = buffer[0];
static uint16_t * volatile front = buffer[1];

void matrix_init(void)
{
    PORT1.PDR.BYTE = 0xE0;
    PORTE.PDR.BYTE = 0xFF;
    
    CLOCK_PIN    = 0;
    LATCH_PIN    = 0;
    DYNAMIC_PORT = 0x00;
}

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

void matrix_clear(void)
{
    memset(back, 0x0000, sizeof(buffer[0]));
}

// フォント機能有効時
#ifdef MATRIX_USE_FONT
#define FONT_WIDTH 8
#define FONT_HEIGHT 16
#define SCROLL_TEXT_SIZE 32

static const uint8_t ucALPHABET[26][8] = {
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

typedef struct {
    char     str[SCROLL_TEXT_SIZE + 1];   // 元の文字列を保存
    uint16_t str_length;                  // 文字列の文字数
    uint16_t pos_x;                       // 横スクロール位置
    uint8_t  pos_y;                       // 縦スクロール位置
    pixel_t  fg_color;                    // 前景色
    pixel_t  bg_color;                    // 背景色
} scroll_text_t;

static scroll_text_t scroll_text = {
    "",
    0,
    0,
    0,
    pixel_off,
    pixel_off
};

void matrix_write_char(char ch, pixel_t fg, pixel_t bg)
{
    uint8_t x, y;

    if(ch < 'A' || ch > 'Z')
    {
        return;
    }
    
    for(x = 0; x < MATRIX_WIDTH; x++)
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

void matrix_scroller_set_text(const char *text)
{
    scroll_text.pos_x = 0;
    scroll_text.pos_y = 0;
    scroll_text.str_length = 0;

    while(*text != '\0' && scroll_text.str_length < SCROLL_TEXT_SIZE)
    {
        scroll_text.str[scroll_text.str_length] = *text;
        scroll_text.str_length++;
        text++;
    }

    scroll_text.str[scroll_text.str_length] = '\0';
}

void matrix_scroller_set_foreground(pixel_t fg)
{
    scroll_text.fg_color = fg;
}

void matrix_scroller_set_background(pixel_t bg)
{
    scroll_text.bg_color = bg;
}

void matrix_scroller_scroll_pos(scroll_t dir)
{
    uint16_t total_width = scroll_text.str_length * FONT_WIDTH;

    if(scroll_text.str_length == 0)
    {
        return;
    }

    // 左
    if(dir & SCROLL_LEFT)
    {
        if(scroll_text.pos_x < total_width - 1)
        {
            scroll_text.pos_x++;
        }
        else
        {
            scroll_text.pos_x = 0;
        }
    }

    // 右
    if(dir & SCROLL_RIGHT)
    {
        if(0 < scroll_text.pos_x)
        {
            scroll_text.pos_x--;
        }
        else
        {
            scroll_text.pos_x = total_width - 1;
        }
    }

    // 上
    if(dir & SCROLL_UP)
    {
        if(scroll_text.pos_y < FONT_HEIGHT - 1)
        {
            scroll_text.pos_y++;
        }
        else
        {
            scroll_text.pos_y = 0;
        }
    }

    // 下
    if(dir & SCROLL_DOWN)
    {
        if(0 < scroll_text.pos_y)
        {
            scroll_text.pos_y--;
        }
        else
        {
            scroll_text.pos_y = FONT_HEIGHT - 1;
        }
    }
}

void matrix_scroller_write_text(void)
{
    uint8_t x, y;
    uint8_t upper_offset  = scroll_text.pos_y;
    uint8_t bottom_offset = FONT_HEIGHT - upper_offset;
    uint16_t total_width = scroll_text.str_length * FONT_WIDTH;

    if(scroll_text.str_length == 0)
    {
        for(x = 0; x < MATRIX_WIDTH; x++)
        {
            for(y = 0; y < MATRIX_HEIGHT; y++)
            {
                matrix_write(x, y, scroll_text.bg_color);
            }
        }
        return;
    }

    for(x = 0; x < MATRIX_WIDTH; x++)
    {
        uint16_t pixel_pos = (scroll_text.pos_x + x) % total_width;
        uint16_t char_index = pixel_pos / FONT_WIDTH;
        uint8_t  bit_index = pixel_pos % FONT_WIDTH;

        char ch = scroll_text.str[char_index];

        uint8_t data;

        if(ch >= 'A' && ch <= 'Z')
        {
            data = ucALPHABET[ch - 'A'][bit_index];
        }
        else
        {
            data = 0x00;  
        }
        
        for(y = 0; y < MATRIX_HEIGHT; y++)
        {
            if((data << upper_offset | data >> bottom_offset) & (1 << y))
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
#endif /* MATRIX_USE_FONT */

void matrix_flush(buff_t option)
{
    uint16_t *tmp = front;
    front = back;
    back  = tmp;

    switch(option)
    {
        case BUFF_KEEP:
            memmove(back, front, sizeof(buffer[0]));
            break;
        case BUFF_CLEAR:
            memset(back, 0x0000, sizeof(buffer[0]));
            break;
        default :
            // 描画バッファの状態は不定
            break;
    }
}

uint16_t matrix_get_data(uint8_t x)
{
    return front[x];
}

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


