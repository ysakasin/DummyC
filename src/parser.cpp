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

PrototypeAST *Parser::visitFunctionDeclaration(){
  int backup = Tokens->getCurIndex();
  PrototypeAST *proto = visitPrototype();
  if(!proto){
    return NULL;
  }

  if(Tokens->getCurString() == ";"){
    // 本来であればここで再定義されていないか確認
    Tokens->getNextToken();
    return proto;
  }
  else{
    SAFE_DELETE(proto);
    Tokens->applyTokenIndex(backup);
    return NULL;
  }
}

FunctionAST *Parser::visitFunctionDefinition(){
  int backup = Tokens->getCurIndex();

  PrototypeAST *proto = visitPrototype();
  if(!proto){
    return NULL;
  }

  // 本来であればここで再定義されていないか確認
  FunctionStmtAST *func_stmt = visitFunctionStatement(proto);
}

PrototypeAST *Parser::visitPrototype(){
  int backup = Tokens->getCurIndex();

  bool is_first_param = true;
  std::vector<std::string> param_list;
  while(true){
    if(!is_first_param && Tokens->getCurType() == TOK_SYMBOL && Tokens->getCurString() == ","){
      Tokens->getNextToken();
    }
    if(Tokens->getCurType() == TOK_INT){
      Tokens->getNextToken();
    }
    else{
      break;
    }

    if(Tokens->getCurType() == TOK_IDENTIFIRE){
      param_list.push_back(Tokens->getCurString());
      Tokens->getNextToken();
    }
    else{
      Tokens->applyTokenIndex(buckup);
      return NULL;
    }
  }
}

FunctionStmtAST *Parser::visitFunvtionStatement(PrototypeAST *proto){
  int buckup = Tokens->getCurIndex();

  if(Tokens->getCurString() == "{"){
    Tokens->getNextToken();
  }
  else{
    return NULL;
  }

  FunctionStmtAST *func_stmt = new FunctionStmtAST();

  for(int i = 0; i < proto->getparamNum(); i++){
    VariableDeclAST *vdecl = new VariableDeclAST(proto->getParamName(i));
    vdecl->setDeclType(VariableDeclAST::param);
    func_stmt->addVariableDeclaration(vdecl);
    VariableTable.push_back(vdecl->getName());
  }

  /* omit */

  else if(var_decl = visitVariableDeclaration()){
    while(var_decl){
      var_decl->setDeclType(VariableDeclAST::local);

      /* TODO : 二重宣言のチェック */

      func_stmt->addVariableDeclaration(var_decl);
      VariableTable.push_back(var_decl->getName());
      var_decl = visitVariabeDeclaration();
    }

    if(stmt = visitStatement()){
      while(stmt){
        last_stmt = stmt;
        func_stmt -> addStatement(stmt);
        stmt = visitStatement();
      }
    }
  }
  else{
    SAFE_DELETE(func_stmt);
    Tokens->applyTokenIndex(bkup);
    return NULL;
  }

  /* TODO : 戻り値の確認 */

  if(Tokens->getCurString() == "}"){
    Tokens->getNextToken();
    return func_stmt;
  }
  else{
    SAFE_DELETE(func_stmt);
    Tokens->applyTokenIndex(bkup);
    return NULL;
  }
}
