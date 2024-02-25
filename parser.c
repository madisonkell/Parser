// **********************************************
// Program3: Parser
// Course: CS451
// Student Name: Madison Kell
// Instructor: Il-Hyung Cho
// Date of submission: 3/16/23
// Program Description: 
//  -a lexical analyzer system for simple arithmetic expressions
//  -adapted to support floating points, if, while, for, and else stmts

// **********************************************
/**
o [X] My program compiles without error on the csis machine.
o [X] The lexical analyzer successfully recognizes floating point number.
o [X] The lexical analyzer successfully recognizes all the new key words if, while, and else.
o [X] The lexical analyzer successfully recognizes the comparison operators as new tokens.
o [X] My program prints the proper messages for newly added tokens and keywords
o [x] My program implements functions for each non-terminals
o [X] I prepared for a typescript file (hw3-lastname.txt) that includes the compilation,
source code listing, and the run result for all four input files.
 * */ 

/* parser.c - a lexical analyzer system for simple arithmetic expressions */

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

int charClass;
char lexeme [100];
char nextChar;
int lexLen;
int token;
int nextToken;
FILE *in_fp, *fopen();

void addChar();
void getChar();
void getNonBlank();
int lex();

void checkFloat();
void error();

void stmts();
void stmt();
void ifstmt();
void whilestmt();
void forstmt();
void asgnstmt();
void expr();
void term();
void factor();
void boolexpr();
void comp();

/* Character classes */
#define LETTER 0
#define DIGIT 1
#define UNKNOWN 99

/* Token codes */
#define INT_LIT 10
#define IDENT 11
#define FLOAT_CONSTANT 12
#define ASSIGN_OP 20
#define ADD_OP 21
#define SUB_OP 22
#define MULT_OP 23
#define DIV_OP 24
#define LEFT_PAREN 25
#define RIGHT_PAREN 26
#define GREATER_THAN 27
#define LESS_THAN 28
#define SEMICOLON 29
#define DECIMAL 30
#define LEFT_CURL 31
#define RIGHT_CURL 32

#define IF_STMT 40
#define WHILE_STMT 41
#define FOR_STMT 42
#define ELSE_STMT 43

/******************************************************/
/* main driver */
int main() {
    int i;
    // array to store all the file names
    char files[5][11] = {"input1.txt", "input2.txt", "input3.txt", "input4.txt", "input5.txt"};
    // loop through all the files
    for(i=0; i<5; i++){
    printf("=========%s=========\n",files[i]);
    /* Open the input data file and process its contents */
    if ((in_fp = fopen(files[i], "r")) == NULL){
    printf("ERROR - cannot open %s \n", files[i]);
    } else {
        getChar();
        do {
            lex();
            stmts();
        } while (nextToken != EOF);
        fclose(in_fp);
    }
     }

    return 0;
}

/*****************************************************/
/* lookup - a function to lookup operators and parentheses
 * and return the token */
int lookup(char ch) {
    switch (ch) {
        case '(':
            addChar();
            nextToken = LEFT_PAREN;
            break;
        case ')':
            addChar();
            nextToken = RIGHT_PAREN;
            break;
        case '+':
            addChar();
            nextToken = ADD_OP;
            break;
        case '-':
            addChar();
            nextToken = SUB_OP;
            break;
        case '*':
            addChar();
            nextToken = MULT_OP;
            break;
        case '/':
            addChar();
            nextToken = DIV_OP;
            break;
        // all of the added tokens
        case '>':
            addChar();
            nextToken = GREATER_THAN;
            break;
        case '<':
            addChar();
            nextToken = LESS_THAN;
            break;
        case ';':
            addChar();
            nextToken = SEMICOLON;
            break;
        case '=':
            addChar();
            nextToken = ASSIGN_OP;
            break;
        case '.':
            addChar();
            nextToken = DECIMAL;
            break;
        case '{':
            addChar();
            nextToken = LEFT_CURL;
            break;
        case '}':
            addChar();
            nextToken = RIGHT_CURL;
            break;

        default:
            addChar();
        nextToken = EOF;
        break;
    }
    return nextToken;
}
/*****************************************************/
/* addChar - a function to add nextChar to lexeme */
void addChar() {
    if (lexLen <= 98) {
        lexeme[lexLen++] = nextChar;
        //printf("lexeme array: %c\n", nextChar);
        lexeme[lexLen] = 0;
    
    }
    else
    printf("Error - lexeme is too long \n");
}
/*****************************************************/
/* getChar - a function to get the next character of
 * input and determine its character class */
void getChar() {
    if ((nextChar = getc(in_fp)) != EOF) {
        if (isalpha(nextChar))
        charClass = LETTER;
        else if (isdigit(nextChar))
        charClass = DIGIT;
        else charClass = UNKNOWN;
    }
    else
    charClass = EOF;
}
/*****************************************************/
/* getNonBlank - a function to call getChar until it
 * returns a non-whitespace character */
void getNonBlank() {
    while (isspace(nextChar))
    getChar();
}

/*****************************************************/
/* lex - a simple lexical analyzer for arithmetic expressions */
int lex() {
    lexLen = 0;
    getNonBlank();
    switch (charClass) {
        /* Parse identifiers */
        case LETTER:
            addChar();
            getChar();
            while (charClass == LETTER || charClass == DIGIT) {
                addChar();
                getChar();
            }
        // check if the lexeme array holds a token for if, while, for or else
        if(strcmp(lexeme, "if") == 0){
            nextToken = IF_STMT;
            break;
        }else if(strcmp(lexeme, "while") == 0){
            nextToken = WHILE_STMT;
            break;
        }else if(strcmp(lexeme, "for") == 0){
            nextToken = FOR_STMT;
            break;
        }else if(strcmp(lexeme, "else") == 0){
            nextToken = ELSE_STMT;
             break;
        }else{
        // if its not any of those tokens, it is an identifer
        nextToken = IDENT;
        }
        break;
        
        /* Parse integer literals */
        case DIGIT:
            addChar();
            getChar();
            while (charClass == DIGIT) {
                addChar();
                getChar();
            }
            // check to see if the next character was a ".", if it was see if the input should be a float
            if(strcmp(&nextChar, ".")== 0)
            checkFloat();
                if(nextToken == FLOAT_CONSTANT)
                    break;
            // if its not a float, its an int
            nextToken = INT_LIT;
            break;
        /* Parentheses and operators */
        case UNKNOWN:
            // check to see if its a double, because its possible for a float to be ".*"
            checkFloat();
            if(nextToken == FLOAT_CONSTANT)
                break;
    
            getChar();
            break;
        /* EOF */
        case EOF:
            nextToken = EOF;
            lexeme[0] = 'E';
            lexeme[1] = 'O';
            lexeme[2] = 'F';
            lexeme[3] = 0;
        break;
    } /* End of switch */

    printf("Next token is: %d, Next lexeme is %s \n",
    nextToken, lexeme);
    return nextToken;
} /* End of function lex */

// function to see if the number entered is a float
// if there is a number followed by a "."
// if theres a "." followed by a number
// or a number followed by a "." followed by a number
void checkFloat(){
    if(charClass == UNKNOWN){
        if(lookup(nextChar) == DECIMAL){
                getChar();
            while (charClass == DIGIT) {
                addChar();
                getChar();
            }   
            nextToken = FLOAT_CONSTANT;
        }
    }
}

//<stmts> → <stmt> {; <stmt> }
void stmts(){
    printf("Enter <stmts>\n");
    stmt();
    if (nextToken == SEMICOLON){
        lex();
        stmt();
    }
    printf("Exit <stmts>\n");
}

// <stmt> → <ifstmt> | <whilestmt> | <forstmt> | <asgnstmt>
void stmt(){
     printf("Enter <stmt>\n");
    if(nextToken == IF_STMT)
        ifstmt();
    if(nextToken == WHILE_STMT)
        whilestmt();
    if(nextToken == FOR_STMT)
        forstmt();
    if(nextToken == IDENT){
        asgnstmt();
    }
    printf("Exit <stmt>\n");
}
// <ifstmt> → if (<boolexpr>) ‘{‘ <stmts> ‘}’ [ else ‘{‘ <stmts> ‘}’ ]
void ifstmt(){
    printf("Enter <ifstmt>\n");
    if(nextToken == IF_STMT){
        lex();
        //make sure next character is (
        if(nextToken == LEFT_PAREN){
            boolexpr();
            lex();
            stmt();
            //make sure next character is )
            if(nextToken == RIGHT_PAREN){
                lex();
                stmt();
                //check if the if has an else
                if(nextToken == ELSE_STMT){
                    stmt();
            }   else{}
                // There is no else
            }
            else
                 error();
        }
        else
             error();
    }
    printf("Exit <ifstmt>\n");
}

//<boolexpr> → id <comp> int_constant
void boolexpr(){
    printf("Enter <boolexpr>\n");
    lex();
    //check if next token is identifier
    if(nextToken == IDENT){
        lex();
        comp();
        lex();
        // check to see if its an int, if not its incorrect
        if(nextToken == INT_LIT){}
        else{
             error();
        }
    }
    else{
    error();
    }
     printf("Exit <boolexpr>\n");
}
//<comp> → < | > 
void comp(){
    printf("Enter <comp>\n");
    //check if next token is > or <
    if(nextToken == GREATER_THAN){}
    else if(nextToken == LESS_THAN){}
    else{
         error();
    }
    printf("Exit <comp>\n");
}
//<whilestmt> → while (<boolexpr>) ‘{‘ <stmts> ‘}’
void whilestmt(){
    printf("Enter <whilestmt>\n");
    //check if next token is while
    if(nextToken == WHILE_STMT){
        lex();
        //check if next token is (
        if(nextToken == LEFT_PAREN){
            boolexpr();
            lex();
            //check if next token is )
            if(nextToken == RIGHT_PAREN){
                lex();
                stmt();
            }else{
               error();
            }
        }else{
             error();
        }
    }else{
        error();
    }
    printf("Exit <whilestmt>\n");
}

// <forstmt> → for (<asgnstmt>; <boolexpr>; <asgnstmt>) ‘{‘ <stmts> ‘}’
void forstmt(){
    printf("Enter <forstmt>\n");
    //check if next token is for stmt
    if(nextToken == FOR_STMT){
        lex();
        //check if next token is (
        if(nextToken == LEFT_PAREN){
            lex();
            asgnstmt();
            if(nextToken != SEMICOLON)
            error();
            boolexpr();\
            lex();
            lex();
            asgnstmt();
            //check if next token is )
            if(nextToken == RIGHT_PAREN){
                lex();
                stmt();
            }else{
                 error();
            }
        }else{
            error();
        }
    }else{
         error();
    }
    printf("Exit <forstmt>\n");
}

//<asgnstmt> → id = <expr>
void asgnstmt(){
    printf("Enter <asgnstmt>\n");
    //check if next token is identifier
    if(nextToken == IDENT){
        lex();
        //check if next token is = 
        if(nextToken == ASSIGN_OP){
            lex();
            expr();
        }else
             error();
        
    }else
         error();
    
    printf("Exit <asgnstmt>\n");
}

/* Parses strings in the language generated by the rule:
 *    <expr> -> <term> {(+ | -) <term>} */
void expr() {
    printf("Enter <expr>\n");
    /* Parse the first term */
    term();
    /* As long as the next token is + or -, get
 *     the next token and parse the next term */
    while (nextToken == ADD_OP || nextToken == SUB_OP) {
        lex();
        term();
    }
    printf("Exit <expr>\n");
}

/* Parses strings in the language generated by the rule:
 *    <term> -> <factor> {(* | /) <factor>) */
void term() {
   printf("Enter <term>\n");
  
    /* Parse the first factor */
    factor();
    /* As long as the next token is * or /, get the
 *     next token and parse the next factor */
    while (nextToken == MULT_OP || nextToken == DIV_OP) {
        lex();
        factor();
    }
    printf("Exit <term>\n");
}

/* Parses strings in the language generated by the rule:
 *     <factor> -> id | int_constant | ( <expr> ) */
void factor() {
    printf("Enter <factor>\n");
    /* Determine which RHS */
    // add the ability for it to handle floats
    if (nextToken == IDENT || nextToken == INT_LIT || nextToken == FLOAT_CONSTANT ) {
    //    printf("ID or Literal\n");
        /* Get the next token */
        lex();
    }
    /* If the RHS is ( <expr>), call lex to pass over the left parenthesis,
 *        call expr, and check for the right parenthesis */
    else {
        if (nextToken == LEFT_PAREN) {
            lex();
            expr();
            if (nextToken == RIGHT_PAREN)
                lex();
            else
                 error();
        } /* End of if (nextToken == ... */
        /* It was not an id, an integer literal, or a left parenthesis */
        else
            error();
    } /* End of else */
    printf("Exit <factor>\n");
}


void error(){
    printf("Error\n");
    exit(0);
}
