/**
	\file
	\ingroup Interrupts
	\brief Declarations of functions and variables related to interrupts

	\addtogroup Interrupts
	\brief Interrupts initiate all tasks of the program

	Every action the program takes is initiated by an interrupt.
	Interrupts are handled by special functions called interrupt handlers.
	Every interrupt that can be generated has its own handler.

	Interrupt handler functions should be as short as possible because
	they interrupt the main program flow and other interrupts are blocked
	until the interrupt handler has finished.
	Normally an interrupt handler makes a quick decision and sets a flag for a task
	that the main() routine can pick up later on.

	Access to global variables that can be changed by interrupt handlers must be
	protected in some cases.
	If the access is non-atomic (not completed within a single clock cycle) the
	program could be interrupted before the access is finished.
	If the interrupt changes the variable that is being accessed the second part
	of the access will get a value different from the originally intended value.
	The resulting access will yield garbage in this case.
	To avoid this interrupts must be suspended before such a non-atomic access
	and resumed afterwards.

	To indicate which variables are changed by interrupt handlers an underscore (_)
	prefix is used in the variable name.
	These remind the programmer to protect these variables.

	Additionally, variables used both in interrupt handlers and the main program
	have to be declared \c volatile.
	This is because an interrupt may unexpectedly change such a variable.
	The compiler does normally assume that values of variable do not change
	beween operations in the main routine.
	Situations can occur where the compiler then optimises an access to variable
	such that the access is omitted.

	In the following example a variable is used to break an infinite loop.

		int wait = 1;

		int main()
		{
			while (wait) {}
		}

	The compiler knows what the value of the variable \c wait is and that
	it is not changed in the \c while loop.
	Therefore, when optimizing, it will not check the variable every time
	the loop repeats.
	If an interrupt handler changes the value of the \c wait variable the
	main routine will not see this change because it never accesses the
	variable.
	Thus the code will not work.

	If variables are declared \c volatile the compiler makes no assumptions
	about the contents of these variables because they may change unexpectedly.
	This is exactly what is wanted when sharing variables between the main
	routine and interrupt handlers.

	\{
 */

#ifndef __INTERRUPT_H__
#define __INTERRUPT_H__

#include <stdint.h>
#include <avr/interrupt.h>

/* *** Interrupt variables ********************************************************************************************************************************* */

extern uint8_t sreg_save;	///< Variable used to store the state of the interrupt enable bit befor suspending interrupts

/* *** Interrupt functions ********************************************************************************************************************************* */

#define interrupts_suspend()	{sreg_save = SREG; cli();}	///< Suspends interrupts such that the interrupt state can be restored afterwards
#define interrupts_resume()	{SREG = sreg_save;}		///< Resumes the interrupt state that was before the interrupts were suspended
#define interrupts_on()		sei()				///< Turns on interrupts
#define interrupts_off()	cli()				///< Turns off interrupts

/** \} */

#endif /* __INTERRUPT_H__ */
