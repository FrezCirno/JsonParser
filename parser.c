#include "jsonparser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef enum
{
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
} token_type;
typedef enum
{
    State_Start,

    State_Object,
    State_Key,
    State_Colon,
    State_Value,
    State_ObjectComma,
    State_ObjectEnd,

    State_Array,
    State_Element,
    State_ArrayComma,
    State_ArrayEnd,

    State_End
} state;
typedef struct _token
{
    token_type type;
    union {
        char *str_val;
        double num_val;
    };
} token;

token tokenizer(const char *json_str)
{
    static char *pstr = NULL;
    if (pstr == NULL)
        pstr = json_str;
    token ret;

    char *start;
    char *endptr;

    while (*pstr != '\0')
    {
        switch (*pstr)
        {
        case ' ':
        case '\n':
        case '\t':
        case '\r':
            pstr++;
            continue;
        case '{':
            ret.type = Token_LeftBrace;
            break;
        case '}':
            ret.type = Token_RightBrace;
            break;
        case '[':
            ret.type = Token_LeftBracket;
            break;
        case ']':
            ret.type = Token_RightBracket;
            break;
        case ':':
            ret.type = Token_Colon;
            break;
        case ',':
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
            ret.type = Token_Number;
            ret.num_val = strtod(pstr, &endptr);
            if (endptr == NULL)
                goto TOKEN_ERROR;
            pstr = endptr - 1; //到后面还要在加一
            break;
        case '\"':
            ret.type = Token_String;
            pstr++;
            start = pstr;
            while (*pstr != '\0')
            {
                if (*pstr == '\"' && *(pstr - 1) != '\\')
                    break;
                pstr++;
            }
            if (*pstr == '\0')
                goto TOKEN_ERROR;
            ret.str_val = (char *)malloc(sizeof(char) * (pstr - start));
            strncpy(ret.str_val, start, pstr - start);
            ret.str_val[pstr - start] = '\0';
            break;
        case 't':
            if (*++pstr == 'r' && *++pstr == 'u' && *++pstr == 'e')
            {
                ret.type = Token_True;
                break;
            }
            else
                goto TOKEN_ERROR;
        case 'f':
            if (*++pstr == 'a' && *++pstr == 'l' && *++pstr == 's' && *++pstr == 'e')
            {
                ret.type = Token_False;
                break;
            }
            else
                goto TOKEN_ERROR;
        case 'n':
            if (*++pstr == 'u' && *++pstr == 'l' && *++pstr == 'l')
            {
                ret.type = Token_Null;
                break;
            }
            else
                goto TOKEN_ERROR;
        default:
            goto TOKEN_UNKNOWN;
        } //switch
        pstr++;
        return ret;
    } //while
    ret.type = Token_End;
    return ret;
TOKEN_UNKNOWN:
    ret.type = Token_Unknown;
    ret.str_val = pstr;
    return ret;
TOKEN_ERROR:
    ret.type = Token_Error;
    ret.str_val = pstr;
    return ret;
}

json_node *parse(const char *json_str)
{
    state now_state = State_Start;
    json_node *node_stack[50];
    state state_stack[50];
    int top = 0;
    json_node *root;
    json_node *this_node;
    token this_token;
    while (1)
    {
        this_token = tokenizer(json_str);
        switch (this_token.type)
        {
        case Token_LeftBrace:
            switch (now_state)
            {
            case State_Start: //ok
                this_node = root = (json_node *)malloc(sizeof(json_node));
                this_node->type = OBJECT;
                this_node->name = NULL;
                this_node->child_head = NULL;
                this_node->next = NULL;

                now_state = State_Object;
                break;
            case State_Array: //ok
                node_stack[top] = this_node;
                state_stack[top++] = (this_node->name == NULL) ? State_Element : State_Value;
                //有名字一定在object中,否则在array中
                this_node->child_head = (json_node *)malloc(sizeof(json_node));
                this_node = this_node->child_head;

                this_node->type = OBJECT;
                this_node->name = NULL;
                this_node->child_head = NULL;
                this_node->next = NULL;

                now_state = State_Object;
                break;
            case State_Colon: //ok
                this_node->type = OBJECT;
                this_node->child_head = NULL;

                now_state = State_Object;
                break;
            case State_ArrayComma: //ok
                this_node->next = (json_node *)malloc(sizeof(json_node));
                this_node = this_node->next;

                this_node->type = OBJECT;
                this_node->name = NULL;
                this_node->child_head = NULL;
                this_node->next = NULL;

                now_state = State_Object;
                break;
            default:
                goto PARSE_ERROR;
            }
            break;
        case Token_LeftBracket:
            switch (now_state)
            {
            case State_Start: //ok
                this_node = root = (json_node *)malloc(sizeof(json_node));
                
                this_node->type = ARRAY;
                this_node->name = NULL;
                this_node->child_head = NULL;
                this_node->next = NULL;

                now_state = State_Array;
                break;
            case State_Array: //ok
                node_stack[top] = this_node;
                state_stack[top++] = (this_node->name == NULL) ? State_Element : State_Value;
                //有名字一定在object中,否则在array中
                this_node->child_head = (json_node *)malloc(sizeof(json_node));
                this_node = this_node->child_head;

                this_node->name = NULL;
                this_node->type = ARRAY;
                this_node->child_head = NULL;
                this_node->next = NULL;

                break;
            case State_Colon: //ok
                this_node->type = ARRAY;

                this_node->next = NULL;

                now_state = State_Array;
                break;
            case State_ArrayComma: //ok
                this_node->next = (json_node *)malloc(sizeof(json_node));
                this_node = this_node->next;

                this_node->type = ARRAY;
                this_node->name = NULL;
                this_node->child_head = NULL;
                this_node->next = NULL;

                now_state = State_Array;
                break;
            default:
                goto PARSE_ERROR;
            }
            break;
        case Token_RightBrace:
            switch (now_state)
            {
            case State_Object: //*ok
                break;
            case State_Value: //ok
                this_node = node_stack[--top];
                now_state = state_stack[top];

                break;
            case State_ObjectComma: //**ok
                this_node = node_stack[--top];
                now_state = state_stack[top];
                break;
            default:
                goto PARSE_ERROR;
            }
            break;
        case Token_RightBracket:
            switch (now_state)
            {
            case State_Array: //*ok
                break;
            case State_Element: //ok
                this_node = node_stack[--top];
                now_state = state_stack[top];
                break;
            case State_ArrayComma: //ok
                this_node = node_stack[--top];
                now_state = state_stack[top];
                break;
            default:
                goto PARSE_ERROR;
            }
            break;
        case Token_Colon:
            switch (now_state)
            {
            case State_Key:
                now_state = State_Colon;
                break;
            default:
                goto PARSE_ERROR;
            }
            break;
        case Token_Comma:
            switch (now_state)
            {
            case State_Value:
                now_state = State_ObjectComma;
                break;
            case State_Element:
                now_state = State_ArrayComma;
                break;
                // case State_ObjectComma:

                // this_node->next = ( json_node*)malloc( sizeof(json_node));
                // this_node = this_node->next;

                // this_node->type = NUL;
                // this_node->next = NULL;

                // break;
            case State_ArrayComma:
                this_node->next = (json_node *)malloc(sizeof(json_node));
                this_node = this_node->next;

                this_node->type = NUL;
                this_node->next = NULL;

                break;
            case State_ObjectEnd:
                now_state = State_ObjectComma;
                break;
            default:
                goto PARSE_ERROR;
            }
            break;
        case Token_String:
            switch (now_state)
            {
            case State_Object: //ok
                node_stack[top] = this_node;
                state_stack[top++] = (this_node->name == NULL) ? State_Element : State_Value;
                //有名字一定在object中,否则在array中
                this_node->child_head = (json_node *)malloc(sizeof(json_node));
                this_node = this_node->child_head;

                this_node->name = this_token.str_val;
                this_node->next = NULL;

                now_state = State_Key;
                break;
            case State_Array: //ok
                node_stack[top] = this_node;
                state_stack[top++] = (this_node->name == NULL) ? State_Element : State_Value;
                //根据名字判断状态
                this_node->child_head = (json_node *)malloc(sizeof(json_node));
                this_node = this_node->child_head;

                this_node->type = STRING;
                this_node->name = NULL;
                this_node->val_str = this_token.str_val;
                this_node->next = NULL;

                now_state = State_Element;
                break;
            case State_Colon: //ok
                this_node->type = STRING;
                this_node->val_str = this_token.str_val;

                now_state = State_Value;
                break;
            case State_ObjectComma: //ok
                this_node->next = (json_node *)malloc(sizeof(json_node));
                this_node = this_node->next;

                this_node->name = this_token.str_val;
                this_node->value = 0L;
                this_node->next = NULL;

                now_state = State_Key;
                break;
            case State_ArrayComma: //ok
                this_node->next = (json_node *)malloc(sizeof(json_node));
                this_node = this_node->next;

                this_node->type = STRING;
                this_node->name = NULL;
                this_node->val_str = this_token.str_val;
                this_node->next = NULL;

                now_state = State_Element;
                break;
            default:
                goto PARSE_ERROR;
            }
            break;
        case Token_Number:
            switch (now_state)
            {
            case State_Array: //ok
                node_stack[top] = this_node;
                state_stack[top++] = (this_node->name == NULL) ? State_Element : State_Value;
                //根据名字判断状态
                this_node->child_head = (json_node *)malloc(sizeof(json_node));
                this_node = this_node->child_head;

                this_node->type = NUMBER;
                this_node->name = NULL;
                this_node->value = this_token.num_val;
                this_node->next = NULL;

                now_state = State_Element;
                break;
            case State_Colon: //ok
                this_node->type = NUMBER;
                this_node->value = this_token.num_val;
                this_node->next = NULL;

                now_state = State_Value;
                break;
            case State_ArrayComma: //ok
                this_node->next = (json_node *)malloc(sizeof(json_node));
                this_node = this_node->next;

                this_node->type = NUMBER;
                this_node->name = NULL;
                this_node->value = this_token.num_val;
                this_node->next = NULL;

                now_state = State_Element;
                break;
            default:
                goto PARSE_ERROR;
            }
            break;
        case Token_True:
            switch (now_state)
            {
            case State_Colon: //ok
                this_node->type = TRUE;
                this_node->value = 0L;
                this_node->next = NULL;
                now_state = State_Value;
                break;
            case State_Array: //ok
                node_stack[top] = this_node;
                state_stack[top++] = (this_node->name == NULL) ? State_Element : State_Value;
                //根据名字判断状态
                this_node->child_head = (json_node *)malloc(sizeof(json_node));
                this_node = this_node->child_head;

                this_node->type = TRUE;
                this_node->name = NULL;
                this_node->value = 0L;
                this_node->next = NULL;

                now_state = State_Element;
                break;
            case State_ArrayComma: //ok
                this_node->next = (json_node *)malloc(sizeof(json_node));
                this_node = this_node->next;

                this_node->type = TRUE;
                this_node->name = NULL;
                this_node->value = 0L;
                this_node->next = NULL;

                now_state = State_Element;
                break;
            default:
                goto PARSE_ERROR;
            }
            break;
        case Token_False:
            switch (now_state)
            {
            case State_Colon: //ok
                this_node->type = FALSE;
                this_node->value = 0L;
                this_node->next = NULL;

                now_state = State_Value;
                break;

            case State_Array: //ok
                node_stack[top] = this_node;
                state_stack[top++] = (this_node->name == NULL) ? State_Element : State_Value;
                //根据名字判断状态
                this_node->child_head = (json_node *)malloc(sizeof(json_node));
                this_node = this_node->child_head;

                this_node->type = FALSE;
                this_node->name = NULL;
                this_node->value = 0L;
                this_node->next = NULL;

                now_state = State_Element;
                break;
            case State_ArrayComma: //ok
                this_node->next = (json_node *)malloc(sizeof(json_node));
                this_node = this_node->next;

                this_node->type = FALSE;
                this_node->name = NULL;
                this_node->value = 0L;
                this_node->next = NULL;

                now_state = State_Element;
                break;
            default:
                goto PARSE_ERROR;
            }
            break;
        case Token_Null:
            switch (now_state)
            {
            case State_Colon: //ok
                this_node->type = NUL;
                this_node->value = 0L;
                this_node->next = NULL;
                now_state = State_Value;
                break;

            case State_Array: //ok
                node_stack[top] = this_node;
                state_stack[top++] = (this_node->name == NULL) ? State_Element : State_Value;
                //根据名字判断状态
                this_node->child_head = (json_node *)malloc(sizeof(json_node));
                this_node = this_node->child_head;

                this_node->type = NUL;
                this_node->name = NULL;
                this_node->value = 0L;
                this_node->next = NULL;

                now_state = State_Element;
                break;
            case State_ArrayComma: //ok
                this_node->next = (json_node *)malloc(sizeof(json_node));
                this_node = this_node->next;

                this_node->type = NUL;
                this_node->name = NULL;
                this_node->value = 0L;
                this_node->next = NULL;

                now_state = State_Element;
                break;
            default:
                goto PARSE_ERROR;
            }
            break;
        case Token_Error:
            break;
        case Token_Unknown:
            break;
        case Token_End:
            return root;
        } //switch
    }
    return root;
PARSE_ERROR:
    //to-do
    return root;
}