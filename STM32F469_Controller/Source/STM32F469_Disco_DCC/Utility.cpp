#include "Utility.h"
#include <string.h>
#include "cmsis_os.h"
#include <malloc.h>
#include <locale>
#include <assert.h>


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
 
void InitMallocChecking()
{
}


void utf8ToChar16(char16_t *dest, const char *src, int destlen)
{
	int i = 0;
	for (i = 0; i < destlen; i++)		
	{
		uint8_t c = (uint8_t)*src++;
		if (c <= 0x007F)
		{
			dest[i] = c;
		}
		else 
		{
			uint32_t u = '?';
			if ((c & 0b11100000) == 0b11000000)
			{
				uint8_t c2 = (uint8_t)*src++;
				if (c2 != 0)
					u = ((c & 0x1F) << 6) | (c2 & 0x3F);
				else
					break;
			}
			else if ((c & 0b11110000) == 0b11100000)
			{
				uint8_t c2 = (uint8_t)*src++;
				if (c2 != 0)
				{
					uint8_t c3 = (uint8_t)*src++;
					if (c3 != 0)
					{
						if ((c2 & 0b11000000) == 0b10000000 && (c3 & 0b11000000) == 0b10000000)
						{
							u = ((c & 0xF) << 12) | ((c2 & 0x3F) << 6) | (c3 & 0x3F);
						}
					}
					else
						break;
				}
				else
					break;
			} 
			else if ((c & 0b11111000) == 0b11110000)
			{
				uint8_t c2 = (uint8_t)*src++;
				if (c2 != 0)
				{
					uint8_t c3 = (uint8_t)*src++;
					if (c3 != 0)
					{
						uint8_t c4 = (uint8_t)*src++;
						if (c4 != 0)
						{
							if ((c2 & 0b11000000) == 0b10000000 && (c3 & 0b11000000) == 0b10000000 && (c4 & 0b11000000) == 0b10000000)
							{
								u = ((c & 0xF) << 18) | ((c2 & 0x3F) << 12) | ((c3 & 0x3F) << 6) | (c4 & 0x3F);
							}
						}
						else
							break;
					}
					else
						break;
				}
				else
					break;
			}
			if (u > 0 && u <= 0xFFFF)
				dest[i] = u;
			else
				dest[i] = '?';
		}	
	}
	if ( i == destlen)
		dest[destlen-1] = 0;
	else
		dest[i] = 0;
}







template<>
	class std::ctype<char16_t> : public std::__ctype_abstract_base<char16_t>
  {
public:
	explicit
	ctype(std::size_t __refs = 0)
		: std::__ctype_abstract_base<char16_t>(__refs)
	{
	}	
	virtual ~ctype() {}
	static std::locale::id			id;
private:
	virtual char16_t do_widen(char __c) const
	{
		return (char16_t)__c;
	}

	virtual const char* do_widen(const char* __lo, const char* __hi, char16_t* __dest) const
	{
		while (__lo != __hi)
			*__dest++ = *__lo++;
		return __hi;
	}
	
	virtual bool do_is(mask __m, char_type __c) const { assert(false); return false; }
	virtual const char_type* do_is(const char_type* __lo, const char_type* __hi, mask* __vec) const { assert(false); return nullptr; }
	virtual const char_type* do_scan_is(mask __m, const char_type* __lo, const char_type* __hi) const { assert(false); return nullptr; }
	virtual const char_type* do_scan_not(mask __m, const char_type* __lo, const char_type* __hi) const { assert(false); return nullptr; }
	virtual char_type do_toupper(char_type __c) const { assert(false); return 0; }
	virtual const char_type* do_toupper(char_type* __lo, const char_type* __hi) const { assert(false); return nullptr; }
	virtual char_type do_tolower(char_type __c) const { assert(false); return 0; }
	virtual const char_type* do_tolower(char_type* __lo, const char_type* __hi) const { assert(false); return nullptr; }
	virtual char do_narrow(char_type, char __dfault) const { assert(false); return 0; }
	virtual const char_type* do_narrow(const char_type* __lo, const char_type* __hi, char __dfault, char* __to) const { assert(false); return nullptr; }
};
std::locale::id std::ctype<char16_t>::id;
template<> void std::numpunct<char16_t>::_M_initialize_numpunct(__c_locale __cloc) 
{
	_M_data = new __numpunct_cache<char16_t>();
	_M_data->_M_grouping = ",";
    _M_data->_M_grouping_size = 1;
	_M_data->_M_use_grouping = false;
    _M_data->_M_truename = u"true";
    _M_data->_M_truename_size = 4;
    _M_data->_M_falsename = u"false";
    _M_data->_M_falsename_size = 5;
    _M_data->_M_decimal_point = u'.';
    _M_data->_M_thousands_sep = u'.';
}

template<> std::numpunct<char16_t>::~numpunct() {}

void setupLocale()
{
	auto *_ctype = new std::ctype<char16_t>();
	auto *_num_put = new std::num_put<char16_t>();
	auto *_numpunct = new std::numpunct<char16_t>();
	
	static std::locale l2(std::locale(std::locale(std::locale(), _ctype), _num_put), _numpunct);

	std::locale::global(l2);
}