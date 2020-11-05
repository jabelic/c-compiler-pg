#include "9cc.h"

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
四則演算, 比較, 変数, 代入, セミコロン.
program = stmt*
stmt    = expr ";" 
        | "if" "(" expr ")" stmt ("else" stmt)?
        | "return" expr ";"
expr       = assign
assign     = equality ("=" assign)?
equality   = relational ("==" relational | "!=" relational)*
relational = add ("<" add | "<=" add | ">" add | ">=" add)*
add        = mul ("+" mul | "-" mul)*
mul        = unary ("*" unary | "/" unary)*
unary      = ("+" | "-")? primary
primary    = num | ident | "(" expr ")"
*/

Node *code[100];


void program(){
    int i = 0;
    while(!at_eof()){
        code[i++] = stmt();
    }
    code[i] = NULL;
}

Node *stmt(){
    Node *node;

    if (consume_kind(TK_IF)){
        expect("(");
        node = calloc(1, sizeof(Node));
        node->kind = ND_IF;
        node->lhs = expr();
        expect(")");
        node->rhs = stmt();
        if (consume_kind(TK_ELSE)){
            Node *els = calloc(1, sizeof(Node));
            els->kind = ND_ELSE;
            els->lhs = node->rhs; // これで枝を1本にすることができる?
            els->rhs = stmt();
            node->rhs = els;
        }
        return node;
    }

    if (consume_kind(TK_RETURN)){
        node = calloc(1, sizeof(Node));
        node->kind = ND_RETURN;
        node->lhs = expr();
    } else {
        node = expr();
    }
    expect(";");
    return node;
}

Node *expr(){
    return assign();
}


Node *assign(){
    Node *node = equality();
    if (consume("=")){
        node = new_node(ND_ASSIGN, node, assign());
    }
    return node;
}



Node *equality(){
    Node *node = relational();
    for(;;){
        if (consume("==")){
            node = new_node(ND_EQ, node, relational());
        }
        else if(consume("!=")){
            node = new_node(ND_NE, node, relational());
        }
        else {
            return node;
        }
    }
}

Node *relational(){
    Node *node = add();

    for(;;){
        if (consume("<")){
            node = new_node(ND_LT, node, add());
        }
        else if (consume("<=")){
            node = new_node(ND_LE, node, add());
        }
        else if (consume(">")){
            node = new_node(ND_LT, add(), node);
        }
        else if (consume(">=")){
            node = new_node(ND_LE, add(), node);
        }
        else{
            return node;
        }
    }
}

Node *add(){
    Node *node = mul();
    // ここでlhsを先に入れて、
    // consumeでtokenが一つ進んでからrhsをいれる?
    for(;;){ // 無限ループ.
        if (consume("+")){
            node = new_node(ND_ADD, node, mul());
        }else if (consume("-")){
            node = new_node(ND_SUB, node, mul());
        }else{
            return node;
        }
    }
}

Node *mul(){
    Node *node = unary();

    for(;;){
        if(consume("*")){
            node = new_node(ND_MUL, node, unary());
        }else if(consume("/")){
            node = new_node(ND_DIV, node, unary());
        }else{
            return node;
        }
    }
}

Node *unary(){//ちゃんと数字にpointerが当たってから見るぞ！！と言うやつ.
    if (consume("+")){
        return unary();
    }
    if (consume("-")){
        return new_node(ND_SUB, new_node_num(0), unary());
    }
    return primary();
}

Node *primary(){
    // 次のトークンが"("なら "(" expr ")"のはず
    if (consume("(")){
        Node *node = expr();
        expect(")");
        return node;
    }
    Token *tok = consume_kind(TK_IDENT);
    if (tok){
        Node *node = calloc(1, sizeof(Node));//未定義の変数の分のメモリを確保
        node->kind = ND_LVAR;
        LVar *lvar = find_lvar(tok);
        if(lvar){
            node->offset = lvar->offset;
        }else{
            lvar = calloc(1, sizeof(LVar));
            lvar->next = locals;
            lvar->name = tok->str;
            lvar->len = tok->len;
            if (locals == NULL){
                lvar->offset = 8;
            }else{
                lvar->offset = locals->offset + 8;
            }
            node->offset = lvar->offset;
            locals = lvar;
        }
        //node->offset = (tok->str[0] - 'a' + 1) * 8;
        return node;
    }
    // そうでなければ数値のはず
    return new_node_num(expect_number());
}



void gen_lval(Node *node){
    if (node->kind != ND_LVAR){
        error("The left side value of the assignment is not a variable");
    }
    printf("  mov rax, rbp\n");
    printf("  sub rax, %d\n", node->offset);
    printf("  push rax\n");
}

/*if else

 Aをコンパイルしたコード // スタックトップに結果が入っているはず
  pop rax
  cmp rax, 0
  je  .LelseXXX
  Bをコンパイルしたコード
  jmp .LendXXX
.LelseXXX
  Cをコンパイルしたコード
.LendXXX
*/


void gen(Node *node){
    switch (node->kind){
    case ND_IF:
        gen(node->lhs);
        printf("  pop rax\n");
        printf("  cmp rax, 0\n");
        printf("  je .LelseXXX\n");
        gen(node->rhs);
        printf("  jmp .LendXXX\n");
        printf(".LelseXXX:\n");
        if (node->rhs->kind == ND_ELSE) {
            gen(node->rhs->rhs);
        }
        printf(".LendXXX:\n");
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
        gen_lval(node);
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
