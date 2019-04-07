#pragma once

typedef struct _string {
  char* value;
  int length;
  int capacity;
} * string;

string init_str();

string push_back(string x, const char c);

string push_back_str(string x, const char* s, int len);

string exstrcat(string dst, string src);

void free_str(string s);