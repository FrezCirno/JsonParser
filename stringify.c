#include "jsonparser.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *json_strify(const node *root)
{
    string     tmp_str;
    char *     ret_str, *child;
    node *p;
    int        first;

    if (root == NULL) return NULL;

    tmp_str = init_str();
    first = 1;

    if (root->name != NULL) {
        push_back(tmp_str, '\"');
        push_back_str(tmp_str, utf8str_jsonstr(root->name));
        push_back(tmp_str, '\"');
        push_back(tmp_str, ':');
    }
    switch (root->type) {
        case OBJECT:
            push_back(tmp_str, '{');
            p = root->child_head;
            while (p != NULL) {
                if (first)
                    first = 0;
                else
                    push_back(tmp_str, ',');

                child = json_strify(p);
                if (child != NULL) {
                    push_back_str(tmp_str, child);
                    free(child);
                }
                p = p->next;
            }
            push_back(tmp_str, '}');
            break;
        case ARRAY:
            push_back(tmp_str, '[');

            p = root->child_head;
            while (p != NULL) {
                if (first)
                    first = 0;
                else
                    push_back(tmp_str, ',');
                child = json_strify(p);
                if (child != NULL) {
                    push_back_str(tmp_str, child);
                    free(child);
                }
                p = p->next;
            }
            push_back(tmp_str, ']');
            break;
        case STRING:
            push_back(tmp_str, '\"');
            push_back_str(tmp_str, utf8str_jsonstr(root->val_str));
            push_back(tmp_str, '\"');
            break;
        case NUMBER:
            child = (char *)malloc(10 * sizeof(char));
            sprintf(child, "%g", root->value);
            push_back_str(tmp_str, child);
            free(child);
            break;
        case TRUE:
            push_back_str(tmp_str, "true");
            break;
        case FALSE:
            push_back_str(tmp_str, "false");
            break;
        case NUL:
            push_back_str(tmp_str, "null");
            break;
    }
    //只保留char*部分
    push_back(tmp_str, '\0');
    ret_str = tmp_str->value;
    free(tmp_str);
    return ret_str;
}

void json_prettify(const node*root) {

}