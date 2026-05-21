/*
 *
 *    This file is a part of nuSolve. nuSolve is a part of CALC/SOLVE system
 *    and is designed to perform data analyis of a geodetic VLBI session.
 *    Copyright (C) 2010-2020 Sergei Bolotin.
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

#ifndef NU_SOLVE_H
#define NU_SOLVE_H


#ifdef HAVE_CONFIG_H
#   include <config.h>
#endif


//    #include <SgGuiTaskConfig.h>

#include <SgParametersDescriptor.h>
#include <SgTaskConfig.h>
#include <SgVersion.h>

#include "NsSetup.h"


/***===================================================================================================*/
/**
 *
 *
 */
/**====================================================================================================*/



extern SgTaskConfig           config;
extern SgParametersDescriptor parametersDescriptor;
extern NsSetup                setup;
extern SgVersion              nuSolveVersion;
extern SgVersion              storedLibraryVersion;
extern SgVersion              storedNuSolveVersion;



/*=====================================================================================================*/
#endif // NU_SOLVE_H
