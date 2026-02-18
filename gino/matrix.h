/* 
 * e2417105
 */

#ifndef MATRIX_H
#define MATRIX_H

#include <stdint.h>

// #define MATRIX_USE_FONT

typedef enum {
    pixel_off,     // 0 ..消灯
    pixel_red,     // 1 ..赤
    pixel_green,   // 2 ..緑
    pixel_orange   // 3 ..橙
} pixel_t;

typedef enum {
    // フラッシュ後も描画バッファの内容を保持する
    BUFF_KEEP,
    // フラッシュ後に描画バッファの内容を破棄する
    BUFF_CLEAR
} buff_t;

void matrix_init(void);
void matrix_write(uint8_t x, uint8_t y, pixel_t c);
pixel_t matrix_read(uint8_t x, uint8_t y);
void matrix_clear(void);

// フォント機能有効時
#ifdef MATRIX_USE_FONT
typedef enum {
    SCROLL_NONE  = 0,         // 0x00 停止
    SCROLL_LEFT  = (1u << 0), // 0x01 左
    SCROLL_RIGHT = (1u << 1), // 0x02 右
    SCROLL_UP    = (1u << 2), // 0x04 上
    SCROLL_DOWN  = (1u << 3), // 0x08 下
} scroll_t;

void matrix_write_char(char ch, pixel_t fg, pixel_t bg);
void matrix_scroller_set_text(const char *text);
void matrix_scroller_set_foreground(pixel_t fg);
void matrix_scroller_set_background(pixel_t bg);
void matrix_scroller_scroll_pos(scroll_t dir);
void matrix_scroller_write_text(void);
#endif /* MATRIX_USE_FONT */

void matrix_flush(buff_t option);
uint16_t matrix_get_data(uint8_t x);
void matrix_out(uint8_t x, uint16_t data);

#endif /* MATRIX_H */
