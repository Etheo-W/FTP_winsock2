#pragma once
#ifndef _AllFileHashValue_H_
#define _AllFileHashValue_H_
#endif // !_AllFileHashValue_H_

#define WIN32_LEAN_AND_MEAN

#pragma warning(disable:4996)
// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#pragma comment (lib, "Ws2_32.lib")

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define DEFAULT_HASH_NUMBER 20
#define FILL_DIVISOR 0.7

typedef struct AllFileHashValue {

	char	**allFileHashValue;
	int	length;
	int	listSize;

}AllFileHashValue, *AFHV;

AFHV	getAllFileHashValue();

int		addHashValue(AFHV afhv, char *hashValueString);
char*	getHashValue(AFHV afhv, int index);

int		deleteAllFileHashValue(AFHV afhv);
