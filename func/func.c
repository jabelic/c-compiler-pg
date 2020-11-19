#include <stdio.h>
int foo(){
    printf("OK\n");
    return 1;
}
int var(int x, int y){
    printf("%d\n", x+y);
    return x+y;
}

int var2(int x, int y, int z) {
  printf("%d + %d + %d = %d\n", x, y, z, x + y + z);
  return x + y + z;
}