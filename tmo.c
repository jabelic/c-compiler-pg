#include <stdio.h>
#include <string.h>
#define STR_EQ(s1,s2)       strcmp(s1, s2)==0
void error(char *fmt){
    va_list ap;
    printf("%s", fmt);
}

int main(void){
    /*if( STR_EQ( "abc", "abc" ) ){
        puts( "OK" );
    }if( !STR_EQ( "abc", "ab" ) ){
        puts( "OK" );
    }*/

    char x = "app";
    printf("%c", x);
    error(x);
    return 0;
}

