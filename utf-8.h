#pragma once
#include "exstring.h"
#define ERR_HEXCODE 0xffffffff


//将json的转义字符序列\uXXXX转换成unicode码值
unsigned jsonhex_unicode(const char *pstr, char **EndPtr);
//将长度在1-4个char的utf-8编码转换成unicode
unsigned utf8_unicode(char *utf8, char **EndPtr);

//将一个从0-10ffff的unicode码值转换成utf-8编码的字符串
string unicode_utf8(unsigned value);
//将一个从0-10ffff的unicode码值转换成json接受的格式
char *unicode_jsonhex(unsigned value);

//将json的转义字符序列XXXX转换成utf-8编码的字符串
string jsonhex_utf8(const char *pstr, char **EndPtr);
//将utf-8编码的字符转换成json的转义字符序列\uXXXX
char *utf8char_jsonhex(char* pstr, char **EndPtr);

//将utf8编码的字符串转换成json字符串
char *utf8str_jsonstr(string pstr);