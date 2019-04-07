#include "exstring.h"
#include <stdlib.h>

string init_str() {
  string ret = (string)malloc(sizeof(struct _string));
  ret->value = (char*)malloc(10 * sizeof(char));
  ret->length = 0;
  ret->capacity = 10;
  return ret;
}

string push_back(string x, const char c) {
  if (x->length == x->capacity)
    realloc(x->value, (x->capacity = 2 * x->capacity) * sizeof(char));
  x->value[x->length++] = c;
}

string push_back_str(string x, const char* s, int len) {
  if (x->length + len >= x->capacity) {
    realloc(x->value, (x->capacity = x->length + len) * sizeof(char));
  }
  for (int i = 0; i < len; i++)
    x->value[x->length++] = s[i];
  return x;
}

string exstrcat(string dst, string src) {
  if (dst->length + src->length >= dst->capacity) {
    realloc(dst->value,
            (dst->capacity = dst->length + src->length) * sizeof(char));
  }
  for (int i = 0; i < src->length; i++)
    dst->value[dst->length++] = src->value[i];
  return dst;
}

void free_str(string s) {
  free(s->value);
  free(s);
}
