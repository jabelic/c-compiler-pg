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
    // printf("main:\n");


    // printf("  push rbp\n");
    // printf("  mov rbp, rsp\n");
    // printf("  sub rsp, 208\n"); // 8 * 26文字

    cur_func = 0;
    //先頭の式から順にアセンブリコード生成
    for (int i=0; code[i]; i++){
        cur_func++;
        gen(code[i]);
        // 式の評価結果としてスタックに一つの値が残っている
        // はずなので, スタックが溢れないようにpopしておく
        // printf("  pop rax\n");
    }


    // エビローグ
    // 最後の式の結果がraxに残っているのでそれが返り値になる.
    // printf("  mov rsp, rbp\n");
    // printf("  pop rbp\n");
    // printf("  ret\n");
    return 0;
}