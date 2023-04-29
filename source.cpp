#define _DEBUG
#include <stdio.h>
#include <stdlib.h>
#include "safememory.h"

int main(void) {
	InitSafeMemory();
	int* data1 = (int*)SafeMemoryAllocate(3 * sizeof(int));
	data1[0] = 121;
	data1[1] = 122;
	data1[2] = 123;
	char* data2 = (char*)SafeMemoryAllocate(3 * sizeof(char));
	data2[0] = 'H';
	data2[1] = 'i';
	data2[2] = '\0';
	float* data3 = (float*)SafeMemoryAllocate(3 * sizeof(float));
	data3[0] = 1.0;
	data3[1] = 1.1;
	data3[2] = 1.2;
	SafeMemoryRelease(data2);
	SafeMemoryRelease(data1);
	UninitSafememory();
}
