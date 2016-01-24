/******************************************************************************
 * Copyright (c) 2014 by Silicon Laboratories Inc. All rights reserved.
 *
 * http://developer.silabs.com/legal/version/v11/Silicon_Labs_Software_License_Agreement.txt
 *****************************************************************************/

#ifndef __ASSERT_H__

#include "efm8_config.h"

/**************************************************************************//**
 * @addtogroup efm8_assert
 * @{
 *
 * @brief Runtime assert for EFM8
 *
 * This module contains a runtime assert macro. It can be compiled out by setting
 * the NDEBUG flag.
 *
 *****************************************************************************/


/**************************************************************************//**
 * @def NDEBUG
 * @brief Controls if the asserts are present.
 *
 * Asserts are removed if this symbol is defined
 *
 *****************************************************************************/

/**************************************************************************//**
 * @def USER_ASSERT
 * @brief User implemented assert function.
 *
 * When asserts are enabled the default handler can be be replaced with a user defined
 * function of the form 'void userAssertName( const char * file, int line )' by setting
 * the value of USER_ASSERT to the userAssertName.
 *
 *****************************************************************************/

/**************************************************************************//**
 * @def SLAB_ASSERT(expr)
 * @brief default implementation of assert_failed.
 *
 * This function can be replaced by a user defined assert function by setting the USER_ASSERT flag
 *****************************************************************************/

#ifdef NDEBUG
  #define SLAB_ASSERT(expr)
#else
  #ifdef  USER_ASSERT
    #define SLAB_ASSERT(expr)    ((expr) ? ((void)0) : USER_ASSERT( __FILE__, __LINE__ ))
  #else
    void slab_Assert( const char * file, int line );
    //Yes this is smaller than if(!expr){assert}
    #define SLAB_ASSERT(expr)    if(expr){}else{slab_Assert( __FILE__, __LINE__ );}
  #endif
#endif

#endif //!__ASSERT_H__
