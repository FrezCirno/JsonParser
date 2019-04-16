#pragma once
#include "exstring.h"//动态字符串支持

//其中json_node内部存储字符串用的是utf-8格式,
//因为utf-8可能含有各种字符,故用动态字符串存储
//stringnify之后输出成json_hex格式,使用char*存储
#include "utf-8.h"
typedef enum {
  FALSE,
  TRUE,
  NUMBER,
  STRING,
  ARRAY,
  OBJECT,
  NUL,
  UNKNOWN
} node_type;
typedef struct _js_node {
  node_type type;
  string name;
  union {
    double value;           //数值类
      struct _js_node *child_head;  // object或者array
    string val_str;         //字符串
  };
  struct _js_node *next;  //下一个结点
}node;


/*
 * 解析json
 * */
node* json_parse(const char* json_str);


/*
 * 生成json
 * */
char* json_strify(const node* root);


/**
 * 随机生成有elem_count个结点的json
 * **/
node* json_gen(int elem_count);

node* json_insert(node*root, int index, node*elem);

node* json_getnext(node*root);

node* json_getchild(node*root, int index);