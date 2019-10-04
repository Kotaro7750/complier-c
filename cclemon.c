#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdarg.h>
#include "cclemon.h"

typedef struct Token Token;
typedef struct Node Node;
typedef struct LVar LVar;

Token* CurTok;
Node* code[100];
char* user_input;
LVar* locals;

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

  locals = (LVar *)calloc(1, sizeof(LVar));

  printf(".intel_syntax noprefix\n");
  printf(".global main\n");
  printf("main:\n");

  printf("  push rbp\n");
  printf("  mov rbp,rsp\n");
  printf("  sub rsp,208\n");

  program();
  for(int i = 0;code[i];i++){
    gen(code[i]);
    printf("  pop rax\n");
  }


  printf("  mov rsp,rbp\n");
  printf("  pop rbp\n");
  printf("  ret\n");
  return 0;
}
