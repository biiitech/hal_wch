/**
 * @file afio.h
 * @author Anuj Pathak (anuj@biii.in)
 * @brief
 * @version 0.1
 * @date 2025-11-16
 *
 * @copyright Copyright (c) 2025 BIII TECH LLP
 *
 */
#ifndef HAL_WCH_CH5XX_AFIO_H_
#define HAL_WCH_CH5XX_AFIO_H_

#include "ch5xx_sfr.h"

#ifdef __cplusplus
extern "C" {
#endif

static inline void ch5xx_afio_remap(uint8_t remap, uint8_t mask, uint8_t pos) {
  WRITE_REG_BITS(R32_PIN_CONFIG, remap, mask, pos);
}

#ifdef __cplusplus
}
#endif

#endif /* HAL_WCH_CH5XX_AFIO_H_ */
