//*****************************************************************************************************
//
// File Name	: 'eval.h'
// Title		: Arithmetic expression evaluation library
// Author		: Wojciech Kaczmarski http://xtal.tk
// Created		: 16-12-2013
// Revised		: 26-01-2014
// Version		: 1.0
// Target MCU	: ATMEGA AVR
//
// This code is distributed under the GNU Public License
//		which can be found at http://www.gnu.org/licenses/gpl.txt
//
//*****************************************************************************************************
#define BASIC_EXPR_LEN	50
#define EXPR_BUFF_LEN	30

char expression[BASIC_EXPR_LEN];
char expr_buff[EXPR_BUFF_LEN];

void substConst(char *expr);
void substVar(char *expr, char variable, double value);
void solvePwr(char *expr);
void solveMult(char *expr);
void solveDiv(char *expr);
void solveAddSub(char *expr);
void replaceMin(char *expr);
void evaluate(char *expr);
void solvePar(char *expr, char *buffer);
void exprEval(char *expr, char *buffer);