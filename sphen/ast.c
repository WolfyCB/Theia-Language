#include "ast.h"
#include <stdio.h>
#include <stdlib.h>

int newastvec(ASTVec* v){
	if(v == NULL){
        printf("ast vec does not exist\n");
        return 0;
    }
	v->len = 0;
	v->cap = 256;
    v->data = (ASTNode*)malloc(v->cap * sizeof(ASTNode));
    if(v->data == NULL) {
    	printf("ast memory allocation error\n");
		exit(EXIT_FAILURE);
    }
	return 1;
}
ASTNode getnode(ASTVec* v, NodeId index){
	if(v == NULL){
        printf("ast vec are null\n");
        exit(EXIT_FAILURE);
    }
    return (index < v->len) ? v->data[index] : v->data[0];
}
int pushnode(ASTVec* v, const ASTNode ast){
	if(v == NULL){
        printf("ast vec are null\n");
        return 0;
    }
    if(v->data == NULL){
        printf("ast vec nodes are null\n");
        return 0;
    }
    if(v->len >= v->cap){
    	v->cap += 256;
    	v->data = (ASTNode*)realloc(v->data, v->cap * sizeof(ASTNode));
    	if(v->data == NULL) {
    		printf("ast memory allocation error\n");
			exit(EXIT_FAILURE);
    	}
    }
    v->data[v->len++] = ast;
    return 1;
}
int delast(ASTVec* v){
	if(v == NULL) return 0;
	free(v->data);
	v->data = NULL;
	v->len = 0;
	v->cap = 0;
	return 1;
}


int newvecid(VecId* v, unsigned long cap){
	if(v == NULL){
        printf("vec id does not exist\n");
        return 0;
    }
	v->len = 0;
	v->cap = cap;
    v->data = (NodeId*)malloc(v->cap * sizeof(NodeId));
    if(v->data == NULL) {
    	printf("vec id memory allocation error\n");
		exit(EXIT_FAILURE);
    }
	return 1;
}
NodeId getvecid(VecId* v, NodeId index){
	if(v == NULL){
        printf("vec id are null\n");
        exit(EXIT_FAILURE);
    }
    return (index < v->len) ? v->data[index] : v->data[0];
}
int pushvecid(VecId* v, NodeId id){
	if(v == NULL){
        printf("vec id are null\n");
        return 0;
    }
    if(v->data == NULL){
        printf("vec id's are null\n");
        return 0;
    }
    if(v->len >= v->cap){
    	v->cap *= 2;
    	v->data = (NodeId*)realloc(v->data, v->cap * sizeof(NodeId));
    	if(v->data == NULL) {
    		printf("vec id memory allocation error\n");
			exit(EXIT_FAILURE);
    	}
    }
    v->data[v->len++] = id;
    return 1;
}
int delvecid(VecId* v){
	if(v == NULL) return 0;
	free(v->data);
	v->data = NULL;
	v->len = 0;
	v->cap = 0;
	return 1;
}
