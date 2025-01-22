#include "stdlib.h"
#include "time.h"
#include "stdio.h"
#include "stdint.h"
#include "fastdiv_test.c"



int main(){
    FILE *f = fopen("data.txt", "w");
    uint32_t m[127];
    uint8_t s1[127], s2[127];
    for(int i = 512, j = 0; i < 65536; i += 512, j++){
        fastdiv_prepare(i, &m[j], &s1[j], &s2[j]);
        fprintf(f, "%d,%d ", j, i);
    }
    fprintf(f, "\n\n");
    for(int i = 0; i < 127; i++){
        fprintf(f, "%u", m[i]);
        if(i != 126)
            fprintf(f, ", ");
    }
    fprintf(f, "\n\n");
    for(int i = 0; i < 127; i++){
        fprintf(f, "%u", s1[i]);
        if(i != 126)
            fprintf(f, ", ");
    }
    fprintf(f, "\n\n");
    for(int i = 0; i < 127; i++){
        fprintf(f, "%u", s2[i]);
        if(i != 126)
            fprintf(f, ", ");
    }
    fclose(f);
}


