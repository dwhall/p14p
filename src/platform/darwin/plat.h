/*
# This file is Copyright 2013 Dean Hall.
#
# This file is part of the Python-on-a-Chip program.
# Python-on-a-Chip is free software: you can redistribute it and/or modify
# it under the terms of the GNU LESSER GENERAL PUBLIC LICENSE Version 2.1.
#
# Python-on-a-Chip is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
# A copy of the GNU LESSER GENERAL PUBLIC LICENSE Version 2.1
# is seen in the file COPYING up one directory from this.
*/

#ifndef _PLAT_H_
#define _PLAT_H_

#define PM_FLOAT_LITTLE_ENDIAN
#ifdef __LP64__
// 64-bit code
#define PM_PLAT_POINTER_SIZE 8
#else
// 32-bit code
#define PM_PLAT_POINTER_SIZE 4
#endif
#define PM_PLAT_HEAP_ATTR __attribute__((aligned (4)))
#define PM_PLAT_PROGMEM const

#endif /* _PLAT_H_ */
