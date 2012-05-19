# This file is Copyright 2010 Dean Hall.
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

def bar1():
   """__NATIVE__
   PmReturn_t retval = PM_RET_OK;

   /* If wrong number of args, raise TypeError */
   if (NATIVE_GET_NUM_ARGS() != 0)
   {
       PM_RAISE(retval, PM_RET_EX_TYPE);
       return retval;
   }

   NATIVE_SET_TOS(PM_NONE);
   return retval;
   """
   pass

def bar2():
   return bar1()
