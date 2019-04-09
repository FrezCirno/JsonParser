#pragma once

unsigned parse_hex4(const char *pstr, char **EndPtr)
{
  int i;
  char ch;
  unsigned value = 0;

  for (i = 0; i < 4; i++)
  {
    ch = *pstr;
    value <<= 4;
    if (ch >= '0' && ch <= '9')
      value |= ch - '0';
    else if (ch >= 'A' && ch <= 'F')
      value |= ch - ('A' - 10);
    else if (ch >= 'a' && ch <= 'f')
      value |= ch - ('a' - 10);
    else // error
      break;
    pstr++;
  }
  *EndPtr = pstr;
  return value;
}
//value to utf-8
string encode_utf8(unsigned value)
{
  string ret = init_str();
  if (value <= 0x7F)
    //ASCII
    push_back(ret, value & 0xFF);
  else if (value <= 0x7FF)
  {
    //0000 0xxx, xxxx xxxx
    //->110x xx|xx, 10xx xxxx
    push_back(ret, 0xC0 | ((value >> 6) & 0xFF));
    push_back(ret, 0x80 | (value & 0x3F));
  }
  else if (value <= 0xFFFF)
  {
    //xxxx xxxx, xxxx xxxx
    //->1110 xxxx, 10xx xx|xx, 10xx xxxx
    push_back(ret, 0xE0 | ((value >> 12) & 0xFF));
    push_back(ret, 0x80 | ((value >> 6) & 0x3F));
    push_back(ret, 0x80 | (value & 0x3F));
  }
  else
  {
    //   json    1101 10 xx,xx  xxxx   xx, 1101 11xx,xx    xx xxxx
    //      result 00\00 xx xx, xxxx   xx         xx,xx    xx xxxx
    //                      +1
    //  (UNICODE)000x\xx    xx, xxxx   xx         xx,xx    xx xxxx
    //        1111 0x\xx,10 xx  xxxx,10xx         xx xx, 10xx xxxx
    push_back(ret, 0xF0 | ((value >> 18) & 0xFF));
    push_back(ret, 0x80 | ((value >> 12) & 0x3F));
    push_back(ret, 0x80 | ((value >> 6) & 0x3F));
    push_back(ret, 0x80 | (value & 0x3F));
  }
  return ret;
}

string decode(char *bytes)
{ //json bytes to UTF-8
  string utf8 = init_str();
  unsigned value = 0, value2 = 0;
  char ch;

  {
    if (bytes->value[++i] != '\\' || bytes->value[++i] != 'u')
      printf("ERROR");
    for (int i = 0; i < 4; i++)
    {
      ch = bytes->value[++i];
      value2 <<= 4;
      if (ch >= '0' && ch <= '9')
        value2 |= ch - '0';
      else if (ch >= 'A' && ch <= 'F')
        value2 |= ch - 'A' + 10;
      else if (ch >= 'a' && ch <= 'f')
        value2 |= ch - 'a' + 10;
      else
        printf("ERROR");
    }
    value = (((value - 0xD800) << 10) | (value2 - 0xDC00)) + 0x10000;
    push_back(utf8, 0xF0 | (value >> 18) & 0x07);
    push_back(utf8, 0x80 | (value >> 12) & 0x3F);
    push_back(utf8, 0x80 | (value >> 6) & 0x3F);
    push_back(utf8, 0x80 | value & 0x3F);
  }
  return utf8;
}

string encode(string utf8)
{ //UTF-8 to json bytes
  string bytes = init_str();
  char result = 0;
  unsigned value = 0, val = 0;
  for (int i = 0; i < utf8->length; i++)
  {
    value = utf8->value[i];
    if (value <= 0x7F)
      //0xxx xxxx (ASCII)
      switch (value)
      {
      case '\n':
        push_back(bytes, '\\');
        push_back(bytes, 'n');
        break;
      case '\r':
        push_back(bytes, '\\');
        push_back(bytes, 'r');
        break;
      case '\0':
        push_back(bytes, '\\');
        push_back(bytes, '0');
        break;
      case '\f':
        push_back(bytes, '\\');
        push_back(bytes, 'f');
        break;
      case '\b':
        push_back(bytes, '\\');
        push_back(bytes, 'b');
        break;
      case '/':
        push_back(bytes, '\\');
        push_back(bytes, '/');
        break;
      case '\\':
        push_back(bytes, '\\');
        push_back(bytes, '\\');
        break;
      default:
        push_back(bytes, value);
        break;
      }
    else if (value <= 0xDF)
    {
      //110x xx|xx, 10xx xxxx
      //->0000 0xxx, xxxx xxxx
      push_back(bytes, '\\');
      push_back(bytes, 'u');
      val |= (value & 0x1f) << 6;
      value = utf8->value[++i];
      val |= value & 0x3f;
      push_back(bytes, (val >> 8) & 0xff);
      push_back(bytes, (val)&0xff);
    }
    else if (value <= 0xEF)
    {
      //1110 xxxx, 10xx xx|xx, 10xx xxxx
      //->xxxx xxxx, xxxx xxxx
      push_back(bytes, '\\');
      push_back(bytes, 'u');
      val |= (value & 0xf) << 12;
      value = utf8->value[++i];
      val |= (value & 0x3f) << 6;
      value = utf8->value[++i];
      val |= (value & 0x3f);

      push_back(bytes, (val >> 8) & 0xff);
      push_back(bytes, (val)&0xff);
    }
    else
    {
      //        1111 0x\xx,10 xx  xxxx,10xx           xx xx, 10xx xxxx
      //->(UNICODE)000x\xx    xx, xxxx   xx           xx,xx    xx xxxx
      //                      -1
      //      result 00\00 xx xx, xxxx   xx           xx,xx    xx xxxx
      // ->json \u 1101 10 xx,xx  xxxx   xx, \u1101 11xx,xx    xx xxxx
      push_back(bytes, '\\');
      push_back(bytes, 'u');
      //b1
      val |= (value & 0x07) << 18;
      value = utf8->value[++i];
      //b2
      val |= (value & 0x3f) << 12;
      value = utf8->value[++i];
      //b3
      val |= (value & 0x3f) << 6;
      value = utf8->value[++i];
      //b4
      val |= (value & 0x3f);
      val = val - 0x010000;
      push_back(bytes, (val >> 18) & 0x3 | 0xD8);
      push_back(bytes, (val >> 10) & 0xff);
      push_back(bytes, (val >> 8) & 0x3 | 0xDC);
      push_back(bytes, (val)&0xff);
    }
  }
  return bytes;
}

string parse_utf8(const char *pstr, char **EndPtr)
{
  unsigned u, u2;
  char *p = pstr;
  u = parse_hex4(pstr, &p);
  if (p - pstr != 4)
  {
    *EndPtr = p;
    return NULL;
  }
  pstr = p;
  if (u >= 0xD800 && u <= 0xDBFF)
  {
    if (*pstr != '\\' || *++pstr != 'u')
    {
      *EndPtr = pstr;
      return NULL;
    }
    pstr++;
    u2 = parse_hex4(pstr, &p);
    if (p - pstr != 4 || u2 < 0xDC00 || u2 > 0xDFFF)
    {
      *EndPtr = p;
      return NULL;
    }
    pstr = p;
    u = (((u - 0xD800) << 10) | (u2 - 0xDC00)) + 0x10000;
  }
  return encode_utf8(u);
}