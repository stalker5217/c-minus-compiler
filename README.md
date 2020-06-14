# c-minus-compiler

C언어 문법을 간소화하여 C minus라고 칭하며 이를 해석하는 컴파일러를 구현한다.

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

![scanner_dfa](https://user-images.githubusercontent.com/51525202/84584310-a0aaaf00-ae3d-11ea-8cc9-5f408c70f298.png)
