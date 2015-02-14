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
