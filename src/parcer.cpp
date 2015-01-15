Parcer::Parcer(std::string filename){
  Tokens = LexicalAnaltysis(filename);
};

/*  構文解析実行
    @return 成功：true 失敗：false
*/
bool Parser::doParse(){
    if(!Tokens){
      fprintf(stderr, "error at lexer\n");
      return false;
    }
    else{
      return VisitTranslationUnit();
    }
};

TranslationUnitAST &Parser::getAST(){
  if(TU){
    return *TU;
  }
  else{
    return *(new TranslationUnitAST());
  }
};

// TranslationUnit用構文解析メソッド
bool Parser::visitTranslationUnit(){
  TU = new TranslatonUnitAST();

  // ExternalDecl
  while(true){
    if(!visitExternalDeclaration(TU)){
      SAFE_DELETE(TU);
      return false;
    }
    if(Tokens->getCurType == TOK_EOF){
      break;
    }
  }
  return true;
}

// ExternalDeclaration用構文解析メソッド
bool Parser::visitExternalDeclaration(TranslationUnitAST *tunit){
  PrototypeAST *proto = visitFunctionDeclaration();
  if(proto){
    tunit->addPrototype(proto);
    return true;
  }

  FunctionAST *func_def = visitFunctionDefinition();
  if(func_def){
    tunit->addFunction(func_def);
    return true;
  }

  return false;
}
