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

# array pointer
assert 3 "int main(){
    int a[2];
    *a = 1;
    *(a + 1) = 2;
    int *p;
    p = a;
    return *p + *(p + 1);
}"

# array
assert 0 "int main() {
  int a[10];
  return 0;
}"
assert 0 "int main() {
  int a[5][5];
  return 0;
}"

# sizeof
assert 4 "int main() {
    int x;
    return sizeof(x+2);
}"
assert 8 "int main() {
    int *x;
    return sizeof(x);
}"
assert 4 "int main() {
    int *x;
    return sizeof(*x);
}"
assert 4 "int main() {
    return sizeof(1);
}"
assert 8 "int main() {
    int *x;
    return sizeof(x+8);
}"

assert 4 "int main() {
  return sizeof(sizeof(1));
}"

assert 4 "int main(){ 
    int *p;
    alloc4(&p, 1, 2, 4, 8);
    int *q;
    q = p + 2;
    return *q;
}"

assert 8 "int main(){ 
    int *p;
    alloc4(&p, 1, 2, 4, 8);
    int *q;
    q = p + 3;
    return *q;
}"

assert 2 "int main() {
  int *p;
  alloc4(&p, 1, 2, 4, 8);
  int *q;
  q = p + 3;
  q = q - 2;
  return *q;
}"


assert 3 "int main() {
  int x;
  int *y;
  y = &x;
  *y = 3;
  return x;
}
"

assert 0 'int main() { return 0; }'
assert 42 'int main() { return 42; }'
assert 21 'int main() { return 5+20-4; }'
assert 41 'int main() { return  12 + 34 - 5 ; }'
assert 47 'int main() { return 5+6*7; }'
assert 15 'int main() { return 5*(9-6); }'
assert 4 'int main() { return (3+5)/2; }'
assert 10 'int main() { return -10+20; }'
assert 10 'int main() { return - -10; }'
assert 10 'int main() { return - - +10; }'

assert 0 'int main() { return 0==1; }'
assert 1 'int main() { return 42==42; }'
assert 1 'int main() { return 0!=1; }'
assert 0 'int main() { return 42!=42; }'

assert 1 'int main() { return 0<1; }'
assert 0 'int main() { return 1<1; }'
assert 0 'int main() { return 2<1; }'
assert 1 'int main() { return 0<=1; }'
assert 1 'int main() { return 1<=1; }'
assert 0 'int main() { return 2<=1; }'

assert 1 'int main() { return 1>0; }'
assert 0 'int main() { return 1>1; }'
assert 0 'int main() { return 1>2; }'
assert 1 'int main() { return 1>=0; }'
assert 1 'int main() { return 1>=1; }'
assert 0 'int main() { return 1>=2; }'

assert 3 'int main() { int a; a=3; return a; }'
assert 8 'int main() { int a; int z; a=3; z=5; return a+z; }'
# assert 3 'int main() { int a=3; return a; }'
# assert 8 'int main() { int a=3; int z=5; return a+z; }'

assert 1 'int main() { return 1; 2; 3; }'
assert 2 'int main() { 1; return 2; 3; }'
assert 3 'int main() { 1; 2; return 3; }'

assert 3 'int main() { int foo; foo=3; return foo; }'
# assert 8 'int main() { int foo123=3; int bar=5; return foo123+bar; }'

assert 3 'int main() { if (0) return 2; return 3; }'
assert 3 'int main() { if (1-1) return 2; return 3; }'
assert 2 'int main() { if (1) return 2; return 3; }'
assert 2 'int main() { if (2-1) return 2; return 3; }'

assert 3 'int main() { {1; {2;} return 3;} }'

assert 10 'int main() { int i; i=0; i=0; while(i<10) i=i+1; return i; }'
# assert 55 'int main() { int i=0; int j=0; while(i<=10) {j=i+j; i=i+1;} return j; }'

# assert 55 'int main() { int i=0; int j=0; for (i=0; i<=10; i=i+1) j=i+j; return j; }'
assert 3 'int main() { for (;;) return 3; return 5; }'
assert 1 "int main() { int i; i = 4; i = 1; return i; }"
echo OK