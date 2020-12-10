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
program    = func*
func       = ident "(" ")"  "{" stmt* "}"
stmt       = expr ";" 
            | "{" stmt* "}"
            | "if" "(" expr ")" stmt ("else" stmt)?
            | "while" "(" expr ")" stmt
            | "for" "(" expr? ";" expr? ";" expr? ")" stmt
            | "return" expr ";"
expr       = assign
assign     = equality ("=" assign)?
equality   = relational ("==" relational | "!=" relational)*
relational = add ("<" add | "<=" add | ">" add | ">=" add)*
add        = mul ("+" mul | "-" mul)*
mul        = unary ("*" unary | "/" unary)*
unary      = ("+" | "-")? primary
primary    = num
            | ident ("(" expr* ")")?
            | "(" expr ")"
*/

Node *code[100];


void program(){
    int i = 0;
    while(!at_eof()){
        code[i++] = func();
    }
    code[i] = NULL;
}

Node *func(){
    Node *node;
    Token *tok = consume_kind(TK_IDENT);
    if(tok == NULL){
        error("This is not a function.");
    }
    node = calloc(1, sizeof(Node));
    node->kind = ND_BLOCK;
    node->funcname = calloc(100, sizeof(char));
    memcpy(node->funcname, tok->str, tok->len);
    expect("(");
    expect(")");
    expect("{");
    node->block = calloc(100, sizeof(Node));
    for(int i = 0; !consume("}"); i++){
        node->block[i] = stmt();
    }
    // node->lhs = stmt();
    return node;
}

Node *stmt(){
    Node *node;
    if(consume("{")){
        node = calloc(1, sizeof(Node));
        node->kind = ND_BLOCK;
        node->block = calloc(100, sizeof(Node));
        for(int i = 0; !consume("}"); i++){
            node->block[i] = stmt();
        }
        return node;
    }

    if (consume_kind(TK_FOR)){
        expect("(");
        node = calloc(1, sizeof(Node));
        node->kind = ND_FOR;

        Node *left = calloc(1, sizeof(Node));
        left->kind = ND_FOR_LEFT;
        Node *right = calloc(1, sizeof(Node));
        right->kind = ND_FOR_RIGHT;

        if (!consume(";")){
            left->lhs = expr();
            expect(";");
        }
        if (!consume(";")){
            left->rhs = expr();
            expect(";");
        }
        if (!consume(")")){
            right->lhs = expr();
            expect(")");
        }
        right->rhs = stmt();

        node->lhs = left;
        node->rhs = right;
        return node;
    }

    if (consume_kind(TK_WHILE)){
        expect("(");
        node = calloc(1, sizeof(Node));
        node->kind = ND_WHILE;
        node->lhs = expr();
        expect(")");
        node->rhs = stmt();
        return node;
    }
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
        if (consume("(")){
            // function call
            Node *node = calloc(1, sizeof(Node));
            node->kind = ND_FUNC_CALL;
            // node->funcname = tok->str;
            node->funcname = calloc(100, sizeof(char));
            // node->len = tok->len;
            memcpy(node->funcname, tok->str, tok->len);
            // expect(")");
            // 引数
            node->block = calloc(10, sizeof(Node));
            for(int i = 0; !consume(")"); i++){
                node->block[i] = expr();
                if(consume(")")){
                    break;
                }
                expect(",");
            }
            return node;
        }
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
        return;
    case ND_BLOCK:
        for(int i = 0; node->block[i]; i++){
            gen(node->block[i]);
            printf("  pop rax\n");
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
