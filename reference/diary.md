# C-compiler / C言語拡張 メモ集

基本的には日記形式で, 下の資料に沿って進めていき, 下の資料で詰まったところなどを書いていく. 資料に掲載されているコード, 説明などは基本的には記載しない. 同じようなCコンパイラを実装したい場合, このリファレンスは下の資料に沿って実装を進めていく際のサイドリファレンスとして活用されることが望ましい.

## 参考資料一覧
[低レイヤを知りたい人のためのCコンパイラ作成入門](https://www.sigbus.info/compilerbook#%E3%82%B9%E3%83%86%E3%83%83%E3%83%9791%E6%96%87%E5%AD%97%E3%81%AE%E3%83%AD%E3%83%BC%E3%82%AB%E3%83%AB%E5%A4%89%E6%95%B0)

[Cコンパイラを作ってみる。/プラスウィングTV](https://www.youtube.com/watch?v=EVn-ZPuogd4&list=PLp_EUEO9JJP08ApAdaTYKHsonrLyKzdvp)

[Cコンパイラを作ってみる。/プラスウィングTV - github](https://github.com/pluswing/c_compiler)



## 環境

MacBook Pro (13-inch, 2020, Two Thunderbolt 3 ports)

macOS Catalina 10.15.6

docker for mac 2.4.0.0

Ubuntu 20.04.1 LTS \n \l



## Day1

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

`$ docker-comopse up -d`

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



## Day2
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


## Day3
[ - 低レイヤを知りたい人のためのCコンパイラ作成入門]()

[ - github/pluswing/c_compiler]()

## Day4
[ - 低レイヤを知りたい人のためのCコンパイラ作成入門]()

[ - github/pluswing/c_compiler]()

## Day5
[ - 低レイヤを知りたい人のためのCコンパイラ作成入門]()

[ - github/pluswing/c_compiler]()


## Day6

[ - 低レイヤを知りたい人のためのCコンパイラ作成入門]()

[ - github/pluswing/c_compiler]()


## Day7

[ - 低レイヤを知りたい人のためのCコンパイラ作成入門]()

[ - github/pluswing/c_compiler]()









### 文字列, 配列

`char *p = "hoge"`と`char p[] = "hoge"`は同じ意味.


## マクロ

関数形式

### atoi






