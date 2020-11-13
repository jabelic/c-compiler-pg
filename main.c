#include "9cc.h"

int main(int argc, char **argv){
    if (argc != 2){
        fprintf(stderr, "Invalid args\n");
        return 1;
    }

    // トークナイズする
    user_input = argv[1];
    token = tokenize(); //global pointer varのtokenにheadをつなげる
    
    // 再帰下降構文木を生成
    program();


    //　アセンブリの前半部分を出力
    printf(".intel_syntax noprefix\n");
    printf(".globl main\n");
    printf("main:\n");


    printf("  push rbp\n");
    printf("  mov rbp, rsp\n");
    printf("  sub rsp, 208\n"); // 8 * 26文字

    //先頭の式から順にコード生成
    for (int i=0; code[i]; i++){
        gen(code[i]);

        // 式の評価結果としてスタックに一つの値が残っている
        // はずなので, スタックが溢れないようにpopしておく
        printf("  pop rax\n");
    }

    
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

    // エビローグ
    // 最後の式の結果がraxに残っているのでそれが返り値になる.
    printf("  mov rsp, rbp\n");
    printf("  pop rbp\n");
    printf("  ret\n");
    return 0;
}