/**
 * @file sys.h
 * @author Anuj Pathak (anuj@biii.in)
 * @brief
 * @version 0.1
 * @date 2025-11-17
 *
 * @copyright Copyright (c) 2025 BIII TECH LLP
 *
 */
#ifndef HAL_WCH_CH5XX_SYS_H_
#define HAL_WCH_CH5XX_SYS_H_

#include "ch5xx_sfr.h"

#ifdef __cplusplus
extern "C" {
#endif

static inline void ch5xx_set_debug_enable(bool enable) {
  WRITE_REG_BIT(R8_SLP_CLK_OFF2, RB_CLK_OFF_DEBUG, !enable);
  WRITE_REG_BIT(R16_PIN_ALTERNATE, RB_PIN_DEBUG_EN, enable);
}

#ifdef __cplusplus
}
#endif

#endif /* HAL_WCH_CH5XX_SYS_H_ */
