/** $Id: init.cpp 4738 2014-07-03 00:55:39Z dchassin $
	Copyright (C) 2008 Battelle Memorial Institute
	@file init.cpp
	@addtogroup residential Residential loads (residential)
	@ingroup modules
 @{
 **/

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <math.h>
#include "parallel.h"
#include <iostream>
#include "parallel_class.h"


EXPORT CLASS *init(CALLBACKS *fntable, MODULE *module, int argc, char *argv[])
{
	if (set_callback(fntable)==NULL)
	{
		errno = EINVAL;
		return NULL;
	}
	//  char1024
	//  gl_global_create("residential::default_line_voltage",PT_char1024,&default_line_voltage,PT_SIZE,3,PT_UNITS,"V",PT_DESCRIPTION,"line voltage to use when no circuit is attached",NULL);

	std::cout<<"before you call the constructor for parallel_class";
	new parallel_class(module);
	return parallel_class::oclass;

}

CDECL int do_kill()
{
	/* if global memory needs to be released, this is a good time to do it */
	return 0;
}

EXPORT int check(){
	return 0;
}


/**@}**/
