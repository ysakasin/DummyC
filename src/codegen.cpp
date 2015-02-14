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
