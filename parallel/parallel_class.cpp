/** $Id $
	Copyright (C) 2008 Battelle Memorial Institute
	@file residential_enduse.cpp
	@addtogroup residential_enduse Residential enduses
	@ingroup residential

 @{
 **/

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <iostream>
#include "parallel.h"
#include "parallel_class.h"

//////////////////////////////////////////////////////////////////////////
// CLASS FUNCTIONS
//////////////////////////////////////////////////////////////////////////
CLASS* parallel_class::oclass = NULL;

// the constructor registers the class and properties and sets the defaults
parallel_class::parallel_class(MODULE *mod)
{


	// first time init
	if (oclass==NULL)
	{
		// register the class definition
		oclass = gld_class::create(mod,"parallel_class",sizeof(parallel_class),PC_BOTTOMUP|PC_AUTOLOCK);
		if (oclass==NULL)
			// GL_THROW("unable to register object class implemented by %s",__FILE__);
			std::cout<<"unable to register object class implemented by "<<__FILE__;
			/* TROUBLESHOOT
				The registration for the residential_enduse class failed.   This is usually caused
				by a coding error in the core implementation of classes or the module implementation.
				Please report this error to the developers.
			 */

		// publish the class properties

			/* TROUBLESHOOT
				The registration for the residential_enduse properties failed.   This is usually caused
				by a coding error in the core implementation of classes or the module implementation.
				Please report this error to the developers.
			 */
	}
}

// create is called every time a new object is loaded
int parallel_class::create()
{

	return 1;
}

int parallel_class::init()
{


	return 1;
}
/*
int parallel_class::isa(char *classname){
	return strcmp(classname,"parallel_class")==0;
}
*/
/*
TIMESTAMP parallel_class::sync(TIMESTAMP t0, TIMESTAMP t1)
{
	gl_debug("%s shape load = %8g", get_name(), gl_get_loadshape_value(&shape));
	if (load.voltage_factor>1.2 || load.voltage_factor<0.8)
		gl_verbose("%s voltage is out of normal +/- 20%% range of nominal (vf=%.2f)", get_name(), load.voltage_factor);
		//  TROUBLESHOOT
		//    The voltage on the enduse circuit is outside the expected range for that enduse.
		//    This is usually caused by an impropely configure circuit (e.g., 110V on 220V or vice versa).
		//    Fix the circuit configuration for that enduse and try again.
		//
	return shape.t2>t1 ? shape.t2 : TS_NEVER;
}
*/
//////////////////////////////////////////////////////////////////////////
// IMPLEMENTATION OF CORE LINKAGE
//////////////////////////////////////////////////////////////////////////



EXPORT int create_parallel_class(OBJECT **obj, OBJECT *parent)
{
	try {
		*obj = gl_create_object(parallel_class::oclass);
		if (*obj!=NULL)
		{
			parallel_class *my = OBJECTDATA(*obj,parallel_class);
			gl_set_parent(*obj,parent);
			return my->create();
		}
		else
			return 0;
	}
	CREATE_CATCHALL(parallel_class);
}
/*
EXPORT int init_residential_enduse(OBJECT *obj)
{
	try {
		residential_enduse *my = OBJECTDATA(obj,residential_enduse);
		return my->init(obj->parent);
	}
	INIT_CATCHALL(residential_enduse);
}

EXPORT int isa_residential_enduse(OBJECT *obj, char *classname)
{
	if(obj != 0 && classname != 0){
		return OBJECTDATA(obj,residential_enduse)->isa(classname);
	} else {
		return 0;
	}
}

EXPORT TIMESTAMP sync_residential_enduse(OBJECT *obj, TIMESTAMP t1)
{
	residential_enduse *my = OBJECTDATA(obj,residential_enduse);
	try {
		TIMESTAMP t2 = my->sync(obj->clock, t1);
		obj->clock = t1;
		return t2;
	}
	SYNC_CATCHALL(residential_enduse);
}
*/

/**@}**/
