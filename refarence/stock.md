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




## Day2
## Day3
## Day4
## Day5
## Day6
## Day7















### 文字列, 配列

`char *p = "hoge"`と`char p[] = "hoge"`は同じ意味.


## マクロ

関数形式

### atoi



---

### コマンドライン引数

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

### ファイルへの権限の設定

`$ ./test.sh`

`$ bash: ./test.sh: /bin/bash: bad interpreter: Permission denied`

`$ chmod 761 test.sh`



| モード(数字) | モード(アルファベット) | 権限|
| --- | --- | --- |
|4|r|読み取り|
|2|w|書き込み|
|1|x|実行|




