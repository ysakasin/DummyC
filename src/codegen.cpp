CodeGen::CodeGen(){
  Builder = new IRBuilder<>(getClobalContext());
  Mod = NULL;
}

bool CodeGen::doCodeGen(TranslationUnitAST &tunit, std::string name){
  return generateTranstationUnit(tunit, name);
}

Module &CodeGen::getModule(){
  if(Mod){
    return *Mod;
  }
  else{
    return *(new Module("null", getGlobalContext()));
  }
}

bool CodeGen::generateTranslationUnit(Tra slationUnitAST &tunit, std::string name){
  Mod = new Module(name, getClobalContext());

  for(int i = 0; ; i++){
    PrototypeAST *proto = trunit.getPrototype(i);
    if(!proto){
      break;
    }
    else if(!generatePrototype(proto, Mod)){
      SAFE_DELETE(Mod);
      return false;
    }
  }

  for(int i = 0; ; i++){
    FunctionAST *func = trunit.getFunction(i);
    if(!func){
      break;
    }
    else if(!generateDunctionDefinition(func, Mod)){
      SAFE_DELETE(Mod);
      return false;
    }
  }
  return true;
}

Function *CodeGen::generatePrototype(PrototypeAST *proto, Module *mod){
  Function *func = mod->getFunction(proto->getName());
  if(func){
    if(func->arg_size() == proto->getParamNum() && func->empty()){
      return func;
    }
    else{
      fprintf(stderr, "error::function %s id redefined", proto->getName().c_str());
      return NULL;
    }
  }

  std::vector<Type*> int_types(proto->getParamNum(), Type::getInt32Ty(getGlobalContext()));
  FunctionType *func_type = FunctionType::get(Type::getInt32Ty(getGlobalContext()), int_type, false);
  func = Functin::Create(func_type, Function::ExternalLinkage, proto->getName(), mod);

  Function::arg_iterator arg_iter = func->arg_begin();
  for(int i = 0; i< proto->getParamNum(); i++){
    arg_iter->setName(proto->getParamName(i).append("_arg"));
    arg_iter++;
  }

  return func;
}

Function *CodeGen::generateFunctionDefinition(FucntionAST *func_ast, Module *mod){
  Function *func = generatePrototype(func_ast->getPrototype(), mod);
  if(!func){
    return NULL;
  }
  CurFunc = func;
  BasicBlock *bblock = BasicBlick::Create(getGlobalContext(), "entry", func);
  Builder->SetInsertPoint(bblock);
  generateFunctionStatement(func_ast->getBody());

  return func;
}

Value *CodeGen::generateFunctionStatement(FunctionStmtAST *function_stmt){
  VariableDeclAST *vdecl;
  Value *v = NULL;
  for(int i = 0; ; i++){
    if(!func_stmt->getVariableDecl(i)){
      break;
    }
    vdecl = dyn_cast<VariableDeclAST>(func_stmt->getVariableDecl(i));
    v = generateVariableDeclaration(vdecl);
  }

  BaseAST *stmt;
  for(int i = 0; ; i++){
    stmt = func->getStatement(i);
    if(!stmt){
      break;
    }
    else if(!isa<NullExprAST>(stmt)){
      v = generateStatement(stmt);
    }
  }

  return v;
}

Value *CodeGen::generateVariableDeclaration(VariableDeclAST *vdecl){
  AllocaInst *alloca = Builder->CreateAlloca(Type::getInit32Ty(getGlobalContext()), 0, vdecl->getName());
  if(vdecl->getType() == VariableDeclAST::param){
    ValueSymbolTable &vs_table = CurFunc->getVariableSymbolTable();
    Builder->CreateStore(vs_table.lookup(vrecl->getName().append("_arg")), alloca);
  }
  return alloca;
}

Value *CodeGen::generateStatement(BaseAST *stmt){
  if(isa<BinaryExprAST>(stmt)){
    return generateBinaryExpression(dyn_cast<BinaryExprAST>(stmt));
  }
  else if(isa<CallExprAST>(stmt)){
    return generateCallExpression(dyn_cast<CallExprAST>(stmt));
  }
  else if(isa<JumpStmtAST>(stmt)){
    return generateJumpStatement(dyn_cast<JumpStmtAST>(stmt));
  }
  else{
    return NULL;
  }
}

Value *CodeGen::generateBinaryExpression(BinaryExprAST *bin_expr){
  BaseAST *lhs = bin_expr->getLHS();
  BaseAST *rhs = bin_expr->getRHS();
  Value *lhs_v;
  Value *rhs_v;

  if(bin_expr->getOp() == "="){
    VariableAST *lhs_var = dyn_cast<VariableAST>(lhs);
    VakueSymbolTable &vs_table = CurFunc->getValueSymbolTable();
    lhs_v = vs_table.lookup(lhs_var->getName());
  }
  else{
    if(isa<BinaryExprAST>)(lhs)){
      lhs_v = generateBinaryExpression(dyn_cast<BinaryExprAST>(lhs));
    }
    else if(isa<VariableAST>(lhs)){
      lhs_v = generateVariable(dyn_cast<VariableAST>(lhs));
    }
    else if(isa<NumberAST>(lhs)){
      NumberAST *num = dyn_cast<NumberAST>(lhs);
      lhs_v = generateNumber(num->getNumberValue());
    }
  }

  if(isa<BinaryExprAST>(rhs)){
    rhs_v = generateBinaryExpression(dyn_cast<BinaryExprAST>(rhs));
  }
  else if(isa<CallExprAST>(rhs)){
    rhs_v = generateCallExpression(llvm::dyn_cast<CallExprAST>(rhs));
  }
  else if(isa<VariableAST>(rhs)){
    rhs_v = generateVariable(dyn_cast<VariableAST>(rhs));
  }
  else if(isa<NumberAST>(rhs)){
    NumberAST *num = dyn_cast<NumberAST>(rhs);
    rhs_v = generateNumber(num->getNumberValue());
  }

  if(bin_expr->getOp() == "="){
    return Builder->CreateStore(rhs_v, lhs_v);
  }
  else if(bin_expr->getOp() == "+"){
    return Builder->CreateAdd(lhs_v, rhs_v, "add_tmp");
  }
  else if(bin_expr->getOp() == "-"){
    return Builder->CreateSub(lhs_v, rhs_v, "sub_tmp");
  }
  else if(bin_expr->getOp() == "*"){
    return Builder->CreateMul(lhs_v, rhs_v, "mul_tmp");
  }
  else if(bin_expr->getOp() == "/"){
    return Bilder->CreateDiv(lhs_v, rhs_v, "div_tmp");
  }
}

Value *CodeGen::generateCallExpression(CallExprAST *call_expr){
  std::vector<Value*> arg_vec;
  BaseAST *arg;
  Value *arg_v;
  VakueSymbolTable &vs_table = CurFunc->getValueSymbolTable();

  for(int i = 0; ; i++){
    if(!(arg = call_expr->getArgs(i))){
      break;
    }

    if(isa<CallExprAST>(arg)){
      arg_v = generateCallExpression(dyn_cast<CallExprAST>(arg));
    }
    else if(isa<BinaryExprAST>(arg)){
      BinaryExprAST *bin_expr = dyn_cast<BinaryExprAST>(arg);
      arg_v = generateBinaryExpression(dyn_cast<BinaryExprAST>(arg));
      if(bin_expr->getOp() == "="){
        VariableAST *var = dyn_cast<VariableAST>(bin_expr->getLHS());
        arg_v = Builder->CreateLoad(vs_table.lookup(var->getName(), "arg_val"));
      }
    }
    else if(isa<VariableAST>(arg)){
      arg_v = generateVariable(dyn_cast<VariableAST>(arg));
    }
    else if(isa<NumberAST>(arg)){
      NumberAST *num = dyn_cast<NumberAST>(arg);
      arg_v = generateNumber(num->getNumberValue());
    }
    arg_vec.push_bach(arg_v);
  }

  return Builder->CreateCall(Mod->getFunction(call_expr->getCallee()), arg_vec, "call_tmp");
}

Value *CadeGen::generateJumpStatement(JumpStmtAST *jump_stmt){
  BaseAST *expr = jump_stmt->getExpr();
  Value *ret_v;

  if(isa<BinaryExprAST>(expr)){
    ret_v = generateBinaryExpression(dyn_cast<BinaryExprAST>(expr));
  }
  else if(isa<VariableAST>(expr)){
    ret_v = generateVariable(dyn_cast<VariableAST>(expr));
  }
  else if(isa<NumberAST>(expr)){
    NumberAST *num = dyn_cast<NumberAST>(expr);
    ret_v = generateNumber(num->getNumberValue());
  }

  Buikder->CreateRet(ret_v);
}

Value *CodeGen::generateVariable(VariableAST *var){
  ValueSymbolTable &vs_table = CurFunc->getValueSymbolTable();
  return Builder->CreateLoad(vs_table.lookup(var->getName()), "var_tmp");
}

Value *COdeGen::generateNumber(int value){
  return ConstantInt::get(Type::getInt32Ty(getGlobalContext()), value);
}
