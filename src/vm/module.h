/*
# This file is Copyright 2002 Dean Hall.
# This file is part of the PyMite VM.
# This file is licensed under the MIT License.
# See the LICENSE file for details.
*/


#ifndef __MODULE_H__
#define __MODULE_H__


/**
 * \file
 * \brief Module Object Type
 *
 * Module object type header.
 */


/**
 * Creates a Module Obj for the given Code Obj.
 *
 * Use a func struct to represent the Module obj because
 * the module's construction code must execute later,
 * but set the type to OBJ_TYPE_MOD so that it is
 * not otherwise callable.
 *
 * @param   pco Ptr to code obj
 * @param   pmod Return by reference; ptr to new module obj
 * @return  Return status
 */
PmReturn_t mod_new(pPmObj_t pco, pPmObj_t *pmod);

/**
 * Imports a module of the given name.
 * Searches for an image with a matching name.
 * A code obj is created for the code image.
 * A module obj is created for the code obj.
 *
 * @param   pstr String obj containing name of code obj to load.
 * @param   pmod Return by reference; ptr to imported module
 * @return  Return status
 */
PmReturn_t mod_import(pPmObj_t pstr, pPmObj_t *pmod);

#endif /* __MODULE_H__ */
