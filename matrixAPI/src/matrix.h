// matrix.h
// Created on : 2025/12/13
// Author : T.Ijiro

#ifndef MATRIX_H
#define MATRIX_H

#include <stdint.h>
#include <stdbool.h>
#include "matrix_config.h"

// 横ドット数
#define MATRIX_WIDTH  8
// 縦ドット数
#define MATRIX_HEIGHT 8 

// uint8_tを色を扱う時の型として定義
typedef uint8_t pixel_color_t;

// LEDの色
enum {
    pixel_off,     // 0 ..消灯
    pixel_red,     // 1 ..赤
    pixel_green,   // 2 ..緑
    pixel_orange   // 3 ..橙
};

#if MATRIX_USE_FONT
// １文字を描画バッファに書き込む
// ch: 描画する文字 (A-Zのみ対応)
void matrix_put_char(const char ch, const pixel_color_t fg, const pixel_color_t bg);

// スクロール文字列を設定
// text: 表示する文字列 (A-Zのみ対応)
// 最大文字数は32文字まで
void matrix_set_scroll_text(const char *text);

// スクロール文字列の前景色・背景色を設定
void matrix_set_scroll_colors(const pixel_color_t fg, const pixel_color_t bg);

// スクロール文字列を指定した方向に１つずらす
// 左：'l'  右：'r'
void matrix_scroll_text(const char dir);
#endif /* MATRIX_USE_FONT */

// 入出力初期化
void matrix_init(void);

// 描画バッファの指定座標に色を書き込む
void matrix_write(const uint8_t x, const uint8_t y, const pixel_color_t c);

// 描画バッファの指定座標の色を読み込む
pixel_color_t matrix_read(const uint8_t x, const uint8_t y);

// 描画バッファ全消去 
void matrix_clear(void);

// 描画バッファを外部バッファにコピー
void matrix_copy(uint16_t dst[MATRIX_WIDTH]);

// 描画バッファに外部バッファを貼り付け
void matrix_paste(const uint16_t src[MATRIX_WIDTH]);

// 描画バッファと表示バッファを入れ替える
// inherit = true で描画バッファを引き継ぎ可能
void matrix_flush(const bool inherit);

// 指定列のマトリックスLED送信用16bitデータを取得
uint16_t matrix_get_data(const uint8_t x);

// 16bitデータをマトリックスLEDの指定列に出力
// data のビット配置:
// bit[15:8] : 指定列の赤LEDの点灯パターン
// bit[7:0]  : 指定列の緑LEDの点灯パターン
// 例 : 10101111 01011111 = 
//        赤
//        緑
//        赤
//        緑
//        橙
//        橙
//        橙
//        橙
void matrix_out(const uint8_t x, const uint16_t data);

#endif /* MATRIX_H */
