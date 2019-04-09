#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "jsonparser.h"

char* stringify(const json_node* root) {
    string pstr,tmp_s;
    char *ret, *tmp;
    json_node* p;
    int first;

    if (root == NULL)
        return NULL;

    pstr = init_str();
    first = 1;

    if (root->name != NULL) {
        push_back(pstr, '\"');
        exstrcat(pstr, encode(root->name));
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
            push_back_str(pstr, tmp,strlen(tmp));
            free(tmp);
            p = p->next;
        }
        push_back(pstr, '}');
        break;
    case ARRAY:
        push_back(pstr, '[');

        p = root->child_head;
        while (p != NULL) {
            if (first)
                first = 0;
            else
                push_back(pstr, ',');
            tmp = stringify(p);
            push_back_str(pstr, tmp, strlen(tmp));
            free(tmp);
            p = p->next;
        }
        push_back(pstr, ']');
        break;
    case STRING:
        push_back(pstr, '\"');
        tmp = encode(root->val_str);
        exstrcat(pstr, tmp);
        push_back(pstr, '\"');
        break;
    case NUMBER:
        tmp = (char*)malloc(10 * sizeof(char));
        sprintf(tmp, "%g", root->value);
        push_back_str(pstr, tmp, strlen(tmp));
        free(tmp);
        break;
    case TRUE:
        tmp = (char*)malloc(10 * sizeof(char));
        strcpy(tmp, "true");
        push_back_str(pstr, tmp, strlen(tmp));
        free(tmp);
        break;
    case FALSE:
        tmp = (char*)malloc(10 * sizeof(char));
        strcpy(tmp, "false");
        push_back_str(pstr, tmp, strlen(tmp));
        free(tmp);
        break;
    case NUL:
        tmp = (char*)malloc(10 * sizeof(char));
        strcpy(tmp, "null");
        push_back_str(pstr, tmp, strlen(tmp));
        free(tmp);
        break;
    }

    ret = (char*)malloc(pstr->length * sizeof(char));
    strncpy(ret, pstr->value, pstr->length);
    free_str(pstr);
    return ret;
}