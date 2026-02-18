#ifndef ROTARY_H
#define ROTARY_H

#include <stdint.h>

/* 
 * ///////////// サンプルコード/////////////
 *
 * // -----------------------------------
 * // 初期化
 * // -----------------------------------
 *
 * // ハードウェア初期化
 * rotary_init();
 *
 * // rotary構造体インスタンス取得
 * rotary_t rotary = rotary_get_instance(0, 0);
 *
 * // -----------------------------------
 * // メイン処理
 * // -----------------------------------
 *
 * // 現在のカウント数を記録
 * rotary_record_new(&rotary);
 * 
 * // 過去と現在のカウント数の差を計算
 * int16_t delta = rotary_calc_delta(&rotary);
 * 
 * // 計算結果をもとに左右どちらにクリックしたかを取得
 * rotary_click_t dir = rotary_get_click_dir(delta);
 * 
 * // 右方向にクリックした場合の処理
 * if(dir == CLICK_RIGHT)
 * {
 *      // 右回りした際の処理 //
 *      
 *      // 過去のカウント数を記録
 *      rotary_record_old(&rotary);
 * }
 * // 左方向にクリックした場合の処理
 * else if(dir == CLICK_LEFT)
 * {
 *      // 左回りした際の処理 //
 *
 *      // 過去のカウント数を記録
 *      rotary_record_old(&rotary);
 * }
 * 
 */

typedef struct{
    int16_t count_new;
    int16_t count_old;
}rotary_t;

typedef enum{
    CLICK_IDLE,
    CLICK_LEFT,
    CLICK_RIGHT
}rotary_click_t;

void rotary_init(void);

void rotary_clear(rotary_t *r);

rotary_t rotary_get_instance(int16_t val_new, int16_t val_old);

void rotary_record_new(rotary_t *r);

void rotary_record_old(rotary_t *r);

int16_t rotary_calc_delta(rotary_t *r);

rotary_click_t rotary_get_click_dir(int16_t delta);

#endif /* ROTARY_H */
