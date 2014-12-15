/**
	\file
	\ingroup Lookup
	\brief Declaration of functions related to lookup tables

	\addtogroup Lookup Lookup tables
	\brief Pre-calculated solutions of complex mathematical computations

	Lookup tables make the computation of complex mathematical formulas fast and efficient.
	On embedded systems computing power is very limited:
	The CPU clock is slow, divisions and floating point operations have to be done in software,
	and standard functions (square root, logarithm, etc.) are extremely slow.

	Lookup tables present a very convenient solution to this problem.
	Instead of calculating the solution when needed, a table with every solution of the
	calculation is stored in memory.
	Then the result needs only to be looked up which can be done in very few clock cycles.

	The speed of the lookup comes at a cost:
	To store every possible solution of an equation requires a lot of memory
	which is also very limited on a microcontroller.

	As a compromise the table can be saved not for all values but for a few.
	If another value than the ones stored is needed then interpolation between
	the existing values is possible.
	This requires a defined step size between values for which the converted result is stored.

	Interpolation presents another problem:
	One has to divide by the step size to obtain the result.
	Since integer division is a slow process for microcontroller that have no hardware divider
	one can use a trick.
	If the step size is a power of two then the integer division is equivalent to a bit shift
	operation to the right.
	This is again very fast.

	There are two types of lookup tables supported, both of which use 16 bit signed integers:
	- linear step size
	- logarithmic step size

	Linear step size means that results are stored for values that have a constant step size.
	Example with step size 8 (power of 2!):
	Table index	| 0	| 1	| 2	| 3	| 4	| 5	| 6	| 7
	--------------- | ----- | ----- | ----- | ----- | ----- | ----- | ----- | -----
	Value		| -30	| -22	| -14	| -6	| 2	| 10	| 18	| 26
	Result		| 391	| 553	| 716	| 878	| 1041	| 1203	| 1365	| 1528

	This table could be stored like this:

		int16_t lookup_table [] = {
			391,	// f(-30)
			553,	// f(-22)
			716,	// f(-14)
			878,	// f(-6)
			1041,	// f(+2)
			1203,	// f(+10)
			1365,	// f(+18)
			1528	// f(+26)
		};

	The table with logarithmic step size looks like this:
	Table index	| 0	| 1	| 2	| 3	| 4	| 5	| 6	| 7
	--------------- | ----- | ----- | ----- | ----- | ----- | ----- | ----- | -----
	Value		| 1	| 2	| 4	| 8	| 16	| 32	| 64	| 128
	Result		| 391	| 553	| 716	| 878	| 1041	| 1203	| 1365	| 1528

	The table would be defined in the same way as for the linear table:

		int16_t lookup_table [] = {
			391,	// f(1)
			553,	// f(2)
			716,	// f(4)
			878,	// f(8)
			1041,	// f(16)
			1203,	// f(32)
			1365,	// f(64)
			1528	// f(128)
		};

	\{
 */

#ifndef __LOOKUP_H__
#define __LOOKUP_H__

#include <stdint.h>

/* *** Lookup functions ************************************************************************************************************************************ */

extern int16_t lookup(int16_t value, int16_t table [], int16_t min, int16_t max, int16_t step_log2);	///< Looks up a value in a table with linear steps
extern int16_t lookup_logstep(int16_t value, int16_t table[], int16_t min, int16_t max);		///< Looks up a value in a table with log steps

/** \} */

#endif /* __LOOKUP_H__ */
