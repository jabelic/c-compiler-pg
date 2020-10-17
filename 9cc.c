#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>
#include <string.h>
#include <stdarg.h>


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
    int val;       // kindがTK_NUMの場合, その数値
    char *str;     // トークン文字列
};
// linked listを書いている??

// 現在着目しているトークン
Token *token;

// エラーを報告するための関数
// printfと同じ引数を取る: *fmt = "hoge%d\n"
void error(char *fmt, ...){
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n"); //"\n"が出力される. つまり改行をもうここに組み込んでしまおうと？
    exit(1); // va_end()の代わりですかね.
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

Token *tokenize(char *p){
    Token head;
    head.next = NULL;
    Token *cur = &head;

    while (*p){
        // 空白文字をスキップ
        if (isspace(*p)){
            p++;
            continue;
        }

        if (*p == '+' || *p == '-'){
            cur = new_token(TK_RESERVED, cur, p++); // 次の文字のポインタをcurにつなげる
            continue;
        }

        if (isdigit(*p)){
            cur = new_token(TK_NUM, cur, p);
            cur->val = strtol(p, &p, 10); // pの先頭が数字ならn(今は10)進法でlong型に変換. 違ったらその文字のポインタを&pに格納
            continue;
        }

        error("Couldn't tokenize...");
    }
    new_token(TK_EOF, cur, p);
    return head.next;
}



int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Invalid char\n");
        return 1;
    }

    // トークナイズする
    token = tokenize(argv[1]);

    //　アセンブリの前半部分を出力
    // char *p = argv[1];

    printf(".intel_syntax noprefix\n");
    printf(".globl main\n");
    printf("main:\n");

    // 式の最初は数でなければならないので、それをチェックして
    // 最初のmov命令を出力
    printf("  mov rax, %d\n", expect_number());


    while (!at_eof()){
        if (consume('+')){
            printf("  add rax, %d\n", expect_number());
            continue;
        }
        expect('-');
        printf("  sub rax, %d\n", expect_number());
    }

    printf("  ret\n");
    return 0;
}