#include "llvm/Assembly/PrintModulePass.h"
#include "llvm/LLVMContext.h"
#include "llvm/Module.h"
#include "llvm/PassManager.h"
#include "llvm/LinkAllPasses.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/FormattedStream.h"
#include "llvm/Support/PrettyStackTrace.h"
#include "llvm/Support/Signals.h"
#include "llvm/Support/TargetSelect.h"
#include "lexer.hpp"
#include "AST.hpp"
#include "parser.hpp"
#include "codegen.hpp"
#include "option_parser.hpp"

int main(int argc, char **argv){
  InitializeNativeTarget();
  sys::PrintStackTraseOnErrorSignal();
  PrettyStackTraceProgram X(argc, argv);
  EnableDebugBuffering = true;

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

  CodeGen *codegen = new CodeGen();
  if(!codegen->doCodeGen(tunit, opt.getInputFileName())){
    fprintf(stderr, "error at codegen\n");
    SAFE_DELETE(parser);
    SAFE_DELETE(codegen);
    exit(1);
  }

  Module &mod = codegen->getModule();
  if(mod.empty()){
    fprintf(stderr, "Module is empty\n");
    SAFE_DELETE(parser);
    SAFE_DELETE(codegen);
    exit(1);
  }

  PassManager pm;
  std::string error;
  raw_fd_ostream raw_stream(opt.getOutputFileName().c_str(), error);
  pm.add(createPrintModulePass(&raw_stream));
  pm.run(mod);
  raw_stream.close();

  SAFE_DELETE(parser);
  SAFE_DELETE(codegen);

  return 0;
}
