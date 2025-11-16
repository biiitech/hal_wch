/**
 * @file usb.h
 * @author Anuj Pathak (anuj@biii.in)
 * @brief CH5xx USB Full-Speed Device Controller HAL
 * @version 0.1
 * @date 2026-03-22
 *
 * @copyright Copyright (c) 2026 BIII TECH LLP
 *
 */
#ifndef HAL_WCH_CH5XX_USB_H_
#define HAL_WCH_CH5XX_USB_H_

#include "ch5xx_sfr.h"

#ifdef __cplusplus
extern "C" {
#endif

#define CH5XX_USB_EP_MPS       64
#define CH5XX_USB_NUM_EPS      8

/* EP control register layout: T_LEN at +0, reserved at +1, CTRL at +2, reserved at +3 */
typedef struct {
	vu8 T_LEN;
	vu8 RESERVED;
	vu8 CTRL;
	vu8 RESERVED1;
} USB_EP_CTRL_TypeDef;

/* USB register block: base 0x40008000, offsets 0x00-0x32 */
typedef struct {
	vu8 CTRL;              /* 0x00 R8_USB_CTRL */
	vu8 UDEV_CTRL;         /* 0x01 R8_UDEV_CTRL */
	vu8 INT_EN;            /* 0x02 R8_USB_INT_EN */
	vu8 DEV_AD;            /* 0x03 R8_USB_DEV_AD */
	vu8 RESERVED0;         /* 0x04 */
	vu8 MIS_ST;            /* 0x05 R8_USB_MIS_ST */
	vu8 INT_FG;            /* 0x06 R8_USB_INT_FG */
	vu8 INT_ST;            /* 0x07 R8_USB_INT_ST */
	vu8 RX_LEN;            /* 0x08 R8_USB_RX_LEN */
	vu8 RESERVED1[3];      /* 0x09-0x0B */
	vu8 UEP4_1_MOD;        /* 0x0C R8_UEP4_1_MOD */
	vu8 UEP2_3_MOD;        /* 0x0D R8_UEP2_3_MOD */
	vu8 UEP567_MOD;        /* 0x0E R8_UEP567_MOD */
	vu8 RESERVED2;         /* 0x0F */
	vu16 UEP0_DMA;         /* 0x10 */
	vu16 RESERVED3;        /* 0x12 */
	vu16 UEP1_DMA;         /* 0x14 */
	vu16 RESERVED4;        /* 0x16 */
	vu16 UEP2_DMA;         /* 0x18 */
	vu16 RESERVED5;        /* 0x1A */
	vu16 UEP3_DMA;         /* 0x1C */
	vu16 RESERVED6;        /* 0x1E */
	USB_EP_CTRL_TypeDef EP[5]; /* 0x20-0x33: EP0..EP4 ctrl */
} USB_TypeDef;

/* EP5-7 DMA + CTRL registers live at a non-contiguous offset.
 * Access via base pointer arithmetic rather than a second struct.
 *
 * EP5 DMA: base+0x54, EP6 DMA: base+0x58, EP7 DMA: base+0x5C
 * EP5 CTRL: base+0x64, EP6 CTRL: base+0x68, EP7 CTRL: base+0x6C
 * EPX_MODE: base+0x70
 */

/* ---------- Indexed register accessors ---------- */

static inline volatile uint8_t *ch5xx_usb_ep_ctrl_ptr(USB_TypeDef *usb, uint8_t ep)
{
	if (ep <= 4) {
		return &usb->EP[ep].CTRL;
	}
	/* EP5: base+0x66, EP6: base+0x6A, EP7: base+0x6E */
	return (volatile uint8_t *)((uintptr_t)usb + 0x66 + (ep - 5) * 4);
}

static inline volatile uint8_t *ch5xx_usb_ep_tlen_ptr(USB_TypeDef *usb, uint8_t ep)
{
	if (ep <= 4) {
		return &usb->EP[ep].T_LEN;
	}
	/* EP5: base+0x64, EP6: base+0x68, EP7: base+0x6C */
	return (volatile uint8_t *)((uintptr_t)usb + 0x64 + (ep - 5) * 4);
}

static inline volatile uint16_t *ch5xx_usb_ep_dma_ptr(USB_TypeDef *usb, uint8_t ep)
{
	/* EP0-3: contiguous at base+0x10 with 4-byte stride */
	if (ep <= 3) {
		return &usb->UEP0_DMA + (ep * 2); /* vu16 pointer, stride=2 vu16 = 4 bytes */
	}
	/* EP4 shares EP0 DMA (no separate DMA register) */
	if (ep == 4) {
		return &usb->UEP0_DMA;
	}
	/* EP5: base+0x54, EP6: base+0x58, EP7: base+0x5C */
	return (volatile uint16_t *)((uintptr_t)usb + 0x54 + (ep - 5) * 4);
}

/* ---------- Endpoint response control ---------- */

/* OUT (receive) response */
static inline void ch5xx_usb_ep_set_rx_res(USB_TypeDef *usb, uint8_t ep, uint8_t res)
{
	volatile uint8_t *ctrl = ch5xx_usb_ep_ctrl_ptr(usb, ep);
	*ctrl = (*ctrl & ~MASK_UEP_R_RES) | (res & MASK_UEP_R_RES);
}

/* IN (transmit) response */
static inline void ch5xx_usb_ep_set_tx_res(USB_TypeDef *usb, uint8_t ep, uint8_t res)
{
	volatile uint8_t *ctrl = ch5xx_usb_ep_ctrl_ptr(usb, ep);
	*ctrl = (*ctrl & ~MASK_UEP_T_RES) | (res & MASK_UEP_T_RES);
}

/* TX data length */
static inline void ch5xx_usb_ep_set_tx_len(USB_TypeDef *usb, uint8_t ep, uint8_t len)
{
	*ch5xx_usb_ep_tlen_ptr(usb, ep) = len;
}

/* Toggle bits */
static inline void ch5xx_usb_ep_set_tx_tog(USB_TypeDef *usb, uint8_t ep, bool data1)
{
	volatile uint8_t *ctrl = ch5xx_usb_ep_ctrl_ptr(usb, ep);
	if (data1) {
		*ctrl |= RB_UEP_T_TOG;
	} else {
		*ctrl &= ~RB_UEP_T_TOG;
	}
}

static inline void ch5xx_usb_ep_set_rx_tog(USB_TypeDef *usb, uint8_t ep, bool data1)
{
	volatile uint8_t *ctrl = ch5xx_usb_ep_ctrl_ptr(usb, ep);
	if (data1) {
		*ctrl |= RB_UEP_R_TOG;
	} else {
		*ctrl &= ~RB_UEP_R_TOG;
	}
}

static inline void ch5xx_usb_ep_set_auto_tog(USB_TypeDef *usb, uint8_t ep, bool enable)
{
	volatile uint8_t *ctrl = ch5xx_usb_ep_ctrl_ptr(usb, ep);
	if (enable) {
		*ctrl |= RB_UEP_AUTO_TOG;
	} else {
		*ctrl &= ~RB_UEP_AUTO_TOG;
	}
}

/* ---------- Endpoint mode (enable TX/RX) ---------- */

static inline void ch5xx_usb_ep_set_tx_en(USB_TypeDef *usb, uint8_t ep, bool enable)
{
	switch (ep) {
	case 1:
		WRITE_REG_BIT(usb->UEP4_1_MOD, RB_UEP1_TX_EN, enable);
		break;
	case 2:
		WRITE_REG_BIT(usb->UEP2_3_MOD, RB_UEP2_TX_EN, enable);
		break;
	case 3:
		WRITE_REG_BIT(usb->UEP2_3_MOD, RB_UEP3_TX_EN, enable);
		break;
	case 4:
		WRITE_REG_BIT(usb->UEP4_1_MOD, RB_UEP4_TX_EN, enable);
		break;
	case 5:
		WRITE_REG_BIT(usb->UEP567_MOD, RB_UEP5_TX_EN, enable);
		break;
	case 6:
		WRITE_REG_BIT(usb->UEP567_MOD, RB_UEP6_TX_EN, enable);
		break;
	case 7:
		WRITE_REG_BIT(usb->UEP567_MOD, RB_UEP7_TX_EN, enable);
		break;
	default:
		break;
	}
}

static inline void ch5xx_usb_ep_set_rx_en(USB_TypeDef *usb, uint8_t ep, bool enable)
{
	switch (ep) {
	case 1:
		WRITE_REG_BIT(usb->UEP4_1_MOD, RB_UEP1_RX_EN, enable);
		break;
	case 2:
		WRITE_REG_BIT(usb->UEP2_3_MOD, RB_UEP2_RX_EN, enable);
		break;
	case 3:
		WRITE_REG_BIT(usb->UEP2_3_MOD, RB_UEP3_RX_EN, enable);
		break;
	case 4:
		WRITE_REG_BIT(usb->UEP4_1_MOD, RB_UEP4_RX_EN, enable);
		break;
	case 5:
		WRITE_REG_BIT(usb->UEP567_MOD, RB_UEP5_RX_EN, enable);
		break;
	case 6:
		WRITE_REG_BIT(usb->UEP567_MOD, RB_UEP6_RX_EN, enable);
		break;
	case 7:
		WRITE_REG_BIT(usb->UEP567_MOD, RB_UEP7_RX_EN, enable);
		break;
	default:
		break;
	}
}

/* ---------- DMA buffer address ---------- */

static inline void ch5xx_usb_ep_set_dma(USB_TypeDef *usb, uint8_t ep, void *buf)
{
	/* Hardware uses lower 16 bits of SRAM address */
	*ch5xx_usb_ep_dma_ptr(usb, ep) = (uint16_t)((uint32_t)buf);
}

static inline uint8_t *ch5xx_usb_ep_get_dma_buf(USB_TypeDef *usb, uint8_t ep)
{
	uint16_t dma = *ch5xx_usb_ep_dma_ptr(usb, ep);
	return (uint8_t *)(0x20000000u + dma);
}

/* ---------- Interrupt helpers ---------- */

static inline uint8_t ch5xx_usb_get_int_fg(USB_TypeDef *usb)
{
	return usb->INT_FG;
}

static inline void ch5xx_usb_clear_int_fg(USB_TypeDef *usb, uint8_t flags)
{
	usb->INT_FG = flags;
}

static inline uint8_t ch5xx_usb_get_int_st(USB_TypeDef *usb)
{
	return usb->INT_ST;
}

static inline uint8_t ch5xx_usb_get_rx_len(USB_TypeDef *usb)
{
	return usb->RX_LEN;
}

/* Extract endpoint number from INT_ST (device mode) */
static inline uint8_t ch5xx_usb_int_st_ep(uint8_t int_st)
{
	return int_st & MASK_UIS_ENDP;
}

/* Extract token type from INT_ST (device mode) */
static inline uint8_t ch5xx_usb_int_st_token(uint8_t int_st)
{
	return int_st & MASK_UIS_TOKEN;
}

static inline bool ch5xx_usb_int_st_is_setup(uint8_t int_st)
{
	return (int_st & RB_UIS_SETUP_ACT) != 0;
}

static inline bool ch5xx_usb_tog_ok(USB_TypeDef *usb)
{
	return (usb->INT_FG & RB_U_TOG_OK) != 0;
}

/* ---------- Device control ---------- */

static inline void ch5xx_usb_set_addr(USB_TypeDef *usb, uint8_t addr)
{
	usb->DEV_AD = (usb->DEV_AD & ~MASK_USB_ADDR) | (addr & MASK_USB_ADDR);
}

static inline void ch5xx_usb_reset_sie(USB_TypeDef *usb)
{
	usb->CTRL |= RB_UC_RESET_SIE;
	usb->CTRL &= ~RB_UC_RESET_SIE;
}

static inline void ch5xx_usb_clr_all(USB_TypeDef *usb)
{
	usb->CTRL |= RB_UC_CLR_ALL;
	usb->CTRL &= ~RB_UC_CLR_ALL;
}

static inline void ch5xx_usb_dev_enable(USB_TypeDef *usb, bool pullup)
{
	/* Device mode, DMA enabled, auto-busy NAK */
	usb->CTRL = RB_UC_DMA_EN | RB_UC_INT_BUSY |
		     (pullup ? RB_UC_DEV_PU_EN | RB_UC_SYS_CTRL0 : 0);
	/* Enable physical port, disable pulldown */
	usb->UDEV_CTRL = RB_UD_PD_DIS | RB_UD_PORT_EN;
}

static inline void ch5xx_usb_dev_pullup(USB_TypeDef *usb, bool enable)
{
	if (enable) {
		usb->CTRL |= RB_UC_DEV_PU_EN | RB_UC_SYS_CTRL0;
	} else {
		usb->CTRL &= ~(RB_UC_DEV_PU_EN | RB_UC_SYS_CTRL0);
	}
}

static inline void ch5xx_usb_set_int_en(USB_TypeDef *usb, uint8_t mask)
{
	usb->INT_EN = mask;
}

#ifdef __cplusplus
}
#endif

#endif /* HAL_WCH_CH5XX_USB_H_ */
