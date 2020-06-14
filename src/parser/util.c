#include "globals.h"
#include "util.h"

//extern int linenum;

static int tab=0;

void printToken(tokenRecord token) {
	switch (token.tokenVal) {
	case IF:
	case ELSE:
	case INT:
	case RETURN:
	case VOID:
	case WHILE:
		fprintf(listing, "reserved word: %s\n", token.stringVal);
		break;
	case PLUS:
		fprintf(listing, "%s\n", token.stringVal);
		break;
	case MINUS:
		fprintf(listing, "%s\n", token.stringVal);
		break;
	case TIMES:
		fprintf(listing, "%s\n", token.stringVal);
		break;
	case DIV:
		fprintf(listing, "%s\n", token.stringVal);
		break;
	case GT:
		fprintf(listing, "%s\n", token.stringVal);
		break;
	case AGT:
		fprintf(listing, "%s\n", token.stringVal);
		break;
	case LT:
		fprintf(listing, "%s\n", token.stringVal);
		break;
	case ALT:
		fprintf(listing, "%s\n", token.stringVal);
		break;
	case EQ:
		fprintf(listing, "%s\n", token.stringVal);
		break;
	case NEQ:
		fprintf(listing, "%s\n", token.stringVal);
		break;
	case ASSIGN:
		fprintf(listing, "%s\n", token.stringVal);
		break;
	case SEMI:
		fprintf(listing, "%s\n", token.stringVal);
		break;
	case COMMA:
		fprintf(listing, "%s\n", token.stringVal);
		break;
	case LPAREN:
		fprintf(listing, "%s\n", token.stringVal);
		break;
	case RPAREN:
		fprintf(listing, "%s\n", token.stringVal);
		break;
	case LBRACKET:
		fprintf(listing, "%s\n", token.stringVal);
		break;
	case RBRACKET:
		fprintf(listing, "%s\n", token.stringVal);
		break;
	case LBRACE:
		fprintf(listing, "%s\n", token.stringVal);
		break;
	case RBRACE:
		fprintf(listing, "%s\n", token.stringVal);
		break;
	case ENDFILE:
		fprintf(listing, "EOF\n");
		break;
	case NUM:
		fprintf(listing, "NUM, value = %s\n", token.stringVal);
		break;
	case ID:
		fprintf(listing, "ID, name = %s\n", token.stringVal);
		break;
	case ERROR:
		fprintf(listing, "error: %s\n", token.stringVal);
		break;
	default:
		fprintf(listing, "Unknown token: %d\n", token.tokenVal);
	}
}

char * copyString(char s[]){
	int n;
	char *t;

	if(s==NULL)
		return NULL;

	n = strlen(s) + 1;
	t = (char*)malloc(n * sizeof(char));
	if(t == NULL)
		fprintf(listing, "Out of memory error at line %d\n", linenum);
	else
		strcpy(t,s);
	return t;
}

TreeNode * newDecNode(DecKind kind){
	TreeNode * t = (TreeNode*) malloc(sizeof(TreeNode));
	int i;
	if(t == NULL)
		fprintf(listing, "Out of Memory error at line %d\n", linenum);
	else{
		for(i=0 ; i < MAXCHILDREN ; i++)
			t->child[i] = NULL;
		t->sibling = NULL;
		t->lineno = linenum;
		t->isParameter = FALSE;
		t->nodekind = DecK;
		t->kind.dec = kind;
	}
	return t;
}

TreeNode * newExpNode(ExpKind kind){
	TreeNode * t = (TreeNode*) malloc(sizeof(TreeNode));
	int i;
	if(t == NULL)
		fprintf(listing, "Out of Memory error at line %d\n", linenum);
	else{
		for(i=0 ; i < MAXCHILDREN ; i++)
			t->child[i] = NULL;
		t->sibling = NULL;
		t->lineno = linenum;
		t->isParameter = FALSE;
		t->nodekind = ExpK;
		t->kind.exp = kind;
	}

	return t;
}

TreeNode * newStmtNode(StmtKind kind){
	TreeNode * t = (TreeNode*) malloc(sizeof(TreeNode));
	int i;
	if(t == NULL)
		fprintf(listing, "Out of Memory error at line %d\n", linenum);
	else{
		for(i=0 ; i < MAXCHILDREN ; i++)
			t->child[i] = NULL;
		t->sibling = NULL;
		t->lineno = linenum;
		t->isParameter = FALSE;
		t->nodekind = StmtK;
		t->kind.stmt = kind;
	}
	return t;
}

static void printTab(){
	int i;
	for(i=0;i<tab;i++)
		fprintf(listing, "\t");
}

char *printType(ExpType type){
	char *i = "Integer";
	char *v = "Void";
	char *invalid = "error";

	switch(type){
	case Integer:
		return i;
	case Void:
		return v;
	default:
		return invalid;
	}
}

void printOp(TreeNode *t){
	switch(t->op){
	case PLUS:
		fprintf(listing, "+\n");
		break;
	case MINUS:
		fprintf(listing, "-\n");
		break;
	case TIMES:
		fprintf(listing, "*\n");
		break;
	case DIV:
		fprintf(listing, "/\n");
		break;
	case GT:
		fprintf(listing, ">\n");
		break;
	case AGT:
		fprintf(listing, ">=\n");
		break;
	case LT:
		fprintf(listing, "<\n");
		break;
	case ALT:
		fprintf(listing, "<=\n");
		break;
	case EQ:
		fprintf(listing, "==\n");
		break;
	case NEQ:
		fprintf(listing, "!=\n");
		break;
	default:
		fprintf(listing, "Unknown op\n");
		break;
	}
}

void printTree(TreeNode *t){
	int i;
	tab++;
	while(t != NULL){
		printTab();

		if(t->nodekind == DecK){
			switch(t->kind.dec){
			case ScalarK:
				fprintf(listing, "Declaration(Scalar)\n");
				printTab();
				fprintf(listing, "type = %s, ID = %s\n", printType(t->varType), t->name);
				break;
			case ArrayK:
				fprintf(listing, "Declaration(Array)\n");
				printTab();
				fprintf(listing, "type = %s, ID = %s", printType(t->varType), t->name);
				if(t->val > 0)
					fprintf(listing, ", size = %d\n", t->val);
				else
					fprintf(listing, "\n");
				break;
			case FunK:
				fprintf(listing, "Declaration(Function)\n");
				printTab();
				fprintf(listing, "ID = %s, return type = %s\n", t->name, printType(t->funcReturnType));
				break;
			default:
				fprintf(listing, "Unknown DecNode\n");
				break;
			}
		}

		else if(t->nodekind == ExpK){
			switch(t->kind.exp){
			case OpK:
				fprintf(listing, "OP : ");
				printOp(t);
				break;
			case IdK:
				fprintf(listing, "ID : %s", t->name);
				if(t->val > 0)
					fprintf(listing, "[%d]\n", t->val);
				else
					fprintf(listing, "\n");
				break;
			case ConstK:
				fprintf(listing, "Const : %d\n", t->val);
				break;
			case AssignK:
				fprintf(listing, "Assign\n");
				break;
			default:
				fprintf(listing, "unknown ExpNode\n");
				break;
			}
		}
		else if(t->nodekind == StmtK){
			switch(t->kind.stmt){
			case CompoundK:
				fprintf(listing, "Compound stmt\n");
				break;
			case IfK:
				fprintf(listing, "Selection stmt\n");
				break;
			case WhileK:
				fprintf(listing, "Iteration stmt\n");
				break;
			case ReturnK:
				fprintf(listing, "Retrun stmt\n");
				break;
			case CallK:
				fprintf(listing, "Call function: %s\n", t->name);
				break;
			default:
				fprintf(listing, "unknown StmtNode\n");
				break;
			}
		}
		else
			fprintf(listing, "Unknown node\n");

		for(i = 0 ; i < MAXCHILDREN; i++)
			printTree(t->child[i]);

		t = t->sibling;
	}

	tab--;
}