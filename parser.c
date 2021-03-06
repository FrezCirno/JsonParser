#include "jsonparser.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char *g_json_str = NULL;
static char *g_pstr = NULL;

typedef enum {
    Token_Colon,
    Token_Comma,
    Token_LeftBrace,
    Token_RightBrace,
    Token_LeftBracket,
    Token_RightBracket,
    Token_String,
    Token_Number,
    Token_True,
    Token_False,
    Token_Null,
    Token_End,
    Token_Unknown,
    Token_Error
} token_t;

typedef enum {
    State_Start,
    State_Object,
    State_Key,
    State_Colon,
    State_Value,
    State_ObjectComma,
    State_Array,
    State_Element,
    State_ArrayComma,
    State_End
} state;

typedef struct {
    token_t type;
    union {
        string str_val;
        double num_val;
    };
} token;
/*
typedef enum {
    UNKNOWN_TOKEN,
    ILLEGAL_TOKEN,

} error_type;

char *error(enum error_type err_no)
{
    static char *error_msg[] = "";
    fprintf(stderr, "%s", error_msg[err_no]);
}
*/
string parse_str(const char *pstr, char **EndPtr)
{  //可能会有\u0000的字符,转换成utf-8为'\0',so返回值必须是string
    int    end = 0;
    string ret = init_str();
    while (!end) {
        switch (*pstr) {
            case '\"':
                pstr++;
                end = 1;
                break;
            case 92:
                pstr++;
                switch (*pstr) {
                    case 92:
                        pstr++;
                        push_back(ret, '\\');
                        break;
                    case '/':
                        pstr++;
                        push_back(ret, '/');
                        break;
                    case 'b':
                        pstr++;
                        push_back(ret, '\b');
                        break;
                    case 'f':
                        pstr++;
                        push_back(ret, '\f');
                        break;
                    case 't':
                        pstr++;
                        push_back(ret, '\t');
                        break;
                    case 'n':
                        pstr++;
                        push_back(ret, '\n');
                        break;
                    case 'r':
                        pstr++;
                        push_back(ret, '\r');
                        break;
                    case '\"':
                        pstr++;
                        push_back(ret, '\"');
                        break;
                    case 'u':
                        pstr++;
                        char  *result;
                        string tmp = jsonhex_utf8(pstr, &result);
                        concat(ret, tmp);
                        free_str(tmp);
                        if (result - pstr != 4 && result - pstr != 10) {
                            *EndPtr = result;
                            break;
                        }
                        pstr = result;
                        break;
                    default: *EndPtr = pstr; return ret;
                }
                break;
            case '\0': 
                end = 1;
                break;
            default:
                push_back(ret, *pstr);
                pstr++;
                break;
        }
    }
    *EndPtr = pstr;
    return ret;
}

token getToken()
{
    token ret;
    char *endptr;
    while (1) {
        switch (*g_pstr) {
            case '\0': ret.type = Token_End; break;
            case ' ':
            case '\n':
            case '\t':
            case '\r': g_pstr++; continue;
            case '{':
                g_pstr++;
                ret.type = Token_LeftBrace;
                break;
            case '}':
                g_pstr++;
                ret.type = Token_RightBrace;
                break;
            case '[':
                g_pstr++;
                ret.type = Token_LeftBracket;
                break;
            case ']':
                g_pstr++;
                ret.type = Token_RightBracket;
                break;
            case ':':
                g_pstr++;
                ret.type = Token_Colon;
                break;
            case ',':
                g_pstr++;
                ret.type = Token_Comma;
                break;
            case '-':
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
                ret.num_val = strtod(g_pstr, &endptr);
                if (endptr != NULL) {
                    ret.type = Token_Number;
                    g_pstr = endptr;
                } else
                    ret.type = Token_Error;
                break;
            case '\"':
                g_pstr++;
                ret.str_val = parse_str(g_pstr, &endptr);
                if (endptr != NULL) {
                    ret.type = Token_String;
                    g_pstr = endptr;
                } else
                    ret.type = Token_Error;
                break;
            case 't':
                if (*++g_pstr == 'r' && *++g_pstr == 'u' && *++g_pstr == 'e') {
                    g_pstr++;
                    ret.type = Token_True;
                } else
                    ret.type = Token_Error;
                break;
            case 'f':
                if (*++g_pstr == 'a' && *++g_pstr == 'l' && *++g_pstr == 's' &&
                    *++g_pstr == 'e') {
                    g_pstr++;
                    ret.type = Token_False;
                } else
                    ret.type = Token_Error;
                break;
            case 'n':
                if (*++g_pstr == 'u' && *++g_pstr == 'l' && *++g_pstr == 'l') {
                    g_pstr++;
                    ret.type = Token_Null;
                } else
                    ret.type = Token_Error;
                break;
            default:
                printf("Unknown char\n");
                g_pstr++;
                ret.type = Token_Unknown;
                break;
        }  // switch
        return ret;
    }  // while
}

node *json_parse(const char *json_str)
{
    node *root = NULL, *this_node = NULL;
    token      this_token;
    state      now_state = State_Start;

    node *node_stack[50];
    state      state_stack[50];

    if (json_str == NULL) return NULL;
    g_pstr = g_json_str = json_str;
    int top = 0, end = 0;

    while (!end) {
        this_token = getToken();
        switch (this_token.type) {
        case Token_LeftBrace:
            switch (now_state) {
            case State_Start:  // ok
                this_node = root =
                    (node *)malloc(sizeof(node));
                this_node->type = OBJECT;
                this_node->name = NULL;
                this_node->child_head = NULL;
                this_node->next = NULL;

                now_state = State_Object;
                break;
            case State_Array:  // ok
                node_stack[top] = this_node;
                state_stack[top++] = (this_node->name == NULL) ?
                    State_Element :
                    State_Value;
                //有名字一定在object中,否则在array中
                this_node->child_head =
                    (node *)malloc(sizeof(node));
                this_node = this_node->child_head;

                this_node->type = OBJECT;
                this_node->name = NULL;
                this_node->child_head = NULL;
                this_node->next = NULL;

                now_state = State_Object;
                break;
            case State_Colon:  // ok
                this_node->type = OBJECT;

                this_node->value = 0L;
                this_node->next = NULL;
                now_state = State_Object;
                break;
            case State_ArrayComma:  // ok
                this_node->next =
                    (node *)malloc(sizeof(node));
                this_node = this_node->next;

                this_node->type = OBJECT;
                this_node->name = NULL;
                this_node->child_head = NULL;
                this_node->next = NULL;

                now_state = State_Object;
                break;
            default: goto PARSE_ERROR;
            }
            break;
        case Token_LeftBracket:
            switch (now_state) {
            case State_Start:  // ok
                this_node = root =
                    (node *)malloc(sizeof(node));

                this_node->type = ARRAY;
                this_node->name = NULL;
                this_node->child_head = NULL;
                this_node->next = NULL;

                now_state = State_Array;
                break;
            case State_Array:  // ok
                node_stack[top] = this_node;
                state_stack[top++] = (this_node->name == NULL) ?
                    State_Element :
                    State_Value;
                //有名字一定在object中,否则在array中
                this_node->child_head =
                    (node *)malloc(sizeof(node));
                this_node = this_node->child_head;

                this_node->name = NULL;
                this_node->type = ARRAY;
                this_node->child_head = NULL;
                this_node->next = NULL;

                break;
            case State_Colon:  // ok
                this_node->type = ARRAY;

                this_node->value = 0L;
                this_node->next = NULL;
                now_state = State_Array;
                break;
            case State_ArrayComma:  // ok
                this_node->next =
                    (node *)malloc(sizeof(node));
                this_node = this_node->next;

                this_node->type = ARRAY;
                this_node->name = NULL;
                this_node->child_head = NULL;
                this_node->next = NULL;

                now_state = State_Array;
                break;
            default: goto PARSE_ERROR;
            }
            break;
        case Token_RightBrace:
            switch (now_state) {
            case State_Object:  //*****
                if (this_node->name == NULL)
                    now_state = State_Element;
                else
                    now_state = State_Value;
                break;
            case State_Value:  // ok
                this_node = node_stack[--top];
                now_state = state_stack[top];

                break;
            case State_ObjectComma:  //**ok
                this_node = node_stack[--top];
                now_state = state_stack[top];
                break;
            default: goto PARSE_ERROR;
            }
            break;
        case Token_RightBracket:
            switch (now_state) {
            case State_Array:  //*****
                if (this_node->name == NULL)
                    now_state = State_Element;
                else
                    now_state = State_Value;
                break;
            case State_Element:  // ok
                this_node = node_stack[--top];
                now_state = state_stack[top];
                break;
            case State_ArrayComma:  // ok
                this_node = node_stack[--top];
                now_state = state_stack[top];
                break;
            default: goto PARSE_ERROR;
            }
            break;
        case Token_Colon:
            switch (now_state) {
            case State_Key: now_state = State_Colon; break;
            default: goto PARSE_ERROR;
            }
            break;
        case Token_Comma:
            switch (now_state) {
            case State_Value: now_state = State_ObjectComma; break;
            case State_Element:
                now_state = State_ArrayComma;
                break;
                // case State_ObjectComma:

                // this_node->next = ( json_node*)malloc(
                // sizeof(json_node)); this_node = this_node->next;

                // this_node->type = NUL;
                // this_node->next = NULL;

                // break;
            case State_ArrayComma:
                this_node->next =
                    (node *)malloc(sizeof(node));
                this_node = this_node->next;

                this_node->type = NUL;
                this_node->next = NULL;

                break;
            default: goto PARSE_ERROR;
            }
            break;
        case Token_String:
            switch (now_state) {
            case State_Object:  // ok
                node_stack[top] = this_node;
                state_stack[top++] = (this_node->name == NULL) ?
                    State_Element :
                    State_Value;
                //有名字一定在object中,否则在array中
                this_node->child_head =
                    (node *)malloc(sizeof(node));
                this_node = this_node->child_head;

                this_node->name = this_token.str_val;
                this_node->next = NULL;

                now_state = State_Key;
                break;
            case State_Array:  // ok
                node_stack[top] = this_node;
                state_stack[top++] = (this_node->name == NULL) ?
                    State_Element :
                    State_Value;
                //根据名字判断状态
                this_node->child_head =
                    (node *)malloc(sizeof(node));
                this_node = this_node->child_head;

                this_node->type = STRING;
                this_node->name = NULL;
                this_node->val_str = this_token.str_val;
                this_node->next = NULL;

                now_state = State_Element;
                break;
            case State_Colon:  // ok
                this_node->type = STRING;
                this_node->val_str = this_token.str_val;

                now_state = State_Value;
                break;
            case State_ObjectComma:  // ok
                this_node->next =
                    (node *)malloc(sizeof(node));
                this_node = this_node->next;

                this_node->name = this_token.str_val;
                this_node->value = 0L;
                this_node->next = NULL;

                now_state = State_Key;
                break;
            case State_ArrayComma:  // ok
                this_node->next =
                    (node *)malloc(sizeof(node));
                this_node = this_node->next;

                this_node->type = STRING;
                this_node->name = NULL;
                this_node->val_str = this_token.str_val;
                this_node->next = NULL;

                now_state = State_Element;
                break;
            default: goto PARSE_ERROR;
            }
            break;
        case Token_Number:
            switch (now_state) {
            case State_Array:  // ok
                node_stack[top] = this_node;
                state_stack[top++] = (this_node->name == NULL) ?
                    State_Element :
                    State_Value;
                //根据名字判断状态
                this_node->child_head =
                    (node *)malloc(sizeof(node));
                this_node = this_node->child_head;

                this_node->type = NUMBER;
                this_node->name = NULL;
                this_node->value = this_token.num_val;
                this_node->next = NULL;

                now_state = State_Element;
                break;
            case State_Colon:  // ok
                this_node->type = NUMBER;
                this_node->value = this_token.num_val;
                this_node->next = NULL;

                now_state = State_Value;
                break;
            case State_ArrayComma:  // ok
                this_node->next =
                    (node *)malloc(sizeof(node));
                this_node = this_node->next;

                this_node->type = NUMBER;
                this_node->name = NULL;
                this_node->value = this_token.num_val;
                this_node->next = NULL;

                now_state = State_Element;
                break;
            default: goto PARSE_ERROR;
            }
            break;
        case Token_True:
            switch (now_state) {
            case State_Colon:  // ok
                this_node->type = TRUE;
                this_node->value = 0L;
                this_node->next = NULL;
                now_state = State_Value;
                break;
            case State_Array:  // ok
                node_stack[top] = this_node;
                state_stack[top++] = (this_node->name == NULL) ?
                    State_Element :
                    State_Value;
                //根据名字判断状态
                this_node->child_head =
                    (node *)malloc(sizeof(node));
                this_node = this_node->child_head;

                this_node->type = TRUE;
                this_node->name = NULL;
                this_node->value = 0L;
                this_node->next = NULL;

                now_state = State_Element;
                break;
            case State_ArrayComma:  // ok
                this_node->next =
                    (node *)malloc(sizeof(node));
                this_node = this_node->next;

                this_node->type = TRUE;
                this_node->name = NULL;
                this_node->value = 0L;
                this_node->next = NULL;

                now_state = State_Element;
                break;
            default: goto PARSE_ERROR;
            }
            break;
        case Token_False:
            switch (now_state) {
            case State_Colon:  // ok
                this_node->type = FALSE;
                this_node->value = 0L;
                this_node->next = NULL;

                now_state = State_Value;
                break;

            case State_Array:  // ok
                node_stack[top] = this_node;
                state_stack[top++] = (this_node->name == NULL) ?
                    State_Element :
                    State_Value;
                //根据名字判断状态
                this_node->child_head =
                    (node *)malloc(sizeof(node));
                this_node = this_node->child_head;

                this_node->type = FALSE;
                this_node->name = NULL;
                this_node->value = 0L;
                this_node->next = NULL;

                now_state = State_Element;
                break;
            case State_ArrayComma:  // ok
                this_node->next =
                    (node *)malloc(sizeof(node));
                this_node = this_node->next;

                this_node->type = FALSE;
                this_node->name = NULL;
                this_node->value = 0L;
                this_node->next = NULL;

                now_state = State_Element;
                break;
            default: goto PARSE_ERROR;
            }
            break;
        case Token_Null:
            switch (now_state) {
            case State_Colon:  // ok
                this_node->type = NUL;
                this_node->value = 0L;
                this_node->next = NULL;
                now_state = State_Value;
                break;

            case State_Array:  // ok
                node_stack[top] = this_node;
                state_stack[top++] = (this_node->name == NULL) ?
                    State_Element :
                    State_Value;
                //根据名字判断状态
                this_node->child_head =
                    (node *)malloc(sizeof(node));
                this_node = this_node->child_head;

                this_node->type = NUL;
                this_node->name = NULL;
                this_node->value = 0L;
                this_node->next = NULL;

                now_state = State_Element;
                break;
            case State_ArrayComma:  // ok
                this_node->next =
                    (node *)malloc(sizeof(node));
                this_node = this_node->next;

                this_node->type = NUL;
                this_node->name = NULL;
                this_node->value = 0L;
                this_node->next = NULL;

                now_state = State_Element;
                break;
            default: goto PARSE_ERROR;
            }
            break;
        case Token_Error: end = 1;
        case Token_Unknown: end = 1;
        case Token_End: end = 1;
        }  // switch
#ifdef DEBUG
        printf("%d\n", now_state);
#endif   
    }
    return root;
PARSE_ERROR:
    // to-do
    return root;
}