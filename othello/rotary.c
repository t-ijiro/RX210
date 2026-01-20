#include <stdint.h>
#include "iodefine.h"
#include "rotary.h"

#define COUNT_PER_CLICK 4

void rotary_init(void)
{
    // プロテクトレジスタ解除
    SYSTEM.PRCR.WORD = 0x0A502;

    // MTU1モジュールストップ解除
    MSTP(MTU1) = 0;

    // プロテクトレジスタを再設定
    SYSTEM.PRCR.WORD = 0X0A500;

    // PORT2.4とPORT2.5を周辺機能として使用
    PORT2.PMR.BIT.B4 = 1;
    PORT2.PMR.BIT.B5 = 1;

    // ピン機能選択レジスタの書き込み保護解除
    MPC.PWPR.BIT.B0WI = 0;
    MPC.PWPR.BIT.PFSWE = 1;

    // PORT2.4とPORT2.5をMTU1の入力ピンとして設定（位相計数モード用）
    MPC.P24PFS.BIT.PSEL = 2;
    MPC.P25PFS.BIT.PSEL = 2;

    // ピン機能選択レジスタの書き込み保護再設定
    MPC.PWPR.BIT.PFSWE = 0;

    // 位相計数モード1に設定（2相エンコーダ入力）
    MTU1.TMDR.BIT.MD = 4;

    // カウンタ初期化
    MTU1.TCNT = 0;

    // MTU1カウント開始
    MTU.TSTR.BIT.CST1 = 1;
}

rotary_t rotary_get_instance(int16_t val_new, int16_t val_old)
{
    rotary_t r = {val_new, val_old};

    return r;
}

void rotary_record_new(rotary_t *r)
{
    r->count_new = MTU1.TCNT;
}

void rotary_record_old(rotary_t *r)
{
    r->count_old = r->count_new;
}

int16_t rotary_calc_delta(rotary_t *r)
{
    return r->count_new - r->count_old;
}

rotary_click_t rotary_get_click_dir(int16_t delta)
{
    if(COUNT_PER_CLICK <= delta)
    {
        return CLICK_LEFT;
    }
    else if(delta <= -COUNT_PER_CLICK)
    {
        return CLICK_RIGHT;
    }
    else
    {
        return CLICK_IDLE;
    }
}
