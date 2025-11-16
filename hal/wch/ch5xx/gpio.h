/**
 * @file gpio.h
 * @author Anuj Pathak (anuj@biii.in)
 * @brief
 * @version 0.1
 * @date 2025-11-16
 *
 * @copyright Copyright (c) 2025 BIII TECH LLP
 *
 */
#ifndef HAL_WCH_CH5XX_GPIO_H_
#define HAL_WCH_CH5XX_GPIO_H_

#include "ch5xx_sfr.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  vu32 DIR;
  vu32 IN;
  vu32 OUT;
  vu32 CLR;
  vu32 PU;
  vu32 PD_DRV;
  vu32 SET;
  vu32 RESERVED; // align to 0x20 bytes
} GPIO_PORT_TypeDef;

typedef struct {
  vu32 EN;
  vu32 MODE;
  vu32 EDGE;
  vu32 IF;
  GPIO_PORT_TypeDef PORT[2];
} GPIO_TypeDef;

#define GPIO ((GPIO_TypeDef *)0x40001090)
#define GPIOA (GPIO->PORT[0])
#ifndef CH570_CH572
#define GPIOB (GPIO->PORT[1])
#endif

static inline void ch5xx_gpio_cfg_out(GPIO_PORT_TypeDef *port, uint8_t pin) {
  WRITE_REG_BIT(port->DIR, BIT(pin), 1);
}

static inline void ch5xx_gpio_cfg_in(GPIO_PORT_TypeDef *port, uint8_t pin) {
  WRITE_REG_BIT(port->DIR, BIT(pin), 0);
}

static inline void ch5xx_gpio_set(GPIO_PORT_TypeDef *port, uint8_t pin) {
  WRITE_REG_BIT(port->SET, BIT(pin), 1);
}

static inline void ch5xx_gpio_clear(GPIO_PORT_TypeDef *port, uint8_t pin) {
  WRITE_REG_BIT(port->CLR, BIT(pin), 1);
}

static inline void ch5xx_gpio_cfg_bias(GPIO_PORT_TypeDef *port, uint8_t pin,
                                       bool pull_up, bool pull_down) {
  WRITE_REG_BIT(port->PU, BIT(pin), pull_up);
  WRITE_REG_BIT(port->PD_DRV, BIT(pin), pull_down);
}

static inline void ch5xx_gpio_set_high_drive(GPIO_PORT_TypeDef *port, uint8_t pin, bool high_drive) {
  WRITE_REG_BIT(port->PD_DRV, BIT(pin), high_drive);
}

static inline uint8_t ch5xx_gpio_read(GPIO_PORT_TypeDef *port) {
  return port->IN;
}

static inline void ch5xx_gpio_write_masked(GPIO_PORT_TypeDef *port,
                                           uint8_t mask, uint8_t value) {
  port->OUT = (port->OUT & ~mask) | (value & mask);
}

static inline void ch5xx_gpio_clear_masked(GPIO_PORT_TypeDef *port,
                                           uint8_t mask) {
  port->CLR = mask;
}

static inline void ch5xx_gpio_set_masked(GPIO_PORT_TypeDef *port,
                                         uint8_t mask) {
  port->SET = mask;
}

static inline void ch5xx_gpio_toggle_masked(GPIO_PORT_TypeDef *port,
                                            uint8_t mask) {
  port->OUT = (port->OUT ^ mask);
}

static inline uint16_t ch5xx_gpio_read_interrupt_flag(GPIO_PORT_TypeDef *port) {
  vu16 *if_reg = (vu16 *)&GPIO->IF;
  uint16_t if_value;
  if (port != &GPIOA) {
    if_reg++;
  }
  if_value = *if_reg; /*cache the value */
  *if_reg = 0xFFFF;   /* clear interrupt flag */
  return if_value;
}

static inline void ch5xx_gpio_set_irq_enable(GPIO_PORT_TypeDef *port,
                                             uint8_t pin, bool enable) {
  vu16 *en_reg = (vu16 *)&GPIO->EN;
  if (port != &GPIOA) {
    en_reg++;
  }
  WRITE_REG_BIT(*en_reg, BIT(pin), enable);
}

static inline void ch5xx_gpio_set_irq_on_level(GPIO_PORT_TypeDef *port,
                                               uint8_t pin, bool level) {
  vu16 *mode_reg = (vu16 *)&GPIO->MODE;
  if (port != &GPIOA) {
    mode_reg++;
  }
  WRITE_REG_BIT(*mode_reg, BIT(pin), false);
}

static inline void ch5xx_gpio_set_irq_on_edge(GPIO_PORT_TypeDef *port,
                                              uint8_t pin, bool edge) {
  vu16 *mode_reg = (vu16 *)&GPIO->MODE;
  if (port != &GPIOA) {
    mode_reg++;
  }
  WRITE_REG_BIT(*mode_reg, BIT(pin), true);
}

static inline void ch5xx_gpio_set_irq_edge_follow(GPIO_PORT_TypeDef *port,
                                                  uint8_t pin,
                                                  bool edge_follow) {
  vu16 *edge_reg = (vu16 *)&GPIO->EDGE;
  if (port != &GPIOA) {
    edge_reg++;
  }
  WRITE_REG_BIT(*edge_reg, BIT(pin), !edge_follow);
}

#ifdef __cplusplus
}
#endif

#endif /* HAL_WCH_CH5XX_GPIO_H_ */
