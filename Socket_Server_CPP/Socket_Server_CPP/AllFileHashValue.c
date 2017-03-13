#include "AllFileHashValue.h"

AFHV getAllFileHashValue()
{
	static AFHV aFHV = NULL;

	if (!aFHV)
	{
		aFHV = (AFHV)malloc(sizeof(AllFileHashValue));

		aFHV->allFileHashValue = (char**)malloc(DEFAULT_HASH_NUMBER * sizeof(char*));
		aFHV->length = 0;
		aFHV->listSize = DEFAULT_HASH_NUMBER;
	}

	return aFHV;

}

int	deleteAllFileHashValue(AFHV afhv)
{
	char *tmpString = afhv->allFileHashValue[0];
	while (tmpString)
	{
		free(tmpString);
		tmpString++;
	}

	free(afhv->allFileHashValue);

	return 1;
}

int	addHashValue(AFHV afhv, char *hashValueString)
{
	for (int i = 0; i < afhv->length; i++)
	{
		if (!strcmp(afhv->allFileHashValue[i], hashValueString)) return 1;
	}

	if ((double)(afhv->length) / afhv->listSize >= FILL_DIVISOR)
	{
		afhv->allFileHashValue = (char**)realloc(afhv->allFileHashValue, (afhv->listSize + DEFAULT_HASH_NUMBER) * sizeof(char*));
		afhv->listSize += DEFAULT_HASH_NUMBER;
	}

	int position = afhv->length;
	afhv->allFileHashValue[position] = (char*)malloc(65 * sizeof(char));
	for (int i = 0; i < 65; i++)
	{
		afhv->allFileHashValue[position][i] = '\0';
	}

	strcpy(afhv->allFileHashValue[position], hashValueString);

	afhv->length++;

	return 1;
}

char* getHashValue(AFHV afhv, int index)
{
	return afhv->allFileHashValue[index];
}
