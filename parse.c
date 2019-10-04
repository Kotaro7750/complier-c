#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include "cclemon.h"

struct Token* CurTok;
struct LVar* locals;

typedef struct Node Node;
typedef struct Token Token;
typedef struct LVar LVar;

extern Node* code[100];

bool consume(char* op){
  if(CurTok->kind != TK_RESERVED || strlen(op) != CurTok->len || memcmp(CurTok->str, op, CurTok->len)){
    return false;
  }
  CurTok = CurTok->next;
  return true;
}

Token* consumeIdent(){
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

int atEOF(){
  return CurTok->kind == TK_EOF;
}

int atIdent(){
  return CurTok->kind == TK_IDENT;
}

struct LVar* findLvar(struct Token* tok){
  for(LVar* var = locals;var;var = var->next){
    if(var->len == tok->len && !memcmp(var->name, tok->str, var->len)){
      return var;
    }
  }
  return NULL;
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

Node** program(){
  for(int i = 0;i < 100;i++){
    if(atEOF()){
      break;
    }
    code[i] = stmt();
  }
  return code;
}

Node* stmt(){
  Node* node;
  if(CurTok->kind == TK_RET){
    node = calloc(1, sizeof(Node));
    node->kind = ND_RET;
    CurTok = CurTok->next;
    node->lhs = expr();
  }else {
    node = expr();
  }
  expect(";");
  return node;
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
  if((atIdent())){
    Node* node = calloc(1, sizeof(Node));
    node->kind = ND_LVAR;

    Token* tok = consumeIdent();
    LVar* lvar = findLvar(tok);
    if(lvar){
      node->offset = lvar->offset;
    }else {
      lvar = calloc(1, sizeof(LVar));
      lvar->next = locals;
      lvar->name = tok->str;
      lvar->len = tok->len;
      lvar->offset = locals->offset + 8;
      node->offset = lvar->offset;
      locals = lvar;
    }
    return node;
  }
  return createNumNode(expectNumber());
}
