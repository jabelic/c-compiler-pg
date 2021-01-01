#! /bin/bash

assert(){
    expected="$1"
    input="$2"
    
    ./9cc "$input" > tmp.s
    # cc -o tmp tmp.s
    cd func
    cc -c func.c
    cd ..
    cc -o tmp tmp.s func/func.o
    ./tmp
    actual="$?"

    if [ "$actual" = "$expected" ]; then
        echo "$input => $actual"
    else
        echo "$input => $expected expected, but got $actual"
        exit 1
    fi
}

assert 3 "main() {
    int x;
    int y;
    x = 3;
    y = &x;
    return *y;
}"

assert 3 "main() {
    int x;
    x = 3;
    return x;
}"
assert 7 "main(){ return 3+4;}"
assert 2 "main(){ return 2; }"
assert 2 "main () return 2;" # これを禁止したい
# assert 3 "
# main(){
#     addfunc(1, 1, 2);
# }
# addfunc(a, b, c){ 
#     return a+c;
# }"
# assert 55 "main () {
#     int a;
#     a = 10;
#     return sum(a);
# }
# sum(n) {
#     int a;
#     int b;
#     b = 20;
#     if (n < 0) return 0;
#     return n + sum(n - 1);
# }"
assert 0 "main(){ return 0; }"
assert 42 "main() { return 42; }"
assert 21 "main(){ return 5+20-4; }"
assert 41 "main(){ return 12 + 34 - 5 ;}"
assert 47 "main(){ return 5+6*7; }"
assert 15 "main(){ return 5*(9-6); }"
assert 4 "main(){ return (3+5)/2; }"
assert 10 "main(){ return -10+20; }"
assert 10 "main(){ return - -10; }"
assert 10 "main(){ return - - +10; }"

assert 0 "main(){ return  0==1; }"
assert 1 "main(){ return 42==42; }"
assert 1 "main(){ return 0!=1; }"
assert 0 "main(){ return 42!=42; }"

assert 1 "main(){ return 0<1; }"
assert 0 "main(){ return 1<1; }"
assert 0 "main(){ return 2<1; }"
assert 1 "main(){ return 0<=1; }"
assert 1 "main(){ return 1<=1; }"
assert 0 "main(){ return 2<=1; }"

assert 1 "main(){ return 1>0; }"
assert 0 "main(){ return 1>1; }"
assert 0 "main(){ return 1>2; }"
assert 1 "main(){ return 1>=0; }"
assert 1 "main(){ return 1>=1; }"
assert 0 "main(){ return 1>=2; }"


# # variable
assert 14 "main(){
    int a;
    a = 3;
    int b;
    b = 5 * 6 - 8;
    return a + b / 2; }"

# # return 
assert 6 "main(){
    int foo;
    int bar;
    foo = 1;
    bar = 2 + 3;
    return (foo + bar); }"
# ()を抜くとError. 何故だろう.

assert 5 "main(){ return 5; }"
# assert 14 "main(){ 
#     int a;
#     int b;
#     a = 3;
#     b = 5 * 6 - 8;
#     return a + b / 2; }"

# # if
assert 3 "main() {
    int a;
    a = 3;
    if(a == 3) return a;
}"
# assert 5 "main(){
# if(3 != 3) return 3;
# return 5;}"
# assert 5 "main(){a = 3;
# b = 5;
# if(a == 5) return a;
# else return b;}"

# assert 11 "i = 0;
# while (i <= 10) i = i + 1;
# return i;"

# assert 30 "a = 0;
# for (i = 0; i < 10; i = i + 1) a = a + 2;
# return i + a;"

# assert 30 "a = 0;
# for (i = 0; i < 10; i = i + 1){
#     a = a + 2;
# }
# return i + a;"

# assert 10 "a = 0;
# for (;a < 10;) a = a + 1;
# return a;"

# assert 6 "a = 3;
# if (a == 1) return 4;
# if (a == 2) return 5;
# if (a == 3) return 6;"

# assert 10 "a = 0;
# for(;;a = a + 1) if (a == 5) return 10;
# return 2;"

# assert 0 "foo();"
# assert 0 "var(3, 4);"
# assert 0 "var2(3, 4, 5);"


echo OK