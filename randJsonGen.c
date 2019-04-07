#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "jsonparser.h"

string random_str() {
  int i;
  char chars[] =
      "0123456789abcdefghighlmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
  char trans[] = "\"bfnr /\\t";
  char hex[] = "1234567890abcdefABCDEF";
  string ret = init_str();
  for (i = 0; i < ret->length; i++)
    if (rand() % 10 == 1) {
      push_back(ret, '/');
      push_back(ret, trans[rand() % 10]);
      if (ret->value[ret->length - 1] == 'u') {
        push_back(ret, hex[rand() % 22]);
        push_back(ret, hex[rand() % 22]);
        push_back(ret, hex[rand() % 22]);
        push_back(ret, hex[rand() % 22]);
      }
    } else
      push_back(ret, chars[rand() % 62]);
  return ret;
}

double random_num() {
  double ret;
  ret = rand();
  return ret;
}

json_node* generate(int elem_count) {
  json_node *ret, *p;
  int count, used, first;

  first = 1;
  used = 0;
  p = ret = (json_node*)malloc(sizeof(json_node));
  ret->next = NULL;
  ret->name = NULL;
  ret->value = 0L;

  if (elem_count == 1) {
    switch (rand() % 5) {
      case 0:
        ret->type = STRING;
        ret->val_str = random_str();
        break;
      case 1:
        ret->type = NUMBER;
        ret->value = random_num();
        break;
      case 2:
        ret->type = TRUE;
        break;
      case 3:
        ret->type = FALSE;
        break;
      case 4:
        ret->type = NUL;
        break;
    }
  } else {
    switch (rand() % 2) {
      case 0:
        ret->type = OBJECT;
        while (used < elem_count) {
          if (elem_count - used == 1)
            count = 1;
          else
            count =
                rand() % (elem_count - used - 1) + 1;  //从1到elem_count - used
          used += count;
          if (first) {
            first = 0;
            p->child_head = generate(count);
            p->child_head->name = random_str();
            p = p->child_head;
          } else {
            p->next = generate(count);
            p->next->name = random_str();
            p = p->next;
          }
        }
        break;
      case 1:
        ret->type = ARRAY;
        while (used < elem_count) {
          if (elem_count - used == 1)
            count = 1;
          else
            count =
                rand() % (elem_count - used - 1) + 1;  //从1到elem_count - used
          used += count;
          if (first) {
            first = 0;
            p->child_head = generate(count);
            p->child_head->name = NULL;
            p = p->child_head;
          } else {
            p->next = generate(count);
            p->next->name = NULL;
            p = p->next;
          }
        }
        break;
    }
  }
  return ret;
}