#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdbool.h>
#include "cclemon.h"

typedef struct Token Token;

Token* createToken(TokenKind kind,Token* cur,char* str,int len){
  Token* new = (Token *)calloc(1, sizeof(Token));
  new->kind = kind;
  new->str = str;
  new->len = len;
  cur->next = new;

  return new;
}

bool startsWith(char* p,char* q){
  return memcmp(p,q,strlen(q)) == 0;
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
    if(startsWith(p,"<=") || startsWith(p, ">=") || startsWith(p, "==") || startsWith(p, "!=")){
      cur = createToken(TK_RESERVED, cur, p,2);
      p+=2;
      continue;
    }
    if(*p == '+' || *p == '-' || *p == '*' || *p == '/' || *p == '(' || *p == ')' || *p == '<' || *p == '>' || *p == '=' || *p == ';'){
      cur = createToken(TK_RESERVED, cur, p++,1);
      continue;
    }
    if(isdigit(*p)){
      char* q = p;
      cur = createToken(TK_NUM, cur, p,0);
      cur->val = strtol(p,&p,10);
      cur->len = p - q;
      continue;
    }
    if('a' <= *p && *p <= 'z'){
      cur = createToken(TK_IDENT, cur, p++, 1);
      while('a' <= *p && *p <= 'z'){
        p++;
        cur->len++;
      }
      continue;
    }
    error_at(p,"トークナイズできません");
  }

  createToken(TK_EOF, cur,p,0);
  return head.next;
}
