#ifndef GLOBAL_H
#define GLOBAL_H

// Global header file contains all standard includes and constants.

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <strings.h>
/* Make sure to add -DNDEBUG flag to your compiler command line to 
   disable aseerts in release*/
#include <assert.h> 


// This macros is used to suppress the "Unused arg/var" compilation warning
#define UNUSED(x) (void)(x)

#define BUFF_SIZE_32   32
#define BUFF_SIZE_64   64
#define BUFF_SIZE_128  128
#define BUFF_SIZE_256  256

#endif /* GLOBAL_H */
