#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "jsonparser.h"

string stringify(const json_node* root) {
  string ret, pstr, tmp;
  char* tmp_s;
  json_node* p;
  int first;

  if (root == NULL)
    return NULL;

  pstr = ret = init_str();
  tmp_s = (char*)malloc(100 * sizeof(char));
  first = 1;

  if (root->name != NULL) {
    push_back(pstr, '\"');
    strncpy(pstr, root->name->value, root->name->length);
    pstr += root->name->length;
    push_back(pstr, '\"');
    push_back(pstr, ':');
  }

  switch (root->type) {
    case OBJECT:
      push_back(pstr, '{');

      p = root->child_head;
      while (p != NULL) {
        if (first)
          first = 0;
        else
          push_back(pstr, ',');
        tmp = stringify(p);
        exstrcat(pstr, tmp);
        free_str(tmp);
        p = p->next;
      }
      push_back(pstr, '}');
      break;
    case ARRAY:
      push_back(pstr, '[');

      p = root->child_head;
      while (p != NULL) {
        if (first)
          push_back(pstr, ',');
        else
          first = 0;

        tmp = stringify(p);
        exstrcat(pstr, tmp);
        free_str(tmp);
        p = p->next;
      }
      push_back(pstr, ']');
      break;
    case STRING:
      push_back(pstr, '\"');
      exstrcat(pstr, root->val_str);
      push_back(pstr, '\"');
      break;
    case NUMBER:
      //   printf("%d", tmp_s[99]);
      //   memset(tmp_s, 0, 100);
      sprintf(tmp_s, "%g", root->value);
      push_back_str(pstr, tmp_s, strlen(tmp_s));
      break;
    case TRUE:
      strcpy(tmp_s, "true");
      push_back_str(pstr, tmp_s, strlen(tmp_s));
      break;
    case FALSE:
      strcpy(tmp_s, "false");
      push_back_str(pstr, tmp_s, strlen(tmp_s));
      break;
    case NUL:
      strcpy(tmp_s, "null");
      push_back_str(pstr, tmp_s, strlen(tmp_s));
      break;
  }

  free(tmp_s);
  return ret;
}