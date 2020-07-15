#include "Utility.h"
#include <string.h>
#include "cmsis_os.h"
#include <malloc.h>


char16_t *strdup16(const char *src)
{
	int len = strlen(src);
	char16_t *dest = new char16_t[len + 1];
	for (int i = 0; i < len+1; i++)		// +1 to include null terminator
		dest[i] = src[i];
	return dest;
}

char16_t *strncpy16(char16_t *dest, const char *src, int num)
{
	int len = strlen(src)+1;			// +1 to include null terminator
	if (len > num)
		len = num;
	for (int i = 0; i < len; i++)
		dest[i] = src[i];
	return dest;
}

std::u16string & strcpy16(std::u16string &dest, const char *src)
{
	int len = strlen(src)+1;			// +1 to include null terminator
	dest.reserve(len);
	for (int i = 0; i < len; i++)
		dest.push_back(src[i]);
	return dest;	
}


void printHeapStatistics()
{
	printf("Heap Statistics\n");
	printf("Total Heap Bytes  = %d\n", configTOTAL_HEAP_SIZE );
	printf("Free Heap Bytes   = %d\n", xPortGetFreeHeapSize() );
	printf("Lowest Free Space = %d\n", xPortGetMinimumEverFreeHeapSize() );
	struct mallinfo info = mallinfo();
	printf("Malloc Statistics\n");
	printf("Total Heap Bytes  = %d\n", info.arena );
	printf("Used Bytes  = %d\n", info.uordblks );
	printf("Free list Bytes  = %d\n", info.fordblks );
}



/* Prototypes for our hooks.  */
extern "C" void *my_malloc_hook(size_t size, const void *caller)
{
}


 
void InitMallocChecking()
{
}
