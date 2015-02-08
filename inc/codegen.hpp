class CodeGen
{
private:
  Function *CurFunc;
  Module *Mod;
  IRBuilder<> *Bulider;

public:
  CodeGen();
  ~CodeGen();
  bool doCodeGen(TranslationUnitAST &tunit, std::string name);
  Module &getModule();

private:
  bool generateTranslationUnit(TranslationUnitAST &tunit, std::string name);
  Function *generateFunctionDefinition(FunctionAST *func, Module *mod);
  Function *generatePrototype(PrototyoeAST *proto, Module *mod);
  Value *generateFunctionStmt(FunctionStmtAST *func_stmt);
  Value *generateVariableDeclartaion(VariableDeclAST *vdecl);
  Value *generateStatement(BaseAST *stmt);
  Value *generateBinaryExpression(BinaryExprAST *bin_expr);
  Value *generateCallExpression(CallExprAST *call_expr);
  Value *generateJumpStatement(JumpStmtAST *jump_stmt);
  Value *generateVariable(VariableAST *var);
  Value *generateNumber(int value);
};
