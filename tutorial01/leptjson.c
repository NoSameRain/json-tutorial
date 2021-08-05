#include "leptjson.h"
#include <assert.h>  /* assert() */
#include <stdlib.h>  /* NULL */

#define EXPECT(c, ch)       do { assert(*c->json == (ch)); c->json++; } while(0)

typedef struct {
    const char* json;
}lept_context;

//一直跳到非空白的字符
/* ws = *(%x20 / %x09 / %x0A / %x0D) */
static void lept_parse_whitespace(lept_context* c) {
    const char *p = c->json;
    while (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r')
        p++;
    c->json = p;
}

/* null  = "null" */
static int lept_parse_null(lept_context* c, lept_value* v) {
    EXPECT(c, 'n'); // 这里c->json已经++了
    if (c->json[0] != 'u' || c->json[1] != 'l' || c->json[2] != 'l') // 为啥从json[0]开始啊！？？？
        return LEPT_PARSE_INVALID_VALUE;
    // tutorial01_task01------------------------------------------------------------------------
    c->json += 3; // 指向第二个ws
    v->type = LEPT_NULL;
    return LEPT_PARSE_OK;
}

// tutorial01_task01------------------------------------------------------------------------
static int lept_parse_true(lept_context* c, lept_value* v) {
    EXPECT(c, 't');
    if (c->json[0] != 'r' || c->json[1] != 'u' || c->json[2] != 'e') 
        return LEPT_PARSE_INVALID_VALUE;
    c->json += 3; 
    v->type = LEPT_TRUE;
    return LEPT_PARSE_OK;
}

static int lept_parse_false(lept_context* c, lept_value* v) {
    EXPECT(c, 'f');
    if (c->json[0] != 'a' || c->json[1] != 'l' || c->json[2] != 's' || c->json[3] != 'e') 
        return LEPT_PARSE_INVALID_VALUE;
    c->json += 4; 
    v->type = LEPT_FALSE;
    return LEPT_PARSE_OK;
}

/* value = null / false / true */
/* 功效：给树型结构赋值，返回测试结果 */
static int lept_parse_value(lept_context* c, lept_value* v) {
    switch (*c->json) {
        case 'n':  return lept_parse_null(c, v);
        case 't':  return lept_parse_true(c, v);
        case 'f':  return lept_parse_false(c, v);
        case '\0': return LEPT_PARSE_EXPECT_VALUE;
        default:   return LEPT_PARSE_INVALID_VALUE;
    }
}

/* 由于 lept_parse_whitespace() 是不会出现错误的，返回类型为void。其它的解析函数会返回错误码，传递至顶层。*/
int lept_parse(lept_value* v, const char* json) {
    lept_context c;
    assert(v != NULL);
    c.json = json;
    v->type = LEPT_NULL;
    lept_parse_whitespace(&c); //跳过文本的第一部分空白符，传递引用过去可以修改c的指向的字符
 
    int ret;
    if ((ret = lept_parse_value(&c, v)) == LEPT_PARSE_OK) {
        lept_parse_whitespace(&c); // 跳过第二个ws
        if (*c.json != '\0') ret = LEPT_PARSE_ROOT_NOT_SINGULAR; 
    }
    return ret;
}

lept_type lept_get_type(const lept_value* v) {
    assert(v != NULL);
    return v->type;
}
