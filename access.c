#include"jsonparser.h"
#include<stdio.h>
node* json_getchild(node*root, int index) {
    if (index < 0)return NULL;

    node*tmp = root->child_head;
    while (tmp != NULL && index-- > 0)
        tmp = tmp->next;

    return tmp;
}

node* json_getnext(node*root) {
    return root->next;
}

node* json_insert(node*root, int index, node*elem) {
    if (index < 0)return NULL;

    node*tmp = root->child_head;

    if (index == 0) {
        elem->next = root->child_head;
        root->child_head = elem;
    }
    else {
        while (tmp != NULL && index-- > 0)
            tmp = tmp->next;

        elem->next = tmp->next;
        tmp->next = elem;
    }

    return root;
}