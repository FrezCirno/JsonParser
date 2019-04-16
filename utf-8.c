#include"utf-8.h"
#include<stdio.h>
#include"exstring.h"
#include<ctype.h>
unsigned jsonhex_unicode(const char *pstr, char **EndPtr)
{
    /*
    输入:4/10个Byte
    */
    int i;
    unsigned value = 0, value2 = 0;

    for (i = 0; i < 4; i++)
    {
        value <<= 4;
        if (*pstr >= '0' && *pstr <= '9')
            value |= *pstr - '0';
        else if (*pstr >= 'A' && *pstr <= 'F')
            value |= *pstr - ('A' - 10);
        else if (*pstr >= 'a' && *pstr <= 'f')
            value |= *pstr - ('a' - 10);
        else  // error
            return ERR_HEXCODE;
        pstr++;
    }
    if (value >= 0xD800 && value <= 0xDBFF) //两个
    {
        if (*pstr != '\\' || *++pstr != 'u') return ERR_HEXCODE;
        pstr++;
        for (i = 0; i < 4; i++) {
            value2 <<= 4;
            if (*pstr >= '0' && *pstr <= '9')
                value2 |= *pstr - '0';
            else if (*pstr >= 'A' && *pstr <= 'F')
                value2 |= *pstr - ('A' - 10);
            else if (*pstr >= 'a' && *pstr <= 'f')
                value2 |= *pstr - ('a' - 10);
            else  
                return ERR_HEXCODE;  // error
            pstr++;
        }
        value = (((value - 0xD800) << 10) | (value2 - 0xDC00)) + 0x10000;
    } 
    if (EndPtr) *EndPtr = pstr;
    return value;
}

unsigned utf8_unicode(unsigned char *utf8, char **EndPtr)
{ 
 /*
 输入范围(首字符):
 [0x00,0x7f](0xxxxxxx)->只读1个char
 (0x7f-0xdf](110xxxxx)->读2个char
 (0xdf-0xef](1110xxxx)->读3个char
 (0xef-0xbf](11110xxx)->读4个char
 (0xbf-0xff]->不合法
 */
    unsigned ret;
    if (*utf8 <= 0x7F) {
        //1.[0x00,0x7f]  0xxx xxxx
        ret = *utf8;
        utf8++;
    } else if (*utf8 <= 0xDF){
        //2.(0x7f-0xdf]  110x xxxx, 10xx xxxx
        //-> 0000 0xxx, xxxx xxxx
        ret = (*utf8 & 0x1F) << 6;
        utf8++;
        ret |= (*utf8 & 0x3F);
        utf8++;
    } else if (*utf8 <= 0xEF) {
        //3.(0xdf-0xef]  1110 xxxx, 10xx xxxx, 10xx xxxx
        //-> xxxx xxxx, xxxx xxxx
        ret = (*utf8 & 0xF) << 4;
        utf8++;
        ret |= (*utf8 & 0x3F) << 6;
        utf8++;
        ret |= (*utf8 & 0x3F);
        utf8++;
    } else if(*utf8<=0xBF){
        //4.(0xef-0xbf]  1111 0xxx,10xx xxxx,10xx xxxx, 10xx xxxx
        // -> 000x xxxx, xxxx xxxx,xxxx xxxx
        ret = (*utf8 & 0x7) << 18;
        utf8++;
        ret |= (*utf8 & 0x3F) << 12;
        utf8++;
        ret |= (*utf8 & 0x3F) << 8;
        utf8++;
        ret |= (*utf8 & 0x3F);
        utf8++;
    } else{
        ret = 0xffffffff;
    }
    if (EndPtr) *EndPtr = utf8;
    return ret;
}

string unicode_utf8(unsigned value)
{ 
    /*
 输入范围:
 [0x00,0x7f]->编码成1 Byte
 [0x80-0x7ff]->编码成2 Byte
 [0x800-0xffff]->编码成3 Byte
 [0x10000-0x10FFFF]->编码成4 Byte
 [0x110000-0xFFFFFFFF]->不合法
 */
    string ret = init_str();
    if (value <= 0x7F)
        //1.[0x00,0x7f]
        //ASCII
        push_back(ret, value & 0xFF);
    else if (value <= 0x7FF)
    {
        //2.[0x80-0x7ff]   0000 0xxx, xxxx xxxx
        //->110x xxxx, 10xx xxxx
        push_back(ret, 0xC0 | ((value >> 6) & 0xFF));
        push_back(ret, 0x80 | (value & 0x3F));
    }
    else if (value <= 0xFFFF)
    {
        //3.[0x800-0xffff]   xxxx xxxx, xxxx xxxx
        //->1110 xxxx, 10xx xxxx, 10xx xxxx
        push_back(ret, 0xE0 | ((value >> 12) & 0xFF));
        push_back(ret, 0x80 | ((value >> 6) & 0x3F));
        push_back(ret, 0x80 | (value & 0x3F));
    } else if (value<=0x10FFFF)
    {
        //4.[0x10000-0x10FFFF]  000x xxxx, xxxx xxxx,xxxx xxxx
        //->1111 0xxx, 10xx xxxx, 10xx xxxx, 10xx xxxx
        push_back(ret, 0xF0 | ((value >> 18) & 0xFF));
        push_back(ret, 0x80 | ((value >> 12) & 0x3F));
        push_back(ret, 0x80 | ((value >> 6) & 0x3F));
        push_back(ret, 0x80 | (value & 0x3F));
    } else {
        //to-do
    }
    return ret;
}

char *unicode_jsonhex(unsigned value)
{
    /*
 输入范围:
 [0x00,0x7f]->编码成转义字符序列
 [0x80-0xffff]->编码成\uXXXX
 [0x10000-0x10FFFF]->编码成\uDBXX \uDFXX
 [0x110000-0xFFFFFFFF]->不合法
 */
    static char hex[] = "0123456789abcdef";
    string ret = init_str();
    if (value <= 0x7F){
        //1. [0x00,0x7f] 0xxx xxxx (ASCII)
        switch (value) {
            case '\n':
                push_back(ret, 92);
                push_back(ret, 'n');
                break;
            case '\r':
                push_back(ret, 92);
                push_back(ret, 'r');
                break;
            case '\f':
                push_back(ret, 92);
                push_back(ret, 'f');
                break;
            case '\b':
                push_back(ret, 92);
                push_back(ret, 'b');
                break;
            case '/':
                push_back(ret, 92);
                push_back(ret, '/');
                break;
            case '\"':
                push_back(ret, 92);
                push_back(ret, '\"');
                break;
            case 92:
                push_back(ret, 92);
                push_back(ret, 92);
                break;
            default:
                if (isprint(value))
                    push_back(ret, value);
                else {
                    push_back(ret, 92);
                    push_back(ret, 'u');
                    push_back(ret, '0');
                    push_back(ret, '0');
                    push_back(ret, hex[(value>>4) & 0x7]);
                    push_back(ret, hex[value & 0xf]);
                }
                break;
        }
    }
    else if (value <= 0xffff) {
        //2.[0x80-0x7ff]   0000 0xxx, xxxx xxxx
        //->0000 0xxx, xxxx xxxx
        //3.[0x800-0xffff]   xxxx xxxx, xxxx xxxx
        //->xxxx xxxx, xxxx xxxx
        push_back(ret, 92);
        push_back(ret, 'u');
        push_back(ret, hex[(value >> 12) & 0xf]);
        push_back(ret, hex[(value >> 8) & 0xf]);
        push_back(ret, hex[(value >> 4) & 0xf]);
        push_back(ret, hex[(value) & 0xf]);
    }
    else if (value <= 0x10ffff) {
        //4.[0x10000-0x10FFFF]  000x xxxx, xxxx xxxx,xxxx xxxx
        //                             -1
        // ->json \u 1101 10xx, xxxx xxxx, (\u)1101 11xx, xxxx xxxx
        value -= 0x10000;
        push_back(ret, 92);
        push_back(ret, 'u');
        push_back(ret, 'd');
        push_back(ret, hex[(value >> 18) & 0x3 | 0x8]);
        push_back(ret, hex[(value >> 14) & 0xf]);
        push_back(ret, hex[(value >> 10) & 0xf]);
        push_back(ret, 92);
        push_back(ret, 'u');
        push_back(ret, 'd');
        push_back(ret, hex[(value >> 8) & 0x3 | 0xC]);
        push_back(ret, hex[(value >> 4) & 0xf]);
        push_back(ret, hex[(value) & 0xf]);
    }
    push_back(ret, '\0');
    char *ret_value = ret->value;
    free(ret);
    return ret_value;
}

string jsonhex_utf8(const char *pstr, char **EndPtr)
{
    /*
    输入范围:
    输入:4/8个Byte

    */
    return unicode_utf8(jsonhex_unicode(pstr, EndPtr));
}
char *utf8char_jsonhex(char* pstr, char **EndPtr)
{
    /*
    输入范围(首字符):
    [0x00,0x7f](0xxxxxxx)->ascii/转义序列
    (0x7f-0xdf](110xxxxx)->转义序列
    (0xdf-0xef](1110xxxx)->转义序列
    (0xef-0xbf](11110xxx)->转义序列
    (0xbf-0xff]->错误
    */
    return unicode_jsonhex(utf8_unicode(pstr, EndPtr));
}


char *utf8str_jsonstr(string utf8) {
    char *now = utf8->value;
    char *end = utf8->value + utf8->length;

    string tmp_str = init_str();//整个字符串转换结果,长度不确定,故用动态字符串
    char *   ret_str;

    while (now != end) {
        push_back_str(tmp_str, unicode_jsonhex(utf8_unicode(now, &now)));
    }
    ret_str = tmp_str->value;
    ret_str[tmp_str->length] = '\0';
    free(tmp_str);
    return ret_str;
}
