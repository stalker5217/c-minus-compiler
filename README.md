# c-minus-compiler

C언어 문법을 간소화하여 C minus라고 칭하며 이를 해석하는 컴파일러를 구현한다.  

![image](https://user-images.githubusercontent.com/51525202/84584558-e321bb00-ae40-11ea-80a8-8a6cf88db86c.png)

<br/>

## Scanner  
code를 token이라고하는 의미있는 단위로 골라내는 어휘분석(lexical analysis)을 실시한다.

Example) 'a[idx] = 4 + 2' 에서 토큰은 8개.

token list  
- a
- [
- idx
- ]
- =
- 4
- \+
- 2

C Minus에서의 토큰 정의

|**keyword**|**special symbol**|**identifier**|**number**
|:--|:--|:--|:--|
|special symbol <br/> identifier <br/> if <br/> else <br/> int <br/> return <br/> void <br/> while|\+  (PLUS) <br/> - (MINUS) <br/> \*  (TIMES) <br/> / (DIV) <br/> <  (GT) <br/> <= (AGT) <br/> \>  (LT) <br/> >= (AGT) <br/> == (EQ)   <br/> != (NEQ) <br/> =  (ASSIGN) <br/> ; (SEMI) <br/> ,  (COMMA) <br/> (  (LPAREN) <br/> ) (RPAREN) <br/> [  (LBRACKET) <br/> ] (RPRACKET) <br/>{  (LBRACE) <br/> } (RBRACE)  <br/> /* */| ID = letter letter* <br/> letter = a\|..\|z\|A\|..\|Z | number = digit digit* <br/> digit = 0\|..\|9 |


<br/>

DFA

![scanner_dfa](https://user-images.githubusercontent.com/51525202/84584310-a0aaaf00-ae3d-11ea-8cc9-5f408c70f298.png)


## Parser 

Recursive descent parser를 만들기 위한 선행 작업: BNF를 EBNF로 변경  

|BNF|EBNF|
|:--|:--|
|program -> declaration_list||변경 없음|
|declaration_list -> declaration_list declaration \| declaration | declaration_list -> declaration {declaration} |
|declaration -> var_declaration \| fun_declaration |변경 없음|
|var_declaration -> type_specifier ID; \| type_specifier ID[NUM]; |var_declaration -> \|type_specifier ID[ [NUM] ]; |
|type_specifier -> int \|　void | 변경 없음 |
|fun_declaration -> type_specifier ID (param) compound_stmt | 변경 없음|
|params -> param_list \| void | 변경 없음|
|param_list -> param_list, param \| param | param_list -> param {, param} |
|param -> type_specifier ID \| type_specifier ID [] | param -> type_sepecifier ID [ [] ] |
|compound_stmt-> {local_declarations statement_list} | 변경 없음 |
|local_declarations -> local_declarations var_declaration \| empty |empty {var_declaration} |
|statement_list -> statement_list statement \| empty | empty {statement} |
|statement -> expression_stmt \| compund_stmt \| selection_stmt \| iteration_stmt \| return_stmt | 변경 없음 |
|expression_stmt -> expression ; \|； | expression_stmt -> [ expression ] ; |
|selection_stmt -> if (expression) statement \| if (expression) statement else statement | selection_stmt -> if (expression) statement [else statement]
|iteration_stmt -> while (expression)　statement|변경 없음|
|return_stmt -> return ; \| return expression ; | return [ expression ] ; |
|expression -> var = expression \| simple_expression | {var =} simple_expression |
|var -> ID | ID [expression] | var -> ID [ [expression] ] |
|simple_expression -> additive_expression relop additive_expression|변경 없음|
|relop -> <= \| < \| > \| >= \| == \| != | 변경 없음 |
|additive_expression -> additive_expression addop term \| term | additive_expression -> term {addop term} |
|addop -> + \| - | 변경 없음 |
|term -> term mulop factor \| factor|term -> factor {mulop factor}|
|mulop -> * \| / | 변경 없음 |
|factor -> (expression) \| var \| call \| NUM | 변경 없음 |
|call -> ID (args) | 변경 없음 |
|args -> arg-list \| empty | 변경 없음 |
|arg_list -> arg_list, expression \| expression | arg_list -> expression {, expression}|


- static TreeNode * declaration_list();  
program-> declaration_list로 최초 파싱을 시작하는 함수.

- static TreeNode * declaration();  
프로그램은 declaration의 연속된 집합이며, declaration 들은 sibling 관계로 연결되어있다. 각 declaration을 구분하여 변수 선언문인 var_declaration과 함수 선언문인 fun_declaration을 구분하여 선택 분기한다.

- static TreeNode * var_declaration();  
변수 선언문 부분으로 선언 된 변수의 type과 identifier를 attribute로 가진다. 또한 BRACKET을 인식하여 배열의 선언일 경우 크기 값 까지 attribute를 가진다.

- static TreeNode * param_list();   
함수 선언문의 매개 변수의 리스트를 나타내며 매개 변수가 없는 void형 또는 다수의 매개 변수가 sibling 관계로 연결 되어있다.

- static TreeNode * param();   
함수 선언문의 매개 변수를 나타내며 type과 identifier를 attribute를 가진다.

- static TreeNode * compound_stmt();    
함수의 Body 부분이며 함수 내의 지역변수들의 선언과 statement로 구성되어있다.

- static TreeNode * local_declarations();   
함수 내부의 지역변수들을 sibling 관계로 묶어 준다.

- static TreeNode * statement_list();   
함수 내부의 statement들의 집합을 나타낸다. 마찬가지로 각 statement들은 sibling 관계로 묶인다.

- static TreeNode * statement();   
statement는 if(선택), while(반복), return(반환) statement와 compound_stmt, 그리고 expression_stmt으로 나누어 진다.

- static TreeNode * expression_stmt();   
세미콜론 하나로 끝나는지 아니면 expression을 포함하는지를 결정 한다.

- static TreeNode * selection_stmt();   
조건 분기의 조건과 조건 만족 시 실행 될 내용, 또한 else문과 함께 나올 경우 조건을 만족하지 않을 시 실행 될 내용을 포함한다.

- static TreeNode * iteration_stmt();   
반복의 조건 내용과 반복의 내용을 결정한다.

- static TreeNode * return_stmt();   
반환의 종류를 결정한다.

- static TreeNode * expression();   
할당문으로 분기 할지,  아니면 simple_expression으로 분기 할지를 결정한다.

- static TreeNode * simple_expression(TreeNode *);   
TreeNode는 operator로 구성 되며 좌측 값을 의미하는 left child와 우측 값을 의미하는 right child로 구성 되어 있다.

- static TreeNode * additive_expression(TreeNode *);   
산술식을 나타나며 사칙연산의 순서와 구조를 나타낸다.

- static TreeNode * term(TreeNode *);   
덧셈과 뺄셈보다 우선 순위를 가지는 곱셈과 나눗셈을 위하여 텀을 주어 우선순위 연산을 가능하게 한다.

- static TreeNode * factor(TreeNode *);   
최우선 순위를 가지는 ( ) 로 묶여진 표현식이 우선순위를 가지게 할 수 있고, 단순한 경우에는 identifier나 숫자 값을 가진다.

- static TreeNode * args();   
함수 호출의 아규먼트를 나타낸다

- static TreeNode * arg_list();   
함수 호출 부분의 아규먼트 리스트를 나타낸다

- static TreeNode * identifier_stmt();   
statement 중 함수 호출을 하는 id(args_list) 구조를 구분하기 위한 함수
