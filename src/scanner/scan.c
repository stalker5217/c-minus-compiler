#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define TRUE 1
#define FALSE 0
#define MAXTOKENLEN 30 // 토큰의 최대 크기
#define BUFLEN 512 // 버퍼의 최대크기
#define MAXRESERVED 6 // 예약어의 숫자

// DFA의 상태
typedef enum _stateType {
	START, INNUM, INID, INAGT, INALT, INNEQ, INEQ, ENTRYCOMMENT, INCOMMENT, EXITCOMMENT, DONE
} stateType;

// token의 종류
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

// 함수 원형
char getNextChar();
void ungetNextChar();
tokenRecord getToken();
tokenType isKeyword(char *);
void printToken(tokenRecord);


// 예약어, keyword 구조체 배열
static struct {
	char * str;
	tokenType tok;
} reservedWords[MAXRESERVED]
= { { "if", IF },{ "else", ELSE },{ "int", INT },{ "return", RETURN },{ "void", VOID },{ "while", WHILE } };

FILE* source; // 소스 파일
FILE* listing; // 출력 파일

int echoSource = TRUE;
int traceScan = TRUE;
int linenum; // 줄 번호

static stateType state; // 현재 token의 상태

static char lineBuf[BUFLEN];
static int linepos = 0; // lineBuf에서 현재 위치
static int bufsize = 0; // 버퍼의 현재 크기


int main(int argc, char * argv[]) {

	if (argc != 3) {
		fprintf(stderr, "usage: scan.exe [input_filename] [output_filename]\n");
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

	while (getToken().tokenVal != ENDFILE);

	return 0;
}

// 토큰 만들기
tokenRecord getToken() {
	int index = 0;
	state = START;
	tokenRecord curRecord;

	int save;

	while (state != DONE) {
		char c;
		save = TRUE;
		c = getNextChar();

		switch (state) {
		case START:
			if (isdigit(c))
				state = INNUM;
			else if (isalpha(c))
				state = INID;
			else if (c == '<')
				state = INALT;
			else if (c == '>')
				state = INAGT;
			else if (c == '=')
				state = INEQ;
			else if (c == '!')
				state = INNEQ;
			else if (c == '/') {
				state = ENTRYCOMMENT;
			}
			else if (c == ' ' || c == '\t' || c == '\n')
				save = FALSE;
			else {
				state = DONE;

				switch (c) {
				case EOF:
					save = FALSE;
					curRecord.tokenVal = ENDFILE;
					break;
				case '+':
					curRecord.tokenVal = PLUS;
					break;
				case '-':
					curRecord.tokenVal = MINUS;
					break;
				case '*':
					curRecord.tokenVal = TIMES;
					break;
				case ';':
					curRecord.tokenVal = SEMI;
					break;
				case ',':
					curRecord.tokenVal = COMMA;
					break;
				case '(':
					curRecord.tokenVal = LPAREN;
					break;
				case ')':
					curRecord.tokenVal = RPAREN;
					break;
				case '[':
					curRecord.tokenVal = LBRACKET;
					break;
				case ']':
					curRecord.tokenVal = RBRACKET;
					break;
				case '{':
					curRecord.tokenVal = LBRACE;
					break;
				case '}':
					curRecord.tokenVal = RBRACE;
					break;
				default:
					curRecord.tokenVal = ERROR;
					break;
				}
			}
			break;

		case INNUM:
			if (!isdigit(c)) {
				ungetNextChar(); // lookahead
				save = FALSE;
				state = DONE;
				curRecord.tokenVal = NUM;
			}
			break;

		case INID:
			if (!isalpha(c)) {
				ungetNextChar();
				save = FALSE;
				state = DONE;
				curRecord.tokenVal = ID;
			}
			break;

		case ENTRYCOMMENT:
			if (c == '*') {
				save = FALSE;
				state = INCOMMENT;
			}
			else {
				ungetNextChar();
				save = FALSE;
				state = DONE;
				curRecord.tokenVal = DIV;
			}
			break;

		case INCOMMENT:
			save = FALSE;
			if (c == '*')
				state = EXITCOMMENT;
			break;

		case EXITCOMMENT:
			save = FALSE;
			if (c == '/') {
				index = 0;
				(curRecord.stringVal)[index] = 0;
				state = START;
			}
			else if (c == '*')
				state = EXITCOMMENT;
			else
				state = INCOMMENT;
			break;

		case INALT:
			state = DONE;
			if (c == '=')
				curRecord.tokenVal = ALT;
			else {
				ungetNextChar();
				save = FALSE;
				curRecord.tokenVal = LT;
			}
			break;

		case INAGT:
			state = DONE;
			if (c == '=')
				curRecord.tokenVal = AGT;
			else {
				ungetNextChar();
				save = FALSE;
				curRecord.tokenVal = GT;
			}
			break;

		case INEQ:
			state = DONE;
			if (c == '=')
				curRecord.tokenVal = EQ;
			else {
				ungetNextChar();
				save = FALSE;
				curRecord.tokenVal = ASSIGN;
			}
			break;

		case INNEQ:
			state = DONE;
			if (c == '=')
				curRecord.tokenVal = NEQ;
			else {
				ungetNextChar();
				save = FALSE;
				curRecord.tokenVal = ERROR;
			}
			break;

		case DONE:
			break;
		default:
			fprintf(listing, "Scanner Bug: state = %d\n", state);
			state = DONE;
			curRecord.tokenVal = ERROR;
			break;
		}

		if ((save) && (index <= MAXTOKENLEN))
			curRecord.stringVal[index++] = c;

		if (state == DONE) {
			curRecord.stringVal[index] = 0;
			if (curRecord.tokenVal == ID)
				curRecord.tokenVal = isKeyword(curRecord.stringVal);
		}
	}

	if (traceScan) {
		fprintf(listing, "\t%d: ", linenum);
		printToken(curRecord);
	}

	return curRecord;
}

// char 하나 가져오기
char getNextChar() {
	if ((linepos < bufsize) == 0) {
		if (fgets(lineBuf, BUFLEN - 1, source)) {
			linenum++;
			if (echoSource)
				fprintf(listing, "%4d: %s\n", linenum, lineBuf);

			bufsize = strlen(lineBuf);
			linepos = 0;
			return lineBuf[linepos++];
		}

		else {
			linepos++;
			if (state == START)
				linenum++;
			return EOF;
		}
	}

	else
		return lineBuf[linepos++];
}

// lookahead, 입력 버퍼에서 소모시키지 않음
void ungetNextChar() {
	linepos--;
}

// 입력 된 문자가 키워드 인지 검사
tokenType isKeyword(char *s) {
	int i;

	for (i = 0; i < MAXRESERVED; i++)
		if (!strcmp(s, reservedWords[i].str))
			return reservedWords[i].tok;

	return ID;
}

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