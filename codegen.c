#include "9cc.h"


void gen_lval(Node *node){
    if (node->kind == ND_DEREF){
        gen(node->lhs);
        return;
    }
    if (node->kind != ND_LVAR){
        error("The left side value of the assignment is not a variable");
    }
    printf("  mov rax, rbp\n");
    printf("  sub rax, %d\n", node->offset);
    printf("  push rax\n");
}

int genCounter = 0;
char *argRegs[] = {"rdi", "rsi", "rdx", "rcx", "r8", "r9"};

void gen(Node *node){
    if (!node){
        return;
    }
    genCounter += 1;
    int id = genCounter;
    //char name[100] = {0};
    int argCount = 0;

    switch (node->kind){
    case ND_GVAR:
        printf("%s:\n", node->varname);
        printf("  .zero %d\n", node->size);
        return;
    case ND_ADDR:
        gen_lval(node->lhs);
        return;
    case ND_DEREF:
        gen(node->lhs);
        printf("  pop rax\n");
        printf("  mov rax, [rax]\n");
        printf("  push rax\n");
        return;
    case ND_FUNC_DEF:
        printf("%s:\n", node->funcname);
        // プロローグ 
        printf("  push rbp\n");
        printf("  mov rbp, rsp\n");

        // 引数の値をスタックに積む
        for (int i = 0; node->args[i]; i++){
            printf("  push %s\n", argRegs[i]);//引数分, rspが移動.
            argCount++;
        }
        // ローカル変数が引数に続いてスタックに積まれる.(ここではなくて, stmtにいく. BNF参照)
        // ローカル変数分もrspをsubしなければならない.(配列も.)
        if(locals[cur_func]){
            // int offset = locals[cur_func][0].offset; 下と同じこと.
            // locals[cur_func]はポインタ. 構造体lvarを持っている.
            // locals[cur_func][0]は汎用ポインタ.
            int offset = locals[cur_func]->offset;
            offset -= argCount * 8;
            printf("  sub rsp, %d\n", offset);
        }
        gen(node->lhs);

        printf("  mov rsp, rbp\n");
        printf("  pop rbp\n");
        printf("  ret\n");
        return;
    case ND_FUNC_CALL:
        //memcpy(name, node->funcname, node->len);
        for(int i = 0; node->block[i]; i++){
            gen(node->block[i]);
            argCount++;
        }
        // 引数: 下に向かってスタックは成長する. FILOなので引数は後ろから読み取る.
        for(int i = argCount - 1; i >= 0; i--){
            printf("  pop %s\n", argRegs[i]);
        }
        printf("  mov rax, rsp\n");
        printf("  and rax, 15\n"); // 下位4bitをマスク. 15 := 0b00001111. 下位4bitが0であれば16の倍数.
        printf("  jnz .L.call.%03d\n", id);
        printf("  mov rax, 0\n");
        printf("  call %s\n", node->funcname);
        printf("  jmp .L.end.%03d\n", id);
        printf(".L.call.%03d:\n", id);
        printf("  sub rsp, 8\n");
        printf("  mov rax, 0\n");
        printf("  call %s\n", node->funcname);
        printf("  add rsp, 8\n");
        printf(".L.end.%03d:\n", id);
        printf("  push rax\n");
        return;
    case ND_BLOCK:
        for(int i = 0; node->block[i]; i++){
            gen(node->block[i]);
            // printf("  pop rax\n");
        }
        return;
    case ND_FOR:
        gen(node->lhs->lhs);
        printf(".Lbegin%03d:\n", id);
        gen(node->lhs->rhs);
        if (!node->lhs->rhs){ // 継続条件のところ
            printf("  push 1\n"); // スタックに何か入れておかないと予期せぬpopをしてしまう.
        }                         // 通常はgen(node->lhs->rhs)の結果が入ってる？
        printf("  pop rax\n");
        printf("  cmp rax, 0\n");
        printf("  je .Lend%03d\n", id);
        gen(node->rhs->rhs);
        gen(node->rhs->lhs);
        printf("  jmp .Lbegin%03d\n", id);
        printf(".Lend%03d:\n", id);
        return;
    case ND_WHILE:
        printf(".Lbegin%03d:\n", id);
        gen(node->lhs);
        printf("  pop rax\n");
        printf("  cmp rax, 0\n");
        printf("  je .Lend%03d\n", id);
        gen(node->rhs);
        printf("  jmp .Lbegin%03d\n", id);
        printf(".Lend%03d:\n", id);
        return;
    case ND_IF:
        gen(node->lhs);
        printf("  pop rax\n");
        printf("  cmp rax, 0\n");
        printf("  je .Lelse%03d\n", id);
        gen(node->rhs);
        printf("  jmp .Lend%03d\n", id);
        printf(".Lelse%03d:\n", id);
        if (node->rhs->kind == ND_ELSE) {
            gen(node->rhs->rhs);
        }
        printf(".Lend%03d:\n", id);
        return;
    case ND_RETURN:
        gen(node->lhs);
        printf("  pop rax\n");
        printf("  mov rsp, rbp\n");
        printf("  pop rbp\n");
        printf("  ret\n");
        return;
    case ND_NUM:
        printf("  push %d\n", node->val);
        return;
    case ND_LVAR:
        // アドレスを取って, RAXにいれる
        gen_lval(node);
        Type *t = get_type(node);
        // アドレスの中身(値)を取ってくる
        if(t && t->ty == ARRAY){
            return;
        }
        printf("  pop rax\n");
        printf("  mov rax, [rax]\n");
        printf("  push rax\n");
        return;
    case ND_ASSIGN:
        gen_lval(node->lhs);
        gen(node->rhs);

        printf("  pop rdi\n");
        printf("  pop rax\n");
        printf("  mov [rax], rdi\n");
        printf("  push rdi\n");
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
    case ND_EQ:
        printf("  cmp rax, rdi\n");
        printf("  sete al\n");
        printf("  movzb rax, al\n");
        break;
    case ND_NE:
        printf("  cmp rax, rdi\n");
        printf("  setne al\n");
        printf("  movzb rax, al\n");
        break;
    case ND_LT:
        printf("  cmp rax, rdi\n");
        printf("  setl al\n");
        printf("  movzb rax, al\n");
        break;
    case ND_LE:
        printf("  cmp rax, rdi\n");
        printf("  setle al\n");
        printf("  movzb rax, al\n");
        break;
    }
    printf("  push rax\n");
}
