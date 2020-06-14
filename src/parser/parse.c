#include "globals.h"
#include "scan.h"
#include "util.h"
#include "parse.h"

FILE* listing;
FILE* source;

static tokenRecord token;

int Error = FALSE;
int echoSource = FALSE;
int traceParse = TRUE;
extern int linenum;
static int startError;

static void syntaxError(char *);
static int match(tokenType);

static TreeNode * declaration_list(); //o
static TreeNode * declaration(); //o
static TreeNode * var_declaration(); //o
//static TreeNode * fun_declaration();
//static TreeNode * params();
static TreeNode * param_list();
static TreeNode * param(); //o
static TreeNode * compound_stmt();
static TreeNode * local_declarations();
static TreeNode * statement_list(); //
static TreeNode * statement(); //
static TreeNode * expression_stmt(); //
static TreeNode * selection_stmt(); //
static TreeNode * iteration_stmt(); //
static TreeNode * return_stmt(); //
static TreeNode * expression();
//static TreeNode * var(); //
static TreeNode * simple_expression(TreeNode *); //
static TreeNode * additive_expression(TreeNode *); //
static TreeNode * term(TreeNode *); //
static TreeNode * factor(TreeNode *); //
//static TreeNode * call();
static TreeNode * args(); //
static TreeNode * arg_list(); //
static TreeNode * identifier_stmt();

int main(int argc, char * argv[]) {
	TreeNode * syntaxTree;

	if (argc != 3) {
		fprintf(stderr, "usage: parse.exe [input_filename] [output_filename]\n");
		exit(1);
	}

	source = fopen(argv[1], "rt");

	if (source == NULL) {
		fprintf(stderr, "cannot open the file(%s)\n", argv[1]);
		exit(1);
	}

	listing = fopen(argv[2], "wt");
	if (listing == NULL) {
		fprintf(stderr, "cannot write the file(%s)\n", argv[2]);
	}

	syntaxTree = parse();
	if(traceParse){
		fprintf(listing, "Syntax tree:\n");
		printTree(syntaxTree);
	}

	fclose(source);
	fclose(listing);
	return 0;
}

static void syntaxError(char * message){ 
	fprintf(listing,"\n>>> ");
	fprintf(listing,"Syntax error at line %d: %s",linenum, message);
	Error = TRUE;
}

static int match(tokenType expected){
	if(token.tokenVal == expected){
		token = getToken();
		return FALSE;
	}
	else{
		syntaxError("unexpected token -> ");
		printToken(token);
		fprintf(listing, "	");
		return TRUE;
	}
}

static ExpType matchType(){
	ExpType type = Void;

	switch(token.tokenVal){
	case INT:
		type = Integer;
		token = getToken();
		startError = FALSE;
		break;
	case VOID:
		type = Void;
		token = getToken();
		startError = FALSE;
		break;
	default:
		syntaxError("unexpected token -> ");
		printToken(token);
		startError = TRUE;
		break;
	}

	return type;
}

static TreeNode * declaration_list(){
	TreeNode * t;
	TreeNode * p;
	
	//puts("declaration_list() execute");
	t = declaration();
	p = t;

	while((token.tokenVal != ENDFILE) && (token.tokenVal != ELSE)){
		TreeNode * q;
		q = declaration();

		if((q != NULL) && (p != NULL)){
			if(t==NULL)
				t = p = q;
			else{
				p->sibling = q;
				p = q;
			}
		}
	}
	return t;
}

static TreeNode * declaration(){
	TreeNode * t = NULL;
	ExpType decType = Void;
	char *id;

	//puts("declaration() execute");
	decType = matchType();

	if(startError == TRUE){
		while(1){
			token = getToken();
			decType = matchType();
			if(token.tokenVal == ENDFILE || startError == FALSE)
				break;
		}
	}

	id = copyString(token.stringVal);
	match(ID);

	switch(token.tokenVal){
	case SEMI:
		t = newDecNode(ScalarK);

		if(t != NULL){
			t->varType = decType;
			t->name = id;
		}

		match(SEMI);
		break;

	case LBRACKET:
		t = newDecNode(ArrayK);

		if(t != NULL){
			t->varType = decType;
			t->name = id;
		}

		match(LBRACKET);

		if(t != NULL)
			t->val = atoi(token.stringVal);

		match(NUM);
		match(RBRACKET);
		match(SEMI);

		break;

	case LPAREN:
		t = newDecNode(FunK);

		if(t != NULL){
			t->funcReturnType = decType;
			t->name = id;
		}

		match(LPAREN);

		if(t != NULL)
			t->child[0] = param_list();
		match(RPAREN);
		if(t != NULL)
			t->child[1] = compound_stmt();
		break;

	default:
		syntaxError("unexpected token -> ");
		printToken(token);
		token = getToken();
		break;
	}

	return t;
}

static TreeNode * var_declaration(){
	TreeNode * t = NULL;
	ExpType decType;
	char *id;
	//puts("var_declaration() execute");
	
	decType = matchType();

	id = copyString(token.stringVal);
	match(ID);

	switch(token.tokenVal){
	case SEMI:
		t = newDecNode(ScalarK);

		if(t != NULL){
			t->varType = decType;
			t->name = id;
		}

		match(SEMI);
		break;

	case LBRACKET:
		t = newDecNode(ArrayK);

		if(t != NULL){
			t->varType = decType;
			t->name = id;
		}

		match(LBRACKET);

		if(t != NULL)
			t->val = atoi(token.stringVal);

		match(NUM);
		match(RBRACKET);
		match(SEMI);
		break;

	default:
		syntaxError("unexpected token -> ");
		printToken(token);
		token = getToken();
		break;
	}

	return t;
}

static TreeNode * param_list(){
	TreeNode *t;
	TreeNode *p;
	TreeNode *next;

	//puts("param_list() execute");
	if(token.tokenVal == VOID){
		match(VOID);
		return NULL;
	}

	t = param();
	p = t;

	while((t!=NULL) && (token.tokenVal == COMMA)){
		match(COMMA);
		next = param();
		if(next != NULL){
			p->sibling = next;
			p = next;
		}
	}

	return t;
}

static TreeNode * param(){
	TreeNode *t;
	ExpType type;
	char *id;

	//puts("param() execute");
	type = matchType();

	id = copyString(token.stringVal);
	match(ID);

	if(token.tokenVal == LBRACKET){
		match(LBRACKET);
		match(RBRACKET);
		t = newDecNode(ArrayK);
	}

	else{
		t = newDecNode(ScalarK);
	}

	if(t!=NULL){
		t->name = id;
		t->varType = type;
		t->isParameter = TRUE;
	}

	return t;
}

static TreeNode * compound_stmt(){
	TreeNode * t = NULL;

	//puts("compound_stmt() execute");
	match(LBRACE);

	if(token.tokenVal != RBRACE){
		t = newStmtNode(CompoundK);
		
		if(token.tokenVal == INT || token.tokenVal == VOID)
			t->child[0] = local_declarations();
		if(token.tokenVal != RBRACE)
			t->child[1] = statement_list();
	}

	match(RBRACE);

	return t;
}

static TreeNode * local_declarations(){
	TreeNode *t;
	TreeNode *p;
	TreeNode *next;

	//puts("local_declaration() execute");
	if(token.tokenVal == INT || token.tokenVal == VOID)
		t = var_declaration();

	if(t != NULL){
		p = t;

		while(token.tokenVal == INT || token.tokenVal == VOID){
			next = var_declaration();
			if(next != NULL){
				p->sibling = next;
				p = next;
			}
		}
	}

	return t;
}

static TreeNode * statement_list(){
	TreeNode * t;
	TreeNode * p;
	TreeNode * next;

	//puts("statement_list() execute");
	if(token.tokenVal != RBRACE){
		t = statement();
		p = t;

		while(token.tokenVal != ENDFILE && token.tokenVal != RBRACE){
			next = statement();
			if((p!=NULL) && (next!=NULL)){
				p->sibling = next;
				p = next;
			}
		}
	}

	return t;
}



static TreeNode * statement(){
	TreeNode * t = NULL;

	//puts("statement() execute");
	switch(token.tokenVal){
	case IF:
		t = selection_stmt();
		break;
	case WHILE:
		t = iteration_stmt();
		break;
	case RETURN:
		t = return_stmt();
		break;
	case LBRACE:
		t = compound_stmt();
		break;
	case SEMI:
	case ID:
	case LPAREN:
	case NUM:
		t = expression_stmt();
		break;
	default:
		syntaxError("unexpected token -> ");
		printToken(token);
		token = getToken();
		break;
	}

	return t;
}


static TreeNode * expression_stmt(){
	TreeNode * t = NULL;

	//puts("expression_stmt() execute");
	if(token.tokenVal == SEMI)
		match(SEMI);
	else if(token.tokenVal != RBRACE){
 		t = expression();
		match(SEMI);
	}

	return t;
}

static TreeNode * selection_stmt(){
	TreeNode *t;
	TreeNode *exp;
	TreeNode *ifStmt;
	TreeNode *elseStmt = NULL;

	//puts("selection_stmt() execute");
	match(IF);
	match(LPAREN);
	exp = expression();
	match(RPAREN);
	ifStmt = statement();

	if(token.tokenVal == ELSE){
		match(ELSE);
		elseStmt = statement();
	}

	t = newStmtNode(IfK);

	if(t != NULL){
		t->child[0] = exp;
		t->child[1] = ifStmt;
		t->child[2] = elseStmt;
	}

	return t;
}

static TreeNode * iteration_stmt(){
	TreeNode *t;
	TreeNode *cond;
	TreeNode *stmt;

	//puts("iteration_stmt() execute");
	match(WHILE);
	match(LPAREN);
	cond = expression();
	match(RPAREN);
	stmt = statement();

	t = newStmtNode(WhileK);
	if(t != NULL){
		t->child[0] = cond;
		t->child[1] = stmt;
	}

	return t;
}

static TreeNode * return_stmt(){
	TreeNode *t;
	TreeNode *exp;

	//puts("return_stmt() execute");
	match(RETURN);

	t = newStmtNode(ReturnK);

	if(token.tokenVal == SEMI)
		exp = NULL;
	else
		exp = expression();

	if(t != NULL)
		t->child[0] = exp;

	match(SEMI);

	return t;
}

static TreeNode * expression(){
	TreeNode * t = NULL;
	TreeNode * l = NULL;
	TreeNode * r = NULL;
	int getL = FALSE;

	//puts("expression() execute");
	if(token.tokenVal == ID){
		l = identifier_stmt();
		getL = TRUE;
	}

	//할당문일 경우
	if((getL == TRUE) && (token.tokenVal == ASSIGN)){
		if(l != NULL && l->nodekind == ExpK && l->kind.exp == IdK)
		{
			match(ASSIGN);
			r = expression();

			t = newExpNode(AssignK);
			if(t != NULL){
				t->child[0] = l;
				t->child[1] = r;
			}
		}
		else{
			syntaxError("unexpected token -> ");
			printToken(token);
			token = getToken();
		}
	}

	else
		t = simple_expression(l);

	return t;
}

static TreeNode * identifier_stmt(){
	TreeNode *t;
	TreeNode *exp = NULL;
	TreeNode *argus = NULL;
	char *id;

	//puts("identifier_stmt() execute");
	if(token.tokenVal == ID)
		id = copyString(token.stringVal);
	match(ID);

	if(token.tokenVal == LPAREN){
		match(LPAREN);
		argus = args();
		match(RPAREN);

		t = newStmtNode(CallK);

		if(t != NULL){
			t->child[0] = argus;
			t->name = id;
		}
	}

	else{
		if(token.tokenVal == LBRACKET){
			match(LBRACKET);
			exp = expression();
			match(RBRACKET);
		}

		t = newExpNode(IdK);
		if(t != NULL){
			t->child[0] = exp;
			t->name = id;
		}
	}
	return t;
}

static TreeNode * simple_expression(TreeNode *p){
	TreeNode * t;
	TreeNode * l = NULL;
	TreeNode * r = NULL;
	tokenType op;

	//puts("simple_expression(TreeNode*) execute");
	l = additive_expression(p);

	if((token.tokenVal == LT) || (token.tokenVal == ALT) || (token.tokenVal == GT) || (token.tokenVal == AGT)
		|| (token.tokenVal == EQ) || (token.tokenVal == NEQ))
	{
		op = token.tokenVal;
		match(token.tokenVal);
		r = additive_expression(NULL);

		t = newExpNode(OpK);
		if(t != NULL){
			t->child[0] = l;
			t->child[1] = r;
			t->op = op;
		}
	}
	else
		t = l;

	return t;
}

static TreeNode * additive_expression(TreeNode *p){
	TreeNode *t;
	TreeNode *next;

	//puts("additive_expression(TreeNode*) execute");
	t = term(p);

	while((token.tokenVal == PLUS) || (token.tokenVal) == MINUS){
		next = newExpNode(OpK);
		if(next != NULL){
			next->child[0] = t;
			next->op = token.tokenVal;
			t = next;
			match(token.tokenVal);
			t->child[1] = term(NULL);
		}
	}

	return t;
}

static TreeNode *term(TreeNode *p){
	TreeNode *t;
	TreeNode *next;

	//puts("term(TreeNode*) execute");
	t = factor(p);

	while ((token.tokenVal == TIMES) || (token.tokenVal == DIV)){
		next = newExpNode(OpK);

		if(next != NULL){
			next->child[0] = t;
			next->op = token.tokenVal;
			t= next;
			match(token.tokenVal);
			next->child[1] = factor(NULL);
		}
	}

	return t;
}

static TreeNode * factor(TreeNode *p){
	TreeNode * t = NULL;

	//puts("factor(TreeNode*) execute");
	if(p != NULL)
		return p;

	switch(token.tokenVal){
	case ID:
		t = identifier_stmt();
		break;
	case LPAREN:
		match(LPAREN);
		t = expression();
		match(RPAREN);
		break;
	case NUM:
		t = newExpNode(ConstK);
		if(t != NULL){
			t->val = atoi(token.stringVal);
			t->varType = Integer;
		}
		match(NUM);
		break;
	default:
		syntaxError("unexpected token -> ");
		printToken(token);
		token = getToken();
	}
	return t;
}

static TreeNode * args(){
	TreeNode *t = NULL;

	//puts("args() execute");
	if(token.tokenVal != RPAREN)
		t = arg_list();

	return t;
}

static TreeNode * arg_list(){
	TreeNode * t;
	TreeNode * p;
	TreeNode * next;

	//puts("arg_list() execute");
	t = expression();
	p = t;

	while(token.tokenVal == COMMA){
		match(COMMA);
		next = expression();

		if(p != NULL && t != NULL){
			p->sibling = next;
			p = next;
		}
	}
	return t;
}

TreeNode * parse(void){
	TreeNode * t;
	token = getToken();
	//puts("parse() execute");
	t = declaration_list();
	if(token.tokenVal != ENDFILE){
		syntaxError("Code ends before file\n");
	}
	return t;
}