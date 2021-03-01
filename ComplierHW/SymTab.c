//
//  SymTab.c
//  homework3
//
//  Created by 李佳轩 on 10/19/19.
//  Copyright © 2019 李佳轩. All rights reserved.
//

#include "SymTab.h"
#include <stdlib.h>
#include <string.h>

unsigned int BKDRHash(char* str, unsigned int len, int SymTabSize)
{
    unsigned int hash = 0;
    unsigned int i    = 0;
    
    for(i = 0; i < len; str++, i++)
    {
        hash = (hash * 31) + (*str);
    }
    
    return hash % (SymTabSize);
}

struct SymTab* CreateSymTab(int Size)
{
    
    struct SymTab* st = NULL;
    st = (struct SymTab*)malloc(sizeof(struct SymTab));
    st->Size = Size;
    st->Contents = (struct SymEntry**)malloc(sizeof(struct SymEntry) * Size);
    
    for (int i = 0; i < Size; i++) {
        struct SymEntry* sy   = (struct SymEntry*)malloc(sizeof(struct SymEntry));
        sy->Attributes = NULL;
        sy->Name = NULL;
        sy->Next = NULL;
        st->Contents[i] = sy;
    }
    
    return st;
}

void DestorySymTab(struct SymTab *ATable)
{
    struct SymEntry* sep  = NULL;
    struct SymEntry* next = NULL;

    for (int i = 0; i < ATable->Size; i++) {
        sep = ATable->Contents[i];
        while (sep != NULL) {
            next = sep->Next;
            free(sep->Attributes);
            free(sep);
            sep = next;
        }
        ATable->Contents[i] = NULL;
    }
    
    free(ATable->Contents);
    ATable->Contents = NULL;
    free(ATable);
    ATable->Size = 0;
    ATable = NULL;
}

void SetAttr(struct SymEntry *AnEntry, void *Attributes)
{
    AnEntry->Attributes = Attributes;
}

void * GetAttr(struct SymEntry *AnEntry)
{
    return AnEntry->Attributes;
}

const char * GetName(struct SymEntry *AnEntry)
{
    return AnEntry->Name;
}

struct SymEntry* FindName(struct SymTab *ATable,const char* Name)
{
    struct SymEntry* se;
    int pos = BKDRHash(Name, strlen(Name), ATable->Size);
    se = ATable->Contents[pos];
    se->Attributes = ATable->Contents[pos]->Attributes;
    se->Name = ATable->Contents[pos]->Name;
    se->Next = ATable->Contents[pos]->Next;
    
    while (se!=NULL) {
        if (se->Name == NULL) {
            return NULL;
        }
        if (se->Name != NULL && strcmp(se->Name, Name) == 0) {
            return se;
        }
        if (se->Next == NULL){
            return NULL;
        }
        se = se->Next;
    }

    return NULL;
}

struct SymEntry * NextEntry(struct SymTab *ATable, struct SymEntry *AnEntry)
{
    int pos = BKDRHash(AnEntry->Name, strlen(AnEntry->Name), ATable->Size);
    struct SymEntry *pointer = ATable->Contents[pos];
    
    while (pointer != NULL) {
        if (pointer->Name != NULL && strcmp(pointer->Name,AnEntry->Name) == 0) {
            //printf("here get the equals\n");
            if (pointer->Next !=NULL){
                return pointer->Next;
            }else{
                //printf("The next is NULL\n");
                for(pos = pos + 1; pos < ATable->Size; pos++){
                    if (ATable->Contents[pos]->Name != NULL) {
                        return ATable->Contents[pos];
                    }
                }
                return NULL;
            }
        }
        pointer = pointer->Next;
    }
    return NULL;
}

int EnterName(struct SymTab *ATable, const char *Name, struct SymEntry **AnEntry)
{
    printf("\n");
    printf(Name);
    printf("start enter name\n");
    char* c = (char *)malloc(sizeof(char)*1);
    printf("1");
    struct SymEntry* pre;
    struct SymEntry* se;
    struct SymEntry *ps = (struct SymEntry*)malloc(sizeof(struct SymEntry));
    strcpy(c, Name);
           printf("1");
    ps->Name = c;
    *AnEntry = ps;
    int pos = BKDRHash(Name, strlen(Name), ATable->Size);
    
    if (ATable->Contents[pos]->Name == NULL) {
        ATable->Contents[pos] = ps;
        printf("finish");
        return 1;
    }
    if (strcmp(Name, ATable->Contents[pos]->Name) == 0) {
        ps->Next = ATable->Contents[pos]->Next;
        ATable->Contents[pos] = ps;
        printf("finish");
        return 0;
    }
    
    pre = ATable->Contents[pos];
    se = pre->Next;
    
    while (se !=NULL ) {
        if (strcmp(Name, se->Name) == 0){
            pre->Next = ps;
            ps->Next = se->Next;
            printf("finish");
            return 0;
        }
        
        pre = se;
        se = pre->Next;
        }
        pre->Next = ps;
    printf("finish");
        return 1;
}


struct SymEntry * FirstEntry(struct SymTab *ATable)
{
    for (int j = 0; j < ATable->Size; j++)
    {
        if (ATable->Contents[j]->Name != NULL) {
            return ATable->Contents[j];
        }
    }
    return NULL;
}


void PrintSymTab (struct SymTab *ATable)
{
    struct SymEntry* sep  = NULL;
    
    for (int i = 0; i < ATable->Size; i++) {
        sep = ATable->Contents[i];
        
        while (sep != NULL) {
            if (sep->Name != NULL) {
                printf("Name[%s     ]", sep->Name);
                printf("Attribute:[%c  ]", *((char*)(sep->Attributes)));
            } else {
                printf("Name[      ]");
                printf("Attribute:[   ]");
            }
            sep = sep->Next;
        }
        
        printf("\n");
    }
}
