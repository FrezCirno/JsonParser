typedef struct {
  char* value;
  int length;
  int capacity;
} string;

#define INIT_STR(x)                            \
  x.value = (char*)malloc(100 * sizeof(char)); \
  memset(x.value, 0, 100);                     \
  x.length = 0;                                \
  x.capacity = 100

#define NEW_STR_FROM(x, str, len)                     \
  string x;                                           \
  x.length = x.capacity = len;                        \
  x.value = (char*)malloc(x.capacity * sizeof(char)); \
  strncpy(x.value, str, len)

#define PUSH_BACK(x, c)                                                 \
  do {                                                                  \
    if (x.length == x.capacity) {                                       \
      realloc(x.value, (x.capacity = 1.5 * x.capacity) * sizeof(char)); \
    }                                                                   \
    x.value[x.length++] = c;                                            \
  } while (0)

#define PUSH_BACK_STR(x, s, len)                                      \
  do {                                                                \
    if (x.length + len >= x.capacity) {                               \
      realloc(x.value, (x.capacity = x.length + len) * sizeof(char)); \
    }                                                                 \
    for (int __i = 0; __i < len; __i++)                               \
      x.value[x.length++] = s[__i];                                   \
  } while (0)

#define FREE(x) free(x.value)