#include <stdio.h>
char* parse_hex4(const char* pstr, unsigned* value) {
  int i;
  char ch;
  value = 0;

  for (i = 0; i < 4; i++) {
    ch = *pstr;
    *value <<= 4;
    if (ch >= '0' && ch <= '9')
      *value |= ch - '0';
    else if (ch >= 'A' && ch <= 'F')
      *value |= ch - ('A' - 10);
    else if (ch >= 'a' && ch <= 'f')
      *value |= ch - ('a' - 10);
    else  // error
      break;
    pstr++;
  }
  return pstr;
}

char* encode_utf8(unsigned value) {
  char ret[5];
  int k = 0;

  if (value <= 0x7F)
    ret[k++] = value & 0xFF;
  else if (value <= 0x7FF) {
    ret[k++] = 0xC0 | ((value >> 6) & 0xFF);
    ret[k++] = 0x80 | (value & 0x3F);
  } else if (value <= 0xFFFF) {
    ret[k++] = 0xE0 | ((value >> 12) & 0xFF);
    ret[k++] = 0x80 | ((value >> 6) & 0x3F);
    ret[k++] = 0x80 | (value & 0x3F);
  } else {
    ret[k++] = 0xF0 | ((value >> 18) & 0xFF);
    ret[k++] = 0x80 | ((value >> 12) & 0x3F);
    ret[k++] = 0x80 | ((value >> 6) & 0x3F);
    ret[k++] = 0x80 | (value & 0x3F);
  }
  ret[k] = '\0';
  return ret;
}
char* parse_utf8(const char* pstr, char* result) {
  char* p = pstr;
  unsigned u;
  p = parse_hex4(pstr, &u);
  if (p - pstr != 4)
    return p;
  if (u >= 0xD800 && u <= 0xDBFF) {
    if (*p == '\\' && *++p == 'u') {
      p++;
      unsigned u2;
      char* p2;
      p2 = parse_hex4(p, &u2);
      if (p2 - p != 4 || u2 < 0xDC00 || u2 > 0xDFFF)
        return p2;
      u = (((u - 0xD800) << 10) | (u2 - 0xDC00)) + 0x10000;
    } else
      return p;
  }
  result = encode_utf8(u);
  return p;
}