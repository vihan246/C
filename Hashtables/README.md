The makefile compiles the test file, breakProbing.cpp which inputs "a" into a hashtable of size 12
six times. On the sixth time, the program hangs, as the add function is unable to find an indice 
to put the key into. This is because the size is not prime, and when the number of elements reaches,
the load factor is .5. Since we attempt to input the same thing again and again, there are no longer
any indices that the quadratic probing can find.