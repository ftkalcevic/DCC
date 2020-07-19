#pragma once

#include <string>

extern char16_t *strdup16(const char *src);
extern char16_t *strncpy16(char16_t *dest, const char *src, int num);
extern std::u16string & strcpy16(std::u16string &dest, const char *src);
extern void printHeapStatistics();
extern void InitMallocChecking();

extern void utf8ToChar16(char16_t *dest, const char *src, int destlen);
extern void setupLocale();

typedef std::basic_stringstream<char16_t> u16stringstream;

