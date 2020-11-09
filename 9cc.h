#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>
#include <string.h>
#include <stdarg.h>

// トークンの種類 列挙型
typedef enum{
    TK_RESERVED, // 記号
    TK_IDENT, // 識別子
    TK_NUM,      // 整数トークン
    TK_RETURN, // return 
    TK_IF, // if
    TK_ELSE, //else
    TK_WHILE, // while
    TK_EOF,      // 入力の終わりを表すトークン
} Tokenkind;     //0,1,2
//TAG名は省略している

typedef struct Token Token; //Tokenという名前のTokenという型を定義
// typedef struct TAG{} NAME; 中身ないので、下で定義

// トークン型
// ここが構造体
struct Token{
    Tokenkind kind; // トークンの型
    Token *next;    // 次の入力トークン
    int val;        // kindがTK_NUMの場合, その数値
    char *str;      // トークン文字列
    int len;    // トークンの長さ : 識別子が==など, 2つになる場合も加味したい.
};
// linked listを書いている??

typedef struct LVar LVar;

struct LVar {
    LVar *next;
    char *name;
    int len;
    int offset;
};


extern char *user_input;

// 現在着目しているトークン
extern Token *token;

extern LVar *locals;


void error(char *fmt,...);
void error_at(char *loc, char *fmt,...);
bool consume(char *op);
Token *consume_kind(Tokenkind kind);
void expect(char *op);
int expect_number();
bool at_eof();
Token *new_token(Tokenkind kind, Token *cur, char *str, int len);
bool startswith(char *p, char *q);
Token *tokenize();
LVar *find_lvar(Token *tok);

typedef enum{
    ND_ADD, // +
    ND_SUB, // - 
    ND_MUL, // *
    ND_DIV, // /
    ND_ASSIGN, // =
    ND_LVAR, // ローカル変数
    ND_EQ, // ==
    ND_NE, // !=
    ND_LT, // <
    ND_LE, // <=
    ND_NUM, // 整数
    ND_RETURN, // return
    ND_IF, // if
    ND_ELSE, // else
    ND_WHILE, // while
} NodeKind;

typedef struct Node Node;

struct Node{
    NodeKind kind; // ノードの型
    Node *lhs; // 左辺
    Node *rhs; // 右辺
    int val;   // kindがND_NUMの場合のみ使う.
    int offset; // kindがND_LVERの場合のみ使う.
};


Node *new_node(NodeKind kind, Node *lhs, Node *rhs);
Node *new_node_num(int val);

void program();
Node *stmt();
Node *expr();
Node *assign();
Node *equality();
Node *relational();
Node *add();
Node *mul();
Node *unary();
Node *primary();
extern Node *code[];

void gen(Node *node);
void gen_lval(Node *node);