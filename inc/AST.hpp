#ifndef AST_HPP
#define AST_HPP


#include <string>
#include <map>
#include <vector>
#include "llvm/Support/Casting.h"
#include "APP.hpp"

enum AstID
{
  BaseID,
  VariableDeclID,
  BinaryExprID,
  NullExprID,
  CallExprID,
  JumpStmtID,
  VariableID,
  NumberID
};

class BaseAST
{
  AstID ID;

public:
  BaseAST(AstID id) : ID(id){};
  virtual ~BaseAST(){};
  AstID getValueID() const {return ID;};
};

// ; を示すAST
class NullExprAST : public BaseAST
{
public:
  NullExprAST() : BaseAST(NullExprID){}

  static inline bool classof(NullExprAST const*){return true;}

  static inline bool classof(BaseAST const* base){
    return base->getValueID() == NullExprID;
  }
};

// 変数宣言
class VariableDeclAST : public BaseAST
{
public:
  typedef enum{
    param,
    local
  }DeclType;

private:
  std::string Name;
  DeclType Type;

public:
  VariableDeclAST(const std::string &name) : BaseAST(VariableDeclID), Name(name){};

  static inline bool classof(VariableDeclAST const*){return true;};

  static inline bool classof(BaseAST const* base){
    return base->getValueID() == VariableDeclID;
  };

  ~VariableDeclAST(){};

  std::string getName(){return Name;};

  bool setDeclType(DeclType type){
    Type = type;
    return true;
  };

  DeclType getType(){return Type;};
};

// 二項演算
class BinaryExprAST : public BaseAST
{
  std::string Op;
  BaseAST *LHS, *RHS;

public:
  BinaryExprAST(std::string op, BaseAST *lhs, BaseAST *rhs) : BaseAST(BinaryExprID), Op(op), LHS(lhs), RHS(rhs){};

  ~BinaryExprAST(){SAFE_DELETE(LHS); SAFE_DELETE(RHS);};

  static inline bool classof(BinaryExprAST const*){return true;};

  static inline bool classof(BaseAST const* base){
    return base->getValueID() == BinaryExprID;
  };

  std::string getOp(){return Op;};

  BaseAST *getLHS(){return LHS;};

  BaseAST *getRHS(){return RHS;};
};

// 関数
class CallExprAST : public BaseAST
{
  std::string Callee;
  std::vector<BaseAST*> Args;

public:
  CallExprAST(const std::string &callee, std::vector<BaseAST*> &args)
    : BaseAST(CallExprID), Callee(callee), Args(args){};

  ~CallExprAST();

  static inline bool classof(CallExprAST const*){return true;};

  static inline bool classof(BaseAST const* base){
    return base->getValueID() == CallExprID;
  };

  std::string getCallee(){return Callee;};

  BaseAST *getArgs(int i){
    if(i<Args.size()){
      return Args.at(i);
    }
    else{
      return NULL;
    }
  };
};

// ジャンプ、return
class JumpStmtAST : public BaseAST
{
  BaseAST *Expr;

public:
  JumpStmtAST(BaseAST *expr) : BaseAST(JumpStmtID), Expr(expr){};

  ~JumpStmtAST(){SAFE_DELETE(Expr);};

  static inline bool classof(JumpStmtAST const*){return true;};

  static inline bool classof(BaseAST const* base){
    return base->getValueID() == JumpStmtID;
  };

  BaseAST *getExpr(){return Expr;};
};

// 変数参照
class VariableAST : public BaseAST
{
  std::string Name;

public:
  VariableAST(const std::string &name) : BaseAST(VariableID), Name(name){};

  ~VariableAST(){};

  static inline bool classof(VariableAST const*){return true;};

  static inline bool classof(BaseAST const* base){
    return base->getValueID() == VariableID;
  };

  std::string getName(){return Name;};
};

class NumberAST : public BaseAST
{
  int Val;

public:
  NumberAST(int val) : BaseAST(NumberID), Val(val){};

  ~NumberAST(){};

  static inline bool classof(NumberAST const*){return true;};

  static inline bool classof(BaseAST const* base){
    return base->getValueID() == NumberID;
  };

  int getNumberValue(){return Val;};
};

// 関数宣言
class PrototypeAST
{
  std::string Name;
  std::vector<std::string> Params;

public:
  PrototypeAST(const std::string &name, const std::vector<std::string> &params)
    : Name(name), Params(params){};

  std::string getName(){return Name;};

  std::string getParamName(int i){
    if(i < Params.size()){
      return Params.at(i);
    }
    else{
      return NULL;
    }
  }
  ;
  int getParamNum(){return Params.size();};
};

// 関数定義（ボディ）
class FunctionStmtAST
{
  std::vector<VariableDeclAST*> VariableDecls;
  std::vector<BaseAST*> StmtLists;

public:
  FunctionStmtAST(){};

  ~FunctionStmtAST();

  bool addVariableDeclaration(VariableDeclAST *vdecl);

  bool addStatement(BaseAST *stmt){StmtLists.push_back(stmt);};

  VariableDeclAST *getVariableDecl(int i){
    if(i < VariableDecls.size()){
      return VariableDecls.at(i);
    }
    else{
      return NULL;
    }
  };

  BaseAST *getStatement(int i){
    if(i < StmtLists.size()){
      return StmtLists.at(i);
    }
    else{
      return NULL;
    }
  };
};

// 関数定義
class FunctionAST
{
  PrototypeAST *Proto;
  FunctionStmtAST *Body;

public:
  FunctionAST(PrototypeAST *proto, FunctionStmtAST * body) : Proto(proto), Body(body){};

  ~FunctionAST();

  std::string getName(){return Proto->getName();};

  PrototypeAST *getPrototype(){return Proto;};

  FunctionStmtAST *getBody(){return Body;};
};

// ソースコード
class TranslationUnitAST
{
  std::vector<PrototypeAST*> Prototypes;
  std::vector<FunctionAST*> Functions;

public:
  TranslationUnitAST(){};
  ~TranslationUnitAST();

  bool addPrototype(PrototypeAST *proto);

  bool addFunction(FunctionAST *func);

  bool empty();

  PrototypeAST *getPrototype(int i){
    if(i < Prototypes.size()){
      return Prototypes.at(i);
    }
    else{
      return NULL;
    }
  }

  FunctionAST *getFunction(int i){
    if(i < Functions.size()){
      return Functions.at(i);
    }
    else{
      return NULL;
    }
  }
};

#endif
