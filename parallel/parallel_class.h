/** $Id: residential_enduse.h 4738 2014-07-03 00:55:39Z dchassin $
	Copyright (C) 2008 Battelle Memorial Institute
	@file residential_enduse.h
	@addtogroup residential_enduse

 @{
 **/

#ifndef _PARALLELCLASS_H
#define _PARALLELCLASS_H

#include "parallel.h"


class parallel_class : public gld_object
{
public:
	static CLASS *oclass;
	parallel_class(MODULE *mod);
	int create();
	int init();

};

#endif // _RESIDENTIALENDUSE_H

/**@}**/
