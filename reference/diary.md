# C-compiler / C言語拡張 メモ集

基本的には日記形式で, 下の資料に沿って進めていき, 詰まったところや気づいたこと, 調べてわかったことなどを書いていく. 資料に掲載されているコード, 説明などは基本的には記載しない. 同じようなCコンパイラを実装したい場合, このリファレンスは下の資料に沿って実装を進めていく際のサイドリファレンスとして活用されることが望ましい.

## 目的

数値計算のためのCのサブセット言語を構築する。

機能要件
- 静的型付け言語
- 演算
    - `3/2 = 1.5`
      - すべての四則演算結果をdoubleにするか？
    - 累乗 `**` ->  `^`
    - インクリメント(デクリメント)演算子の禁止 `++, --`
      - `+=1, -=1`
- 論理演算子
  - XOR: `A^B` -> `A XOR B`
  - AND: `A & B` -> `A AND B`
  - OR: `A | B` -> `A OR B`
    - 難しいかも...
- 関数
  - すべての関数の処理内容は`{}`内に記述すること。
- 組み込み関数
  - `len(hoge)`
    - `#define ARRAY_LENGTH(array) (sizeof(array)/sizeof(array[0]))`
  - `map(func, ...)`
    - [C言語でmapしたかった](https://takoeight0821.hatenablog.jp/entry/2015/03/06/231814)
  - `dict()`(辞書型, 連想配列)
- できれば
  - RICS-V対応
    - 命令セットが違うので、書き換える必要がある。大変...
  - `*a = [2,1,3,4] `


## 参考資料一覧
[低レイヤを知りたい人のためのCコンパイラ作成入門](https://www.sigbus.info/compilerbook#%E3%82%B9%E3%83%86%E3%83%83%E3%83%9791%E6%96%87%E5%AD%97%E3%81%AE%E3%83%AD%E3%83%BC%E3%82%AB%E3%83%AB%E5%A4%89%E6%95%B0)

[Cコンパイラを作ってみる。/プラスウィングTV](https://www.youtube.com/watch?v=EVn-ZPuogd4&list=PLp_EUEO9JJP08ApAdaTYKHsonrLyKzdvp)

[Cコンパイラを作ってみる。/プラスウィングTV - github](https://github.com/pluswing/c_compiler)



## 環境

MacBook Pro (13-inch, 2020, Two Thunderbolt 3 ports)

macOS Catalina 10.15.6

docker for mac 2.4.0.0

Ubuntu 20.04.1 LTS \n \l



## Part1

`$ mkdir c-compiler && cd c-compiler`

```dockerfile
FROM ubuntu:latest
RUN apt update && DEBIAN_FRONTEND=noninteractive apt install -y gcc make git binutils libc6-dev gdb sudo
RUN adduser --disabled-password --gecos '' user
RUN echo 'user ALL=(root) NOPASSWD:ALL' > /etc/sudoers.d/user
USER user
WORKDIR /home/user
```


```docker-compose.yaml
version: '3'
services:
  app:
    build: ./
    volumes:
      - "./:/home/user/data"
    tty: true
```

`$ docker-compose up -d`

`$ docker-compose exec web bash`


[電卓レベルの言語の作成 - 低レイヤを知りたい人のためのCコンパイラ作成入門](https://www.sigbus.info/compilerbook#%E9%9B%BB%E5%8D%93%E3%83%AC%E3%83%99%E3%83%AB%E3%81%AE%E8%A8%80%E8%AA%9E%E3%81%AE%E4%BD%9C%E6%88%90)
に入っていく.


[ステップ1：整数1個をコンパイルする言語の作成 - 低レイヤを知りたい人のためのCコンパイラ作成入門](https://www.sigbus.info/compilerbook#%E3%82%B9%E3%83%86%E3%83%83%E3%83%971%E6%95%B4%E6%95%B01%E5%80%8B%E3%82%92%E3%82%B3%E3%83%B3%E3%83%91%E3%82%A4%E3%83%AB%E3%81%99%E3%82%8B%E8%A8%80%E8%AA%9E%E3%81%AE%E4%BD%9C%E6%88%90)


[#02 first implements and test - github/pluswing/c_compiler](https://github.com/pluswing/c_compiler/tree/9d4d97bd5c249e524ab5c3b3f1d81e1fb9c52149)

- ### コマンドライン引数

```c
int main(int argc, char **argv){
    ...
}

```

`> ./a.out 100 abc`
と実行した時, 


```bash
argc=3; コマンドライン引数の数
argv[0]="./a.out";
argv[1]="100";
argv[2]="abc";
```

である.

- ### atoi

関数形式のマクロ. 

`printf("  mov rax, %d\n", atoi(argv[1]));// atoi: 文字をint型に変換`


- ### ファイルの権限の設定

`$ ./test.sh`

`$ bash: ./test.sh: /bin/bash: bad interpreter: Permission denied`

`$ chmod 761 test.sh`



| モード(数字) | モード(アルファベット) | 権限|
| --- | --- | --- |
|4|r|読み取り|
|2|w|書き込み|
|1|x|実行|

- 参考: [Linuxの権限確認と変更(chmod)（超初心者向け） - Qiita](https://qiita.com/shisama/items/5f4c4fa768642aad9e06)

- ### ccコンパイラでアセンブラを実行形式に

CCコンパイラはアセンブルを実行形式に変えることもできる。

`$ cc -o main main.c`

 `main.c`のコンパイル結果を`main`とする

`$ ./main 123 > temp.s`

 `main.c`はアセンブラコードを`printf`しているだけ。`.s`はアセンブラの拡張子

`$ cc -o temp temp.s`

　アセンブラ`temp.s`の実行形式を`temp`とする

`$ ./temp`

`$ echo $?`

`>123`


[ステップ2：加減算のできるコンパイラの作成 - 低レイヤを知りたい人のためのCコンパイラ作成入門](https://www.sigbus.info/compilerbook#%E3%82%B9%E3%83%86%E3%83%83%E3%83%972%E5%8A%A0%E6%B8%9B%E7%AE%97%E3%81%AE%E3%81%A7%E3%81%8D%E3%82%8B%E3%82%B3%E3%83%B3%E3%83%91%E3%82%A4%E3%83%A9%E3%81%AE%E4%BD%9C%E6%88%90)


[#03 makefile, add, sub - github/pluswing/c_compiler](https://github.com/pluswing/c_compiler/tree/5bece5ac01d7fbc84985e6f0df5fdafa7df3f3b6)

- ### strtol

関数形式のマクロ. 

```code.c
#include <stdio.h>
#include <stdlib.h>
 
int main(){
    char *user_input;
    user_input = "pospodf";
    char *p = user_input;
    printf("%ld\n", strtol(p, &p, 10));
    p = "100";
    printf("%ld\n", strtol(p, &p, 10));
    return 0;
}
```

```bash
> 0
> 100
```

```def.c
#include <stdlib.h>
long strtol(const char *s, char **endptr, int base);
```
`s`が数字ならその数字を`base`進数でlon型に変換して値を返す. そうでないならその文字列`s`のポインタを`**endptr`に格納し, `0`を返す.



## Part2
[ステップ3：トークナイザを導入 - 低レイヤを知りたい人のためのCコンパイラ作成入門](https://www.sigbus.info/compilerbook#%E3%82%B9%E3%83%86%E3%83%83%E3%83%973%E3%83%88%E3%83%BC%E3%82%AF%E3%83%8A%E3%82%A4%E3%82%B6%E3%82%92%E5%B0%8E%E5%85%A5)

[#04 tokenizer - github/pluswing/c_compiler](https://github.com/pluswing/c_compiler/tree/cd213d0d2820475796a0048e50bdaabc77124d7b)

- ### Arror演算子

`token->kind`

アロー演算子「->」は「*」と「.」を一つにまとめた演算子. 構造体のメンバをアクセスしている.

左側はpointer変数, 右側は構造体のメンバ.

アロー演算子とは、ポインタから構造体のメンバへアクセスするための演算子

`(*構造体ポインタ型変数).メンバ名 = 構造体ポインタ型変数->メンバ名`

参考: https://daeudaeu.com/arrow/


- ### 構造体

```C
struct　point {
  int x;
  int y;
}; // 構造体の宣言
struct point a; // 構造体変数を宣言
```

`point`を構造体タグ名, `a`は構造体変数である.
構造体変数を宣言することで, `a.x`のようにメンバにアクセスできるようになる.

これは

```C
struct　point {
  int x;
  int y;
} a;
```
と言う風にまとめることができる.


- #### typedef
typedef文を使って構造体を自作の型として定義できる.

`typedef char * STRIGN;`

とすると, 

`STRING s1 = "hoge";`と言う風にできる.


```C
typedef struct point {
  int x;
  int y;
} Point;
```
とすると, 

`Point b`とすることで構造体変数を宣言することができる.


- ### enum(型列挙)

入力された文字列の種類を表すために列挙型を使う.

```C
typedef enum {
    TK_RESERVED,
    TK_NUM,
    TK_EOF,
    TK_RESERVED, // 記号
    TK_NUM,  // 整数トークン
    TK_EOF, // 入力の終わりを表すトークン
} Tokenkind;

typedef struct Token Token;

struct Token{
    Tokenkind kind; // トークンの型
    Token *next;   // 次の入力トークン
    int val;       // kindがTK_NUMの場合, その数値
    char *str;     // トークン文字列
};
```

構造体`Token`のメンバ`kind`の型は, 列挙型(構造体変数名: `Tokenkind`)である.


- ### calloc
```c
#include <stdlib.h>
void *calloc(size_t n, size_t size);
```
[http://www9.plala.or.jp/sgwr-t/lib/calloc.html]
メモリを確保

>ヒープメモリから size バイトのブロックを n個割り当てます。確保されたブロックは 0 クリアされます。

- ### isdigit

`isdigit()`

10進数であるかどうか


- ### error報告関数
要はprintfを独自実装している

```c
void error(char *fmt, ...){
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    exit(1);
}
```

- 可変長引数をとる関数にしていて, fmtは文字列.
- `va_list`
  - `typedef char *va_list`
  - char型のポインタ `ap`を宣言.
- `va_start`
  - 可変な部分の直前に置かれる引数の名前をとる. つまり関数が肩を認識している最後のパラメータ. ここでは`fmt`.
  - `ap`に各引数のポインタを格納している.
- `vfprintf`
  - apに格納されたアドレスを辿って, fmtの変換書式文字列で指定された書式を適用して当てはめた文字列が`stderr`に格納される 
  
  ```c
  #include <stdio.h>
  #include <stdarg.h>
  int vfprintf(
    FILE * restrict stream,
    const char * restrict format,
    va_list arg
  );
  ```
  - streamには`stdout`か`stderr`を指定する.


## Part3
[ステップ4：エラーメッセージを改良 - 低レイヤを知りたい人のためのCコンパイラ作成入門](https://www.sigbus.info/compilerbook#%E3%82%B9%E3%83%86%E3%83%83%E3%83%974%E3%82%A8%E3%83%A9%E3%83%BC%E3%83%A1%E3%83%83%E3%82%BB%E3%83%BC%E3%82%B8%E3%82%92%E6%94%B9%E8%89%AF)

[#06 error_at - github/pluswing/c_compiler](https://github.com/pluswing/c_compiler/commit/bb37f9fe273ee377f61ab4388111e7cf07e5897f)

- 配列は線形に取るので, 該当の文字のアドレスから先頭のアドレスを引けば必要な空白文字の数を知ることができる, と言う性質を使ってエラー箇所を指摘するerror関数を実装.

## Part4
[文法の記述方法と再帰下降構文解析 - 低レイヤを知りたい人のためのCコンパイラ作成入門](https://www.sigbus.info/compilerbook#%E6%96%87%E6%B3%95%E3%81%AE%E8%A8%98%E8%BF%B0%E6%96%B9%E6%B3%95%E3%81%A8%E5%86%8D%E5%B8%B0%E4%B8%8B%E9%99%8D%E6%A7%8B%E6%96%87%E8%A7%A3%E6%9E%90)

[ステップ5：四則演算のできる言語の作成 - 低レイヤを知りたい人のためのCコンパイラ作成入門](https://www.sigbus.info/compilerbook#%E3%82%B9%E3%83%86%E3%83%83%E3%83%975%E5%9B%9B%E5%89%87%E6%BC%94%E7%AE%97%E3%81%AE%E3%81%A7%E3%81%8D%E3%82%8B%E8%A8%80%E8%AA%9E%E3%81%AE%E4%BD%9C%E6%88%90)

[ステップ6：単項プラスと単項マイナス - 低レイヤを知りたい人のためのCコンパイラ作成入門](https://www.sigbus.info/compilerbook#%E3%82%B9%E3%83%86%E3%83%83%E3%83%976%E5%8D%98%E9%A0%85%E3%83%97%E3%83%A9%E3%82%B9%E3%81%A8%E5%8D%98%E9%A0%85%E3%83%9E%E3%82%A4%E3%83%8A%E3%82%B9)

[#08 tree to code - github/pluswing/c_compiler](https://github.com/pluswing/c_compiler/tree/70d15375940c4d43ce2e83d908a583cec870e22b)

再帰下降構文解析
```
expr    = mul ("+" mul | "-" mul)*
mul     = primary ("*" primary | "/" primary)*
primary = num | "(" expr ")"
```

`user_input`をglobal変数として宣言しているので, 引数で取らなくても参照できるようにしている. 


## Part5

[ステップ7: 比較演算子 - 低レイヤを知りたい人のためのCコンパイラ作成入門](https://www.sigbus.info/compilerbook#%E3%82%B9%E3%83%86%E3%83%83%E3%83%977-%E6%AF%94%E8%BC%83%E6%BC%94%E7%AE%97%E5%AD%90)

[#11 compare expression (tokenize) - github/pluswing/c_compiler](https://github.com/pluswing/c_compiler/commit/9f9f6b0d85cd3254bc9c0dccc464118aba66b352)

比較演算子は二文字なので, 演算子の文字数を表す項を構造体(Token)のメンバに追加.


```
expr       = equality
equality   = relational ("==" relational | "!=" relational)*
relational = add ("<" add | "<=" add | ">" add | ">=" add)*
add        = mul ("+" mul | "-" mul)*
mul        = unary ("*" unary | "/" unary)*
unary      = ("+" | "-")? primary
primary    = num | "(" expr ")"
```


```x86
 pop rdi
 pop rax
 cmp rax, rdi
 sete al
 movzb rax, al
```

cmp
- 比較命令
- 結果はフラグレジスタと言うところに格納される
- >フラグレジスタは整数演算や比較演算命令が実行されるたびに更新されるレジスタで、結果が0かどうかといったビットや、桁あふれが発生したかどうかというビット、結果が0未満かどうかといったビットなどを持っています。

sete
- フラグレジスタの特定のbitをALにコピーする. 比較演算子`==`の場合に使う. `<`ではsetl、`<=`ではsetle、`!=`ではsetneを使う.

al
- RAXの下位8ビットを表す.

movzb
- RAX全体をゼロクリアする.



## Part6

[分割コンパイルとリンク - 低レイヤを知りたい人のためのCコンパイラ作成入門](https://www.sigbus.info/compilerbook#%E5%88%86%E5%89%B2%E3%82%B3%E3%83%B3%E3%83%91%E3%82%A4%E3%83%AB%E3%81%A8%E3%83%AA%E3%83%B3%E3%82%AF)

[#13 code division - github/pluswing/c_compiler](https://github.com/pluswing/c_compiler/commit/0daade15023146c48508d0a7069b28a3d2b9daba)


[ステップ8: ファイル分割とMakefileの変更 - 低レイヤを知りたい人のためのCコンパイラ作成入門](https://www.sigbus.info/compilerbook#%E3%82%B9%E3%83%86%E3%83%83%E3%83%978-%E3%83%95%E3%82%A1%E3%82%A4%E3%83%AB%E5%88%86%E5%89%B2%E3%81%A8makefile%E3%81%AE%E5%A4%89%E6%9B%B4)

[#14 code division and update makefile - github/pluswing/c_compiler](https://github.com/pluswing/c_compiler/tree/a4ad44da31a78c005d3a3140245d3f73a0abb5e7)




## Part7

[ステップ9：1文字のローカル変数 - 低レイヤを知りたい人のためのCコンパイラ作成入門](https://www.sigbus.info/compilerbook#%E3%82%B9%E3%83%86%E3%83%83%E3%83%9791%E6%96%87%E5%AD%97%E3%81%AE%E3%83%AD%E3%83%BC%E3%82%AB%E3%83%AB%E5%A4%89%E6%95%B0)

[#18 variable 04 - github/pluswing/c_compiler](https://github.com/pluswing/c_compiler/tree/6b6e425284689618a4d91efb2d85d29922211ce2)


文を`;`で区切ってコンパイルさせることができるようになった.



## Part8

[ステップ10：複数文字のローカル変数 - 低レイヤを知りたい人のためのCコンパイラ作成入門](https://www.sigbus.info/compilerbook#%E3%82%B9%E3%83%86%E3%83%83%E3%83%9710%E8%A4%87%E6%95%B0%E6%96%87%E5%AD%97%E3%81%AE%E3%83%AD%E3%83%BC%E3%82%AB%E3%83%AB%E5%A4%89%E6%95%B0)

[#19 multiple char variable 01 - github/pluswing/c_compiler](https://github.com/pluswing/c_compiler/tree/885b35c690c165f3080fa96831383d9e15f3ff36)


`memcmp(tok->str, var->name, var->len)`は一致していれば0が返ってくるので, `!`でひっくり返してあげないとTRUEとして認識されない.

- tokenize
  - 入力文字列を構造体(Token)を用いて連結リストにする.
  - 変数の認識：文字であれば, その後に文字が続くかどうか判断して, 続くところまで調べて, 何文字の変数かを`int len`として持たせる.
  - `char user_input`の先頭の文字のアドレスを`char p`に入れて, アドレスを進めていく.
  - `Token head`を宣言, そのアドレスを`cur`にコピー.
  - `cur`を`new_token`に渡して, `cur.next`に`new_token`内で新しく宣言した`Token *tok`のアドレスを格納する.
    - `Token *tok`には`p`の現在の文字の情報が格納される.
  - `cur`が`new_token`の返した`tok`を受け取るので, 最後に登録したトークンを`cur`が逐一参照している事になる. 

- generater
  - 再帰下降構文木を生成
    - 文字であれば, スタック文字データ分進ませる. 型はLVar.
  - 再帰下降構文木を辿って, アセンブラのコードを出力

## Part9

[ステップ11：return文 - 低レイヤを知りたい人のためのCコンパイラ作成入門](https://www.sigbus.info/compilerbook#%E3%82%B9%E3%83%86%E3%83%83%E3%83%9711return%E6%96%87)

[#20 multiple char variable 02, return syntax - github/pluswing/c_compiler](https://github.com/pluswing/c_compiler/tree/87b344479270a58f5ccab88e50c04c2161621e7e)


reference branchのコミットログ[rui314/chibicc](https://github.com/rui314/chibicc/commits/reference?after=ce61154cf542e630bc3e40262fdacdf20bf91b90+69&branch=reference)
- これで良いのかな. 

[pluswing/c_compiler](https://github.com/pluswing/c_compiler/tree/master)
- こっちに合わせた方が良いかもと言う気持ちになってきた

pop命令
- スタック領域からレジスタ領域にデータを持ってくる



## Part10

[ステップ12: 制御構文を足す - 低レイヤを知りたい人のためのCコンパイラ作成入門](https://www.sigbus.info/compilerbook#%E3%82%B9%E3%83%86%E3%83%83%E3%83%9712-%E5%88%B6%E5%BE%A1%E6%A7%8B%E6%96%87%E3%82%92%E8%B6%B3%E3%81%99)

[#22 if syntax - github/pluswing/c_compiler](chrome-extension://klbibkeccnjlkjkiokjodocebajanakg/suspended.html#ttl=%2322%20if%20syntax%20%C2%B7%20pluswing%2Fc_compiler%40bfc7cdc&pos=0&uri=https://github.com/pluswing/c_compiler/commit/bfc7cdc92e497353df6f9bb35e27be270798140d)


`if (A) return B;`を実装した. まだ`else`は通らない.




## Part11

[ステップ12: 制御構文を足す - 低レイヤを知りたい人のためのCコンパイラ作成入門](https://www.sigbus.info/compilerbook#%E3%82%B9%E3%83%86%E3%83%83%E3%83%9712-%E5%88%B6%E5%BE%A1%E6%A7%8B%E6%96%87%E3%82%92%E8%B6%B3%E3%81%99)

[#24 refactor else - github/pluswing/c_compiler](https://github.com/pluswing/c_compiler/commit/7f23a743863ea66bed6efff5879af89ffaa890e1)



## Part12

[ステップ12: 制御構文を足す - 低レイヤを知りたい人のためのCコンパイラ作成入門](https://www.sigbus.info/compilerbook#%E3%82%B9%E3%83%86%E3%83%83%E3%83%9712-%E5%88%B6%E5%BE%A1%E6%A7%8B%E6%96%87%E3%82%92%E8%B6%B3%E3%81%99)
[#27 for syntax, label id - github/pluswing/c_compiler](https://github.com/pluswing/c_compiler/tree/9fd446547623bc9096e40028b36af5ddeb182c3d)


- `if else`では`rhs`に`else`の枝を持たせることで構文木を生成している.
- `while`
- `for`ではトークンを先に進めて`;`があるかどうか調べながら`;`の手間のコードをコンパイルする.
- `if`, `else`, `while`, `return`を予約語として`TK_XX`と一緒に持たせて処理を`for`で回すことで同じ処理を何度も書くことを避けた.

## part13

[ステップ12: 制御構文を足す - 低レイヤを知りたい人のためのCコンパイラ作成入門](https://www.sigbus.info/compilerbook#%E3%82%B9%E3%83%86%E3%83%83%E3%83%9712-%E5%88%B6%E5%BE%A1%E6%A7%8B%E6%96%87%E3%82%92%E8%B6%B3%E3%81%99)
[#27 for syntax, label id - github/pluswing/c_compiler](https://github.com/pluswing/c_compiler/tree/9fd446547623bc9096e40028b36af5ddeb182c3d)

アセンブリについて調べた
- [x86 (IA-32) Assembly Language](http://nw.tsuda.ac.jp/lec/i486/)
- ラベルの宣言について. 例えばwhileだと

  ```x86
  .LbeginXXX: <-ラベルの宣言.
    Aをコンパイルしたコード
    pop rax
    cmp rax, 0
    je  .LendXXX <- rax == 0ならば.LendXXXに飛ぶ
    Bをコンパイルしたコード
    jmp .LbeginXXX
  .LendXXX:
  ```


## part14

[ステップ14: 関数の呼び出しに対応する - 低レイヤを知りたい人のためのCコンパイラ作成入門](https://www.sigbus.info/compilerbook#%E3%82%B9%E3%83%86%E3%83%83%E3%83%9714-%E9%96%A2%E6%95%B0%E3%81%AE%E5%91%BC%E3%81%B3%E5%87%BA%E3%81%97%E3%81%AB%E5%AF%BE%E5%BF%9C%E3%81%99%E3%82%8B)
[#29 call without args - github/pluswing/c_compiler](https://github.com/pluswing/c_compiler/commit/1251ce351b725e1d858c243a6fe191403fa8cdc7)


OKと出力する`func/func.c`を用意. 
`func/func.c`と同じ結果になるアセンブリファイル`func/test.s`を用意.
この二つのオブジェクトファイルをccに渡して一つの実行形式にまとめて実行.

$ cc -c func.c <- オブジェクトファイルを生成
$ cc -c test.s
$ cc -o test test.o func.o
$ ./test
> OK
> OK

- nodeに関数名を持たせる枝を追加した.



[#30 function call with args - github/pluswing/c_compiler](https://github.com/pluswing/c_compiler/commit/f89b6a84faf5d5baecb8e74af54f7f38d6f672bf)

## part15

[ステップ14: 関数の呼び出しに対応する - 低レイヤを知りたい人のためのCコンパイラ作成入門](https://www.sigbus.info/compilerbook#%E3%82%B9%E3%83%86%E3%83%83%E3%83%9714-%E9%96%A2%E6%95%B0%E3%81%AE%E5%91%BC%E3%81%B3%E5%87%BA%E3%81%97%E3%81%AB%E5%AF%BE%E5%BF%9C%E3%81%99%E3%82%8B)
[#30 function call with args - github/pluswing/c_compiler](https://github.com/pluswing/c_compiler/commit/f89b6a84faf5d5baecb8e74af54f7f38d6f672bf)

- 関数で引数を取る.
- 引数はx86のABIで規定されている順番でレジスタにコピーして関数をcallする
  
  - 第一引数から RDI, RSI, RDX, RCX.
- func/func.c に関数`foo(3, 4)`を追加. アセンブリにも同じコードを追加.

  ```asembly
    push 3
    push 4
    pop rsi
    pop rdi
    call var  
  ```

  ```
    mov rdi, 3
    mov rsi, 4
    call var
  ```
  
  でも良い?

- test.shを変えた.




## part16

[ステップ14: 関数の呼び出しに対応する - 低レイヤを知りたい人のためのCコンパイラ作成入門](https://www.sigbus.info/compilerbook#%E3%82%B9%E3%83%86%E3%83%83%E3%83%9714-%E9%96%A2%E6%95%B0%E3%81%AE%E5%91%BC%E3%81%B3%E5%87%BA%E3%81%97%E3%81%AB%E5%AF%BE%E5%BF%9C%E3%81%99%E3%82%8B)
[#32 function RSP boundary - github/pluswing/c_compiler](https://github.com/pluswing/c_compiler/commit/92910a3c224312fa75ccd3afe727a602a0a20542)

- 関数引数を6つまで取る
- 7つの汎用レジスタのうち, RAX以外の6つを引数にあてがう.
  - `char *argRegs[] = {"rdi", "rsi", "rdx", "rcx", "r8", "r9"};`
- X86の呼出規約ではRSPの値は16の倍数でなければならない. push, popは8バイト単位で変更するのでcallする前にRSPが16の倍数であるかどうかをチェックする必要がある.
- スタックは下に向かって伸びる(-8). 上にRSPを移動させるにはRSP+8をする. 関数のcallの場合, 16の倍数でなければ`sub rsp, 8`する. callが終わったら元に戻す(`add rsp, 8`).


[AMD64 ABI の特徴](https://docs.oracle.com/cd/E19253-01/819-0389/fcowb/index.html)

>7 つの汎用レジスタ (%rdi、%rsi、%rdx、%rcx、%r8、%r9、%rax) は、引数をレジスタに渡す関数呼び出しシーケンスにお>ける適切な役割を定義されています。
>2 つのレジスタ (%rsp、%rbp) はスタック管理のために使用されます。
>2 つのレジスタ (%r10、%r11) は一時的なものです。
>5 つのレジスタ (%r12、%r13、%r14、%r15、 %rbx) は呼び出し先保管です。

>基本的な関数呼び出し規約は、AMD ABI では異なります。引数はレジスタに格納されます。単純な整数の引数の場合、最初の引数から順に %rdi、%rsi、%rdx、%rcx、%r8、%r9 レジスタに格納されます。


```c
for(int i = argCount - 1; i >= 0; i--){
  printf("  pop %s\n", argRegs[i]);
}
printf("  mov rax, rsp\n");
printf("  and rax, 15\n"); // 下位4bitをマスク. 15 := 0b00001111. 下位4bitが0であれば16の倍数.
printf("  jnz .L.call.%03d\n", id);
printf("  mov rax, 0\n");
printf("  call %s\n", node->funcname);
printf("  jmp .L.end.%03d\n", id);
printf(".L.call.%03d:\n", id); // 計算結果が0でないとここに飛ぶ.
printf("  sub rsp, 8\n");      // RSPを16の倍数に
printf("  mov rax, 0\n");
printf("  call %s\n", node->funcname);
printf("  add rsp, 8\n");
printf(".L.end.%03d:\n", id);
```
- RSPは16の倍数である必要があるので, RAX && 15することで下位4bitをマスクする.
  - RSPが16nであれば, AND命令は0を返す. 返した値はフラグレジスタに格納される.
  - ここではZFフラグを用いる. 計算結果が0のときに1を返し, 負の数であれば0になる.
  - ZFフラグの状態によってジャンプするかどうかを決定する命令はJZ命令とJNZ命令がある.
    
    > JZ命令は、ZFフラグがセットされている場合に、ディスティネーション・オペランドに指定された場所にジャンプします。逆にJNZ命令は、ZFフラグがセットされていない場合に、ディスティネーション・オペランドに指定された場所にジャンプします。
  - JNZ命令はZFが0の時にジャンプする.
  - RAXは戻り値, 引数の数を格納するので, 今は0.



[フラグレジスタFR](http://www.ics.teikyo-u.ac.jp/wcasl2/tutorial/lesson01/comet08.html)
> FR(フラグレジスタ，Flag Register)は，OF(Overflow Flag)，SF(Sign Flag)，ZF(Zero Flag)の3個のビットからなり，演算命令などの実行によって次の値が設定されます．これらの値は，条件付分岐命令で参照されます．
> OF	算術演算命令の場合は，演算結果が-32768～32767に収まらなくなったとき1になり，それ以外のときは0になる．論理演算命令の場合は，演算結果が0～65535に収まらなくなったとき1になり，それ以外のとき0になる．
> SF	演算結果の符号が負(ビット番号15が1)のとき1，それ以外のとき0になる．
> ZF	演算結果がゼロ(全部のビットが0)のとき1，それ以外のとき0になる．
> 上の図では3桁で描かれていますが，これは16進数でなく，2進数で3桁です．左からOF，SF，ZFになります．


[インラインアセンブラで学ぶアセンブリ言語 第3回 (1/3)：CodeZine（コードジン）](https://codezine.jp/article/detail/485)

- Cではmain関数の戻り値はプログラムの終了コードになる. shellの`$?`という変数にセットされている.
  - RAX


## part17

[ステップ15: 関数の定義に対応する - 低レイヤを知りたい人のためのCコンパイラ作成入門](https://www.sigbus.info/compilerbook#%E3%82%B9%E3%83%86%E3%83%83%E3%83%9715-%E9%96%A2%E6%95%B0%E3%81%AE%E5%AE%9A%E7%BE%A9%E3%81%AB%E5%AF%BE%E5%BF%9C%E3%81%99%E3%82%8B)

- プロローグ とエピローグを`codegen.c`の`gen()`に押し込んだ.
  - 必ず関数内でコードを書くことにした。`main`関数は必須.

- 関数定義
  - 引数の値をスタックに積む
  - ローカル変数が引数に続いてスタックに積まれる.
  - ローカル変数分ズレたRSPを引数のに戻す.
  - ブロック内を処理.
- 関数呼び出し
  - スタックをpopして引数を後ろ側から格納
    - スタックはFILOなので, 頭から引数を入れて取り出すときはケツからということになるs  


## part18

[ステップ16: 単項&と単項* - 低レイヤを知りたい人のためのCコンパイラ作成入門](https://www.sigbus.info/compilerbook#%E3%82%B9%E3%83%86%E3%83%83%E3%83%9716-%E5%8D%98%E9%A0%85%E3%81%A8%E5%8D%98%E9%A0%85)
[#39 & operator and * operator - github/pluswing/c_compiler](https://github.com/pluswing/c_compiler/commit/ad40cf0faeccc6e4dcde8a56ab1087a600097e3d)

- 単項演算子`* &`を追加
- `&x`
  - `x`のアドレスを整数として返す.
  ```
    mov rax, rbp
    sub rax, %d
    push rax
  ```
- `*x`
  - `x`の値を見に行って, その値がアドレスであるとして, そのアドレスにある値を見に行って返す.
  ```
    pop rax
    mov rax, [rax]
    push rax
  ```

## part19

[ステップ17: 暗黙の変数定義を廃止して、intというキーワードを導入する - 低レイヤを知りたい人のためのCコンパイラ作成入門](https://www.sigbus.info/compilerbook#%E3%82%B9%E3%83%86%E3%83%83%E3%83%9717-%E6%9A%97%E9%BB%99%E3%81%AE%E5%A4%89%E6%95%B0%E5%AE%9A%E7%BE%A9%E3%82%92%E5%BB%83%E6%AD%A2%E3%81%97%E3%81%A6int%E3%81%A8%E3%81%84%E3%81%86%E3%82%AD%E3%83%BC%E3%83%AF%E3%83%BC%E3%83%89%E3%82%92%E5%B0%8E%E5%85%A5%E3%81%99%E3%82%8B)

[#40 int (variable) - github/pluswing/c_compiler](https://github.com/pluswing/c_compiler/commit/e8c5e8d7cdc361e58800a56d526f1858da74a285)



## part20

[ - 低レイヤを知りたい人のためのCコンパイラ作成入門]()
[ - github/pluswing/c_compiler]()


## part21

[ - 低レイヤを知りたい人のためのCコンパイラ作成入門]()
[ - github/pluswing/c_compiler]()

