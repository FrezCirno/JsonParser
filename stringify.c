#include "jsonparser.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

char *stringify(const json_node *root)
{
    char *ret, *pstr, *tmp;
    json_node *p;
    int first;

    pstr = ret = (char *)malloc(500 * sizeof(char));
    memset(ret, 0, 500);
    first = 1;

    if (root == NULL)
        return NULL;

    if (root->name != NULL)
    {
        *pstr++ = '\"';
        strcpy(pstr, root->name);
        pstr += strlen(root->name);
        *pstr++ = '\"';
        *pstr++ = ':';
    }

    switch (root->type)
    {
    case OBJECT:
        *pstr++ = '{';

        p = root->child_head;
        while (p != NULL)
        {
            if (!first)
                *pstr++ = ',';
            else
                first = 0;
            tmp = stringify(p);
            strcpy(pstr, tmp);
            pstr += strlen(tmp);
            free(tmp);
            p = p->next;
        }
        *pstr++ = '}';
        break;
    case ARRAY:
        *pstr++ = '[';

        p = root->child_head;
        while (p != NULL)
        {
            if (!first)
                *pstr++ = ',';
            else
                first = 0;

            tmp = stringify(p);
            strcpy(pstr, tmp);
            pstr += strlen(tmp);
            free(tmp);
            p = p->next;
        }
        *pstr++ = ']';
        break;
    case STRING:
        *pstr++ = '\"';
        strcpy(pstr, root->val_str);
        pstr += strlen(root->val_str);
        *pstr++ = '\"';
        break;
    case NUMBER:
        tmp = (char *)malloc(100 * sizeof(char));
        sprintf(tmp, "%g", root->value);
        strcpy(pstr, tmp);
        pstr += strlen(tmp);
        break;
    case TRUE:
        strcpy(pstr, "true");
        pstr += 4;
        break;
    case FALSE:
        strcpy(pstr, "false");
        pstr += 5;
        break;
    case NUL:
        strcpy(pstr, "null");
        pstr += 4;
        break;
    }
    return ret;
}