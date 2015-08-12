#include "AST.hpp"

TranslationUnitAST::~TranslationUnitAST(){
  for(int i = 0; i < Prototypes.size(); i++){
    SAFE_DELETE(Prototypes[i]);
  }
  Prototypes.clear();

  for(int i = 0; i < Functions.size(); i++){
    SAFE_DELETE(Functions[i]);
  }
  Functions.clear();
}

bool TranslationUnitAST::addPrototype(PrototypeAST *proto){
    Prototypes.push_back(proto);
    return true;
}

bool TranslationUnitAST::addFunction(FunctionAST *func){
    Functions.push_back(func);
    return true;
}

bool TranslationUnitAST::empty(){
  if(Prototypes.size() == 0 && Functions.size() == 0){
    return true;
  }
  else{
    return false;
  }
}

FunctionAST::~FunctionAST(){
  SAFE_DELETE(Proto);
  SAFE_DELETE(Body);
}

FunctionStmtAST::~FunctionStmtAST(){
  for(int i = 0; i < VariableDecls.size(); i++){
    SAFE_DELETE(VariableDecls[i]);
  }
  VariableDecls.clear();

  for(int i = 0; i < StmtLists.size(); i++){
    SAFE_DELETE(StmtLists[i]);
  }
  StmtLists.clear();
}

bool FunctionStmtAST::addVariableDeclaration(VariableDeclAST *vdecl){
    VariableDecls.push_back(vdecl);
    return true;
}

CallExprAST::~CallExprAST(){
  for(int i = 0; i < Args.size(); i++){
    SAFE_DELETE(Args[i]);
  }
}
