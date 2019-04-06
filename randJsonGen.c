#include "jsonparser.h"
#include <time.h>
#include <stdio.h>
#include <stdlib.h>

char *random_str()
{
    int i, length;
    char chars[] = "0123456789abcdefghighlmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";

    length = rand() % 15 + 5;
    char *ret = (char *)malloc(length * sizeof(char));

    for (i = 0; i < length; i++)
        ret[i] = chars[rand() % 63];
    ret[i] = '\0';
    return ret;
}
double random_num()
{
    double ret;
    ret = rand();
    return ret;
}

json_node *generate(int elem_count)
{
    json_node *ret, *p;
    int count, used, first;

    first = 1;
    used = 0;
    p = ret = (json_node *)malloc(sizeof(json_node));
    ret->next = NULL;
    ret->name = NULL;
    ret->value = 0L;

    if (elem_count == 1)
    {
        switch (rand() % 5)
        {
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
    }
    else
    {
        switch (rand() % 2)
        {
        case 0:
            ret->type = OBJECT;
            while (used < elem_count)
            {
                if (elem_count - used == 1)
                    count = 1;
                else
                    count = rand() % (elem_count - used - 1) + 1; //从1到elem_count - used
                used += count;
                if (first)
                {
                    first = 0;
                    p->child_head = generate(count);
                    p->child_head->name = random_str();
                    p = p->child_head;
                }
                else
                {
                    p->next = generate(count);
                    p->next->name = random_str();
                    p = p->next;
                }
            }
            break;
        case 1:
            ret->type = ARRAY;
            while (used < elem_count)
            {
                if (elem_count - used == 1)
                    count = 1;
                else
                    count = rand() % (elem_count - used - 1) + 1; //从1到elem_count - used
                used += count;
                if (first)
                {
                    first = 0;
                    p->child_head = generate(count);
                    p->child_head->name = NULL;
                    p = p->child_head;
                }
                else
                {
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