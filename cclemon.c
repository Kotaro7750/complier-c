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

typedef enum{
  ND_ADD,
  ND_SUB,
  ND_MUL,
  ND_DIV,
  ND_NUM
} NodeKind;

typedef struct Token Token;

char* user_input;
Token *CurTok;

struct Token {
  TokenKind kind;
  Token* next;
  int val;
  char* str;
};

typedef struct Node Node;

struct Node {
  NodeKind kind;
  Node* lhs;
  Node* rhs;
  int val;
};


Token* createToken(TokenKind kind,Token* cur,char* str){
  Token* new = (Token *)calloc(1, sizeof(Token));
  new->kind = kind;
  new->str = str;
  cur->next = new;

  return new;
}

void error(char* loc,char* fmt,...){
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

Token* tokenize(char* p){
  Token head;
  head.next = NULL;
  Token* cur=&head;

  while(*p){
    if(isspace(*p)){
      ++p;
      continue;
    }
    if(*p == '+' || *p == '-' || *p == '*' || *p == '/' || *p == '(' || *p == ')'){
      cur = createToken(TK_RESERVED, cur, p++);
      continue;
    }
    if(isdigit(*p)){
      cur = createToken(TK_NUM, cur, p);
      cur->val = strtol(p,&p,10);
      continue;
    }
    error(p,"トークナイズできません");
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
    error(CurTok->str,"数ではありません");
  }

  int val = CurTok->val;
  CurTok = CurTok->next;

  return val;
}

void expect(char op){
  if (CurTok->kind != TK_RESERVED || CurTok->str[0] != op) {
    error(CurTok->str,"'%c'ではありません\n",op);
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

Node* expr();
Node* mul();
Node* primary();

Node* expr(){
  Node* node = mul();

  for(;;){
    if(consume('+')){
      node = createNode(ND_ADD, node, mul());
    }else if (consume('-')) {
      node = createNode(ND_SUB, node, mul());
    }else {
      return node;
    }
  }
}

Node* mul(){
  Node* node = primary();

  for(;;){
    if(consume('*')){
      node = createNode(ND_MUL, node, primary());
    }
    else if(consume('/')){
      node = createNode(ND_DIV, node, primary());
    }else {
      return node;
    }
  }
}

Node* primary(){
  if(consume('(')){
    Node* node = expr();
    expect(')');
    return node;
  }
  return createNumNode(expectNumber());
}

void gen(Node* node){
  if(node->kind == ND_NUM){
    printf("  push %d\n",node->val);
    return;
  }

  gen(node->lhs);
  gen(node->rhs);

  printf("  pop rdi\n");
  printf("  pop rax\n");

  switch(node->kind){
    case ND_ADD:
      printf("  add rax,rdi\n");
      break;
    case ND_SUB:
      printf("  sub rax, rdi\n");
      break;
    case ND_MUL:
      printf("  imul rax, rdi\n");
      break;
    case ND_DIV:
      printf("  cqo\n");
      printf("  idiv rdi\n");
      break;
    default:
      break;
  }
  printf("  push rax\n");
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

  Node* node = expr();
  gen(node);

  printf("  pop rax\n");
  printf("  ret\n");
  return 0;
}
