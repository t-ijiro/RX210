/* 
 * hardwareInit.c
 * e2417105
 */

#include <stdint.h>
#include <machine.h>
#include "iodefine.h"
#include "hardwareInit.h"

// クロック初期化関数
void init_CLK(void)
{
    uint32_t i;

    // プロテクトレジスタ解除（クロック関連レジスタへの書き込みを許可）
    SYSTEM.PRCR.WORD = 0xA50F;

    // 電圧監視回路を無効化
    SYSTEM.VRCR = 0x00;

    // サブクロック発振器を停止
    SYSTEM.SOSCCR.BIT.SOSTP = 1;
    while (SYSTEM.SOSCCR.BIT.SOSTP != 1)
        ;  // 停止完了待ち

    // RTCを無効化
    RTC.RCR3.BYTE = 0x0C;
    while (RTC.RCR3.BIT.RTCEN != 0)
        ;  // 無効化完了待ち

    // メインクロック発振器の設定
    SYSTEM.MOFCR.BYTE = 0x0D;

    // メインクロック発振安定待ち時間の設定（約262ms）
    SYSTEM.MOSCWTCR.BYTE = 0x0D;

    // メインクロック発振器を起動
    SYSTEM.MOSCCR.BIT.MOSTP = 0x00;
    while (0x00 != SYSTEM.MOSCCR.BIT.MOSTP)
        ;  // 起動完了待ち

    // 発振安定化のための待機
    for (i = 0; i < 100; i++)
        nop();

    // PLL設定（入力周波数を10分周、出力を10倍 → 結果的に等倍）
    SYSTEM.PLLCR.WORD = 0x0901;

    // PLL発振安定待ち時間の設定（約1.05ms）
    SYSTEM.PLLWTCR.BYTE = 0x09;

    // PLLを起動
    SYSTEM.PLLCR2.BYTE = 0x00;

    // PLL安定化のための待機
    for (i = 0; i < 100; i++)
        nop();

    // 高速オンチップオシレータを停止
    SYSTEM.OPCCR.BYTE = 0x00;
    while (0 != SYSTEM.OPCCR.BIT.OPCMTSF)
        ;  // 停止完了待ち

    // システムクロック分周比設定
    // ICLK: PLL等倍, PCLKA: 1/2, PCLKB: 1/4, PCLKC: 1/4, PCLKD: 1/2, BCLK: 1/2, FCLK: 1/4
    SYSTEM.SCKCR.LONG = 0x21821211;
    while (0x21821211 != SYSTEM.SCKCR.LONG)
        ;  // 設定完了待ち

    // システムクロックソースをPLLに切り替え
    SYSTEM.SCKCR3.WORD = 0x0400;
    while (0x0400 != SYSTEM.SCKCR3.WORD)
        ;  // 切り替え完了待ち

    // プロテクトレジスタを再設定（書き込み禁止）
    SYSTEM.PRCR.WORD = 0xA500;
}

// コンペアマッチタイマ0初期化関数
void init_CMT0(uint16_t period_ms, uint8_t is_start)
{
    // プロテクトレジスタ解除（モジュールストップ制御用）
    SYSTEM.PRCR.WORD = 0x0A502;

    // CMT0モジュールストップ解除（クロック供給開始）
    MSTP(CMT0) = 0;

    // プロテクトレジスタを再設定
    SYSTEM.PRCR.WORD = 0x0A500;

    // コンペアマッチ値設定
    CMT0.CMCOR = (25000 * period_ms) / 8 - 1;

    // コンペアマッチ割り込み有効、クロック分周比1/8
    CMT0.CMCR.WORD |= 0x00C0;

    // CMT0割り込み有効化
    IEN(CMT0, CMI0) = 1;

    // CMT0割り込み優先度設定（レベル1）
    IPR(CMT0, CMI0) = 1;

    // CMT0カウント開始
    CMT.CMSTR0.BIT.STR0 = is_start;
}

// コンペアマッチタイマ1初期化関数
void init_CMT1(uint16_t period_ms, uint8_t is_start)
{
    // プロテクトレジスタ解除
    SYSTEM.PRCR.WORD = 0x0A502;

    // CMT1モジュールストップ解除
    MSTP(CMT1) = 0;

    // プロテクトレジスタを再設定
    SYSTEM.PRCR.WORD = 0x0A500;

    // コンペアマッチ値設定
    CMT1.CMCOR = (25000 * period_ms) / 8 - 1;

    // コンペアマッチ割り込み有効、クロック分周比1/8
    CMT1.CMCR.WORD |= 0x00C0;

    // CMT1割り込み有効化
    IEN(CMT1, CMI1) = 1;

    // CMT1割り込み優先度設定（レベル1）
    IPR(CMT1, CMI1) = 1;

    // CMT1カウント開始
    CMT.CMSTR0.BIT.STR1 = is_start;
}

// コンペアマッチタイマ2初期化関数
void init_CMT2(uint16_t period_ms, uint8_t is_start)
{
    // プロテクトレジスタ解除
    SYSTEM.PRCR.WORD = 0x0A502;

    // CMT2モジュールストップ解除
    MSTP(CMT2) = 0;

    // プロテクトレジスタを再設定
    SYSTEM.PRCR.WORD = 0x0A500;

    // コンペアマッチ値設定
    CMT2.CMCOR = (25000 * period_ms) / 8 - 1;

    // コンペアマッチ割り込み有効、クロック分周比1/8
    CMT2.CMCR.WORD |= 0x00C0;

    // CMT2割り込み有効化
    IEN(CMT2, CMI2) = 1;

    // CMT2割り込み優先度設定（レベル1）
    IPR(CMT2, CMI2) = 1;

    // CMT2カウント開始
    CMT.CMSTR1.BIT.STR2 = is_start;
}

// コンペアマッチタイマ3初期化関数（約10msごとに割り込み）
void init_CMT3(uint16_t period_ms, uint8_t is_start)
{
    // プロテクトレジスタ解除
    SYSTEM.PRCR.WORD = 0x0A502;

    // CMT3モジュールストップ解除
    MSTP(CMT3) = 0;

    // プロテクトレジスタを再設定
    SYSTEM.PRCR.WORD = 0x0A500;

    // コンペアマッチ値設定（(25000*10)/8 - 1 = 31249、約10ms周期）
    CMT3.CMCOR = (25000 * period_ms) / 8 - 1;

    // コンペアマッチ割り込み有効、クロック分周比1/8
    CMT3.CMCR.WORD |= 0x00C0;

    // CMT3割り込み有効化
    IEN(CMT3, CMI3) = 1;

    // CMT3割り込み優先度設定（レベル1）
    IPR(CMT3, CMI3) = 1;

    // CMT3カウント開始
    CMT.CMSTR1.BIT.STR3 = is_start;
}

// 外部割り込み0初期化関数（PORTH1ピン）
void init_IRQ0(void)
{
    // IRQ0割り込み一時無効化
    IEN(ICU, IRQ0) = 0;

    // デジタルフィルタ一時無効化
    ICU.IRQFLTE0.BIT.FLTEN0 = 0;

    // デジタルフィルタのサンプリングクロック設定（PCLK/64）
    ICU.IRQFLTC0.BIT.FCLKSEL0 = 3;

    // PORTH1ピンを入力に設定
    PORTH.PDR.BIT.B1 = 0;

    // PORTH1ピンを周辺機能として使用
    PORTH.PMR.BIT.B1 = 1;

    // ピン機能選択レジスタの書き込み保護解除
    MPC.PWPR.BIT.B0WI = 0;
    MPC.PWPR.BIT.PFSWE = 1;

    // PORTH1をIRQ0入力ピンとして設定
    MPC.PH1PFS.BIT.ISEL = 1;

    // IRQ0を立ち下がりエッジで検出
    ICU.IRQCR[0].BIT.IRQMD = 1;

    // デジタルフィルタ有効化
    ICU.IRQFLTE0.BIT.FLTEN0 = 1;

    // 割り込みフラグクリア
    IR(ICU, IRQ0) = 0;

    // IRQ0割り込み有効化
    IEN(ICU, IRQ0) = 1;

    // IRQ0割り込み優先度設定（レベル1）
    IPR(ICU, IRQ0) = 1;
}

// 外部割り込み1初期化関数（PORTH2ピン）
void init_IRQ1(void)
{
    // IRQ1割り込み一時無効化
    IEN(ICU, IRQ1) = 0;

    // デジタルフィルタ一時無効化
    ICU.IRQFLTE0.BIT.FLTEN1 = 0;

    // デジタルフィルタのサンプリングクロック設定（PCLK/64）
    ICU.IRQFLTC0.BIT.FCLKSEL1 = 3;

    // PORTH2ピンを入力に設定
    PORTH.PDR.BIT.B2 = 0;

    // PORTH2ピンを周辺機能として使用
    PORTH.PMR.BIT.B2 = 1;

    // ピン機能選択レジスタの書き込み保護解除
    MPC.PWPR.BIT.B0WI = 0;
    MPC.PWPR.BIT.PFSWE = 1;

    // PORTH2をIRQ1入力ピンとして設定
    MPC.PH2PFS.BIT.ISEL = 1;

    // IRQ1を立ち下がりエッジで検出
    ICU.IRQCR[1].BIT.IRQMD  = 1;

    // デジタルフィルタ有効化
    ICU.IRQFLTE0.BIT.FLTEN1 = 1;

    // 割り込みフラグクリア
    IR(ICU, IRQ1) = 0;

    // IRQ1割り込み有効化
    IEN(ICU, IRQ1) = 1;

    // IRQ1割り込み優先度設定（レベル1）
    IPR(ICU, IRQ1) = 1;
}

