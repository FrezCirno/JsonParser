#include "exstring.h"
#include <stdlib.h>

string init_str()
{
    string ret = (string)malloc(sizeof(struct _string));
    ret->value = (char *)malloc(10 * sizeof(char));
    memset(ret->value, 0, 10);
    ret->length = 0;
    ret->capacity = 10;
    return ret;
}

void push_back(string x, const char c)
{
    char *new_val = NULL;
    if (x == NULL) return;
    if (x->length == x->capacity) {
        new_val = (char *)realloc(x->value, (x->capacity *= 2) * sizeof(char));
        if (new_val == NULL) {
            printf("ERROR");
            free(x->value);
            return NULL;
        }
        x->value = new_val;
    }
    x->value[x->length++] = c;
}

void push_back_str(string x, const char *s, int len)
{
    char *new_val;
    if (x == NULL || s == NULL || len == 0) return;
    if (x->length + len >= x->capacity) {
        new_val = (char *)realloc(x->value, (x->capacity = x->length + len) *
                                                sizeof(char));
        if (new_val == NULL) {
            free(x->value);
            return NULL;
        }
        x->value = new_val;
    }
    for (int i = 0; i < len; i++) x->value[x->length++] = s[i];
}

string exstrcat(string dst, string src)
{
    char *new_val;
    if (dst == NULL || src == NULL) return NULL;
    if (dst->length + src->length >= dst->capacity) {
        new_val = (char *)realloc(dst->value,
                                  (dst->capacity = dst->length + src->length) *
                                      sizeof(char));
        if (new_val == NULL) {
            free(dst->value);
            return NULL;
        }
        dst->value = new_val;
    }
    for (int i = 0; i < src->length; i++)
        dst->value[dst->length++] = src->value[i];
    return dst;
}

void free_str(string s)
{
    free(s->value);
    free(s);
}
