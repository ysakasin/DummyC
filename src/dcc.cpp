#include "llvm/IR/IRPrintingPasses.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/PassManager.h"
#include "llvm/LinkAllPasses.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/FormattedStream.h"
#include "llvm/Support/PrettyStackTrace.h"
#include "llvm/Support/Signals.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Support/raw_ostream.h"
#include <fcntl.h>
#include "lexer.hpp"
#include "AST.hpp"
#include "APP.hpp"
#include "parser.hpp"
#include "codegen.hpp"
#include "option_parser.hpp"

int main(int argc, char **argv){
  llvm::InitializeNativeTarget();
  llvm::sys::PrintStackTraceOnErrorSignal();
	llvm::PrettyStackTraceProgram X(argc, argv);
  llvm::EnableDebugBuffering = true;

  OptionParser opt(argc, argv);
  if(!opt.parseOption()){
    exit(1);
  }

  if(opt.getInputFileName().length() == 0){
    fprintf(stderr, "入力ファイル名が設定されていません\n");
    exit(1);
  }

  Parser *parser = new Parser(opt.getInputFileName());
  if(parser->doParse()){
    fprintf(stderr, "error at parser or lexer\n");
    SAFE_DELETE(parser);
    exit(1);
  }

  TranslationUnitAST &tunit = parser->getAST();
  if(tunit.empty()){
    fprintf(stderr, "TranslationUnit is empty\n");
    SAFE_DELETE(parser);
    exit(1);
  }

  CodeGen *codegen = new CodeGen();
  if(!codegen->doCodeGen(tunit, opt.getInputFileName())){
    fprintf(stderr, "error at codegen\n");
    SAFE_DELETE(parser);
    SAFE_DELETE(codegen);
    exit(1);
  }

  llvm::Module &mod = codegen->getModule();
  if(mod.empty()){
    fprintf(stderr, "Module is empty\n");
    SAFE_DELETE(parser);
    SAFE_DELETE(codegen);
    exit(1);
  }


  int fd = open(opt.getOutputFileName().c_str(), O_WRONLY | O_CREAT, S_IWRITE);
  if(fd == -1){
    fprintf(stderr, "output file error\n");
    SAFE_DELETE(parser);
    SAFE_DELETE(codegen);
    exit(1);
  }

  llvm::raw_fd_ostream raw_stream(fd, true);
  llvm::PrintModulePass pm(raw_stream);
  pm.run(mod);
  raw_stream.close();

  SAFE_DELETE(parser);
  SAFE_DELETE(codegen);

  return 0;
}
