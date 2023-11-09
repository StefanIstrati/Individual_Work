#include <stdio.h>
#include <string.h>
#include<stdlib.h>
#include<stdbool.h>
union d_type{
    int INT;
    float FLOAT;
};

typedef enum{
    INTEGER,
    FLOAT
}DATA_TYPE;

typedef struct{
    union d_type var;
    char *var_name;
    DATA_TYPE type;
    struct Node* next;
}Node;

void append_int(Node** root, int value, char name[]) {
    Node* new_node = malloc(sizeof(Node));
    if (new_node == NULL) {
        exit(1);
    }
    new_node->next = NULL;
    new_node->var.INT = value;
    new_node->type = INTEGER;
    strcpy(new_node->var_name,name);
    if (*root == NULL) {
        *root = new_node;
    } else {    
        Node* curr = *root;
        while (curr->next != NULL) {
            curr = curr->next;
        }
        curr->next = new_node;
    }
}

Node* Var(int n,char line[n]){
    Node* root = NULL;
    int i;
    for (i = 0;i < n;i++){
        if (line[i] == ' '){
            break;
        }
    }
    i++;
    int k;
    k=0;
    char t[8];
    while (line[i] != ' '){
        t[k] = line[i];
        k++;
        i++;
    }
    i++;
    i++;
    k=0;
    char* name;
    char c;
    name=(char *)malloc(sizeof(char));
    while((c=line[i]) != ','){
        name[k] = c;
        i++;
        k++;
        name = (char *)realloc(name, (k + 1) * sizeof(char));
    }
    if(strcmp(t,"int") == 0){
        append_int(&root,0,name);
    }
    return root;

}

void Input(){
    
}

void Equality(){

}

int ver(char index[20]){
     if(strcmp(index,"var") == 0){
        return 1;
     }else if(strcmp(index,"input") == 0){
        
     }else if(strcmp(index,"=")==0){

     }else{
        printf("\nUndefined function:%s",index);
     }
}

void Deallocate(Node* root){
    while (root != NULL) {
        Node** temp = root;
        root = root->next;
        free(temp);
    }
}

int main() {
    char *line;
    int i = 0;
    char c;
    char index[20]="";
    Node* curr;
    line = (char *)malloc(sizeof(char));
    
        while ((c = getc(stdin)) != ';') {
            line[i] = c;
            i++;
            line = (char *)realloc(line, (i + 1) * sizeof(char));
        }

        line[i] = '\0';
        for(i=0;i<strlen(line);i++){
            if(line[i] == ' '){
                break;
            }
            index[i]=line[i];
        }
        free(line);
    printf("\n%s\n",index);
    int p=ver(index);
    Node* root=NULL;
    if(p == 1){
        root=Var(strlen(line),line);
    }
    if(root!=NULL){
            for (curr = root; curr != NULL; curr = curr->next) {
                if(curr->type == INTEGER){
                    printf("%d ",curr->var.INT);
                }
                if(curr->type == FLOAT){
                    printf("%f ",curr->var.FLOAT);
                }
            }
    }
    Deallocate(&root);
    return 0;
}
