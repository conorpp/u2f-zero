/*
 * tests.h
 *
 *  Created on: Feb 14, 2016
 *      Author: Conor
 */

#ifndef TESTS_H_
#define TESTS_H_

// #define ENABLE_TESTS

#ifdef ENABLE_TESTS

	void run_tests();

#else

	#define run_tests(x)

#endif

#endif /* TESTS_H_ */
