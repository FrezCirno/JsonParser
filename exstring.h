#pragma once

typedef struct _string {
    int   length;
    int   capacity;
    char *value;
} * string;

string init_str();

void push_back(string x, const char c);

void push_back_str(string x, const char *s);

string concat(string dst, string src);

void free_str(string s);