#include "9cc.h"

char *user_input;

// 現在着目しているトークン
Token *token;

// 複数文字のlocal変数
LVar *locals[100];
int cur_func = 0;

void error_at(char *loc, char *fmt, ...){
    va_list ap;
    va_start(ap, fmt);
    int pos = loc - user_input;
    fprintf(stderr, "%s\n", user_input);
    fprintf(stderr, "%*s", pos, ""); // print pos spaces.
    fprintf(stderr, "^ ");
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    exit(1);
}

void error(char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    exit(1);
}

// 次のトークンが期待している記号のときには、トークンを1つ読み進めて
// 真を返す。それ以外の場合には偽を返す。
bool consume(char *op){
    if (token->kind != TK_RESERVED ||
        strlen(op) != token -> len ||
        memcmp(token->str, op, token->len)){
        return false;
    }
    token = token->next;
    return true;
}

/*
Token *consume_ident(){
    if (token->kind != TK_IDENT ||
        strlen(tok) != token -> len ||
        memcmp(token->str, tok, token->len)){
            return NULL;
        }
    token = token->next;
    return token;
}
*/

Token *consume_kind(Tokenkind kind) {
  if (token->kind != kind) {
    return NULL;
  }
  Token *tok = token;
  token = token->next;
  return tok;
}


void expect(char *op){
    if (token->kind != TK_RESERVED ||
        strlen(op) != token -> len ||
        memcmp(token->str, op, token->len)){ // str=opなら0を返すので, falseと解釈.
        error_at(token->str, "'%s'ではありません", op);
    }
    token = token->next;
}

// 次のトークンが数値の場合、トークンを1つ読み進めてその数値を返す。
// それ以外の場合にはエラーを報告する。
int expect_number(){
    if (token->kind != TK_NUM){
        error_at(token->str, "Not a number");
    }
    int val = token->val;
    token = token->next;
    return val;
}

bool at_eof(){
    return token->kind == TK_EOF;
}

// 新しいトークンを作成してcurに繋げる
Token *new_token(Tokenkind kind, Token *cur, char *str, int len){
    Token *tok = calloc(1, sizeof(Token));
    tok->kind = kind;
    tok->str = str;
    tok->len = len;
    cur->next = tok;
    return tok;
}

bool startswith(char *p, char *q){
    return memcmp(p, q, strlen(q)) == 0;
}

// 英数字 or _　であるかどうかを判定.
int is_alnum(char c){
    return ('a' <= c && c <= 'z') || 
            ('A' <= c && c <= 'Z') ||
            ('0' <= c && c <= '9') ||
            (c == '_');
}

typedef struct ReservedWord ReservedWord;
struct ReservedWord {
    char *word;
    Tokenkind kind;
};

ReservedWord reservedWords[] = {
    {"return", TK_RETURN},
    {"if", TK_IF},
    {"else", TK_ELSE},
    {"while", TK_WHILE},
    {"for", TK_FOR},
    {"int", TK_TYPE},
    {"", TK_EOF},
};


// 入力されたコード列から連結リストを作成. BNFへの準備.
Token *tokenize(){ 
    char *p = user_input;
    Token head;
    head.next = NULL;
    Token *cur = &head;

    while (*p){
        // 空白文字をスキップ
        if (isspace(*p)){
            p++;
            continue;
        }
        if (startswith(p, "==") || startswith(p, "!=") ||
            startswith(p, "<=") || startswith(p, ">=")){
                cur = new_token(TK_RESERVED, cur, p, 2);
                p += 2;
                continue;
            }

        bool found = false;
        for(int i = 0; reservedWords[i].kind != TK_EOF; i++){
            char *wrd = reservedWords[i].word;
            int len = strlen(wrd);
            Tokenkind kind = reservedWords[i].kind;
            if (startswith(p, wrd) && !is_alnum(p[len])){
                cur = new_token(kind, cur, p, len);
                p += len;
                found = true;
                break;
            }
        }
        if (found){
            continue;
        }// なんでこうしないといけない？

        /*if (startswith(p, "return") && !is_alnum(p[6])){
            cur = new_token(TK_RETURN, cur, p, 6);
            //tokens[i].ty = TK_RETURN;
            //tokens[i].str = p;
            //i++;
            p += 6;
            continue;
        }

        if (startswith(p, "if") && !is_alnum(p[2])){
            cur = new_token(TK_IF, cur, p, 2);
            p += 2;
            continue;
        }
        if (startswith(p, "else") && !is_alnum(p[4])){
            cur = new_token(TK_ELSE, cur, p, 4);
            p += 4;
            continue;
        }
        if (startswith(p, "while") && !is_alnum(p[5])){
            cur = new_token(TK_WHILE, cur, p, 5);
            p += 5;
            continue;
        }*/


        if (strchr("+-*/()<>=;{},&", *p)){// char *strchr(const char *s, int c); 
            // 文字列sのなかで最初のcharにcastされたcが見つかった位置を返す
            // なかったらNULLを返す

            cur = new_token(TK_RESERVED, cur, p++, 1); // 文字のポインタをcurにつなげる.
            /*
            cur = new_token(TK_RESERVED, cur, p);
            p++ 
            と同じ. 後方で++するやつらしい...
            だから, ここでは文字を入れて、次の数字のところにポインタを一つ進めている.
            */
            continue;
        }

        if ('a' <= *p && *p <= 'z'){ // ""を使うとtokenizeしてくれない...
            char *c = p;
            while(is_alnum(*c)){
                c++;
            }
            int len = c - p;
            //fprintf(stderr, "debug  %s, %d\n", p, len);
            cur = new_token(TK_IDENT, cur, p, len);
            p = c;
            continue;
        }
        
        if (isdigit(*p)){
            cur = new_token(TK_NUM, cur, p, 0);
            char *q = p;
            cur->val = strtol(p, &p, 10); // pが数字ならn(今は10)進法でlong型に変換して返す.
                                        // 数として認識できない文字に行き当たるとpにその文字のポインタを格納.
            cur->len = p - q;
            continue;
        }

        error_at(p, "Couldn't tokenize...");
    }
    new_token(TK_EOF, cur, p, 0);
    return head.next;
}

LVar *find_lvar(Token *tok){
    for (LVar *var = locals[cur_func]; var; var = var->next){ // 全ての変数名を参照してその変数名が使われているかどうか確認.
        if (var->len == tok->len && !memcmp(tok->str, var->name, var->len)){
            return var;
        }
    }
    return NULL;
}

