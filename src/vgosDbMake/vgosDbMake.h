/*
 *
 *    This file is a part of vgosDbMake. vgosDbMake is a part of CALC/SOLVE
 *    system and is designed to convert correlator output data into VgosDb format.
 *    Copyright (C) 2015-2020 Sergei Bolotin.
 *
 *    This program is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation, either version 3 of the License, or
 *    (at your option) any later version.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef VGOS_DB_MAKE_H
#define VGOS_DB_MAKE_H


#ifdef HAVE_CONFIG_H
#   include <config.h>
#endif


#include <SgVersion.h>



#include "VmSetup.h"



/***===================================================================================================*/
/**
 *
 *
 */
/**====================================================================================================*/

extern VmSetup                  setup;



extern SgVersion                vgosDbMakeVersion;



/*=====================================================================================================*/
#endif // VGOS_DB_MAKE_H
