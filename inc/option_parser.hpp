class OptionParser
{
private:
  std::string InputFilename;
  std::string OutputFilename;
  int Argc;
  char **Argv;

public:
  OptionParser(int argc, char **argv):Argc(argc), Argv(argv){}
  void printHelp(){
    fprintf(stdout, "Compiler for DummyC\n");
  }
  void printHelp();
  std::string getInputFileName(){return InputFilename;}
  std::string getOutputFileName(){return OutputFilename;}
  bool parseOption();
}
