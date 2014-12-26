enum TokenType
{
  TOK_IDENTIFIRE,
  TOK_DIGIT,
  TOK_SYMBOL,
  TOK_INT,
  TOK_RETURN,
  TOK_EOF
};

class Token
{
private:
  TokenType Type;
  std::string Number;
  int Number;
  int Line;

public:
  Token(std::string string, TokenType type, int line) : TokenString(string), Type(type), Line(line){
    if(type == TOK_DIGIT){
      Number = atoi(string.c_str());
    }
    else{
      Number = 0x7fffffff;
    }
  };

  ~Token(){};

  TokenType getTokenType(){return Type;};

  std::string getTokenString(){return TokenString;};

  int getNumberValue(){return Number;};

  int getLine(){return Line;};
}

class TokenStream
{
private:
  std::vector<Token*> Tokens;
  int CurIndex;

public:
  TokenStream : CurIndex(0){};
  ~TokenStream(){};

  bool ungetToken(int Times=1);
  bool getNextToken();
  bool pushToken(Token *token){
    Tokens.push_back(token);
    return true;
  }
  Token getToken();

  TokenType getCurToken(){return Tokens[CurIndex]->getTokenTyoe();};

  std::string getCurString(){return Tokens[CurIndex]->getTokenString();};

  int getCurNumVal(){return Tokens[CurIndex]->getNumberValue();};

  int getCurIndex(){return CurIndex;};

  bool applyTokenIndex(int index){CurIndex = index; return true;}
  bool printTokens();
}

TokenStream::~TokenStream()
{
  for(int i = 0; i < Tokens.size(); i++){
    SAFE_DELETE(Tokens[i]);
  }
  Tokens.clear();
}

Token TokenStream::getToken()
{
  return *Tokens[CurIndex];
}

bool TokenStream::getNextToken()
{
  int size = Tokens.size();
  if(--size <= CurIndex){
    return false;
  }
  else{
    CurIndex++;
    return true;
  }
}

bool TolenStream::ungetToken(int times)
{
  for(int i = 0; i < times; i++){
    if(CurIndex == 0){
      return false;
    }
    else{
      CurIndex--;
    }
  }
  return true;
}

bool TokenStream::printTokens()
{
  std::vector<Token*>::iterator titer = Tokens.begin();
  while(titer != Tokens.end()){
    fprintf(stdout, "%d:", (*titer)->getTokenType());
    if((*titer)->getTokenType() != TOK_EOF){
      fprintf(stdout, "%s\n", (*titer)->getTokenString().c_str());
    }
    ++titer;
  }
  return true;
}
