#pragma once
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
char* parse_utf8(const char* pstr, char** result) {
  char* ret = pstr;
  unsigned u;
  ret = parse_hex4(pstr, &u);
  if (ret - pstr != 4)
    return ret;
  if (u >= 0xD800 && u <= 0xDBFF) {
    if (*ret != '\\' || *++ret != 'u')
      return ret;
    ret++;
    unsigned u2;
    char* ret2;
    ret2 = parse_hex4(ret, &u2);
    if (ret2 - ret != 4 || u2 < 0xDC00 || u2 > 0xDFFF)
      return ret2;
    u = (((u - 0xD800) << 10) | (u2 - 0xDC00)) + 0x10000;
  }

  *result = encode_utf8(u);
  return ret;
}