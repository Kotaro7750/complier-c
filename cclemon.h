#ifndef BOOL
#define BOOL
#include <stdbool.h>
#endif /* end of include guard */

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

struct Token {
  TokenKind kind;
  struct Token* next;
  int val;
  char* str;
  int len;
};

struct Node {
  NodeKind kind;
  struct Node* lhs;
  struct Node* rhs;
  int val;
};

extern struct Token* CurTok;


void error(char* loc,char* fmt,...);

//lexer.c
struct Token* createToken(TokenKind kind,struct Token* cur,char* str,int len);
bool startsWith(char* p,char* q);
struct Token* tokenize(char* p);

//parse.c
bool consume(char* op);
int expectNumber();
void expect(char* op);
int at_eof();
struct Node* createNode(NodeKind kind,struct Node* lhs,struct Node* rhs);
struct Node* createNumNode(int val);
struct Node* expr();
struct Node* mul();
struct Node* unary();
struct Node* primary();

//codegen.c
void gen(struct Node* node);
