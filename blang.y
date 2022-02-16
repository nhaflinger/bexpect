%{
  #include <stdio.h>
  #include <stdlib.h>
  #include <string.h>
  #include <algorithm>
  #include <vector>
  #include <map>
  #include <string>
  #include <iostream>
  #include <cctype>

  // stuff from flex that bison needs to know about:

  int yylex();
  
  extern FILE* yyin;
  
  void yyerror(const char *s);
  std::string removeHexQuotes(std::string str);
  extern int line_num;

  std::vector<std::pair<std::string, std::string>> m_command_list;

%}

%union {
  int ival;
  float fval;
  char *sval;
}

// define the constant-string tokens:
%token START
%token WAIT 
%token SEND 
%token EXPECT 
%token COMMENT
%token END ENDL

// define the "terminal symbol" token types I'm going to use (in CAPS
// by convention), and associate each with a field of the union:
%token <ival> INT
%token <fval> FLOAT
%token <sval> STRING
%token <sval> HEX_QUOTES
%token <sval> STRING_QUOTES

%%
start:
  header body_section footer {
#ifdef BLANG
     fprintf(stdout, "done reading bexpect command file!\n");
#endif
    }
  ;
header:
  START ENDLS {
#ifdef BLANG
      fprintf(stdout, "reading bexpect command file\n");
#endif
    }
  ;
body_section:
  body_lines
  ;
body_lines:
  body_lines wait_line
  | body_lines wait_line_comment
  | body_lines send_line
  | body_lines send_line_comment
  | body_lines expect_line
  | body_lines expect_line_comment
  | body_lines comment_line
  | wait_line
  | wait_line_comment
  | send_line
  | send_line_comment
  | expect_line
  | expect_line_comment
  | comment_line
  ;
wait_line:
  WAIT INT ENDLS {
      std::pair<std::string, std::string> command;
      command.first = "wait";
      command.second = std::to_string($2);
      m_command_list.push_back(command);
    }
  ;
wait_line_comment:
  WAIT INT ENDLS COMMENT {
      std::pair<std::string, std::string> command;
      command.first = "wait";
      command.second = std::to_string($2);
      m_command_list.push_back(command);
    }
  ;
send_line:
  SEND HEX_QUOTES ENDLS {
      std::pair<std::string, std::string> command;
      command.first = "send";
      std::string fullstr = $2;
      std::string shortstr = removeHexQuotes(fullstr);
      command.second = shortstr;
      m_command_list.push_back(command);
      free($2);
    }
  ;
send_line_comment:
  SEND HEX_QUOTES ENDLS COMMENT {
      std::pair<std::string, std::string> command;
      command.first = "send";
      std::string fullstr = $2;
      std::string shortstr = removeHexQuotes(fullstr);
      command.second = shortstr;
      m_command_list.push_back(command);
      free($2);
    }
  ;
expect_line:
  EXPECT HEX_QUOTES ENDLS {
      std::pair<std::string, std::string> command;
      command.first = "expect";
      std::string fullstr = $2;
      std::string shortstr = removeHexQuotes(fullstr);
      command.second = shortstr;
      m_command_list.push_back(command);
      free($2);
    }
  ;
expect_line_comment:
  EXPECT HEX_QUOTES ENDLS COMMENT {
      std::pair<std::string, std::string> command;
      command.first = "expect";
      std::string fullstr = $2;
      std::string shortstr = removeHexQuotes(fullstr);
      command.second = shortstr;
      m_command_list.push_back(command);
      free($2);
    }
  ;
comment_line:
  COMMENT {
    }
  ;
footer:
  END ENDLS
  ;
ENDLS:
  ENDLS ENDL
  | ENDL ;
%%

std::string removeHexQuotes(std::string str)
{
    std::string chars = "x\"";
    std::string retval = str;
    for (char c: chars) 
    {
        retval.erase(std::remove(retval.begin(), retval.end(), c), retval.end());
    }
    return retval;
}

void yyerror(const char *s) 
{
    fprintf(stderr, "Parse error on line %d! Message: %s\n", line_num, s);
    exit(-1);
}

void yyread(FILE *fh)
{
    // Set flex to read from it instead of defaulting to STDIN:
    yyin = fh;

    // Parse through the input:
    do {
        yyparse();
    } while (!feof(yyin));
}

#ifdef BLANG
int main(int argc, char* argv[]) 
{
    // open a file handle to a particular file:
    FILE *fh = fopen(argv[1], "r");
    // make sure it's valid:
    if (!fh) 
    {
        fprintf(stdout, "I can't open bexpect command file!\n");
        return -1;
    }
    yyread(fh);

    std::vector<std::pair<std::string, std::string>>::iterator it;
    for (it = m_command_list.begin(); it != m_command_list.end(); ++it)
    {
        std::cout << (*it).first << " " << (*it).second << std::endl; 
    }

    return 0;
}
#endif //BLANG
