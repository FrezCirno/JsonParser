#pragma once

typedef struct _string {
    char *value;
    int   length;
    int   capacity;
} * string;

string init_str();

void push_back(string x, const char c);

void push_back_str(string x, const char *s, int len);

string exstrcat(string dst, string src);

void free_str(string s);