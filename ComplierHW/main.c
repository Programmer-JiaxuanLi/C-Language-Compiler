#include <stdio.h>
#include "Semantics.h"
#include "CodeGen.h"
#include "SymTab.h"
#include "IOMngr.h"
#include "list.h"

extern int yyparse();

struct SymTab *table;
struct SymTab *lvartable;
struct SymTab *funtable;
struct SymTab *ProcSymTab;
struct list* varList;
struct list* addrList;
struct list* paraaddrList;
struct SymEntry *entry;
int regid = -1;
int inProc =0;
FILE *aFile;

int main(int argc, char * argv[]) {
	table = CreateSymTab(33);
    funtable = CreateSymTab(33);
    lvartable = CreateSymTab(33);
    varList = (struct list*)malloc(sizeof(struct list));
    List_Init(varList);
    addrList = (struct list*)malloc(sizeof(struct list));
    List_Init(addrList);
    paraaddrList = (struct list*)malloc(sizeof(struct list));
    List_Init(paraaddrList);
	//assumes there is a listing file
	OpenFiles(argv[1], argv[2]);
	if (argc == 4) 
		aFile = fopen(argv[3], "w");
	else
		aFile = stdout;

	yyparse();
}

