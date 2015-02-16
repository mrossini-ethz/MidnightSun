#ifndef __MACROS_H__
#define __MACROS_H__

#include <avr/io.h>

/* --- Preprocessor magic --- */
#define PP_NARG(...)						PP_NARG_N(__VA_ARGS__, 8, 7, 6, 5, 4, 3, 2, 1, 0)
#define PP_NARG_N(_1, _2, _3, _4, _5, _6, _7, _8, N, ...)	N

#define CAT(a, b)		PRIMITIVE_CAT(a, b)
#define PRIMITIVE_CAT(a, b)	a ## b
#define EXPAND(...)		__VA_ARGS__

/* --- Generic bit manipulation --- */
#define SHIFT_FLAG(x)		(1 << (x))
#define FLAG_OR(...)		(CAT(FLAG_OR, PP_NARG(__VA_ARGS__))(__VA_ARGS__))
#define FLAG_OR8(x, ...)	SHIFT_FLAG(x) | FLAG_OR7(__VA_ARGS__)
#define FLAG_OR7(x, ...)	SHIFT_FLAG(x) | FLAG_OR6(__VA_ARGS__)
#define FLAG_OR6(x, ...)	SHIFT_FLAG(x) | FLAG_OR5(__VA_ARGS__)
#define FLAG_OR5(x, ...)	SHIFT_FLAG(x) | FLAG_OR4(__VA_ARGS__)
#define FLAG_OR4(x, ...)	SHIFT_FLAG(x) | FLAG_OR3(__VA_ARGS__)
#define FLAG_OR3(x, ...)	SHIFT_FLAG(x) | FLAG_OR2(__VA_ARGS__)
#define FLAG_OR2(x, ...)	SHIFT_FLAG(x) | FLAG_OR1(__VA_ARGS__)
#define FLAG_OR1(x, ...)	SHIFT_FLAG(x)
#define SET_FLAGS(reg, ...)	(reg |= FLAG_OR(__VA_ARGS__))
#define CLR_FLAGS(reg, ...)	(reg &= ~FLAG_OR(__VA_ARGS__))
#define SET_BITS(reg, bits)	(reg |= (bits))
#define CLR_BITS(reg, bits)	(reg &= ~(bits))
#define TGL_BITS(reg, bits)	(reg ^= (bits))
#define SET_BIT(reg, bit)	SET_BITS(reg, SHIFT_FLAG(bit))
#define CLR_BIT(reg, bit)	CLR_BITS(reg, SHIFT_FLAG(bit))
#define TGL_BIT(reg, bit)	TGL_BITS(reg, SHIFT_FLAG(bit))
#define GET_BIT(reg, bit)	(!!((reg) & SHIFT_FLAG(bit)))
#define GET_FLAG(reg, bit)	(!!((reg) & SHIFT_FLAG(bit)))

/* --- General purpose I/O pins --- */
/* Internal */
#define _GPIO_A0		A, 0
#define _GPIO_A1		A, 1
#define _GPIO_A2		A, 2
#define _GPIO_A3		A, 3
#define _GPIO_A4		A, 4
#define _GPIO_A5		A, 5
#define _GPIO_A6		A, 6
#define _GPIO_A7		A, 7
#define _GPIO_B0		B, 0
#define _GPIO_B1		B, 1
#define _GPIO_B2		B, 2
#define _GPIO_B3		B, 3
#define _GPIO_B4		B, 4
#define _GPIO_B5		B, 5
#define _GPIO_B6		B, 6
#define _GPIO_B7		B, 7
#define _GPIO_C0		C, 0
#define _GPIO_C1		C, 1
#define _GPIO_C2		C, 2
#define _GPIO_C3		C, 3
#define _GPIO_C4		C, 4
#define _GPIO_C5		C, 5
#define _GPIO_C6		C, 6
#define _GPIO_C7		C, 7
#define _GPIO_D0		D, 0
#define _GPIO_D1		D, 1
#define _GPIO_D2		D, 2
#define _GPIO_D3		D, 3
#define _GPIO_D4		D, 4
#define _GPIO_D5		D, 5
#define _GPIO_D6		D, 6
#define _GPIO_D7		D, 7
#define _GPIO_E0		E, 0
#define _GPIO_E1		E, 1
#define _GPIO_E2		E, 2
#define _GPIO_E3		E, 3
#define _GPIO_E4		E, 4
#define _GPIO_E5		E, 5
#define _GPIO_E6		E, 6
#define _GPIO_E7		E, 7
#define _GPIO_F0		F, 0
#define _GPIO_F1		F, 1
#define _GPIO_F2		F, 2
#define _GPIO_F3		F, 3
#define _GPIO_F4		F, 4
#define _GPIO_F5		F, 5
#define _GPIO_F6		F, 6
#define _GPIO_F7		F, 7
#define ___GPIO_PORT(port, pin)	PORT ## port
#define __GPIO_PORT(id)		___GPIO_PORT(id)
#define _GPIO_PORT(id)		__GPIO_PORT(CAT(_GPIO_, id))
#define ___GPIO_PIN(port, pin)	PIN ## port
#define __GPIO_PIN(id)		___GPIO_PIN(id)
#define _GPIO_PIN(id)		__GPIO_PIN(CAT(_GPIO_, id))
#define ___GPIO_DDR(port, pin)	DDR ## port
#define __GPIO_DDR(id)		___GPIO_DDR(id)
#define _GPIO_DDR(id)		__GPIO_DDR(CAT(_GPIO_, id))
#define _GPIO_PAIR(reg, id)	CAT(_GPIO_, reg)(id), P ## id
#define __GPIO_SET(pair)	SET_BIT(pair)
#define __GPIO_CLR(pair)	CLR_BIT(pair)
#define __GPIO_TGL(pair)	TGL_BIT(pair)
#define __GPIO_GET(pair)	GET_BIT(pair)
#define _GPIO_SET(reg, id)	__GPIO_SET(_GPIO_PAIR(reg, id))
#define _GPIO_CLR(reg, id)	__GPIO_CLR(_GPIO_PAIR(reg, id))
#define _GPIO_TGL(reg, id)	__GPIO_TGL(_GPIO_PAIR(reg, id))
#define _GPIO_GET(reg, id)	__GPIO_GET(_GPIO_PAIR(reg, id))
/* External */
#define GPIO_SET_OUTPUT(id)	_GPIO_SET(DDR, id)
#define GPIO_SET_INPUT(id)	_GPIO_CLR(DDR, id)
#define GPIO_SET_PULLUP(id)	_GPIO_SET(PORT, id)
#define GPIO_TOGGLE(id)		_GPIO_TGL(PORT, id)
#define GPIO_SET(id)		_GPIO_SET(PORT, id)
#define GPIO_CLR(id)		_GPIO_CLR(PORT, id)
#define GPIO_TGL(id)		_GPIO_TGL(PORT, id)
#define GPIO_H(id)		_GPIO_SET(PORT, id)
#define GPIO_L(id)		_GPIO_CLR(PORT, id)
#define GPIO_Z(id)		_GPIO_SET(PORT, id)
#define GPIO_P(id)		_GPIO_SET(PORT, id)
#define GPIO_T(id)		_GPIO_TGL(PORT, id)
#define GPIO_1(id)		_GPIO_SET(PORT, id)
#define GPIO_0(id)		_GPIO_CLR(PORT, id)
#define GPIO(id)		_GPIO_GET(PIN, id)

#endif
