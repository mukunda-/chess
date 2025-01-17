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

#endif /* GLOBAL_H */
