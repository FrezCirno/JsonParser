#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "jsonparser.h"
int main() {
    node* rand_json, *parse_result;
    char* json_str, *result_str;
    int select, count;
    char s[1000];

    srand((unsigned)time(NULL));
    printf("LightJson 演示Demo:\n");
    printf("1.生成随机Json.\n");
    printf("2.Json解析\n");
    scanf("%d", &select);

    switch (select) {
    case 1:
        printf("请输入随机生成的json的节点数:\n");
        scanf("%d", &count);
        rand_json = json_gen(count);           //生成具有20个节点的json
        json_str = json_strify(rand_json);  //转换成字符串
        printf("%s\n\n", json_str);
        break;
    case 2:
        fgets(s, 1000, stdin);
        node*result = json_parse(s);
        json_str = json_strify(rand_json);  //转换成字符串
        printf("%s\n\n", json_str);
        break;
    }
    return 0;
}