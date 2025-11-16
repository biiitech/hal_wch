/**
 * @file rcc.h
 * @author Anuj Pathak (anuj@biii.in)
 * @brief
 * @version 0.1
 * @date 2025-11-16
 *
 * @copyright Copyright (c) 2025 BIII TECH LLP
 *
 */

#ifndef HAL_WCH_CH5XX_RCC_H_
#define HAL_WCH_CH5XX_RCC_H_

#include "ch5xx_sfr.h"

#ifdef __cplusplus
extern "C" {
#endif

enum ch5xx_rcc_src {
  CH5XX_RCC_SRC_HSE = 0x00,
  CH5XX_RCC_SRC_PLL = 0x01,
  CH5XX_RCC_SRC_LSI = 0x11,
};

static inline void
ch5xx_rcc_set_hse_tuning_param(uint8_t load_cap_pf,
                               uint8_t rated_current_percent) {
  /* adjust bits, loadcap = (2*bit + 6) */
  load_cap_pf -= 10;
  load_cap_pf /= 2;
  WRITE_REG_BITS_SAFE(R8_XT32M_TUNE, load_cap_pf, RB_XT32M_C_LOAD_MASK,
                      RB_XT32M_C_LOAD_POS);

  /* adjust bits, rated current = (25*bit + 75) */
  rated_current_percent -= 75;
  rated_current_percent /= 25;
  WRITE_REG_BITS_SAFE(R8_XT32M_TUNE, rated_current_percent,
                      RB_XT32M_I_BIAS_MASK, RB_XT32M_I_BIAS_POS);
}

static inline void ch5xx_rcc_set_lsi_enable(bool enable) {
  WRITE_REG_BIT_SAFE(R8_LSI_CONFIG, RB_CLK_INT32K_PON, enable);
}

static inline void ch5xx_rcc_set_pll_enable(bool enable) {
  WRITE_REG_BIT_SAFE(R8_HFCK_PWR_CTRL, RB_CLK_PLL_PON, enable);
}

static inline void ch5xx_rcc_set_hse_enable(bool enable) {
  WRITE_REG_BIT_SAFE(R8_HFCK_PWR_CTRL, RB_CLK_XT32M_PON, enable);
}

static inline void ch5xx_rcc_keep_hse_on_in_stop(bool keep_on) {
  WRITE_REG_BIT_SAFE(R8_HFCK_PWR_CTRL, RB_CLK_XT32M_KEEP, keep_on);
}

static inline void ch5xx_rcc_set_pll_divider(uint8_t div) {
  /* minimum div value should be 2 */
  if (div < 2) {
    div = 2;
  }
  /* maximum div value is 32, but we need to set 0 in register */
  if (div > 31) {
    div = 0;
  }
  WRITE_REG_BITS_SAFE(R8_CLK_SYS_CFG, div, RB_CLK_PLL_DIV_MASK,
                      RB_CLK_PLL_DIV_POS);
}

static inline void ch5xx_rcc_set_sys_clock_source(uint8_t src) {
  WRITE_REG_BITS_SAFE(R8_CLK_SYS_CFG, src, RB_CLK_SYS_MOD_MASK,
                      RB_CLK_SYS_MOD_POS);
}

#ifdef __cplusplus
}
#endif

#endif /* HAL_WCH_CH5XX_RCC_H_ */
