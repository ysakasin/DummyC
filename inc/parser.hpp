// 構文解析・意味解析
class Parser
{
private:
  TokenStream *Tokens;
  TranslationUnitAST *TU;

public:
  Parser(std::string filename);
  ~Parser(){
    SAFE_DELETE(TU);
    SAFE_DELETE(TOKENS);
  };
  bool doParse();
  TranslationUnitAST &getAST();

private:
  bool visitTranslationUnit();
  bool visitExternalDeclaration(TranslationUnitAST *tunit);
  PrototypeAST *visitFunctionDeclation();
  FunctionAST *visitFunctionDefinition();
  PrototypeAST *visitPrototype();
  FunctionStmtAST *visitFunctionStatement(PrototypeAST *proto);
  VariableDeclAST *visitVariableDeclaration();
  BaseAST *visitStatenebt();
  BaseAST *visitExpressionStatement();
  BaseAST *visitJumpStatement();
  BaseAST *visitAssignmentExpression();
  BaseAST *visitAdditiveExpression(BaseAST *lhs);
  BaseAST *visitMultiplicativeExpression(BaseAST *lhs);
  BaseAST *visitPostfixExpression();
  BaseAST *visitPrimaryExpression();
};
