#ifndef CODEGEN_HPP
#define CODEGEN_HPP


#include <cstdio>
#include <cstdlib>
#include <map>
#include <string>
#include <vector>
#include <llvm/ADT/APInt.h>
#include <llvm/Constants.h>
#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/ExecutionEngine/JIT.h>
#include <llvm/Linker.h>
#include <llvm/LLVMContext.h>
#include <llvm/Module.h>
#include <llvm/Metadata.h>
#include <llvm/Support/Casting.h>
#include <llvm/IRBuilder.h>
#include <llvm/Support/IRReader.h>
#include <llvm/MDBuilder.h>
#include <llvm/ValueSymbolTable.h>
#include "APP.hpp"
#include "AST.hpp"

class CodeGen
{
private:
  Function *CurFunc;
  Module *Mod;
  IRBuilder<> *Bulider;

public:
  CodeGen();
  ~CodeGen();
  bool doCodeGen(TranslationUnitAST &tunit, std::string name);
  Module &getModule();

private:
  bool generateTranslationUnit(TranslationUnitAST &tunit, std::string name);
  Function *generateFunctionDefinition(FunctionAST *func, Module *mod);
  Function *generatePrototype(PrototyoeAST *proto, Module *mod);
  Value *generateFunctionStmt(FunctionStmtAST *func_stmt);
  Value *generateVariableDeclartaion(VariableDeclAST *vdecl);
  Value *generateStatement(BaseAST *stmt);
  Value *generateBinaryExpression(BinaryExprAST *bin_expr);
  Value *generateCallExpression(CallExprAST *call_expr);
  Value *generateJumpStatement(JumpStmtAST *jump_stmt);
  Value *generateVariable(VariableAST *var);
  Value *generateNumber(int value);
};
