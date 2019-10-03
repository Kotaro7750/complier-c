#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include "cclemon.h"

extern struct Token* CurTok;

typedef struct Node Node;
typedef struct Token Token;

bool consume(char* op){
  if(CurTok->kind != TK_RESERVED || strlen(op) != CurTok->len || memcmp(CurTok->str, op, CurTok->len)){
    return false;
  }
  CurTok = CurTok->next;
  return true;
}

Token* consumeToken(){
  if(CurTok->kind != TK_IDENT){
    return NULL;
  }
  Token* ret = CurTok;
  CurTok = CurTok->next;
  return ret;
}

int expectNumber(){
  if(CurTok->kind != TK_NUM){
    error_at(CurTok->str,"数ではありません");
  }

  int val = CurTok->val;
  CurTok = CurTok->next;

  return val;
}

void expect(char* op){
  if (CurTok->kind != TK_RESERVED || strlen(op) != CurTok->len || memcmp(CurTok->str, op, CurTok->len)) {
    error_at(CurTok->str,"'%s'ではありません\n",op);
  }
  CurTok = CurTok->next;
}

int at_eof(){
  return CurTok->kind == TK_EOF;
}

int at_ident(){
  return CurTok->kind == TK_IDENT;
}

Node* createNode(NodeKind kind,Node* lhs,Node* rhs){
  Node* new = (Node *)calloc(1, sizeof(Node));
  new->kind = kind;
  new->lhs = lhs;
  new->rhs = rhs;

  return new;
}

Node* createNumNode(int val){
  Node* new = (Node *)calloc(1, sizeof(Node));
  new->kind = ND_NUM;
  new->val = val;

  return new;
}

Node* createIdentNode(Token* tok){
  Node* new = (Node*)calloc(1, sizeof(Node));
  new->kind = ND_LVAR;
  new->offset = (tok->str[0] - 'a' + 1)*8;
  return new;
}

Node* expr(){
  return assign();
}

Node* assign(){
  Node* node = equality();
  if(consume("=")){
    node = createNode(ND_ASSIGN, node, assign());
  }
  return node;
}

Node* equality(){
  Node* node = relational();

  for(;;){
    if(consume("==")){
      node = createNode(ND_EQ, node, relational());
    }else if (consume("!=")) {
      node = createNode(ND_NEQ,node,relational());
    }else {
      return node;
    }
  }
}

Node* relational(){
  Node* node = add();
  for(;;){
    if(consume(">")){
      node = createNode(ND_GT, node, add());
    }else if (consume("<")) {
      node = createNode(ND_GT, add(), node);
    }else if (consume(">=")) {
      node = createNode(ND_GTE, node, add());
    }else if(consume("<=")){
      node= createNode(ND_GTE, add(), node);
    }else {
      return node;
    }
  }
  return add();
}

Node* add(){
  Node* node = mul();

  for(;;){
    if(consume("+")){
      node = createNode(ND_ADD, node, mul());
    }else if (consume("-")) {
      node = createNode(ND_SUB, node, mul());
    }else {
      return node;
    }
  }
}

Node* mul(){
  Node* node = unary();

  for(;;){
    if(consume("*")){
      node = createNode(ND_MUL, node, unary());
    }
    else if(consume("/")){
      node = createNode(ND_DIV, node, unary());
    }else {
      return node;
    }
  }
}

Node* unary(){
  if(consume("+")){
    return primary();
  }else if (consume("-")) {
    return createNode(ND_SUB, createNumNode(0), primary());
  }
  return primary();
}

Node* primary(){
  if(consume("(")){
    Node* node = expr();
    expect(")");
    return node;
  }
  if(at_ident()){
    Token* tok = consumeToken();
    return createIdentNode(tok);
  }
  return createNumNode(expectNumber());
}
