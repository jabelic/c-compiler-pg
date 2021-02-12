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
func       = "int" ident "(" ("int" ident ("," "int" ident)*)? ")" stmt
stmt       = expr ";" 
            | "{" stmt* "}"
            | "if" "(" expr ")" stmt ("else" stmt)?
            | "while" "(" expr ")" stmt
            | "for" "(" expr? ";" expr? ";" expr? ")" stmt
            | "int" ident ";"
            | "return" expr ";"
expr       = assign
assign     = equality ("=" assign)?
equality   = relational ("==" relational | "!=" relational)*
relational = add ("<" add | "<=" add | ">" add | ">=" add)*
add        = mul ("+" mul | "-" mul)*
mul        = unary ("*" unary | "/" unary)*
unary      = "sizeof" unary | ("+" | "-" | "*" | "&" )? primary
primary    = num
            | ident ("(" expr* ")")?
            | "(" expr ")"
*/

Node *code[100];


// program    = func*
void program(){
    int i = 0;
    while(!at_eof()){
        code[i++] = func();
    }
    code[i] = NULL;
}

// func       = "int" ident "(" ("int" ident ("," "int" ident)*)? ")" stmt
Node *func(){
    cur_func++;
    Node *node;
    if (!consume_kind(TK_TYPE)){
        error("function return type not found");
    }
    Token *tok = consume_kind(TK_IDENT);
    if(tok == NULL){
        error("This is not a function."); // int for(;;){}とか.
    }
    node = calloc(1, sizeof(Node));
    node->kind = ND_FUNC_DEF;
    node->funcname = calloc(100, sizeof(char));
    node->args = calloc(10, sizeof(Node*));
    memcpy(node->funcname, tok->str, tok->len);
    expect("(");// 次にtokenをつなげる
    for(int i = 0; !consume(")"); i++){
        if (consume_kind(TK_TYPE)){
            error("function arges type not found");
        }
        // Token *tok = consume_kind(TK_IDENT);
        // if (tok == NULL){
        //     error("Invalid args");
        // }
        node->args[i] = define_variable();
        if (consume(")")) { // これいる？？？
            break;
        }
        expect(",");
    }

    // expect(")");
    // expect("{");
    // node->block = calloc(100, sizeof(Node));
    // for(int i = 0; !consume("}"); i++){
    //     node->block[i] = stmt();
    // }
    node->lhs = stmt();
    return node;
}

// stmt       = expr ";" 
//             | "{" stmt* "}"
//             | "if" "(" expr ")" stmt ("else" stmt)?
//             | "while" "(" expr ")" stmt
//             | "for" "(" expr? ";" expr? ";" expr? ")" stmt
//             | "return" expr ";"
//             | "int" "*"*? ident ";"
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
        expect(";");
        return node;
    }
    if (consume_kind(TK_TYPE)){
        // Token *tok = consume_kind(TK_IDENT);
        // if (tok == NULL){
        //     error("Invalid define variable");
        // }
        node = define_variable();
        expect(";");
        return node;
    } 
    
    node = expr();
    expect(";");
    return node;
}

// expr       = assign
Node *expr(){
    return assign();
}

// assign     = equality ("=" assign)?
Node *assign(){
    Node *node = equality(); // 枠を取って
    if (consume("=")){
        node = new_node(ND_ASSIGN, node, assign()); // equalityに渡すためにnodeの各枝を設定.
    }
    return node;
}


// equality   = relational ("==" relational | "!=" relational)*
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

// relational = add ("<" add | "<=" add | ">" add | ">=" add)*
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

// add        = mul ("+" mul | "-" mul)*
Node *add(){
    Node *node = mul();
    // ここでlhsを先に入れて、
    // consumeでtokenが一つ進んでからrhsをいれる?
    for(;;){ // 無限ループ.
        if (consume("+")){
            Node *r = mul();
            // 配列とポインタ演算. (配列要素へのアクセスは実質ポインタ演算)
            if (node->type && (node->type->ty == PTR || node->type->ty == ARRAY)){
                int n = node->type->ptr_to->ty == INT ? 4 : 8;
                r = new_node(ND_MUL, r, new_node_num(n)); // + に続く式
            }
            node = new_node(ND_ADD, node, r);
            // node = new_node(ND_ADD, node, mul());
        }else if (consume("-")){
            Node *r = mul();
            if (node->type && (node->type->ty == PTR || node->type->ty == ARRAY)){
                int n = node->type->ptr_to->ty == INT ? 4 : 8;
                r = new_node(ND_MUL, r, new_node_num(n));
            }
            node = new_node(ND_SUB, node, r);
        }else{
            return node;
        }
    }
}

// mul        = unary ("*" unary | "/" unary)*
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

// unary      = "sizeof" unary
//                | ("+" | "-" | "*" | "&" )? primary
Node *unary(){//ちゃんと数字にpointerが当たってから見るぞ！！と言うやつ.
    if (consume_kind(TK_SIZEOF)){
        Node *n = unary(); // これが中身. sizeofが入っていても対応できる. 中身の処理はせずに, 捨てる.
        Type *t = get_type(n);
        // TODD: 中身に*がついていればnをデリファレンス. 変数を一段階リファレンスした結果を返す.
        int size = t && t->ty == PTR ? 8 : 4;
        return new_node_num(size); // 枝はここで終了.
    }
    if (consume("+")){
        return unary();
    }
    if (consume("-")){
        return new_node(ND_SUB, new_node_num(0), unary());
    }
    if (consume("&")){
        return new_node(ND_ADDR, unary(), NULL);
    }
    if (consume("*")){
        return new_node(ND_DEREF, unary(), NULL);
    }
    return primary();
}

Type *get_type(Node *node){
    if (node == NULL) {
        return NULL;
    }
    if (node->type){
        return node->type; 
    }
    Type *t = get_type(node->lhs);
    if (t == NULL){ // lhsにはnodeがない場合
        t = get_type(node->rhs);
    }
    if (t && node->kind == ND_DEREF){ // node dereference(ポインタのこと)
        t = t->ptr_to;
        if (t == NULL){
            error("invalid dereference");
        }
        return t;
    }
    return t;
}

// primary    = num
//             | ident ("(" expr* ")")?
//             | "(" expr ")"
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
        // 関数呼び出しでない場合: 変数.
        return variable(tok);
    }
    // そうでなければ数値のはず
    return new_node_num(expect_number());
}


Node* define_variable(){

    // pointer対策. linked list.
    Type *type;
    type = calloc(1, sizeof(Type));
    type->ty = INT;
    type->ptr_to = NULL;
    while (consume("*")){// pointerであれば
        Type *t;
        t = calloc(1, sizeof(Type));
        t->ty = PTR;
        t->ptr_to = type;
        type = t; // これ, 大丈夫? overwriteしてない?
    }// pointer宣言しているので, type, t自体はアドレスが入っている
     // なので, typeというpointerはt->ptr_toに入れているので, overwriteしておk
     // typeにt(pointer)を入れて, このtのpointerを次のt->ptr_toにまた入れる, を繰り返す.
    
    Token *tok = consume_kind(TK_IDENT); // 識別子
    if (tok == NULL){
        error("Invalid args");
    }

    // 配列かチェック
    int offset_size = type->ty == ARRAY ? 8 : 4; // 4byteのデータの型(intとか)の配列を想定
    // もしarray(int)なら, 4*配列サイズ. 変数(int)なら8.
    while (consume("[")){ // 二重, 三重,...配列に対応
        Type *t;
        t = calloc(1, sizeof(Type));
        t->ty = ARRAY;
        t->ptr_to = type;
        t->array_size = expect_number();
        type = t;
        offset_size *= t->array_size;
        //type->ty = ARRAY;
        //type->array_size =expect_number(); // 中身の数字を返してくれる.
        expect("]");
    }

    // REVIEW: offset_sizeを8の倍数に合わせる
    while((offset_size%8) != 0){
        offset_size += 4;
    }

    Node *node = calloc(1, sizeof(Node));//未定義の変数の分のメモリを確保
    node->kind = ND_LVAR;
    LVar *lvar = find_lvar(tok);
    if(lvar != NULL){
        char name[100] = {0};
        memcpy(name, tok->str, tok->len);
        error("redefined variable: %s", name);
    }
    lvar = calloc(1, sizeof(LVar));
    lvar->next = locals[cur_func];
    lvar->name = tok->str;
    lvar->len = tok->len;
    if (locals[cur_func] == NULL){
        lvar->offset = 8;
    }else{
        lvar->offset = locals[cur_func]->offset + offset_size;
    }
    lvar->type = type; // Type *typeで定義した情報を
    node->offset = lvar->offset;
    node->type = lvar->type;
    locals[cur_func] = lvar;
    return node;
}

// a[x] は　*(a+x)
Node* variable(Token *tok){
    Node *node = calloc(1, sizeof(Node));//未定義の変数の分のメモリを確保
    node->kind = ND_LVAR;
    LVar *lvar = find_lvar(tok);
    if(lvar == NULL){
        char name[100] = {0};
        memcpy(name, tok->str, tok->len);
        error("undefined variable: %s", name);
    }
    node->offset = lvar->offset;
    node->type = lvar->type;
    while(consume("[")){
        // *(a + 1)
        Node *add = calloc(1, sizeof(Node));
        add->kind = ND_ADD;
        add->lhs = node;
        add->rhs = expr(); // 上の例では, 1
        node = calloc(1, sizeof(Node));
        node->kind = ND_DEREF;
        node->lhs = add;
        expect("]");
    }
    return node;
}
