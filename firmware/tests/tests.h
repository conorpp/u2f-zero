/*
 * tests.h
 *
 *  Created on: Feb 14, 2016
 *      Author: Conor
 */

#ifndef TESTS_H_
#define TESTS_H_

//#define ENABLE_TESTS

//#define TEST_SHA
//#define TEST_ATECC_EEPROM
//#define TEST_EFM8UB1_EEPROM
//#define TEST_KEY_SIGNING		// requires key and locked eeprom

#ifdef ENABLE_TESTS

	void run_tests();

#else

	#define run_tests(x)

#endif

#endif /* TESTS_H_ */
