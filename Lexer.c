#include <stdio.h>
#include <string.h>
#include<stdlib.h>

union d_type{
    int INT;
    float FLOAT;
};

typedef enum{
    INT,
    FLOAT
}DATA_TYPE;

typedef struct{
    union d_type var;
    char var_name;
    DATA_TYPE var_type;
    struct Node* Next;
}Node;

void ver(){
     
}

int main() {
    char *line;
    int i = 0;
    char c;
    char index[20]="";
    line = (char *)malloc(sizeof(char));
    
        while ((c = getc(stdin)) != ';') {
            line[i] = c;
            i++;
            line = (char *)realloc(line, (i + 1) * sizeof(char));
        }

        line[i] = '\0';
        printf("%s", line);
        for(i=0;i<strlen(line);i++){
            if(line[i] == ' '){
                break;
            }
            index[i]=line[i];
        }
        free(line);
    
    printf("\n%s",index);
    return 0;
}
