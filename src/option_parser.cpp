#include "option_parser.hpp"

bool OptionParser::parseOption(){
  if(Argc < 2){
    fprintf(stderr, "引数が足りません\n");
    return false;
  }

  for(int i = 0; i < Argc; i++){
    if(Argv[i][0] == '-' && Argv[i][1] == 'O' && Argv[i][2] == '\0'){
      OutputFilename.assign(Argv[++i]);
    }
    else if(Argv[i][0] == '-' && Argv[i][1] == 'h' && Argv[i][2] == '\0'){
      printHelp();
      return false;
    }
    else if(Argv[i][0] == '-'){
      fprintf(stderr, "%s は不明なオプションです\n", Argv[i]);
      return false;
    }
    else {
      InputFilename.assign(Argv[i]);
    }
  }

  std::string ifn = InputFilename;
  int len = ifn.length();
  if(OutputFilename.empty() && (len > 2) && ifn[len-3] == '.' && (ifn[len-2] == 'd' && ifn[len-1] == 'c')){
    OutputFilename = std::string(ifn.begin(), ifn.end()-3);
    OutputFilename += ".ll";
  }
  else if(OutputFilename.empty()){
    OutputFilename = ifn;
    OutputFilename += ".ll";
  }

  return true;
}
