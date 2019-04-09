#ifndef JSONPARSER_H
#define JSONPARSER_H
#include "exstring.h"
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
typedef struct _json_node json_node;
struct _json_node {
  node_type type;
  string name;
  union {
    double value;           //数值类
    json_node* child_head;  // object或者array
    string val_str;         //字符串
  };
  json_node* next;
};
/*
 * 解析json
 * */
json_node* parse(const char* json_str);
/*
 * 生成json
 * */
char* stringify(const json_node* root);
/**
 * 随机生成有elem_count个叶子的json
 * **/
json_node* generate(int elem_count);
#endif