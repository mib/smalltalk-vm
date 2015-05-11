#include <stdio.h>
#include <stdlib.h>
#include "common.h"
#include "constants.h"
#include "vm.h"


void error(char * message) {
	printf(message);
	printf("\n");
	
	deallocateAll();
	exit(1);
}
