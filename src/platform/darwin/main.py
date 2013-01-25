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

#
# Runs the interactive interpreter
#

# I don't yet know how to connect ipm-host to ipm-target
# when ipm-target is running in a gdb session
#import ipm
#ipm.ipm()

import sys
sys.gc()
print "Heap: ", sys.heap(), " [available, total]"