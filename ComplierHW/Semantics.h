/* Semantics.h
   The action and supporting routines for performing semantics processing.
*/

/* Semantic Records */
struct IdList {
  struct SymEntry * TheEntry;
  struct IdList * Next;
};

struct ExprRes {
  int Reg;
  struct InstrSeq * Instrs;
};

struct ExprResList {
	struct ExprRes *Expr;
	struct ExprResList * Next;
};

struct BExprRes {
  char * Label;
  struct InstrSeq * Instrs;
};


/* Semantics Actions */
extern struct ExprRes *  doIntLit(char * digits);
extern struct ExprRes *  doRval(char * name);
extern struct ExprRes *  doArRval(char * name,  struct ExprRes * Expr);
extern struct InstrSeq * doAssign(char * name,  struct ExprRes * Res1);
extern struct InstrSeq * doBExprAssign (char * name,  struct ExprRes * Res1);
extern struct InstrSeq * doBExprTAssign (char * name);
extern struct InstrSeq * doBExprFAssign (char * name);
extern void Arraydodeclaration(char* artype,struct SymEntry *AnEntry,char* arsize);
extern void declareFunction(struct SymEntry *AnEntry,struct InstrSeq * seq1 , struct ExprRes * Res1);
extern struct ExprRes * callfun(char* name);
extern struct InstrSeq * callfunonly(char* name);
extern struct InstrSeq * callParameterFun(char* funcname);

extern struct ExprRes *  doAdd(struct ExprRes * Res1,  struct ExprRes * Res2);
extern struct ExprRes *  doMult(struct ExprRes * Res1,  struct ExprRes * Res2);
extern struct ExprRes *  doDivide(struct ExprRes * Res1, struct ExprRes * Res2);
extern struct ExprRes *  doMinus(struct ExprRes * Res1, struct ExprRes * Res2);
extern struct ExprRes *  doMod(struct ExprRes * Res1, struct ExprRes * Res2);
extern struct ExprRes *  doNegative(struct ExprRes * Res1);
extern struct ExprRes *  doExponentiation(struct ExprRes * Res1, struct ExprRes * Res2);
extern struct InstrSeq * doPrintln();
extern struct InstrSeq * doPrintstr(char * str1);
extern struct InstrSeq * doPrintsp(struct ExprRes * Res1);
extern struct InstrSeq * doPrint(struct ExprRes * Expr);
extern struct InstrSeq * doAddPrint(struct InstrSeq * Expr1, struct ExprRes * Expr2);
extern void pen();
extern void testcoference(struct ExprRes * Res1);

extern struct InstrSeq * doread(char* name);
extern struct InstrSeq * doaddread(struct InstrSeq * Expr1,char* name);

extern struct InstrSeq * doArAssignment(char* Id,struct ExprRes * Expr1,struct ExprRes * Res1);

extern struct ExprRes * doNotBoExpr(struct ExprRes * Res1);
extern struct ExprRes * doOrBoExpr(struct ExprRes * Res1,struct ExprRes * Res2);
extern struct ExprRes * doAndBoExpr(struct ExprRes * Res1,struct ExprRes * Res2);
extern struct ExprRes * doOrBoId(struct ExprRes * Res1,char * name);

extern struct ExprRes * doBoExpr(struct ExprRes * Res1,  struct ExprRes * Res2);
extern struct ExprRes * doBoLTExpr(struct ExprRes * Res1,  struct ExprRes * Res2);
extern struct ExprRes * doBoLEExpr(struct ExprRes * Res1,  struct ExprRes * Res2);
extern struct ExprRes * doBoBTExpr(struct ExprRes * Res1,  struct ExprRes * Res2);
extern struct ExprRes * doBoBEExpr(struct ExprRes * Res1,  struct ExprRes * Res2);
extern struct ExprRes * doBoNEExpr(struct ExprRes * Res1,  struct ExprRes * Res2);
extern char *join1(char *a, char *b);
extern void declareParameterFunction(struct SymEntry *AnEntry,struct InstrSeq * seq1, struct ExprRes * Res1);
extern struct BExprRes * doBExpr (struct ExprRes * Res1,  struct ExprRes * Res2);
extern struct BExprRes * doBLTExpr (struct ExprRes * Res1,  struct ExprRes * Res2);
extern struct BExprRes * doBLEExpr (struct ExprRes * Res1,  struct ExprRes * Res2);
extern struct BExprRes * doBBTExpr (struct ExprRes * Res1,  struct ExprRes * Res2);
extern struct BExprRes * doBBEExpr (struct ExprRes * Res1,  struct ExprRes * Res2);
extern struct BExprRes * doBNEExpr (struct ExprRes * Res1,  struct ExprRes * Res2);
extern struct InstrSeq * doIf(struct ExprRes *Expr, struct InstrSeq * seq1, struct InstrSeq * seq2);
extern struct InstrSeq * dowhile(struct ExprRes *Expr, struct InstrSeq * seq1);
extern void	Finish(struct InstrSeq *Code);
