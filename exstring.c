#include "exstring.h"
#include<stdio.h>
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
            return;
        }
        x->value = new_val;
    }
    x->value[x->length++] = c;
}

void push_back_str(string dst, const char *src)
{
    char *new_val;
    int   src_len = strlen(src);
    if (dst == NULL || src == NULL || src_len == 0) return;
    if (dst->length + src_len >= dst->capacity) {
        new_val = (char *)realloc(dst->value, (dst->capacity = 2 * (dst->length + src_len)) *
            sizeof(char));
        if (new_val == NULL) {
            printf("memory overflow\n");
            free(dst->value);
            return;
        }
        dst->value = new_val;
    }
    for (int i = 0; i < src_len; i++)
        dst->value[dst->length++] = src[i];
    //printf("pushbackstr <%s> to char* s\n", s);
    return;
}

string concat(string dst, string src)
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
