/**************************************************************************//**
 * Copyright (c) 2015 by Silicon Laboratories Inc. All rights reserved.
 *
 * http://developer.silabs.com/legal/version/v11/Silicon_Labs_Software_License_Agreement.txt
 *****************************************************************************/

#ifndef NDEBUG
void slab_Assert( const char * file, int line )
{
  file = file;
  line = line;

  while ( 1 );
}
#endif
