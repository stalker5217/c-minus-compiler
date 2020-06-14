#pragma once

void printToken(tokenRecord);
char * copyString(char *);
TreeNode * newDecNode(DecKind);
TreeNode * newStmtNode(StmtKind);
TreeNode * newExpNode(ExpKind);
void printTree(TreeNode*);