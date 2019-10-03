#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdarg.h>
#include "cclemon.h"

typedef struct Token Token;
typedef struct Node Node;

Token* CurTok;
char* user_input;

void error_at(char* loc,char* fmt,...){
  int pos = loc - user_input;
  fprintf(stderr, "%s\n",user_input);
  fprintf(stderr, "%*s",pos,"");
  fprintf(stderr, "^ \n");
  va_list ap;
  va_start(ap, fmt);
  vprintf(fmt, ap);
  va_end(ap);
  fprintf(stderr, "\n");

  exit(1);
}

void error(char* fmt,...){
  va_list ap;
  va_start(ap, fmt);
  vprintf(fmt, ap);
  va_end(ap);
  fprintf(stderr, "\n");

  exit(1);
}

int main(int argc, char** argv){
  if(argc != 2){
    fprintf(stderr,"引数の個数が正しくありません");
    return 1;
  }
  user_input = argv[1];

  CurTok = tokenize(user_input);

  printf(".intel_syntax noprefix\n");
  printf(".global main\n");
  printf("main:\n");

  printf("  push rbp\n");
  printf("  mov rbp,rsp\n");
  printf("  sub rsp,208\n");

  Node* node = expr();
  gen(node);

  printf("  pop rax\n");

  printf("  mov rsp,rbp\n");
  printf("  pop rbp\n");
  printf("  ret\n");
  return 0;
}
