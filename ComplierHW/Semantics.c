/* Semantics.c
   Support and semantic action routines.
   
*/

#include <strings.h>
#include <stdlib.h>

#include "CodeGen.h"
#include "Semantics.h"
#include "SymTab.h"
#include "IOMngr.h"
#include "list.h"

extern struct SymTab *table;
extern struct list* paraaddrList;
extern struct SymTab *lvartable;
extern struct SymTab *funtable;
extern struct list* varList;
extern struct list* addrList;
extern int inpf = 0 ;
extern int isBo = 0 ;
extern int paranums = 0;
extern char* strname = "str";
extern char* strContext = NULL;
/* Semantics support routines */

struct ExprRes *  doIntLit(char * digits)  {
    //生成语句，将lex识别出来的立即数赋予寄存器
   struct ExprRes *res;
  
  res = (struct ExprRes *) malloc(sizeof(struct ExprRes));
  res->Reg = AvailTmpReg();
  res->Instrs = GenInstr(NULL,"li",TmpRegName(res->Reg),digits,NULL);
  return res;
    
}

struct ExprRes *  doRval(char * name)  { 
//将name替换成name对应的值
   struct ExprRes *res;
    struct SymEntry* sym;
    printf("the inpf is%d",inpf);
   if (!FindName(table, name)) {
       if (!FindName(lvartable,name)) {
           WriteIndicator(GetCurrentColumn());
           WriteMessage("Undeclared variable");
       }
   }
    if (FindName(table, name)) {
        sym = FindName(table, name);
    } else {
        sym =  FindName(lvartable, name);
    }
  struct typeInfo* tfo = GetAttr(sym);
  if (strcmp(tfo->type ,"boolean") == 0) {
      isBo =1;
  }
    if (inpf==1) {
        res = (struct ExprRes *) malloc(sizeof(struct ExprRes));
        res->Reg = AvailTmpReg();
       // printf(List_n(addrList,List_Search(varList,name)));
        res->Instrs = GenInstr(NULL,"lw",TmpRegName(res->Reg), List_n(addrList,List_Search(varList,name)),NULL);
    } else {
        res = (struct ExprRes *) malloc(sizeof(struct ExprRes));
        res->Reg = AvailTmpReg();
        res->Instrs = GenInstr(NULL,"lw",TmpRegName(res->Reg),name,NULL);
    }
   printf("dorval");
  return res;
}

extern struct ExprRes *  doArRval(char * name,struct ExprRes * Expr){
//    printf("NNNNNNN");
//    printf("\n");
//    printf(name);
     if (!FindName(table, name)) {
          WriteIndicator(GetCurrentColumn());
          WriteMessage("Undeclared variable");
     }
    struct SymEntry* sym = FindName(table, name);
    struct typeInfo* tfo = GetAttr(sym);
//    printf("\n");
//    printf("type  is     ");
//    printf("%s;\n",tfo->type);
    if (strcmp(tfo->type ,"boolar") == 0) {
        isBo =1;
        printf("isboar\n");
    }else{
        isBo = 0 ;
    }
    struct ExprRes *res;
    res = (struct ExprRes *) malloc(sizeof(struct ExprRes));
    int reg = AvailTmpReg();
    int reg1 = AvailTmpReg();
    
    res->Instrs = Expr->Instrs;
    AppendSeq(res->Instrs,GenInstr(NULL,"add",TmpRegName(reg),TmpRegName(Expr->Reg),"$zero"));
    AppendSeq(res->Instrs,GenInstr(NULL,"la",TmpRegName(reg1),name,NULL));
    AppendSeq(res->Instrs,GenInstr(NULL,"sll",TmpRegName(reg),TmpRegName(reg),"2"));
    AppendSeq(res->Instrs,GenInstr(NULL,"add",TmpRegName(reg1),TmpRegName(reg1),TmpRegName(reg)));
    char * s = (char *) malloc(8);
    s = join1("(",TmpRegName(reg1));
    s = join1(s,")");
    AppendSeq(res->Instrs,GenInstr(NULL,"lw",TmpRegName(reg1),s,NULL));
    res->Reg = reg1;
    ReleaseTmpReg(Expr->Reg);
    ReleaseTmpReg(reg);
    return res;
}

extern struct ExprRes *  callfun(char* name){
    struct ExprRes * res = (struct ExprRes *) malloc(sizeof(struct ExprRes));
    printf("callfun");
    res->Instrs = (struct InstrSeq*) malloc(sizeof(struct InstrSeq));
    AppendSeq(res->Instrs,SaveSeq());
    AppendSeq(res->Instrs,GenInstr(NULL,"jal",name,NULL,NULL));
    int reg = AvailTmpReg();
    AppendSeq(res->Instrs,GenInstr(NULL,"lw",TmpRegName(reg),"-4($sp)",NULL));
    AppendSeq(res->Instrs,RestoreSeq(reg));
    res->Reg = reg;
    printf(name);
    return res;
}

extern void addressinit(){
    printf("\naddrinit\n");
    int temp =0;
    for (int i =0; i < paranums; i++) {
          char * c= (char *) malloc(8);
        sprintf(c,"%d($sp)",4*(i+varList->n-1));
        printf(c);
        printf("addddddr%s\n\n",c);
        List_Append(addrList,c);
    }
    for (int j=0; j<varList->n-paranums; j++) {
          char * d= (char *) malloc(8);
        sprintf(d,"%d($sp)",4*j);
        List_Append(addrList,d);
    }
    for (int k=0; k<paranums; k++) {
          char * e= (char *) malloc(8);
        sprintf(e,"%d($sp)",4*k);
        List_Append(paraaddrList,e);
    }
    printf("\n");
    List_Show(addrList);
    printf("\n");
    //List_Append(addrList,"8($sp)"); List_Append(addrList,"4($sp)"); List_Append(paraaddrList,"0($sp)");
};

extern struct InstrSeq * callParameterFun(char* funcname){
    
    struct InstrSeq *code;
    code = (struct InstrSeq*) malloc(sizeof(struct InstrSeq));
    int regs[paranums];
    for (int k=0; k<paranums; k++) {
        regs[k]=AvailTmpReg();
    }
    char deep[8];
    sprintf(deep,"%d",paranums*4);
    if (inpf==0) {
        for (int j=0; j<paranums; j++) {
            AppendSeq(code,GenInstr(NULL,"lw",TmpRegName(regs[j]),List_n(varList,j),NULL));
        }
    } else {
        for (int i=0; i<paranums; i++) {
                AppendSeq(code,GenInstr(NULL,"lw",TmpRegName(regs[i]),List_n(addrList,i),NULL));
        }
    }
      AppendSeq(code,GenInstr(NULL,"subu","$sp","$sp",deep));
    for (int l= 0; l<paranums; l++) {
        //a还是需要修改地址
         AppendSeq(code,GenInstr(NULL,"sw",TmpRegName(regs[l]),List_n(paraaddrList,l),NULL));
    }
      AppendSeq(code,GenInstr(NULL,"jal",funcname,NULL,NULL));
      for (int m= 0; m<paranums; m++) {
          ReleaseTmpReg(regs[m]);}
      int reg = AvailTmpReg();
      AppendSeq(code,GenInstr(NULL,"addi","$sp","$sp",deep));
      return code;
}

extern struct InstrSeq * callfunonly(char* name){
    
    printf("call funonly");
    printf(name);
    struct InstrSeq *code;
    code = (struct InstrSeq*) malloc(sizeof(struct InstrSeq));
    AppendSeq(code,GenInstr(NULL,"jal",name,NULL,NULL));
    return code;
//      struct InstrSeq *code;
//      code = (struct InstrSeq*) malloc(sizeof(struct InstrSeq));
//
//      AppendSeq(code,GenInstr(NULL,"li","$v0","4",NULL));
//      AppendSeq(code,GenInstr(NULL,"la","$a0","_nl",NULL));
//      AppendSeq(code,GenInstr(NULL,"syscall",NULL,NULL,NULL));
//
//      return code;
}

struct ExprRes *  doAdd(struct ExprRes * Res1, struct ExprRes * Res2)  { 

   int reg;
  reg = AvailTmpReg();
  AppendSeq(Res1->Instrs,Res2->Instrs);
  AppendSeq(Res1->Instrs,GenInstr(NULL,"add",
                                       TmpRegName(reg),
                                       TmpRegName(Res1->Reg),
                                       TmpRegName(Res2->Reg)));
  ReleaseTmpReg(Res1->Reg);
  ReleaseTmpReg(Res2->Reg);
  Res1->Reg = reg;
  free(Res2);
  return Res1;
}

struct ExprRes *  doMult(struct ExprRes * Res1, struct ExprRes * Res2)  { 

   int reg;
   
  reg = AvailTmpReg();
  AppendSeq(Res1->Instrs,Res2->Instrs);
  AppendSeq(Res1->Instrs,GenInstr(NULL,"mul",
                                       TmpRegName(reg),
                                       TmpRegName(Res1->Reg),
                                       TmpRegName(Res2->Reg)));
  ReleaseTmpReg(Res1->Reg);
  ReleaseTmpReg(Res2->Reg);
  Res1->Reg = reg;
  free(Res2);
  return Res1;
}

extern struct InstrSeq * doIf(struct ExprRes *Expr, struct InstrSeq * seq1, struct InstrSeq * seq2){
    isBo=0;
    printf("在方法doif里");
    struct InstrSeq * code = (struct InstrSeq *) malloc(sizeof(struct InstrSeq));
   // printf("0");
    char* Label = GenLabel();
    char* Label2 = GenLabel();
    struct ExprRes *res;
   // printf("1");
    AppendSeq(code, Expr->Instrs);
    AppendSeq(code, GenInstr(NULL, "beq", TmpRegName(Expr->Reg), "$zero",
                                              Label));
    AppendSeq(code,seq1);
    AppendSeq(code, GenInstr(NULL,"j",Label2,NULL,NULL));
    AppendSeq(code, GenInstr(Label, NULL, NULL, NULL, NULL));

    AppendSeq(code,seq2);
    
    AppendSeq(code, GenInstr(Label2, NULL, NULL, NULL, NULL));
    ReleaseTmpReg(Expr->Reg);
    free(Expr);
    return code;
    }

extern struct InstrSeq * dowhile(struct ExprRes *Expr, struct InstrSeq * seq1){
    struct InstrSeq * code = (struct InstrSeq *) malloc(sizeof(struct InstrSeq));
    char* Label = GenLabel();
    char* Label2 = GenLabel();
    struct ExprRes *res;
    printf("在while函数里");
    AppendSeq(code, GenInstr(Label, NULL, NULL, NULL, NULL));
    AppendSeq(code, Expr->Instrs);
    AppendSeq(code, GenInstr(NULL, "beq", TmpRegName(Expr->Reg), "$zero",
                                                Label2));
    AppendSeq(code,seq1);
    AppendSeq(code, GenInstr(NULL,"j",Label,NULL,NULL));
    AppendSeq(code, GenInstr(Label2, NULL, NULL, NULL, NULL));
    ReleaseTmpReg(Expr->Reg);
    free(Expr);
    return code;
}

struct ExprRes *  doExponentiation(struct ExprRes * Res1, struct ExprRes * Res2)  {
    //第一个是底数第二个是幂数
    int reg = AvailTmpReg();
    struct BExprRes * bRes2;
    int Reg = AvailTmpReg();
    
    AppendSeq(Res1->Instrs,Res2->Instrs);
    AppendSeq(Res1->Instrs, GenInstr(NULL,"li",TmpRegName(Reg),"2",NULL));
    AppendSeq(Res1->Instrs, GenInstr(NULL,"add",TmpRegName(reg),TmpRegName(Res1->Reg),"$zero"));
    bRes2 = (struct BExprRes *) malloc(sizeof(struct BExprRes));
    bRes2->Label = GenLabel();
    AppendSeq(Res2->Instrs, GenInstr(bRes2->Label, NULL, NULL, NULL, NULL));
    AppendSeq(Res1->Instrs,GenInstr(NULL,"mul",
                                       TmpRegName(reg),
                                       TmpRegName(reg),
                                       TmpRegName(Res1->Reg)));
    AppendSeq(Res1->Instrs,GenInstr(NULL,"add",
                                          TmpRegName(Reg),
                                          TmpRegName(Reg),"1"));
    AppendSeq(Res1->Instrs, GenInstr(NULL, "ble", TmpRegName(Reg), TmpRegName(Res2->Reg), bRes2->Label));
    char* Label2 = GenLabel();
    char* Label3 = GenLabel();
    char* Label4 = GenLabel();
    AppendSeq(Res1->Instrs, GenInstr(NULL, "bne",TmpRegName(Res2->Reg),"0",Label3));
    AppendSeq(Res1->Instrs, GenInstr(NULL,"li",TmpRegName(reg),"1",NULL));
    AppendSeq(Res1->Instrs, GenInstr(NULL,"j",Label4,NULL,NULL));
 
    AppendSeq(Res2->Instrs, GenInstr(Label3, NULL, NULL, NULL, NULL));
    AppendSeq(Res1->Instrs, GenInstr(NULL, "bne",TmpRegName(Res2->Reg),"1",Label4));
    AppendSeq(Res1->Instrs, GenInstr(NULL,"add",TmpRegName(reg),TmpRegName(Res1->Reg),"0"));
    
    AppendSeq(Res2->Instrs, GenInstr(Label4, NULL, NULL, NULL, NULL));
    ReleaseTmpReg(Res1->Reg);
    ReleaseTmpReg(Res2->Reg);
    ReleaseTmpReg(Reg);
    
    Res1->Reg = reg;
    free(Res2);
    free(bRes2);
    
    return Res1;
}

struct ExprRes *  doDivide(struct ExprRes * Res1, struct ExprRes * Res2)  {

    int reg;
   
  reg = AvailTmpReg();
  AppendSeq(Res1->Instrs,Res2->Instrs);
  AppendSeq(Res1->Instrs,GenInstr(NULL,"div",
                                       TmpRegName(reg),
                                       TmpRegName(Res1->Reg),
                                       TmpRegName(Res2->Reg)));
  ReleaseTmpReg(Res1->Reg);
  ReleaseTmpReg(Res2->Reg);
  Res1->Reg = reg;
  free(Res2);
  return Res1;
}

struct ExprRes *  doMod(struct ExprRes * Res1, struct ExprRes * Res2)  {

    int reg;
    
    reg = AvailTmpReg();
    AppendSeq(Res1->Instrs,Res2->Instrs);
    AppendSeq(Res1->Instrs,GenInstr(NULL,"rem",
                                       TmpRegName(reg),
                                       TmpRegName(Res1->Reg),
                                       TmpRegName(Res2->Reg)));
    ReleaseTmpReg(Res1->Reg);
    ReleaseTmpReg(Res2->Reg);
    Res1->Reg = reg;
    free(Res2);
    return Res1;
}

struct ExprRes *  doMinus(struct ExprRes * Res1, struct ExprRes * Res2)  {

    int reg;
   
  reg = AvailTmpReg();
  AppendSeq(Res1->Instrs,Res2->Instrs);
  AppendSeq(Res1->Instrs,GenInstr(NULL,"sub",
                                       TmpRegName(reg),
                                       TmpRegName(Res1->Reg),
                                       TmpRegName(Res2->Reg)));
  ReleaseTmpReg(Res1->Reg);
  ReleaseTmpReg(Res2->Reg);
  Res1->Reg = reg;
  free(Res2);
  return Res1;
}

extern char *join1(char *a, char *b) {
    char *c = (char *) malloc(strlen(a) + strlen(b) + 1); //局部变量，用malloc申请内存
    if (c == NULL) exit (1);
    char *tempc = c; //把首地址存下来
    while (*a != '\0') {
        *c++ = *a++;
    }
    while ((*c++ = *b++) != '\0') {
        ;
    }
    //注意，此时指针c已经指向拼接之后的字符串的结尾'\0' !
    return tempc;//返回值是局部malloc申请的指针变量，需在函数调用结束后free之
}

struct ExprRes *  doNegative(struct ExprRes * Res1)  {

    int reg;
    
  reg = AvailTmpReg();
  AppendSeq(Res1->Instrs,GenInstr(NULL,"sub",
                                       TmpRegName(reg),
                                       "$zero" ,
                                       TmpRegName(Res1->Reg)));
  ReleaseTmpReg(Res1->Reg);
  Res1->Reg = reg;
  return Res1;
}

extern struct InstrSeq * doPrintstr(char * str1){
     struct InstrSeq *code;
     char* c ="\"";
     char* d ="\"";
     char* tempc = "";
     tempc = join1(c,str1);
     strContext = join1(tempc,d);
    
    code = (struct InstrSeq*) malloc(sizeof(struct InstrSeq));
    printf(strContext);
    AppendSeq(code,GenInstr(NULL,"li","$v0","4",NULL));
    AppendSeq(code,GenInstr(NULL,"la","$a0",strname,NULL));
    AppendSeq(code,GenInstr(NULL,"syscall",NULL,NULL,NULL));
    return code;
}

struct InstrSeq * doPrintsp(struct ExprRes * Res1) {
    struct InstrSeq *code;
    
    code = Res1->Instrs;
    char* label = GenLabel();
    int reg = AvailTmpReg();
    int reg1 = AvailTmpReg();
 
    
    AppendSeq(code,GenInstr(NULL,"li",TmpRegName(reg),"1",NULL));
    AppendSeq(code,GenInstr(NULL,"li",TmpRegName(reg1),"1",NULL));
    AppendSeq(code,GenInstr(label,NULL,NULL,NULL,NULL));
AppendSeq(code,GenInstr(NULL,"add",TmpRegName(reg),TmpRegName(reg),TmpRegName(reg1)));
    AppendSeq(code,GenInstr(NULL,"li","$v0","4",NULL));
    AppendSeq(code,GenInstr(NULL,"la","$a0","blank",NULL));
    AppendSeq(code,GenInstr(NULL,"syscall",NULL,NULL,NULL));
    AppendSeq(code,GenInstr(NULL,"ble",TmpRegName(reg),TmpRegName(Res1->Reg),label));
    
    ReleaseTmpReg(reg);
    ReleaseTmpReg(reg1);
    return code;
}

struct InstrSeq * doPrintln() {
    struct InstrSeq *code;
    code = (struct InstrSeq*) malloc(sizeof(struct InstrSeq));
    
    AppendSeq(code,GenInstr(NULL,"li","$v0","4",NULL));
    AppendSeq(code,GenInstr(NULL,"la","$a0","_nl",NULL));
    AppendSeq(code,GenInstr(NULL,"syscall",NULL,NULL,NULL));
    
    return code;
}

struct InstrSeq * doPrint(struct ExprRes * Expr) {
    printf("The expression isbo?%d\n",isBo);
    struct InstrSeq *code;
    code = Expr->Instrs;

      if (isBo == 0) {
          AppendSeq(code,GenInstr(NULL,"li","$v0","1",NULL));
          AppendSeq(code,GenInstr(NULL,"move","$a0",TmpRegName(Expr->Reg),NULL));
          AppendSeq(code,GenInstr(NULL,"syscall",NULL,NULL,NULL));

          AppendSeq(code,GenInstr(NULL,"li","$v0","4",NULL));
          AppendSeq(code,GenInstr(NULL,"la","$a0","blank",NULL));
          AppendSeq(code,GenInstr(NULL,"syscall",NULL,NULL,NULL));
      }else{

          char * Label = GenLabel();
          char * Label2 = GenLabel();
          AppendSeq(code, GenInstr(NULL, "beq", TmpRegName(Expr->Reg), "$zero",
                                           Label));
          AppendSeq(code,GenInstr(NULL,"li","$v0","4",NULL));
          AppendSeq(code,GenInstr(NULL,"la","$a0","strue",NULL));
          AppendSeq(code,GenInstr(NULL,"syscall",NULL,NULL,NULL));
          
          AppendSeq(code,GenInstr(NULL,"li","$v0","4",NULL));
          AppendSeq(code,GenInstr(NULL,"la","$a0","blank",NULL));
          AppendSeq(code,GenInstr(NULL,"syscall",NULL,NULL,NULL));
          
          AppendSeq(code, GenInstr(NULL,"j",Label2,NULL,NULL));
          
          AppendSeq(code, GenInstr(Label, NULL, NULL, NULL, NULL));
          AppendSeq(code,GenInstr(NULL,"li","$v0","4",NULL));
          AppendSeq(code,GenInstr(NULL,"la","$a0","sfalse",NULL));
          AppendSeq(code,GenInstr(NULL,"syscall",NULL,NULL,NULL));

          AppendSeq(code,GenInstr(NULL,"li","$v0","4",NULL));
          AppendSeq(code,GenInstr(NULL,"la","$a0","blank",NULL));
          AppendSeq(code,GenInstr(NULL,"syscall",NULL,NULL,NULL));
          
          AppendSeq(code, GenInstr(Label2, NULL, NULL, NULL, NULL));
          
             
      }
    
    ReleaseTmpReg(Expr->Reg);
    free(Expr);
    isBo=0;
    return code;
}

struct InstrSeq * doAddPrint(struct InstrSeq * Expr1, struct ExprRes * Expr2) {
  
   // printf("The expression isbo?%d\n",isBo);
    struct InstrSeq *code;
    code = Expr1;
    AppendSeq(code, Expr2->Instrs);
    if (isBo == 0) {
        AppendSeq(code,GenInstr(NULL,"li","$v0","1",NULL));
        AppendSeq(code,GenInstr(NULL,"move","$a0",TmpRegName(Expr2->Reg),NULL));
        AppendSeq(code,GenInstr(NULL,"syscall",NULL,NULL,NULL));

        AppendSeq(code,GenInstr(NULL,"li","$v0","4",NULL));
        AppendSeq(code,GenInstr(NULL,"la","$a0","blank",NULL));
        AppendSeq(code,GenInstr(NULL,"syscall",NULL,NULL,NULL));
    }else{

        char * Label = GenLabel();
        char * Label2 = GenLabel();
        
        AppendSeq(code, GenInstr(NULL, "beq", TmpRegName(Expr2->Reg), "$zero",
                                         Label));
        
        AppendSeq(code,GenInstr(NULL,"li","$v0","4",NULL));
        AppendSeq(code,GenInstr(NULL,"la","$a0","strue",NULL));
        AppendSeq(code,GenInstr(NULL,"syscall",NULL,NULL,NULL));
        
        AppendSeq(code,GenInstr(NULL,"li","$v0","4",NULL));
        AppendSeq(code,GenInstr(NULL,"la","$a0","blank",NULL));
        AppendSeq(code,GenInstr(NULL,"syscall",NULL,NULL,NULL));
        
        AppendSeq(code, GenInstr(NULL,"j",Label2,NULL,NULL));
        
        AppendSeq(code,GenInstr(Label, NULL, NULL, NULL, NULL));
        AppendSeq(code,GenInstr(NULL,"li","$v0","4",NULL));
        AppendSeq(code,GenInstr(NULL,"la","$a0","sfalse",NULL));
        AppendSeq(code,GenInstr(NULL,"syscall",NULL,NULL,NULL));

        AppendSeq(code,GenInstr(NULL,"li","$v0","4",NULL));
        AppendSeq(code,GenInstr(NULL,"la","$a0","blank",NULL));
        AppendSeq(code,GenInstr(NULL,"syscall",NULL,NULL,NULL));
        
        AppendSeq(code, GenInstr(Label2, NULL, NULL, NULL, NULL));
        
           
    }
  //    AppendSeq(code,GenInstr("_nl",".asciiz","\"\\n\"",NULL,NULL));
   // AppendSeq(code,GenInstr("str:",".asciiz",""hello word"" ,NULL,NULL));
    
    isBo=0;
    ReleaseTmpReg(Expr2->Reg);
    //free(Expr1);
    free(Expr2);
    return code;
}

extern struct InstrSeq * doread(char* name){
    struct InstrSeq *code;
    code = (struct InstrSeq *) malloc(sizeof(struct InstrSeq));
    printf(name);
    if (!FindName(table, name)) {
           WriteIndicator(GetCurrentColumn());
           WriteMessage("Undeclared variable");
    }
    
    AppendSeq(code,GenInstr(NULL,"li","$v0", "5",NULL));
    AppendSeq(code,GenInstr(NULL,"syscall",NULL ,NULL, NULL));
    AppendSeq(code,GenInstr(NULL,"sw","$v0", name,NULL));
    
    return code;
}

extern struct InstrSeq * doaddread(struct InstrSeq * InstrSeq1,char* name){
      
    struct InstrSeq *code;
    code = InstrSeq1;
    AppendSeq(code,GenInstr(NULL,"li","$v0", "5",NULL));
    AppendSeq(code,GenInstr(NULL,"syscall",NULL ,NULL, NULL));
    AppendSeq(code,GenInstr(NULL,"sw","$v0", name,NULL));
    printf(name);
    return code;
}

struct InstrSeq * doAssign(char *name, struct ExprRes * Expr) { 

  struct InstrSeq *code;
  
   if (!FindName(table, name)) {
		WriteIndicator(GetCurrentColumn());
		WriteMessage("Undeclared variable");
   }

  code = Expr->Instrs;
    if (inpf==1) {
  AppendSeq(code,GenInstr(NULL,"sw",TmpRegName(Expr->Reg),List_n(addrList,List_Search(varList,name)),NULL));
        ReleaseTmpReg(Expr->Reg);
        free(Expr);
    } else {    AppendSeq(code,GenInstr(NULL,"sw",TmpRegName(Expr->Reg),List_n(addrList,List_Search(varList,name)),NULL));
        ReleaseTmpReg(Expr->Reg);
        free(Expr);
    }
  return code;
}

extern struct InstrSeq * doArAssignment(char* Id, struct ExprRes * Expr1,struct ExprRes * Expr){
    struct InstrSeq *code;
    printf("doarassign");
    printf("\n");
    int reg = AvailTmpReg();
    int reg1 = AvailTmpReg();
    int reg2 = AvailTmpReg();
    printf("get");
    code = Expr1->Instrs;
    AppendSeq(code,Expr->Instrs);
    AppendSeq(code,GenInstr(NULL,"add",TmpRegName(reg),TmpRegName(Expr->Reg),"$zero"));
    AppendSeq(code,GenInstr(NULL,"add",TmpRegName(reg1),TmpRegName(Expr1->Reg),"$zero"));
    AppendSeq(code,GenInstr(NULL,"la",TmpRegName(reg2),Id,NULL));
    AppendSeq(code,GenInstr(NULL,"sll",TmpRegName(reg1),TmpRegName(reg1),"2"));
    AppendSeq(code,GenInstr(NULL,"add",TmpRegName(reg2),TmpRegName(reg2),TmpRegName(reg1)));
    char * s = (char *) malloc(8);
    s = join1("(",TmpRegName(reg2));
    s = join1(s,")");
    AppendSeq(code,GenInstr(NULL,"sw",TmpRegName(reg),s,NULL));
    ReleaseTmpReg(Expr->Reg);
    ReleaseTmpReg(Expr1->Reg);
    ReleaseTmpReg(reg);
    ReleaseTmpReg(reg1);
    ReleaseTmpReg(reg2);
    printf("finish assign");
    return code;
    
}

extern void declareFunction(struct SymEntry *AnEntry,struct InstrSeq * seq1, struct ExprRes * Res1){
    struct InstrSeq *code;
    struct ExprRes *res;
    res = (struct ExprRes *) malloc(sizeof(struct ExprRes));
    res->Instrs = GenInstr(AnEntry->Name,NULL,NULL,NULL,NULL);
    AppendSeq(res->Instrs,GenInstr(NULL,"addi","$sp","$sp","-8"));
    AppendSeq(res->Instrs,GenInstr(NULL,"sw","$ra","($sp)",NULL));
    AppendSeq(res->Instrs,seq1);
    AppendSeq(res->Instrs,Res1->Instrs);
    AppendSeq(res->Instrs,GenInstr(NULL,"sw",TmpRegName(Res1->Reg),"4($sp)",NULL));
    AppendSeq(res->Instrs,GenInstr(NULL,"lw","$ra","($sp)",NULL));
    AppendSeq(res->Instrs,GenInstr(NULL,"addi","$sp","$sp","8"));
    AppendSeq(res->Instrs,GenInstr(NULL,"jr","$ra",NULL,NULL));
    res->Reg = Res1->Reg;
    SetAttr(AnEntry,res);
    ReleaseTmpReg(res->Reg);
}

extern void declareParameterFunction(struct SymEntry *AnEntry,struct InstrSeq * seq1, struct ExprRes * Res1){
    struct InstrSeq *code;
    struct ExprRes *res;
    printf("paranum is %d\n\n",paranums);
    printf("var num is%d\n\n",addrList->n);
    List_Show(varList);
    char deep[8];
    sprintf(deep,"%d",(varList->n-paranums+1)*4);
    res = (struct ExprRes *) malloc(sizeof(struct ExprRes));
    res->Instrs = GenInstr(AnEntry->Name,NULL,NULL,NULL,NULL);
    AppendSeq(res->Instrs,GenInstr(NULL,"subu","$sp","$sp",deep));
    AppendSeq(res->Instrs,GenInstr(NULL,"sw","$ra","($sp)",NULL));
    AppendSeq(res->Instrs,seq1);
    AppendSeq(res->Instrs,Res1->Instrs);
    AppendSeq(res->Instrs,GenInstr(NULL,"lw","$ra","($sp)",NULL));
    AppendSeq(res->Instrs,GenInstr(NULL,"addi","$sp","$sp",deep));
    AppendSeq(res->Instrs,GenInstr(NULL,"jr","$ra",NULL,NULL));
    SetAttr(AnEntry,res);
    ReleaseTmpReg(res->Reg);
    inpf = 0;
    
}

extern struct InstrSeq *  doBExprAssign (char *name, struct ExprRes * Expr) {
   printf("do asign");
     isBo = 0 ;
    struct InstrSeq *code;

    if (!FindName(table, name)) {
          WriteIndicator(GetCurrentColumn());
          WriteMessage("Undeclared variable");
    }

    code = Expr->Instrs;
     if (inpf==1) {
    AppendSeq(code,GenInstr(NULL,"sw",TmpRegName(Expr->Reg),List_n(addrList,List_Search(varList,name)),NULL));
          ReleaseTmpReg(Expr->Reg);
          free(Expr);
      } else {    AppendSeq(code,GenInstr(NULL,"sw",TmpRegName(Expr->Reg),name,NULL));
          ReleaseTmpReg(Expr->Reg);
          free(Expr);
      }
    return code;
      printf("finish do asign");
}

extern struct InstrSeq *  doBExprTAssign (char *name) {
     struct InstrSeq *code;
    
     int reg;
     reg = AvailTmpReg();
     code = (struct InstrSeq *) malloc(sizeof(struct InstrSeq));
    
     if (!FindName(table, name)) {
          WriteIndicator(GetCurrentColumn());
          WriteMessage("Undeclared variable");
     }

    AppendSeq(code, GenInstr(NULL, "li",TmpRegName(reg), "1",
    NULL));
    AppendSeq(code,GenInstr(NULL,"sw",TmpRegName(reg), name,NULL));
    ReleaseTmpReg(reg);
    return code;
}

extern struct InstrSeq * doBExprFAssign (char *name) {
    struct InstrSeq *code;
    int reg;
    reg = AvailTmpReg();
    code = (struct InstrSeq *) malloc(sizeof(struct InstrSeq));
     
    if (!FindName(table, name)) {
          WriteIndicator(GetCurrentColumn());
          WriteMessage("Undeclared variable");
     }

    AppendSeq(code, GenInstr(NULL, "li", TmpRegName(reg), "0",
                                      NULL));
    AppendSeq(code,GenInstr(NULL,"sw",TmpRegName(reg), name,NULL));
    ReleaseTmpReg(reg);
    return code;
}

extern struct ExprRes * doNotBoExpr(struct ExprRes * Res1){
    isBo = 1 ;
    
    int reg;
    reg = AvailTmpReg();
    AppendSeq(Res1->Instrs, GenInstr(NULL, "li", TmpRegName(reg), "1",
                                      NULL));
    AppendSeq(Res1->Instrs,GenInstr(NULL,"sub",
                                         TmpRegName(reg),
                                         TmpRegName(reg),
                                         TmpRegName(Res1->Reg)));
    ReleaseTmpReg(Res1->Reg);
    Res1->Reg = reg;
    return Res1;
}

extern struct ExprRes * doOrBoExpr(struct ExprRes * Res1,struct ExprRes * Res2){
     isBo = 1 ;
    int reg;
    reg = AvailTmpReg();
    char * Label = GenLabel();
    AppendSeq(Res1->Instrs,Res2->Instrs);
    AppendSeq(Res1->Instrs,GenInstr(NULL,"add",
                                         TmpRegName(reg),
                                         TmpRegName(Res1->Reg),
                                         TmpRegName(Res2->Reg)));
    AppendSeq(Res1->Instrs, GenInstr(NULL, "beq",TmpRegName(reg), "$zero",
                                     Label));
    AppendSeq(Res1->Instrs, GenInstr(NULL, "li", TmpRegName(reg), "1",
                                      NULL));
    AppendSeq(Res1->Instrs, GenInstr(Label, NULL, NULL, NULL, NULL));
    ReleaseTmpReg(Res1->Reg);
    ReleaseTmpReg(Res2->Reg);
    Res1->Reg = reg;
    free(Res2);
    return Res1;
}
extern struct ExprRes * doAndBoExpr(struct ExprRes * Res1,struct ExprRes * Res2){
     isBo = 1 ;
    int reg;
    reg = AvailTmpReg();
    char * Label = GenLabel();
    AppendSeq(Res1->Instrs,Res2->Instrs);
    AppendSeq(Res1->Instrs,GenInstr(NULL,"mul",
                                         TmpRegName(reg),
                                         TmpRegName(Res1->Reg),
                                         TmpRegName(Res2->Reg)));
    AppendSeq(Res1->Instrs, GenInstr(NULL, "beq",TmpRegName(reg), "$zero",
                                       Label));
    AppendSeq(Res1->Instrs, GenInstr(NULL, "li", TmpRegName(reg), "1",
                                        NULL));
    AppendSeq(Res1->Instrs, GenInstr(Label, NULL, NULL, NULL, NULL));
    ReleaseTmpReg(Res1->Reg);
    ReleaseTmpReg(Res2->Reg);
    Res1->Reg = reg;
    free(Res2);
    return Res1;
}

extern struct ExprRes * doBoExpr(struct ExprRes * Res1,  struct ExprRes * Res2) {
     isBo = 1 ;
    struct ExprRes * Res;
    AppendSeq(Res1->Instrs, Res2->Instrs);
    Res = (struct ExprRes *) malloc(sizeof(struct ExprRes));
    char * Label = GenLabel();
    char * Label2 = GenLabel();
    
    AppendSeq(Res1->Instrs, GenInstr(NULL, "bne", TmpRegName(Res1->Reg), TmpRegName(Res2->Reg),
                                     Label));
    AppendSeq(Res1->Instrs, GenInstr(NULL, "li", TmpRegName(Res1->Reg), "1",
                                     NULL));
    AppendSeq(Res1->Instrs, GenInstr(NULL,"j",Label2,NULL,NULL));
    AppendSeq(Res1->Instrs, GenInstr(Label, NULL, NULL, NULL, NULL));
    AppendSeq(Res1->Instrs, GenInstr(NULL, "li", TmpRegName(Res1->Reg), "0",
                                     NULL));
    AppendSeq(Res1->Instrs, GenInstr(Label2, NULL, NULL, NULL, NULL));
    Res->Instrs = Res1->Instrs;
    Res->Reg = Res1->Reg;
    
    //不确定  ReleaseTmpReg(Res1->Reg);
    ReleaseTmpReg(Res2->Reg);
    free(Res1);
    free(Res2);
    return Res;
}

extern struct ExprRes * doBoLTExpr(struct ExprRes * Res1,  struct ExprRes * Res2) {
     isBo = 1 ;
    struct ExprRes * Res;
    AppendSeq(Res1->Instrs, Res2->Instrs);
    Res = (struct ExprRes *) malloc(sizeof(struct ExprRes));
    char * Label = GenLabel();
    char * Label2 = GenLabel();
    
    AppendSeq(Res1->Instrs, GenInstr(NULL, "bge", TmpRegName(Res1->Reg), TmpRegName(Res2->Reg),
                                     Label));
    AppendSeq(Res1->Instrs, GenInstr(NULL, "li", TmpRegName(Res1->Reg), "1",
                                    NULL));
    AppendSeq(Res1->Instrs, GenInstr(NULL,"j",Label2,NULL,NULL));
    AppendSeq(Res1->Instrs, GenInstr(Label, NULL, NULL, NULL, NULL));
    AppendSeq(Res1->Instrs, GenInstr(NULL, "li", TmpRegName(Res1->Reg), "0",
                                     NULL));
    AppendSeq(Res1->Instrs, GenInstr(Label2, NULL, NULL, NULL, NULL));
    Res->Instrs = Res1->Instrs;
    Res->Reg = Res1->Reg;
    //不确定 ReleaseTmpReg(Res1->Reg);
    ReleaseTmpReg(Res2->Reg);
    free(Res1);
    free(Res2);
    return Res;
}

extern struct ExprRes * doBoLEExpr(struct ExprRes * Res1,  struct ExprRes * Res2) {
    isBo = 1 ;
    struct ExprRes * Res;
    AppendSeq(Res1->Instrs, Res2->Instrs);
    Res = (struct ExprRes *) malloc(sizeof(struct ExprRes));
    char * Label = GenLabel();
    char * Label2 = GenLabel();
    
    AppendSeq(Res1->Instrs, GenInstr(NULL, "bgt", TmpRegName(Res1->Reg), TmpRegName(Res2->Reg),Label));
    AppendSeq(Res1->Instrs, GenInstr(NULL, "li", TmpRegName(Res1->Reg), "1",
                                    NULL));
    AppendSeq(Res1->Instrs, GenInstr(NULL,"j",Label2,NULL,NULL));
    AppendSeq(Res1->Instrs, GenInstr(Label, NULL, NULL, NULL, NULL));
    AppendSeq(Res1->Instrs, GenInstr(NULL, "li", TmpRegName(Res1->Reg), "0",
                                     NULL));
    AppendSeq(Res1->Instrs, GenInstr(Label2, NULL, NULL, NULL, NULL));
    Res->Instrs = Res1->Instrs;
    Res->Reg = Res1->Reg;
    //ReleaseTmpReg(Res1->Reg);
    ReleaseTmpReg(Res2->Reg);
    free(Res1);
    free(Res2);
    return Res;
}

extern struct ExprRes * doBoBTExpr(struct ExprRes * Res1,  struct ExprRes * Res2) {
     isBo = 1 ;
    struct ExprRes * Res;
    AppendSeq(Res1->Instrs, Res2->Instrs);
    Res = (struct ExprRes *) malloc(sizeof(struct ExprRes));
    char * Label = GenLabel();
    char * Label2 = GenLabel();
    
    AppendSeq(Res1->Instrs, GenInstr(NULL, "ble", TmpRegName(Res1->Reg), TmpRegName(Res2->Reg),
                                     Label));
    AppendSeq(Res1->Instrs, GenInstr(NULL, "li", TmpRegName(Res1->Reg), "1",
                                    NULL));
    AppendSeq(Res1->Instrs, GenInstr(NULL,"j",Label2,NULL,NULL));
    AppendSeq(Res1->Instrs, GenInstr(Label, NULL, NULL, NULL, NULL));
    AppendSeq(Res1->Instrs, GenInstr(NULL, "li", TmpRegName(Res1->Reg), "0",
                                     NULL));
    AppendSeq(Res1->Instrs, GenInstr(Label2, NULL, NULL, NULL, NULL));
    Res->Instrs = Res1->Instrs;
    Res->Reg = Res1->Reg;
    //ReleaseTmpReg(Res1->Reg);
    ReleaseTmpReg(Res2->Reg);
    free(Res1);
    free(Res2);
    return Res;
}

extern struct ExprRes * doBoBEExpr(struct ExprRes * Res1,  struct ExprRes * Res2) {
     isBo = 1 ;
    struct ExprRes * Res;
    AppendSeq(Res1->Instrs, Res2->Instrs);
    Res = (struct ExprRes *) malloc(sizeof(struct ExprRes));
    char * Label = GenLabel();
    char * Label2 = GenLabel();
    AppendSeq(Res1->Instrs, GenInstr(NULL, "blt", TmpRegName(Res1->Reg), TmpRegName(Res2->Reg),
                                     Label));
    AppendSeq(Res1->Instrs, GenInstr(NULL, "li", TmpRegName(Res1->Reg), "1",
                                    NULL));
    AppendSeq(Res1->Instrs, GenInstr(NULL,"j",Label2,NULL,NULL));
    AppendSeq(Res1->Instrs, GenInstr(Label, NULL, NULL, NULL, NULL));
    AppendSeq(Res1->Instrs, GenInstr(NULL, "li", TmpRegName(Res1->Reg), "0",
                                     NULL));
    AppendSeq(Res1->Instrs, GenInstr(Label2, NULL, NULL, NULL, NULL));
    Res->Instrs = Res1->Instrs;
    Res->Reg = Res1->Reg;
    //ReleaseTmpReg(Res1->Reg);
    ReleaseTmpReg(Res2->Reg);
    free(Res1);
    free(Res2);
    return Res;
}

extern struct ExprRes * doBoNEExpr(struct ExprRes * Res1,  struct ExprRes * Res2) {
     isBo = 1 ;
    struct ExprRes * Res;
    AppendSeq(Res1->Instrs, Res2->Instrs);
    Res = (struct ExprRes *) malloc(sizeof(struct ExprRes));
    char * Label = GenLabel();
    char * Label2 = GenLabel();
    
    AppendSeq(Res1->Instrs, GenInstr(NULL, "beq", TmpRegName(Res1->Reg), TmpRegName(Res2->Reg),
                                     Label));
    AppendSeq(Res1->Instrs, GenInstr(NULL, "li", TmpRegName(Res1->Reg), "1",
                                    NULL));
    AppendSeq(Res1->Instrs, GenInstr(NULL,"j",Label2,NULL,NULL));
    AppendSeq(Res1->Instrs, GenInstr(Label, NULL, NULL, NULL, NULL));
    AppendSeq(Res1->Instrs, GenInstr(NULL, "li", TmpRegName(Res1->Reg), "0",
                                     NULL));
    AppendSeq(Res1->Instrs, GenInstr(Label2, NULL, NULL, NULL, NULL));
    Res->Instrs = Res1->Instrs;
    Res->Reg = Res1->Reg;
    //ReleaseTmpReg(Res1->Reg);
    ReleaseTmpReg(Res2->Reg);
    free(Res1);
    free(Res2);
    return Res;
}

extern struct BExprRes * doBExpr(struct ExprRes * Res1,  struct ExprRes * Res2) {
	struct BExprRes * bRes;
	AppendSeq(Res1->Instrs, Res2->Instrs);
 	bRes = (struct BExprRes *) malloc(sizeof(struct BExprRes));
	bRes->Label = GenLabel();
	AppendSeq(Res1->Instrs, GenInstr(NULL, "bne", TmpRegName(Res1->Reg), TmpRegName(Res2->Reg), bRes->Label));
	bRes->Instrs = Res1->Instrs;
	ReleaseTmpReg(Res1->Reg);
  	ReleaseTmpReg(Res2->Reg);
	free(Res1);
	free(Res2);
	return bRes;
}

extern struct BExprRes * doBLTExpr (struct ExprRes * Res1,  struct ExprRes * Res2){
     struct BExprRes * bRes;
     AppendSeq(Res1->Instrs, Res2->Instrs);
     bRes = (struct BExprRes *) malloc(sizeof(struct BExprRes));
     bRes->Label = GenLabel();
     AppendSeq(Res1->Instrs, GenInstr(NULL, "bge", TmpRegName(Res1->Reg), TmpRegName(Res2->Reg), bRes->Label));
     bRes->Instrs = Res1->Instrs;
     ReleaseTmpReg(Res1->Reg);
     ReleaseTmpReg(Res2->Reg);
     free(Res1);
     free(Res2);
     return bRes;
}

extern void dodeclaration(char* type,struct SymEntry *AnEntry){
    struct typeInfo *attribute;
    attribute = (struct typeInfo *) malloc(sizeof(struct typeInfo));
    attribute->type = type;
    SetAttr(AnEntry, attribute);
    //printf("finish dodeclaration");
}

extern void Arraydodeclaration(char* artype,struct SymEntry *AnEntry,char* arsize){
    printf("arsize:  ");
    printf(arsize);
    printf("\n");
    
    struct typeInfo *attribute;
    attribute = (struct typeInfo *) malloc(sizeof(struct typeInfo));
    char* at = (char *)malloc(sizeof(char)*20);
    strcpy(at, artype);
    char* as = (char *)malloc(sizeof(char)*20);
    strcpy(as, arsize);
    
    attribute->type = at;
    attribute->size = as;
    SetAttr(AnEntry, attribute);
}


extern void testcoference(struct ExprRes * Res1){
    printf ("reg");
    printf (TmpRegName(Res1->Reg));
}

extern void pen(){
    printf("pen");
}

extern struct BExprRes * doBLEExpr (struct ExprRes * Res1,  struct ExprRes * Res2){
     struct BExprRes * bRes;
       AppendSeq(Res1->Instrs, Res2->Instrs);
       bRes = (struct BExprRes *) malloc(sizeof(struct BExprRes));
       bRes->Label = GenLabel();
       AppendSeq(Res1->Instrs, GenInstr(NULL, "bgt", TmpRegName(Res1->Reg), TmpRegName(Res2->Reg), bRes->Label));
       bRes->Instrs = Res1->Instrs;
       ReleaseTmpReg(Res1->Reg);
       ReleaseTmpReg(Res2->Reg);
       free(Res1);
       free(Res2);
       return bRes;
}
extern struct BExprRes * doBBTExpr (struct ExprRes * Res1,  struct ExprRes * Res2){
    struct BExprRes * bRes;
       AppendSeq(Res1->Instrs, Res2->Instrs);
       bRes = (struct BExprRes *) malloc(sizeof(struct BExprRes));
       bRes->Label = GenLabel();
       AppendSeq(Res1->Instrs, GenInstr(NULL, "ble", TmpRegName(Res1->Reg), TmpRegName(Res2->Reg), bRes->Label));
       bRes->Instrs = Res1->Instrs;
       ReleaseTmpReg(Res1->Reg);
       ReleaseTmpReg(Res2->Reg);
       free(Res1);
       free(Res2);
       return bRes;
}
extern struct BExprRes * doBBEExpr (struct ExprRes * Res1,  struct ExprRes * Res2){
       struct BExprRes * bRes;
       AppendSeq(Res1->Instrs, Res2->Instrs);
       bRes = (struct BExprRes *) malloc(sizeof(struct BExprRes));
       bRes->Label = GenLabel();
       AppendSeq(Res1->Instrs, GenInstr(NULL, "blt", TmpRegName(Res1->Reg), TmpRegName(Res2->Reg), bRes->Label));
       bRes->Instrs = Res1->Instrs;
       ReleaseTmpReg(Res1->Reg);
       ReleaseTmpReg(Res2->Reg);
       free(Res1);
       free(Res2);
       return bRes;
}
extern struct BExprRes * doBNEExpr (struct ExprRes * Res1,  struct ExprRes * Res2){
        struct BExprRes * bRes;
       AppendSeq(Res1->Instrs, Res2->Instrs);
       bRes = (struct BExprRes *) malloc(sizeof(struct BExprRes));
       bRes->Label = GenLabel();
       AppendSeq(Res1->Instrs, GenInstr(NULL, "beq", TmpRegName(Res1->Reg), TmpRegName(Res2->Reg), bRes->Label));
       bRes->Instrs = Res1->Instrs;
       ReleaseTmpReg(Res1->Reg);
       ReleaseTmpReg(Res2->Reg);
       free(Res1);
       free(Res2);
       return bRes;
}


/*

extern struct InstrSeq * doIf(struct ExprRes *res1, struct ExprRes *res2, struct InstrSeq * seq) {
	struct InstrSeq *seq2;
	char * label;
	label = GenLabel();
	AppendSeq(res1->Instrs, res2->Instrs);
	AppendSeq(res1->Instrs, GenInstr(NULL, "bne", TmpRegName(res1->Reg), TmpRegName(res2->Reg), label));
	seq2 = AppendSeq(res1->Instrs, seq);
	AppendSeq(seq2, GenInstr(label, NULL, NULL, NULL, NULL));
	ReleaseTmpReg(res1->Reg);
  	ReleaseTmpReg(res2->Reg);
	free(res1);
	free(res2);
	return seq2;
}

*/
void							 
Finish(struct InstrSeq *Code2)
{ struct InstrSeq *code;
  struct SymEntry *entry;
  struct Attr * attr;


  code = GenInstr(NULL,".text",NULL,NULL,NULL);
  //AppendSeq(code,GenInstr(NULL,".align","2",NULL,NULL));
  AppendSeq(code,GenInstr(NULL,".globl","main",NULL,NULL));
  AppendSeq(code, GenInstr("main",NULL,NULL,NULL,NULL));
  AppendSeq(code,Code2);
  AppendSeq(code, GenInstr(NULL, "li", "$v0", "10", NULL)); 
  AppendSeq(code, GenInstr(NULL,"syscall",NULL,NULL,NULL));
    entry = FirstEntry(funtable);
    while (entry) {
        struct ExprRes * attribute1=NULL;
        attribute1 = GetAttr(entry);
        printf("\n");
        printf("name\n");
        printf(entry->Name);
         printf("\n");
        printf(TmpRegName(attribute1->Reg));
        AppendSeq(code,attribute1->Instrs);
        entry = NextEntry(funtable, entry);
    }
  AppendSeq(code,GenInstr(NULL,".data",NULL,NULL,NULL));
  AppendSeq(code,GenInstr("str",".asciiz",strContext,NULL,NULL));
  AppendSeq(code,GenInstr("blank",".asciiz","\" \"",NULL,NULL));
  AppendSeq(code,GenInstr("strue",".asciiz","\"true\"",NULL,NULL));
  AppendSeq(code,GenInstr("sfalse",".asciiz","\"false\"",NULL,NULL));
  AppendSeq(code,GenInstr("_nl",".asciiz","\"\\n\"",NULL,NULL));
  entry = FirstEntry(table);
  AppendSeq(code,GenInstr(NULL,".align","4",NULL,NULL));
    while (entry) {
        struct typeInfo *attribute=NULL;
             attribute = GetAttr(entry);
    if (strcmp(attribute->type,"intar") == 0) {
        int isize =  atoi(attribute->size);
        char* s;
        s = (char *) malloc(8);
        printf("size    ");
        isize = 4*isize;
        printf("\n");
        sprintf(s, "%d", isize);
        AppendSeq(code,GenInstr((char *) GetName(entry),".space",s,NULL,NULL));
    }else{
        AppendSeq(code,GenInstr((char *) GetName(entry),".word","0",NULL,NULL));
    }
    entry = NextEntry(table, entry);
    }
    
  /*  entry = FirstEntry(table);
 while (entry) {
	AppendSeq(code,GenInstr((char *) GetName(entry),".word","0",NULL,NULL));
        entry = NextEntry(table, entry);
 }
  */
  WriteSeq(code);
  
  return;
}
