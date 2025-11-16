/**
 * @file uart.h
 * @author Anuj Pathak (anuj@biii.in)
 * @brief
 * @version 0.1
 * @date 2025-11-16
 *
 * @copyright Copyright (c) 2025 BIII TECH LLP
 *
 */
#ifndef HAL_WCH_CH5XX_UART_H_
#define HAL_WCH_CH5XX_UART_H_

#ifdef __cplusplus
extern "C" {
#endif

enum ch5xx_uart_stop_bits {
  CH5XX_UART_STOP_BITS_1,
  CH5XX_UART_STOP_BITS_2,
};

enum ch5xx_uart_parity {
  CH5XX_UART_PARITY_NONE,
  CH5XX_UART_PARITY_EVEN,
  CH5XX_UART_PARITY_ODD,
};

typedef struct {
  vu8 MCR;          // offset 0x00, RW, UART0 modem control
  vu8 IER;          // offset 0x01, RW, UART0 interrupt enable
  vu8 FCR;          // offset 0x02, RW, UART0 FIFO control
  vu8 LCR;          // offset 0x03, RW, UART0 line control
  vu8 IIR;          // offset 0x04, RO, UART0 interrupt identification
  vu8 LSR;          // offset 0x05, RO, UART0 line status
  vu8 RESERVED0[2]; // offset 0x06, 0x07, reserved
  vu8 DR;           // offset 0x08, RO, UART0 receiver buffer, receiving byte
  vu8 RESERVED1;    // offset 0x09, reserved
  vu8 RFC;          // offset 0x0A, RO, UART0 receiver FIFO count
  vu8 TFC;          // offset 0x0B, RO, UART0 transmitter FIFO count
  vu16 DLL;         // offset 0x0C 0x0D, RW, UART0 divisor latch LSB byte
  vu8 DIV; // offset 0x0E, RW, UART0 pre-divisor latch byte, only low 7 bit
} UART_TypeDef;

static inline bool ch5xx_uart_tx_fifo_full(UART_TypeDef *regs) {
  return (regs->TFC >= UART_FIFO_SIZE);
}

static inline bool ch5xx_uart_tx_fifo_ready(UART_TypeDef *regs) {
  return (regs->TFC < UART_FIFO_SIZE);
}

static inline bool ch5xx_uart_tx_fifo_empty(UART_TypeDef *regs) {
  return (regs->TFC == 0);
}

static inline void ch5xx_uart_tx_fifo_push(UART_TypeDef *regs, uint8_t data) {
  regs->DR = data;
}

static inline bool ch5xx_uart_rx_fifo_empty(UART_TypeDef *regs) {
  return (regs->RFC == 0);
}

static inline bool ch5xx_uart_rx_fifo_ready(UART_TypeDef *regs) {
  return (regs->RFC > 0);
}

static inline uint8_t ch5xx_uart_rx_fifo_pop(UART_TypeDef *regs) {
  return regs->DR;
}

static inline void ch5xx_uart_irq_set_tx_enable(UART_TypeDef *regs,
                                                bool enable) {

  WRITE_REG_BIT(regs->IER, RB_IER_THR_EMPTY, enable);
}

static inline void ch5xx_uart_irq_set_rx_enable(UART_TypeDef *regs,
                                                bool enable) {

  WRITE_REG_BIT(regs->IER, RB_IER_RECV_RDY, enable);
}

static inline void ch5xx_uart_irq_set_line_status_enable(UART_TypeDef *regs,
                                                         bool enable) {

  WRITE_REG_BIT(regs->IER, RB_IER_LINE_STAT, enable);
}

static inline bool ch5xx_uart_irq_is_pending(UART_TypeDef *regs) {
  return (regs->IIR & RB_IIR_NO_INT) == 0;
}

static inline bool ch5xx_uart_irq_is_enabled(UART_TypeDef *regs) {
  return (regs->IER & RB_IER_THR_EMPTY) || (regs->IER & RB_IER_RECV_RDY) ||
         (regs->IER & RB_IER_LINE_STAT);
}

static inline uint8_t ch5xx_uart_line_status_get(UART_TypeDef *regs) {
  return regs->LSR;
}

static inline bool ch5xx_uart_has_line_error(UART_TypeDef *regs) {
  return (regs->IIR & RB_IIR_INT_MASK) == RB_IIR_P0_LINE_ERR;
}

static inline bool ch5xx_uart_has_rx_timeout(UART_TypeDef *regs) {
  return (regs->IIR & RB_IIR_INT_MASK) == RB_IIR_P2_RX_TOUT;
}

static inline bool ch5xx_uart_line_status_has_parity_error(uint8_t status) {
  return (status & RB_LSR_PAR_ERR) != 0;
}

static inline bool ch5xx_uart_line_status_has_break_error(uint8_t status) {
  return (status & RB_LSR_BREAK_ERR) != 0;
}

static inline bool ch5xx_uart_line_status_has_framing_error(uint8_t status) {
  return (status & RB_LSR_FRAME_ERR) != 0;
}

static inline bool ch5xx_uart_line_status_has_overrun_error(uint8_t status) {
  return (status & RB_LSR_OVER_ERR) != 0;
}

static inline void ch5xx_uart_reset(UART_TypeDef *regs) {
  regs->IER = RB_IER_RESET;
}

static inline void ch5xx_uart_set_baud_rate(UART_TypeDef *regs, uint32_t fsys,
                                            uint32_t baud_rate) {
  /* Baud rate = Fsys * 2 / R8_UART_DIV / 16 / R16_UART_DL
   * So, R16_UART_DL = Fsys * 2 / R8_UART_DIV / 16 / Baud rate
   * if R8_UART_DIV = 1, then
   * R16_UART_DL = Fsys / 8 / Baud rate = (Fsys >> 3) / Baud rate
   */
  regs->DIV = 1;
  regs->DLL = (fsys >> 3) / baud_rate;
}

static inline void ch5xx_uart_set_fifo_enable(UART_TypeDef *regs, bool enable) {
  WRITE_REG_BIT(regs->FCR, RB_FCR_FIFO_EN, false);
  if (enable) {
    WRITE_REG_BIT(regs->FCR, RB_FCR_RX_FIFO_CLR, true);
    WRITE_REG_BIT(regs->FCR, RB_FCR_TX_FIFO_CLR, true);
    WRITE_REG_BIT(regs->FCR, RB_FCR_FIFO_EN, true);
  }
}

static inline void ch5xx_uart_set_fifo_rx_threshold(UART_TypeDef *regs,
                                                    uint8_t threshold) {
  switch (threshold) {
  case 1:
    WRITE_REG_BITS(regs->FCR, RB_FCR_TRIG_01, RB_FCR_FIFO_TRIG_MASK,
                   RB_FCR_FIFO_TRIG_POS);
    break;
  case 2:
    WRITE_REG_BITS(regs->FCR, RB_FCR_TRIG_02, RB_FCR_FIFO_TRIG_MASK,
                   RB_FCR_FIFO_TRIG_POS);
    break;
  case 4:
    WRITE_REG_BITS(regs->FCR, RB_FCR_TRIG_04, RB_FCR_FIFO_TRIG_MASK,
                   RB_FCR_FIFO_TRIG_POS);
    break;
  case 7:
    WRITE_REG_BITS(regs->FCR, RB_FCR_TRIG_07, RB_FCR_FIFO_TRIG_MASK,
                   RB_FCR_FIFO_TRIG_POS);
    break;
  default:
    break;
  }
}

static inline void ch5xx_uart_set_data_bits(UART_TypeDef *regs,
                                            uint8_t data_bits) {
  switch (data_bits) {
  case 5:
  case 6:
  case 7:
  case 8:
    WRITE_REG_BITS(regs->LCR, (data_bits - 5), RB_LCR_WORD_SZ_MASK,
                   RB_LCR_WORD_SZ_POS);
    break;
  default:
    break;
  }
}

static inline void ch5xx_uart_set_stop_bits(UART_TypeDef *regs,
                                            uint8_t stop_bits) {
  switch (stop_bits) {
  case 1:
    WRITE_REG_BIT(regs->LCR, RB_LCR_STOP_BIT, false);
    break;
  case 2:
    WRITE_REG_BIT(regs->LCR, RB_LCR_STOP_BIT, true);
    break;
  default:
    break;
  }
}

static inline void ch5xx_uart_set_parity(UART_TypeDef *regs, uint8_t parity) {
  switch (parity) {
  case CH5XX_UART_PARITY_NONE:
    WRITE_REG_BIT(regs->LCR, RB_LCR_PAR_EN, false);
    break;
  case CH5XX_UART_PARITY_EVEN:
    WRITE_REG_BIT(regs->LCR, RB_LCR_PAR_EN, true);
    WRITE_REG_BIT(regs->LCR, RB_LCR_PAR_MOD, RB_LCR_PAR_MOD_EVN);
    break;
  case CH5XX_UART_PARITY_ODD:
    WRITE_REG_BIT(regs->LCR, RB_LCR_PAR_EN, true);
    WRITE_REG_BIT(regs->LCR, RB_LCR_PAR_MOD, RB_LCR_PAR_MOD_ODD);
    break;
  default:
    break;
  }
}

static inline void ch5xx_uart_tx_enable(UART_TypeDef *regs, bool enable) {
  WRITE_REG_BIT(regs->IER, RB_IER_TXD_EN, enable);
}

static inline void ch5xx_uart_set_irq_enable(UART_TypeDef *regs, bool enable) {
  WRITE_REG_BIT(regs->MCR, RB_MCR_INT_OE, enable);
}

#ifdef __cplusplus
}
#endif

#endif /* HAL_WCH_CH5XX_UART_H_ */