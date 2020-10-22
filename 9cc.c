#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>
#include <string.h>
#include <stdarg.h>

// トークンの種類 列挙型
typedef enum{
    TK_RESERVED, // 記号
    TK_NUM,      // 整数トークン
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
};
// linked listを書いている??

char *user_input;

// 現在着目しているトークン
Token *token;

void error_at(char *loc, char *fmt, ...){
    va_list ap;
    va_start(ap, fmt);

    int pos = loc - user_input;
    fprintf(stderr, "%s\n", user_input);
    fprintf(stderr, "%*s", pos, ""); // print pos spaces.
    fprintf(stderr, "^ ");
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    exit(1);
}

// 次のトークンが期待している記号のときには、トークンを1つ読み進めて
// 真を返す。それ以外の場合には偽を返す。
bool consume(char op){
    if (token->kind != TK_RESERVED || token->str[0] != op)
    { //記号でないか、先頭の文字が指定した文字でなければfalse
        return false;
    }
    token = token->next;
    return true;
}

void expect(char op){
    if (token->kind != TK_RESERVED || token->str[0] != op){
        error_at(token->str, "'%c'ではありません", op);
    }
    token = token->next;
}

// 次のトークンが数値の場合、トークンを1つ読み進めてその数値を返す。
// それ以外の場合にはエラーを報告する。
int expect_number(){
    if (token->kind != TK_NUM){
        error_at(token->str, "Not a number");
    }
    int val = token->val;
    token = token->next;
    return val;
}

bool at_eof(){
    return token->kind == TK_EOF;
}

// 新しいトークンを作成してcurに繋げる
Token *new_token(Tokenkind kind, Token *cur, char *str){
    Token *tok = calloc(1, sizeof(Token));
    tok->kind = kind;
    tok->str = str;
    cur->next = tok;
    return tok;
}

Token *tokenize(){
    char *p = user_input;
    Token head;
    head.next = NULL;
    Token *cur = &head;

    while (*p){
        // 空白文字をスキップ
        if (isspace(*p)){
            p++;
            continue;
        }

        if (strchr("+-*/()", *p)){// char *strchr(const char *s, int c); 
            // 文字列sのなかで最初のcharにcastされたcが見つかった位置を返す
            // なかったらNULLを返す

            cur = new_token(TK_RESERVED, cur, p++); // 文字のポインタをcurにつなげる.
            /*
            cur = new_token(TK_RESERVED, cur, p);
            p++ 
            と同じ. 後方で++するやつらしい...
            だから, ここでは文字を入れて、次の数字のところにポインタを一つ進めている.
            */
            continue;
        }

        if (isdigit(*p)){
            cur = new_token(TK_NUM, cur, p);
            cur->val = strtol(p, &p, 10); // pの先頭が数字ならn(今は10)進法でlong型に変換して文字のポインタを&pに格納
            continue;
        }

        error_at(p, "Couldn't tokenize...");
    }
    new_token(TK_EOF, cur, p);
    return head.next;
}



typedef enum{
    ND_ADD, // +
    ND_SUB, // - 
    ND_MUL, // *
    ND_DIV, // /
    ND_NUM, // 整数
} NodeKind;

typedef struct Node Node;

struct Node{
    NodeKind kind; // ノードの型
    Node *lhs; // 左辺
    Node *rhs; // 右辺
    int val;   // kindがND_NUMの場合のみ使う.
};


Node *new_node(NodeKind kind, Node *lhs, Node *rhs){
    Node *node = calloc(1, sizeof(Node));
    node->kind = kind;
    node->lhs = lhs;
    node->rhs = rhs;
    return node;
}

Node *new_node_num(int val){
    Node *node = calloc(1, sizeof(Node));
    node->kind = ND_NUM;
    node->val = val;
    return node;
}

/*
四則演算: 再帰下降構文解析
expr    = mul ("+" mul | "-" mul)*
mul     = primary ("*" primary | "/" primary)*
primary = num | "(" expr ")"
*/

Node *expr();
Node *mul();
Node *primary();

Node *expr(){
    Node *node = mul();
    // ここでlhsを先に入れて、
    // consumeでtokenが一つ進んでからrhsをいれる?
    for(;;){ // 無限ループ.
        if (consume('+')){
            node = new_node(ND_ADD, node, mul());
        }else if (consume('-')){
            node = new_node(ND_SUB, node, mul());
        }else{
            return node;
        }
    }
}

Node *mul(){
    Node *node = primary();

    for(;;){
        if(consume('*')){
            node = new_node(ND_MUL, node, primary());
        }else if(consume('/')){
            node = new_node(ND_DIV, node, primary());
        }else{
            return node;
        }
    }
}

Node *primary(){
    // 次のトークンが"("なら "(" expr ")"のはず
    if (consume('(')){
        Node *node = expr();
        expect(')');
        return node;
    }
    // そうでなければ数値のはず
    return new_node_num(expect_number());
}


void gen(Node *node){
    if (node->kind == ND_NUM){
        printf("  push %d\n", node->val);
        return;
    }

    gen(node->lhs);
    gen(node->rhs);

    printf("  pop rdi\n");
    printf("  pop rax\n");

    switch (node->kind){
    case ND_ADD:
        printf("  add rax, rdi\n");
        break;
    case ND_SUB:
        printf("  sub rax, rdi\n");
        break;
    case ND_MUL:
        printf("  imul rax, rdi\n");
        break;
    case ND_DIV:
        printf("  cqo\n");
        printf("  idiv rdi\n");
        break;
    }
    printf("  push rax\n");
}


int main(int argc, char **argv){
    if (argc != 2){
        fprintf(stderr, "Invalid args\n");
        return 1;
    }

    // トークナイズする
    user_input = argv[1];
    token = tokenize(); //global pointer varのtokenにheadをつなげる
    Node *node = expr();

    //　アセンブリの前半部分を出力
    printf(".intel_syntax noprefix\n");
    printf(".globl main\n");
    printf("main:\n");

    // 抽象構文木を下りながらコード生成
    gen(node);
    
    // 式の最初は数でなければならないので、それをチェックして
    // 最初のmov命令を出力
    /*printf("  mov rax, %d\n", expect_number());

    while (!at_eof()){
        if (consume('+')){
            printf("  add rax, %d\n", expect_number());
            continue;
        }
        expect('-');
        printf("  sub rax, %d\n", expect_number());
    }*/


    // スタックトップに式全体の値が残っているはずなので
    // それをRAXにロードして関数からの返り値とする.
    printf("  pop rax\n");
    printf("  ret\n");
    return 0;
}