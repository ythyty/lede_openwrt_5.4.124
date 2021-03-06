#ifndef _NAS782X_PINMUX_H
#define _NAS782X_PINMUX_H

#include <asm/arch/cpu.h>

#define PINMUX_GPIO		0
#define PINMUX_2		1
#define PINMUX_3		2
#define PINMUX_4		3
#define PINMUX_DEBUG		4
#define PINMUX_ALT		5

#define PINMUX_BANK_MFA		0
#define PINMUX_BANK_MFB		1

/* System control multi-function pin function selection */
#define PINMUX_SECONDARY_SEL		0x14
#define PINMUX_TERTIARY_SEL		0x8c
#define PINMUX_QUATERNARY_SEL		0x94
#define PINMUX_DEBUG_SEL		0x9c
#define PINMUX_ALTERNATIVE_SEL		0xa4
#define PINMUX_PULLUP_SEL		0xac

#define PINMUX_UARTA_SIN		PINMUX_ALT
#define PINMUX_UARTA_SOUT		PINMUX_ALT

#define PINMUX_STATIC_DATA0		PINMUX_2
#define PINMUX_STATIC_DATA1		PINMUX_2
#define PINMUX_STATIC_DATA2		PINMUX_2
#define PINMUX_STATIC_DATA3		PINMUX_2
#define PINMUX_STATIC_DATA4		PINMUX_2
#define PINMUX_STATIC_DATA5		PINMUX_2
#define PINMUX_STATIC_DATA6		PINMUX_2
#define PINMUX_STATIC_DATA7		PINMUX_2
#define PINMUX_STATIC_NWE		PINMUX_2
#define PINMUX_STATIC_NOE		PINMUX_2
#define PINMUX_STATIC_NCS		PINMUX_2
#define PINMUX_STATIC_ADDR18		PINMUX_2
#define PINMUX_STATIC_ADDR19		PINMUX_2

#define PINMUX_MACA_MDC			PINMUX_2
#define PINMUX_MACA_MDIO		PINMUX_2

extern void pinmux_set(int bank, int pin, int func);

#endif /* _NAS782X_PINMUX_H */
