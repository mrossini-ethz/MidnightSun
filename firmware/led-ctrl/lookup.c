/**
	\file
	\ingroup Lookup
	\brief Definitions of functions related to lookup tables.
 */

#include "lookup.h"

/**
	Uses a lookup table to convert a value bounded within [min, max].
	The table is not complete but values are listed with constant step size.
	The step size has to be a power of 2 for computing efficiency.
	Example: -30, -22, -14, -6, 2, 10, 18, 26 (step size = 8 = 2^3).
	Values between the steps are calculated by linear interpolation.
	\param value Value that should be converted
	\param table Lookup table that should be used for the conversion
	\param min Value the first item of the lookup table corresponds to
	\param max Value the last item of the lookup table corresponds to
	\param step_log2 Logarithm to base 2 of the lookup table step size.
	It has to be an integer because step sizes have to be powers of 2.
	Minimum value for this parameter is 1.
	\return The conversion result of the value parameter.
 */
int16_t lookup(int16_t value, int16_t table [], int16_t min, int16_t max, int16_t step_log2)
{
	/* Limit the values to the range that the lookup table offers */
	if (value < min)
		return table[0];
	if (value >= max)
		return table[(max - min) >> step_log2];

	/* Find the index in the lookup table that is nearest to the value */
	uint8_t index = (value - min) >> step_log2;

	/* Get the lookup table entry corresponding to the index */
	int16_t a = (index << step_log2) + min;

	/* Calculate the lookup table step size, divided by 2 */
	int16_t half_step = 1 << (step_log2 - 1);

	/* Calculate the result */
	return table[index] + (((table[index + 1] - table[index]) * ((int32_t)(value - a)) + half_step) >> step_log2);
}

/**
	Uses a lookup table to convert a value bounded within [min, max].
	The table is not complete but values are listed with special logarithmic step sizes:
	1, 2, 4, 8, 16, ... (powers of two).
	Values between these steps are be calculated by linear interpolations.
	\param value Value that should be converted
	\param table Lookup table that should be used for the conversion
	\param min Value the first item of the lookup table corresponds to
	\param max Value the last item of the lookup table corresponds to
	\return The conversion result of the value parameter.
 */
int16_t lookup_logstep(int16_t value, int16_t table[], int16_t min, int16_t max)
{
	/* Limit the values to the range that the lookup table offers */
	if (value < min)
		value = min;
	if (value >= max)
		value = max - 1;

	/* Find the index in the lookup table that is nearest to the value */
	uint8_t index = 0;
	int16_t tmp = value;
	while (tmp) {
		tmp >>= 1;
		index += 1;
	}
	index -= 1;

	/* Get the lookup table entry corresponding to the index */
	int16_t a = 1 << index;

	/* Calculate the lookup table step size, divided by 2 */
	int16_t half_step = 1 << (index - 1);

	/* Calculate the result */
	return table[index] + (((table[index + 1] - table[index]) * ((int32_t)(value - a)) + half_step) >> index);
}
