#ifndef BOOL
#define BOOL
#include <stdbool.h>
#endif /* end of include guard */

typedef enum{
  TK_RESERVED,
  TK_NUM,
  TK_IDENT,
  TK_EOF
} TokenKind;

typedef enum{
  ND_ADD,
  ND_SUB,
  ND_MUL,
  ND_DIV,
  ND_EQ,
  ND_NEQ,
  ND_NUM,
  ND_GT,
  ND_GTE,
  ND_ASSIGN,
  ND_LVAR
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
  int offset;
};

struct LVar {
  struct LVar* next;
  char* name;
  int len;
  int offset;
};

extern struct Token* CurTok;
extern struct Node* code[100];
extern struct LVar* locals;


void error_at(char* loc,char* fmt,...);
void error(char* fmt,...);

//lexer.c
struct Token* createIdent(TokenKind kind,struct Token* cur,char* str,int len);
bool startsWith(char* p,char* q);
struct Token* tokenize(char* p);

//parse.c
bool consume(char* op);
int expectNumber();
void expect(char* op);
int atEOF();
struct LVar* findLvar(struct Token* tok);
struct Node* createNode(NodeKind kind,struct Node* lhs,struct Node* rhs);
struct Node* createNumNode(int val);
struct Node** program();
struct Node* stmt();
struct Node* expr();
struct Node* assign();
struct Node* equality();
struct Node* relational();
struct Node* add();
struct Node* mul();
struct Node* unary();
struct Node* primary();

//codegen.c
void gen(struct Node* node);
