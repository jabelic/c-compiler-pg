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
} Tokenkind;


typedef struct Token Token; //Tokenという名前のTokenという型を定義

// typedef struct TAG NAME;

// トークン型
// ここが構造体
struct Token{
    Tokenkind kind; // トークンの型
    Token *next;   // 次の入力トークン
    int val;       // kindがTK_NUMの場合, その数値
    char *str;     // トークン文字列
};
// linked list????


// 現在着目しているトークン
Token *token;

// エラーを報告するための関数
// printfと同じ引数を取る
void error(char *fmt, ...){
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n"); //stderrに"\n"を渡す
    exit(1);
}

// 次のトークンが期待している記号のときには、トークンを1つ読み進めて
// 真を返す。それ以外の場合には偽を返す。
bool consume (char op){
    if (token->kind != TK_RESERVED || token->str[0] != op)
        return false;
    token = token->next;
    return true;
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