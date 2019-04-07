#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "jsonparser.h"
int main() {
  srand(time(NULL));
  json_node* random;
  char* random_str;
  json_node* parse_result;
  char* result_str;

  random = generate(20);           //生成具有20个叶子节点的json
  random_str = stringify(random);  //转换成字符串

  printf("%s\n", random_str);

  parse_result = parse(random_str);      //解析
  result_str = stringify(parse_result);  //再次转换

  printf("%s\n", result_str);

  return 0;
}