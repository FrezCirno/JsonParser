#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "jsonparser.h"

string random_str() {//UTF8格式
    int i, length;
    unsigned value;
    string ret = init_str();
    length = rand() % 5 + 5;
    for (i = 0; i < length; i++) {
        if (rand()%10==1)
            value = rand() * rand() % 0xffff;
        else
            value = rand() % 100 + 27;
        concat(ret, unicode_utf8(value));
    }
    return ret;
}

double random_num() {
    double ret;
    ret = rand();
    return ret;
}

node* json_gen(int elem_count) {
    node *ret, *p;
    int count, used, first;

    p = ret = (node*)malloc(sizeof(node));
    ret->next = NULL;
    ret->name = NULL;
    ret->value = 0L;

    first = 1;
    used = 0;
    if (elem_count == 1) {//生成单个结点
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
    }
    else {//生成2+个结点
        ret->type = rand() % 2 ? OBJECT : ARRAY;

        used = 1;//数组或对象本身也算一个结点
        while (used < elem_count) {
            //生成一个随机数count表示子节点的度数
            if (elem_count - used == 1) count = 1;
            //从1到elem_count - used的随机数
            else count = rand() % (elem_count - used - 1) + 1;

            //更新已使用节点数
            used += count;
                
            //插入子节点
            if (first) {//首个子节点,插入到child_head下
                first = 0;
                p->child_head = json_gen(count);
                p = p->child_head;
            }
            else {//非首个子节点,插入到next下
                p->next = json_gen(count);
                p = p->next;
            }
            //给子节点加上key
            if(ret->type==OBJECT) p->name = random_str();
            else p->name = NULL;
        }
    }
    return ret;
}