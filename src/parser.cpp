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
  int bkup = Tokens->getCurIndex();
  PrototypeAST *proto = visitPrototype();
  if(!proto){
    return NULL;
  }

  // prototype
  if(Tokens->getCurString() == ";"){
    // 再定義されていないか確認
    if( PrototypeTable.find(proto->getName()) != PrototypeTable.end() ||
        (FunctionTable.find(proto->getName()) != FunctionTable.end() &&
        FunctionTable[proto->getName()] != proto->getParamNum()) ){
      fprintf(stderr, "Function : %s is redefined", proto->getName().c_str());
      SAFE_DELETE(proto);
      return NULL;
    }
    // プロトタイプ宣言テーブルに追加
    PrototypeTable[proto->getName()] = proto->getParamNum();
    Tokens->getNextToken();
    return proto;
  }
  else{
    SAFE_DELETE(proto);
    Tokens->applyTokenIndex(bkup);
    return NULL;
  }
}

FunctionAST *Parser::visitFunctionDefinition(){
  int bkup = Tokens->getCurIndex();

  PrototypeAST *proto = visitPrototype();
  if(!proto){
    return NULL;
  }
  else if(// 関数定義の重複が無いか確認
      (PrototypeTable.find(proto->getName() != PrototypeTable.end() &&
       PrototypeTable[proto->getName()] != proto->getParamNum()) ||
      FunctionTable.find(proto->getName()) != FunctionTable.end()){
    fprintf(stderr, "Function : %s is redefined", proto->getName().c_str());
    SADE_DELETE(proto);
    return NULL
  }

  VariableTable.clear();
  FunctionStmtAST *func_stmt = visitFunctionStatement(proto);
  if(func_stmt){
    FunctionTable[proto->getName()] = proto->getParamNum();
    return new FunctionAST(proto, func_stmt);
  }

  /* omit */
}

PrototypeAST *Parser::visitPrototype(){
  int bkup = Tokens->getCurIndex();

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
      // 引数の変数名に重複が無いか確認
      if(std::find(param_list.begin(), param_list_end()), Tokens->getCurString()) != param_list.end()){
        Tokens->applyTokenIndex(bkup);
        return NULL;
      }
      param_list.push_back(Tokens->getCurString());
      Tokens->getNextToken();
    }
    else{
      Tokens->applyTokenIndex(bkup);
      return NULL;
    }
  }
}

FunctionStmtAST *Parser::visitFunctionStatement(PrototypeAST *proto){
  int bkup = Tokens->getCurIndex();

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

      // 変数名の重複チェック
      if(std::find(VariableTable.begin(), VariableTable.end(), var_decl.getName()) != VariableTable.end()){
        SAFE_DELETE(var_decl);
        SAFE_DELETE(func_stmt);
        return NULL
      }

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

  // 最後のStatementがjump_statementであるか確認
  if(!last_stmt || !isa<JumpStmtAST>(last_stmt)){
    SAFE_DELETE(func_stmt);
    Tokens->applyTokenIndex(bkup);
    return NULL;
  }

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

BaseAST *Parser::visitAssignmentExpression(){
  int bkup = Tokens-> getCurIndex();

  BaseAST *lhs;
  if(Tokens->getCurType() == TOK_IDENTIFIRE){
    // 変数宣言されているか確認
    if(std::find(VariableTable.begin(), VariableTable.end(), Tokens->getCurString() != VariableTable.end()){
      lhs = new VariableAST(Tokens->getCurString());
      Tokens->getNextToken();
      BaseAST *rhs;
      if(Tokens->getCurType() == TOK_SYMBOL && Tokens->getCurString() == "="){
        Tokens->getNextToken();
        if(rhs = visit AdditiveExpression(NULL)){
          return new BinaryExprAST("=", lhs, rhs);
        }
        else{
          SAFE_DELETE(lhs);
          Tokens->applyTokenIndex(bkup);
        }
      }
      else{
        SAFE_DELETE(lhs);
        Tokens->applyTokenIndex(bkup);
      }
    }
    else{
      Tokens->applyTokenIndex(bkup);
    }
  }
  BaseAST *add_expr = visitAdditiveExpression(NULL);
  if(add_expr){
    return add_expr;
  }
  return NULL;
}

BaseAST *Parser::visitPrimaryExpression(){
  int bkup = Tokens->getCurIndex();

  if(Tokens->getCurType() == TOK_IDENTIFIER &&
    (std::find(VariableTable.begin(), VariableTable.end(), Tokens->getCurString()) != VariableTable.end())){
    std::string var_name = Tokens->getCurString();
    Tokens->getNextToken();
    return new VariableAST(var_name);
  }
  else if(Tokens->getCurType() == TOK_DIGIT){
    int val = Tokens->getCurNumVal();
    Tokens->getNextToken();
    return new NumberAST(val);
  }
  else if(Tokens->getCurType() == TOK_SYMBOL && Tokens->getCurString == "-"){
    /* omit */
  }
  return NULL;
}

BaseAST *Parser::visitPostfixExpression(){
  int bkup = Tokens->getCurIndex();

  /* omit */

  if(Tokens->getCurType() == TOK_IDENTIFIRE){
    int param_num;
    // プロトタイプ宣言されているか確認
    if(PrototypeTable.find(Tokens->getCurString()) != PrototypeTable.end()){
      param_num = FunctionTable[Tokens->getCurString()];
    }
    // 関数定義みであるか確認
    else if(FunctionTable.find(Tokens->getCurString()) != PrototypeTable.end()){
      param_num = FunctionTable[Tokens->getCurString()];
    }
    else{
      return NULL;
    }

    std::string Callee = Tokens->getCurString();
    Tokens->getNextToken();

    if(Tokens->getCurType() != TOK_SYMBOL || Tokens->getCurString() != "("){
      Tokens->applyTokenIndex(bkup);
      return NULL;
    }

    Tokens->getNextToken();
    std::vector<BaseAST*> args;
    BaseAST *assign_expr = visitAssignmentExpression();
    if(assign_expr){
      args.push_back(assign_expr);
      while(Tokens->getCurType() == TOK_SYMBOL && Tokens->getCurString() == ","){
        Tokens->getNextToken();
        assign_expre = visitAssignmentExpression();
        if(assign_expre){
          args.push_back(assign_expr);
        }
        else{
          break;
        }
      }
    }

    // 引数の数を確認
    if(args.size() != param_num){
      for(int i = 0; i < args.size(); i++){
        SAFE_DELETE(args[i]);
      }
      Tokens->applyTokenIndex(bkup);
      return NULL;
    }

    if(Tokens->getCurType() == TOK_SYMBOL && Tokens->getCurString == ")"){
      Tokens->getNextToken();
      return new CallExprAST(Callee, args);
    }
    else{
      for(int i = 0; i < args.size(); i++){
        SAFE_DELETE(args[i]);
      }
      Tokens->applyTokenIndex(bkup);
      return NULL;
    }
  }
  else{
    return NULL;
  }
}

BaseAST *Parser::visitAdditiveExpression(BaseAST *lhs){
  int bkup = Tokens->getCurIndex();

  if(!lhs){
    lhs = visitMultiplicativeExpression(NULL);
  }

  if(!lhs){
    return NULL;
  }

  BaseAST *rhs;
  if(Tokens->getCurType() == TOR_SYMBOL && Tokens->getCurString() == "+"){
    Tokens->getNextToken();
    rhs = visitMultiplcativeExpression(NULL);
    if(rhs){
      return visitAdditiveExpression(new BinaryExprAST("+", lhs, rhs));
    }
    else{
      SAFE_DELETE(lhs);
      Tokens->applyTokenIndex(bkup);
      return NULL;
    }
  }
  else if(Tokens->getCurType() == TOR_SYMBOL && Tokens->getCurString() == "-"){
    Tokens->getNextToken();
    rhs = visitMultiplcativeExpression(NULL);
    if(rhs){
      return visitAdditiveExpression(new BinaryExprAST("-", lhs, rhs));
    }
    else{
      SAFE_DELETE(lhs);
      Tokens->applyTokenIndex(bkup);
      return NULL;
    }
  }

  return lhs;
}

BaseAST *Parser::visitExpressionStatement(){
  BaseAST *assign_expr;
  if(Tokens->getCurString() == ";"){
    Tokens->getNextToken();
    return new NullExprAST();
  }
  else if(assign_expr = visitAssignmentExpression()){
    if(Tokens->getCurString() == ";"){
      Tokens->getNextToken();
      return assign_expr;
    }
  }
  return NULL;
}
