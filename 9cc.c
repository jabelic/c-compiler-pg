#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>
#include <string.h>

// トークンの種類 列挙型
typedef enum {
    TK_RESERVED, // 記号
    TK_NUM,  // 整数トークン
    TK_EOF, // 入力の終わりを表すトークン
} Tokenkind; //0,1,2
//TAG名は省略している

typedef struct Token Token; //Tokenという名前のTokenという型を定義
// typedef struct TAG{} NAME; 中身ないので、下で定義

// トークン型
// ここが構造体
struct Token{
    Tokenkind kind; // トークンの型  kind.TK_NUM = 1;と言うふうにできる？
    Token *next;   // 次の入力トークン
    int val;       // kindがTK_NUM(つまり1?)の場合, その数値
    char *str;     // トークン文字列
};
// linked listを書いている??


// 現在着目しているトークン
Token *token;

// エラーを報告するための関数
// printfと同じ引数を取る: *fnt = "hoge%d\n"
void error(char *fmt, ...){
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n"); //stderrに"\n"を渡す
    exit(1);
}// stderrにErrorメッセージが格納される？


// 次のトークンが期待している記号のときには、トークンを1つ読み進めて
// 真を返す。それ以外の場合には偽を返す。
bool consume (char op){
    if (token->kind != TK_RESERVED || token->str[0] != op){ //記号でないか、先頭の文字が指定した文字でなければfalse
        return false;
    }
    token = token->next;
    return true;
}

void expect(char op){
    if (token->kind != TK_RESERVED || token->str[0] != op){
        error("'%cではありません", op);
    }
    token = token->next;
}

// 次のトークンが数値の場合、トークンを1つ読み進めてその数値を返す。
// それ以外の場合にはエラーを報告する。
int expect_number(){
    if (token->kind != TK_NUM){
        error("Not a number");
    }
    int val = token->val;
    token = token->next;
    return val;
}

bool at_eof() {
    return token->kind == TK_EOF;
}

// 新しいトークンを作成してcurに繋げる
Token *new_token(Tokenkind kind, Token *cur, char *str) {
    Token *tok = calloc(1, sizeof(Token));
    tok->kind = kind;
    tok->str = str;
    cur->next = tok;
    return tok;
}





int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Invalid char\n");
        return 1;
    }

    char *p = argv[1];

    printf(".intel_syntax noprefix\n");
    printf(".globl main\n");
    printf("main:\n");
    printf("  mov rax, %ld\n", strtol(p, &p, 10));

    while (*p) {
        if (*p == '+') {
        p++;
        printf("  add rax, %ld\n", strtol(p, &p, 10));
        continue;
    }

    if (*p == '-') {
        p++;
        printf("  sub rax, %ld\n", strtol(p, &p, 10));
        continue;
    }
    fprintf(stderr, "Unexpected char: '%c'\n", *p);
    return 1;
    }

    printf("  ret\n");
    return 0;
}