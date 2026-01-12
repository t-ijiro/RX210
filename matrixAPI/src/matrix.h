// matrix.h
// Created on : 2025/12/13
// Author : T.Ijiro

#ifndef MATRIX_H
#define MATRIX_H

#include <stdint.h>

// *******************************************************
// ビルド設定
// *******************************************************
//
// フォント機能を有効にする場合はコメントアウトを外す
// #define MATRIX_USE_FONT 
//
// *******************************************************
// 行列番号割り振り
// *******************************************************
// 
// y行 ________________
//  7 |_|_|_|_|_|_|_|_|
//  6 |_|_|_|_|_|_|_|_|
//  5 |_|_|_|_|_|_|_|_|
//  4 |_|_|_|_|_|_|_|_|
//  3 |_|_|_|_|_|_|_|_|
//  2 |_|_|_|_|_|_|_|_|
//  1 |_|_|_|_|_|_|_|_|
//  0 |_|_|_|_|_|_|_|_|
//     0 1 2 3 4 5 6 7 x列
//
// *******************************************************
// *******************************************************

// LEDの色の型
typedef enum {
    pixel_off,     // 0 ..消灯
    pixel_red,     // 1 ..赤
    pixel_green,   // 2 ..緑
    pixel_orange   // 3 ..橙
} pixel_t;

// フラッシュ時の描画バッファ処理用の型
typedef enum {
    // フラッシュ後も描画バッファの内容を保持する
    BUFF_KEEP,
    // フラッシュ後に描画バッファの内容を破棄する
    BUFF_CLEAR
} buff_t;

// 入出力初期化
void matrix_init(void);

// 描画バッファの指定座標に色を書き込む
void matrix_write(uint8_t x, uint8_t y, pixel_t c);

// 描画バッファの指定座標の色を読み込む
pixel_t matrix_read(uint8_t x, uint8_t y);

// 描画バッファ全消去 
void matrix_clear(void);

// フォント機能有効時
#ifdef MATRIX_USE_FONT
// １文字を描画バッファに書き込む
// ch: 描画する文字 (A-Zのみ対応)
void matrix_put_char(char ch, pixel_t fg, pixel_t bg);

// スクロール文字列を設定
// text: 表示する文字列 (A-Zのみ対応)
// 最大文字数は32文字まで
void matrix_scroller_set_text(const char *text);

// スクロール文字列の前景色を設定
void matrix_scroller_set_foreground(pixel_t fg);

// スクロール文字列の背景色を設定
void matrix_scroller_set_background(pixel_t bg);

// スクロール文字列の位置を指定した方向に１つずらす
// 左：'l'  右：'r'  上：'u'  下：'d'
void matrix_scroller_scroll_pos(char dir);

// 描画バッファにスクロール文字列を書き込む
void matrix_scroller_write_text(void);
#endif /* MATRIX_USE_FONT */

// 描画バッファと表示バッファを入れ替える
// option = BUFF_KEEP  で描画バッファ内容を保持
// option = BUFF_CLEAR で描画バッファ内容を破棄
void matrix_flush(buff_t option);

// 指定列の表示バッファからマトリックスLED送信用16bitデータを取得
uint16_t matrix_get_data(uint8_t x);

// 16bitデータをマトリックスLEDの指定列に出力
// uint16_t dataのbit配置:
// bit[15:8] : 赤LEDの点灯パターン
// bit[7:0]  : 緑LEDの点灯パターン
// 例 : 0xAF5F = 0b1010111101011111
// 意味 : 上から順に赤緑赤緑橙橙橙橙で点灯
void matrix_out(uint8_t x, uint16_t data);

#endif /* MATRIX_H */