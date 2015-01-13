enum AstID
{
  BaseID,
  VariableDeclID,
  BinaryExprID,
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

// 変数宣言
class VariableDexlAST : public BaseAST
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

  static inline bool classof(VariableDexlAST const*){return true;};

  static inline bool classof(BaseAST const* base){
    return base->getValueID() == VariableDeclID
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
  BinaryExpreAST(std::string op, BaseAST *lhs, BaseAST *rhs) : BaseAST(BinaryExpreID), Op(op), LHS(lhs), RHS(rhs){};

  ~BinaryExpreAST(){SAFE_DELETE(LHS); SAFE_DELETE(RHS);};

  static inline bool classof(BinaryExpreAST const*){return true;};

  static inline bool classof(BaseAST const* base){
    return base->getValueID() == BinaryExpreAST;
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

  ~CallExpreAST();

  static inline bool classof(CallexprAST const*){return true;};

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
}

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
}

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
}
