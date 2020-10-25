#include "9cc.h"

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