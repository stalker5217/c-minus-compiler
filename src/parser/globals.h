#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define TRUE 1
#define FALSE 0

#define MAXRESERVED 6 // 예약어의 숫자
#define MAXTOKENLEN 30 

typedef enum _tokenType {
	ENDFILE, ERROR, // EOF, 에러
	IF, ELSE, INT, RETURN, VOID, WHILE, // keyword
	ID, NUM, //identifier, number
	PLUS, MINUS, TIMES, DIV, GT, AGT, LT, ALT, EQ, NEQ, ASSIGN, SEMI, COMMA, LPAREN, RPAREN, LBRACKET, RBRACKET, LBRACE, RBRACE // special symbols
} tokenType;

typedef struct _tokenRecord {
	tokenType tokenVal;
	char stringVal[MAXTOKENLEN+1];
} tokenRecord;

extern FILE *source;
extern FILE *listing;

extern int EchoSource;
extern int TraceScan;
extern int TraceParse;
extern int linenum;

#define MAXCHILDREN 3

typedef enum {DecK, StmtK, ExpK} NodeKind;
typedef enum {ScalarK, ArrayK, FunK} DecKind;
typedef enum {CallK, CompoundK, IfK, WhileK, ReturnK} StmtKind;
typedef enum {OpK, ConstK, IdK, AssignK} ExpKind;
typedef enum {Void, Integer} ExpType;

typedef struct treeNode{
	struct treeNode *child[MAXCHILDREN];
	struct treeNode *sibling;
	int lineno;
	
	NodeKind nodekind;
	
	union{
		DecKind dec;
		StmtKind stmt;
		ExpKind exp;
	} kind;
	
	tokenType op;
	int val;
	char *name;
	ExpType funcReturnType;
	ExpType varType;
	int isParameter;
} TreeNode;

extern int echoSource;
extern int traceScan;