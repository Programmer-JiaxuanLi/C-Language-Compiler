//
//  list.h
//  List
//
//  Created by 李佳轩 on 10/25/19.
//  Copyright © 2019 李佳轩. All rights reserved.
//

#ifndef list_h
#define list_h
#include <stdio.h>

extern struct list {//定义列表结构体
    char * A[100];
    int n;
};
void List_Init(struct list *list);
void List_Append(struct list* list, char * value);
void List_Show(struct list *list);
void List_Insert(struct list *list, int index, char * value);
char * List_n(struct list *list,int n);
int List_Search(struct list *list,char * c);
int listsize(struct list *list);
#endif /* list_h */
