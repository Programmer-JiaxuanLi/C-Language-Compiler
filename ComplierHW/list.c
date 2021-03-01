//
//  list.c
//  List
//
//  Created by 李佳轩 on 10/25/19.
//  Copyright © 2019 李佳轩. All rights reserved.
//
#include "list.h"

void List_Init(struct list *list) {
    //初始列表
    printf("\n");
    printf("llllll");
     printf("\n");
    list->n = 0;
}

void List_Append(struct list* list, char* value) { //参考python中的append()函数
    list->A[list->n++] = value;
}

void List_Insert(struct list *list, int index, char* value) {
    for (int i = list->n - 1; i >= index; i--) {
        list->A[i + 1] = list->A[i];
    }
    list->A[index] = value;
    list->n++;
}

int listsize(struct list *list){
    return list->n;
}

void List_Show(struct list *list) { //打印列表
    printf("len=%d values=", list->n);
    for (int i = 0; i < list->n; ++i)
    {
        printf(list->A[i]);
        printf("   ");
    }
    printf("\n");
}

char * List_n(struct list *list,int n){
    return list->A[n];
}

int List_Search(struct list *list,char * c){
    for (int i=0; i<list->n; i++) {
        if (strcmp(List_n(list, i),c)==0) {
            return i;
        }
    }
    return -1;
}
