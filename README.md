# JsonParser
C语言实现的Json解析器,支持json的解析、生成，json对象的访问与修改.
还附赠一个随机json生成器，用于测试

## 数据结构定义：
typedef struct _js_node {
  node_type type;           //节点的类型,分为 FALSE,  TRUE,  NUMBER,  STRING,  ARRAY,  OBJECT,  NUL,  UNKNOWN 八种
  string name;              //节点的Tag/Key,内部使用utf-8编码存储转义字符
  union {
    double value;                   //数值类
    struct _js_node *child_head;    // object或者array,指向第一个子节点
    string val_str;                 //字符串
  };
  struct _js_node *next;    //下一个结点
} node;

## 特点:
1.解析模块使用有限状态自动机实现
2.生成模块简单使用递归实现
3.支持从0x0-0x10FFFF的转义字符序列
