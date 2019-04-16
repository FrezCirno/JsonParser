#pragma once
#include "exstring.h"
#define ERR_HEXCODE 0xffffffff


//��json��ת���ַ�����\uXXXXת����unicode��ֵ
unsigned jsonhex_unicode(const char *pstr, char **EndPtr);
//��������1-4��char��utf-8����ת����unicode
unsigned utf8_unicode(char *utf8, char **EndPtr);

//��һ����0-10ffff��unicode��ֵת����utf-8������ַ���
string unicode_utf8(unsigned value);
//��һ����0-10ffff��unicode��ֵת����json���ܵĸ�ʽ
char *unicode_jsonhex(unsigned value);

//��json��ת���ַ�����XXXXת����utf-8������ַ���
string jsonhex_utf8(const char *pstr, char **EndPtr);
//��utf-8������ַ�ת����json��ת���ַ�����\uXXXX
char *utf8char_jsonhex(char* pstr, char **EndPtr);

//��utf8������ַ���ת����json�ַ���
char *utf8str_jsonstr(string pstr);