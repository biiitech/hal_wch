/**
 * @file spi.h
 * @author Anuj Pathak (anuj@biii.in)
 * @brief
 * @version 0.1
 * @date 2025-11-16
 *
 * @copyright Copyright (c) 2025 BIII TECH LLP
 *
 */

#ifndef HAL_WCH_CH5XX_SPI_H_
#define HAL_WCH_CH5XX_SPI_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "ch5xx_sfr.h"

typedef struct {
  vu8 CTRL_MOD; // 0x00
  vu8 CTRL_CFG; // 0x01
  vu8 INTER_EN; // 0x02
  union {
    vu8 CLOCK_DIV; // 0x03
    vu8 SLAVE_PRE; // 0x03
  };
  vu8 BUFFER;      // 0x04
  vu8 RUN_FLAG;    // 0x05
  vu8 INT_FLAG;    // 0x06
  vu8 FIFO_COUNT;  // 0x07
  vu8 INT_TYPE;    // 0x08
  vu8 INTER1_EN;   // 0x09
  vu8 INT1_FLAG;   // 0x0A
  vu8 RESERVED0;   // 0x0B
  vu16 TOTAL_CNT;  // 0x0C
  vu16 RESERVED1;  // 0x0E
  vu8 FIFO;        // 0x10
  vu8 RESERVED2;   // 0x11
  vu8 RESERVED3;   // 0x12
  vu8 FIFO_COUNT1; // 0x13
  vu32 DMA_NOW;    // 0x14
  vu32 DMA_BEG;    // 0x18
  vu32 DMA_END;    // 0x1C
} SPI_TypeDef;

static inline void ch5xx_spi_set_clock_frequency(SPI_TypeDef *regs,
                                                 uint32_t fsys,
                                                 uint32_t frequency) {
  uint8_t div = fsys / frequency;
  if (div < 2) {
    div = 2;
  }
  if (div > 254) {
    div = 254;
  }
  regs->CLOCK_DIV = div;
}

static inline void ch5xx_spi_set_master_mode(SPI_TypeDef *regs, bool master) {
  WRITE_REG_BIT(regs->CTRL_MOD, RB_SPI_MODE_SLAVE, !master);
  WRITE_REG_BIT(regs->CTRL_CFG, RB_SPI_AUTO_IF, true);
}

static inline void ch5xx_spi_set_mode(SPI_TypeDef *regs, bool cpol, bool cpha) {
  WRITE_REG_BIT(regs->CTRL_MOD, RB_SPI_MST_SCK_MOD, cpol);
  WRITE_REG_BIT(regs->CTRL_CFG, RB_MST_CLK_SEL, cpha); /* reverse polarity */
}

static inline void ch5xx_spi_set_fifo_dir(SPI_TypeDef *regs, bool rx) {
  WRITE_REG_BIT(regs->CTRL_MOD, RB_SPI_FIFO_DIR, rx);
}

static inline void ch5xx_spi_set_pin_mode(SPI_TypeDef *regs, bool mosi,
                                          bool miso, bool sck) {
  WRITE_REG_BIT(regs->CTRL_MOD, RB_SPI_MOSI_OE, mosi);
  WRITE_REG_BIT(regs->CTRL_MOD, RB_SPI_MISO_OE, miso);
  WRITE_REG_BIT(regs->CTRL_MOD, RB_SPI_SCK_OE, sck);
}

static inline void ch5xx_spi_cfg_dma(SPI_TypeDef *regs, uint8_t *buf,
                                     uint32_t len, bool rx) {
  ch5xx_spi_set_fifo_dir(regs, rx);
  regs->DMA_BEG = (uint32_t)buf;
  regs->DMA_END = (uint32_t)(buf + len);
  regs->TOTAL_CNT = len;
}

static inline void ch5xx_spi_start_dma(SPI_TypeDef *regs, bool interrupt) {
  WRITE_REG_BIT(regs->INTER_EN, RB_SPI_IE_DMA_END, interrupt);
  WRITE_REG_BIT(regs->INTER_EN, RB_SPI_IE_CNT_END, interrupt);
  WRITE_REG_BIT(regs->CTRL_CFG, RB_SPI_DMA_ENABLE, true);
}

static inline void ch5xx_spi_stop_dma(SPI_TypeDef *regs) {
  WRITE_REG_BIT(regs->INTER_EN, RB_SPI_IE_DMA_END, false);
  WRITE_REG_BIT(regs->INTER_EN, RB_SPI_IE_CNT_END, false);
  WRITE_REG_BIT(regs->CTRL_CFG, RB_SPI_DMA_ENABLE, false);
}

static inline uint8_t ch5xx_spi_get_int_flag(SPI_TypeDef *regs) {
  return regs->INT_FLAG;
}

static inline bool ch5xx_spi_int_flag_has_dma_end(uint8_t flags) {
  return flags & RB_SPI_IF_DMA_END;
}

static inline bool ch5xx_spi_int_flag_has_cnt_end(uint8_t flags) {
  return flags & RB_SPI_IF_CNT_END;
}

static inline void ch5xx_spi_clear_all(SPI_TypeDef *regs) {
  /* clear fifo/counter/interrupt flags*/
  WRITE_REG_BIT(regs->CTRL_MOD, RB_SPI_ALL_CLEAR, true);
}

#ifdef __cplusplus
}
#endif

#endif /* HAL_WCH_CH5XX_SPI_H_ */