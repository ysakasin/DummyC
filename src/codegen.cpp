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
