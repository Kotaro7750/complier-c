#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdarg.h>

typedef enum{
  TK_RESERVED,
  TK_NUM,
  TK_EOF
} TokenKind;


typedef struct Token Token;

struct Token {
  TokenKind kind;
  Token* next;
  int val;
  char* str;
};

Token *CurTok;


Token* createToken(TokenKind kind,Token* cur,char* str){
  Token* new = (Token *)calloc(1, sizeof(Token));
  new->kind = kind;
  new->str = str;
  cur->next = new;

  return new;
}

void error(char* fmt,...){
  va_list ap;
  va_start(ap, fmt);
  vprintf(fmt, ap);
  va_end(ap);
}

Token* tokenize(char* p){
  Token head;
  head.next = NULL;
  Token* cur=&head;

  while(*p){
    if(isspace(*p)){
      ++p;
      continue;
    }
    if(*p == '+' || *p == '-'){
      cur = createToken(TK_RESERVED, cur, p++);
      continue;
    }
    if(isdigit(*p)){
      cur = createToken(TK_NUM, cur, p);
      cur->val = strtol(p,&p,10);
      continue;
    }
    error("トークナイズできません");
  }

  createToken(TK_EOF, cur,p);
  return head.next;
}

bool consume(char op){
  if(CurTok->kind != TK_RESERVED || CurTok->str[0]  != op){
    return false;
  }
  CurTok = CurTok->next;
  return true;
}

int expectNumber(){
  if(CurTok->kind != TK_NUM){
    error("数ではありません");
  }

  int val = CurTok->val;
  CurTok = CurTok->next;

  return val;
}

void expect(char op){
  if (CurTok->kind != TK_RESERVED || CurTok->str[0] != op) {
    error("'%c'ではありません\n",op);
  }
  CurTok = CurTok->next;
}

int at_eof(){
  return CurTok->kind == TK_EOF;
}

int main(int argc, char** argv){
  if(argc != 2){
    fprintf(stderr,"引数の個数が正しくありません");
    return 1;
  }
  char* p = argv[1];

  CurTok = tokenize(p);

  printf(".intel_syntax noprefix\n");
  printf(".global main\n");
  printf("main:\n");


  printf("  mov rax,%d\n",expectNumber());

  while(!at_eof()){
    if(consume('+')){
      printf("  add rax,%d\n",expectNumber());
      continue;
    }
    expect('-');
    printf("  sub rax,%d\n",expectNumber());
  }
  printf("  ret\n");
  return 0;
}
