#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include "cclemon.h"

extern struct Token* CurTok;

typedef struct Node Node;

bool consume(char* op){
  if(CurTok->kind != TK_RESERVED || strlen(op) != CurTok->len || memcmp(CurTok->str, op, CurTok->len)){
    return false;
  }
  CurTok = CurTok->next;
  return true;
}

int expectNumber(){
  if(CurTok->kind != TK_NUM){
    error(CurTok->str,"数ではありません");
  }

  int val = CurTok->val;
  CurTok = CurTok->next;

  return val;
}

void expect(char* op){
  if (CurTok->kind != TK_RESERVED || strlen(op) != CurTok->len || memcmp(CurTok->str, op, CurTok->len)) {
    error(CurTok->str,"'%s'ではありません\n",op);
  }
  CurTok = CurTok->next;
}

int at_eof(){
  return CurTok->kind == TK_EOF;
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

Node* expr(){
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
  return createNumNode(expectNumber());
}
