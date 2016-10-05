#include "strlwr.h"

char *strlwr (char *a) {
	char *ret = a;
	
	while (*a != '\0')
    {
		if (isupper (*a))
			*a = tolower (*a);
		++a;
    }
	
	return ret;
}
