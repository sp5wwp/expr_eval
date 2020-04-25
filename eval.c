//*****************************************************************************************************
//
// File Name	: 'eval.c'
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
#include <string.h>
#include <inttypes.h>
#include "eval.h"

//----------------------------------SUBSTITUTE CONSTANTS-------------------------
void substConst(char *expr)
{
	char left[BASIC_EXPR_LEN];
	char right[BASIC_EXPR_LEN];
	
	//look for constants
	for(uint8_t j=0; j<BASIC_EXPR_LEN; j++)
	{
		//Euler's number
		if(expr[j]=='e')
		{			
			//get the left side (GNLS ;)
			for(uint8_t i=0; i<BASIC_EXPR_LEN; i++)
				left[i]=0;
			for(uint8_t i=0; i<j; i++)
				left[i]=expr[i];
				
			//get the right side
			for(uint8_t i=0; i<BASIC_EXPR_LEN; i++)
				right[i]=0;
			for(uint8_t i=j+1; i<BASIC_EXPR_LEN; i++)
			right[i-(j+1)]=expr[i];
			
			sprintf(expr, "%s%.5f%s", left, 2.71828182846, right);
		}
		
		//Pi
		if(expr[j]=='p' && expr[j+1]=='i')
		{			
			//get the left side (GNLS ;)
			for(uint8_t i=0; i<BASIC_EXPR_LEN; i++)
				left[i]=0;
			for(uint8_t i=0; i<j; i++)
				left[i]=expr[i];
				
			//get the right side
			for(uint8_t i=0; i<BASIC_EXPR_LEN; i++)
				right[i]=0;
			for(uint8_t i=j+2; i<BASIC_EXPR_LEN; i++)
			right[i-(j+2)]=expr[i];
			
			sprintf(expr, "%s%.5f%s", left, 3.14159265359, right);
		}
	}
}

//-----------------------------------SUBSTITUTE VARIABLE--------------------------
void substVar(char *expr, char variable, double value)
{
	char left[BASIC_EXPR_LEN];
	char right[BASIC_EXPR_LEN];
	
	//look for variable
	for(uint8_t j=0; j<BASIC_EXPR_LEN; j++)
	{
		if(expr[j]==variable)
		{			
			//get the left side (GNLS ;)
			for(uint8_t i=0; i<BASIC_EXPR_LEN; i++)
				left[i]=0;
			for(uint8_t i=0; i<j; i++)
				left[i]=expr[i];
				
			//get the right side
			for(uint8_t i=0; i<BASIC_EXPR_LEN; i++)
				right[i]=0;
			for(uint8_t i=j+1; i<BASIC_EXPR_LEN; i++)
			right[i-(j+1)]=expr[i];
			
			sprintf(expr, "%s%.5f%s", left, value, right);
		}
	}
}

//--------------------------------------SOLVE POWERS------------------------------
void solvePwr(char *expr)
{
	char tmp[BASIC_EXPR_LEN];
	char left[BASIC_EXPR_LEN];
	char right[BASIC_EXPR_LEN];
	double arg_a, arg_b, res;
	
	uint8_t start=0, stop=0, pwr_mark=0, solvable=0, neg_arg=0;
	
	//look for '^' operator
	for(uint8_t i=strlen(expr)-1; i>0; i--)
	{
		if(expr[i]=='^' && i<strlen(expr))
		{
			pwr_mark = i;
			
			//negative 2nd argument?
			if(expr[i+1]=='-')
			{
				expr[i+1]='0';
				neg_arg=1;
			}
			else
			{
				neg_arg=0;
			}
		
			//where does it start?
			for(uint8_t i=pwr_mark-1; i>=0; i--)
			{
				if(i==0)
				{
					start=0;
					solvable=1;
					break;
				}
				if(expr[i]=='+' || expr[i]=='-' || expr[i]=='*' || expr[i]=='/' || expr[i]=='(' || expr[i]=='^')
				{
					start=i+1;
					solvable=1;
					break;
				}
			}
			
			if(solvable)
			{
				//...and where does it end?
				for(uint8_t i=pwr_mark+1; i<BASIC_EXPR_LEN; i++)
				{
					if(expr[i]=='+' || expr[i]=='-' || expr[i]=='/' || expr[i]=='*' || expr[i]==')' || expr[i]==0)
					{
						stop=i-1;
						break;
					}
					if(expr[i]=='^')
					{
						solvable=0;
						break;	
					}
				}
				
				if(solvable)
				{
					//take the left argument
					for(uint8_t i=0; i<BASIC_EXPR_LEN; i++)
						tmp[i]=0;
					for(uint8_t i=start; i<pwr_mark; i++)
						tmp[i-start]=expr[i];
					sscanf(tmp, "%f", &arg_a);
					
					//take the right argument
					for(uint8_t i=0; i<BASIC_EXPR_LEN; i++)
						tmp[i]=0;
					for(uint8_t i=pwr_mark+1; i<=stop; i++)
						tmp[i-(pwr_mark+1)]=expr[i];
					sscanf(tmp, "%f", &arg_b);
					
					//calculate the result
					if(!neg_arg)
						res=pow(arg_a, arg_b);
					else
						res=pow(arg_a, -arg_b);
					
					//get the left side (GNLS ;)
					for(uint8_t i=0; i<BASIC_EXPR_LEN; i++)
						left[i]=0;
					for(uint8_t i=0; i<start; i++)
						left[i]=expr[i];
						
					//get the right side
					for(uint8_t i=0; i<BASIC_EXPR_LEN; i++)
						right[i]=0;
					for(uint8_t i=stop+1; i<BASIC_EXPR_LEN; i++)
						right[i-(stop+1)]=expr[i];
						
					//after calculation
					for(uint8_t i=0; i<BASIC_EXPR_LEN; i++)
						tmp[i]=0;
					sprintf(tmp, "%.5f", res);
					sprintf(expr, "%s%s%s", left, tmp, right);
				}
			}

			//i=BASIC_EXPR_LEN;
		}
	}
}

//----------------------------------SOLVE MULTIPLICATIONS-------------------------
void solveMult(char *expr)
{
	char tmp[BASIC_EXPR_LEN];
	char left[BASIC_EXPR_LEN];
	char right[BASIC_EXPR_LEN];
	double arg_a, arg_b, res;

	uint8_t start=0, stop=0, mult_mark=0, solvable=0, neg_arg=0;
	
	//look for '*' multiplication operator
	for(uint8_t i=1; i<BASIC_EXPR_LEN; i++)
	{
		if(expr[i]==0)
			break;
			
		if(expr[i]=='*' && i<strlen(expr))
		{
			mult_mark = i;
			
			//negative 2nd argument?
			if(expr[i+1]=='-')
			{
				expr[i+1]='0';
				neg_arg=1;
			}
			else
			{
				neg_arg=0;
			}
		
			//where does the multiplication start?
			for(uint8_t i=mult_mark-1; i>=0; i--)
			{
				if(i==0)
				{
					start=0;
					solvable=1;
					break;
				}
				if(expr[i]=='+' || expr[i]=='-' || expr[i]=='*' || expr[i]=='(')
				{
					start=i+1;
					solvable=1;
					break;
				}
				if(expr[i]=='/' || expr[i]=='^')
				{
					solvable=0;
					break;	
				}
			}
			
			if(solvable)
			{
				//...and where does it end?
				for(uint8_t i=mult_mark+1; i<BASIC_EXPR_LEN; i++)
				{
					if(expr[i]=='+' || expr[i]=='-' || expr[i]=='/' || expr[i]=='*' || expr[i]==')' || expr[i]==0)
					{
						stop=i-1;
						break;
					}
					if(expr[i]=='^')
					{
						solvable=0;
						break;	
					}
				}
				
				if(solvable)
				{				
					//take the left argument
					for(uint8_t i=0; i<BASIC_EXPR_LEN; i++)
						tmp[i]=0;
					for(uint8_t i=start; i<mult_mark; i++)
						tmp[i-start]=expr[i];
					sscanf(tmp, "%f", &arg_a);
					
					//take the right argument
					for(uint8_t i=0; i<BASIC_EXPR_LEN; i++)
						tmp[i]=0;
					for(uint8_t i=mult_mark+1; i<=stop; i++)
						tmp[i-(mult_mark+1)]=expr[i];
					sscanf(tmp, "%f", &arg_b);
					
					//calculate the result
					res=arg_a*arg_b;
					if(neg_arg)
						res=-res;
					
					//get the left side (GNLS ;)
					for(uint8_t i=0; i<BASIC_EXPR_LEN; i++)
						left[i]=0;
					for(uint8_t i=0; i<start; i++)
						left[i]=expr[i];
						
					//get the right side
					for(uint8_t i=0; i<BASIC_EXPR_LEN; i++)
						right[i]=0;
					for(uint8_t i=stop+1; i<BASIC_EXPR_LEN; i++)
						right[i-(stop+1)]=expr[i];
						
					//after calculation
					for(uint8_t i=0; i<BASIC_EXPR_LEN; i++)
						tmp[i]=0;
					sprintf(tmp, "%.5f", res);
					sprintf(expr, "%s%s%s", left, tmp, right);
					
					i=start;
				}
			}
		}
	}	
}

//-------------------------------------SOLVE DIVISIONS----------------------------
void solveDiv(char *expr)
{
	char tmp[BASIC_EXPR_LEN];
	char left[BASIC_EXPR_LEN];
	char right[BASIC_EXPR_LEN];
	double arg_a, arg_b, res;

	uint8_t start=0, stop=0, div_mark=0, solvable=0, neg_arg=0;
	
	//look for '/' division operator
	for(uint8_t i=1; i<BASIC_EXPR_LEN; i++)
	{
		if(expr[i]==0)
			break;
			
		if(expr[i]=='/' && i<strlen(expr))
		{
			div_mark = i;
			
			//negative 2nd argument?
			if(expr[i+1]=='-')
			{
				expr[i+1]='0';
				neg_arg=1;
			}
			else
			{
				neg_arg=0;
			}
		
			//where does the division start?
			for(uint8_t i=div_mark-1; i>=0; i--)
			{
				if(i==0)
				{
					start=0;
					solvable=1;
					break;
				}
				if(expr[i]=='+' || expr[i]=='-' || expr[i]=='/' || expr[i]=='(')
				{
					start=i+1;
					solvable=1;
					break;
				}
				if(expr[i]=='*' || expr[i]=='^')
				{
					solvable=0;
					break;	
				}
			}
			
			if(solvable)
			{
				//...and where does it end?
				for(uint8_t i=div_mark+1; i<BASIC_EXPR_LEN; i++)
				{
					if(expr[i]=='+' || expr[i]=='-' || expr[i]=='/' || expr[i]=='*' || expr[i]==')' || expr[i]==0)
					{
						stop=i-1;
						break;
					}
					if(expr[i]=='^')
					{
						solvable=0;
						break;	
					}
				}
				
				if(solvable)
				{
					//take the left argument
					for(uint8_t i=0; i<BASIC_EXPR_LEN; i++)
						tmp[i]=0;
					for(uint8_t i=start; i<div_mark; i++)
						tmp[i-start]=expr[i];
					sscanf(tmp, "%f", &arg_a);
					
					//take the right argument
					for(uint8_t i=0; i<BASIC_EXPR_LEN; i++)
						tmp[i]=0;
					for(uint8_t i=div_mark+1; i<=stop; i++)
						tmp[i-(div_mark+1)]=expr[i];
					sscanf(tmp, "%f", &arg_b);
					
					//check if arg_b != 0
					if(arg_b==0.0)
					{
						sprintf(expr, "%s", "DIV0!");
						return;
					}
					
					//calculate the result
					res=arg_a/arg_b;
					if(neg_arg)
						res=-res;
					
					//get the left side (GNLS ;)
					for(uint8_t i=0; i<BASIC_EXPR_LEN; i++)
						left[i]=0;
					for(uint8_t i=0; i<start; i++)
						left[i]=expr[i];
						
					//get the right side
					for(uint8_t i=0; i<BASIC_EXPR_LEN; i++)
						right[i]=0;
					for(uint8_t i=stop+1; i<BASIC_EXPR_LEN; i++)
						right[i-(stop+1)]=expr[i];
						
					//after calculation
					for(uint8_t i=0; i<BASIC_EXPR_LEN; i++)
						tmp[i]=0;
					sprintf(tmp, "%.5f", res);
					sprintf(expr, "%s%s%s", left, tmp, right);
					
					i=start;
				}
			}
		}
	}	
}

//--------------------------SOLVE ADDITIONS & SUBTRACTIONS------------------------
void solveAddSub(char *expr)
{
	char tmp[BASIC_EXPR_LEN];
	char left[BASIC_EXPR_LEN];
	char right[BASIC_EXPR_LEN];
	double arg_a, arg_b, res;

	uint8_t start=0, stop=0, mark=0, neg_arg=0;
	
	//look for '-' and '+' operator
	for(uint8_t i=1; i<BASIC_EXPR_LEN-1; i++)
	{
		if(expr[i]=='-' && i<strlen(expr))
		{
			mark = i;
			
			//negative 2nd argument?
			if(expr[i+1]=='-')
			{
				expr[i+1]='0';
				neg_arg=1;
			}
			else
			{
				neg_arg=0;
			}
		
			//where does it start?
			for(uint8_t i=mark-1; i>=0; i--)
			{
				if(i==0)
				{
					start=0;
					arg_a=0;
					break;
				}
				if(expr[i]=='+' || expr[i]=='-' || expr[i]=='(')
				{
					start=i+1;
					break;
				}
			}
			
			//...and where does it end?
			for(uint8_t i=mark+1; i<BASIC_EXPR_LEN; i++)
			{
				if(expr[i]=='+' || expr[i]=='-' || expr[i]==')' || expr[i]==0)
				{
					stop=i-1;
					break;
				}
			}
			
			
			//take the left argument
			for(uint8_t i=0; i<BASIC_EXPR_LEN; i++)
				tmp[i]=0;
			for(uint8_t i=start; i<mark; i++)
				tmp[i-start]=expr[i];
			sscanf(tmp, "%f", &arg_a);
			
			//take the right argument
			for(uint8_t i=0; i<BASIC_EXPR_LEN; i++)
				tmp[i]=0;
			for(uint8_t i=mark+1; i<=stop; i++)
				tmp[i-(mark+1)]=expr[i];
			sscanf(tmp, "%f", &arg_b);
	       
			//calculate the result
			if(neg_arg)
				res=arg_a+arg_b;
			else
				res=arg_a-arg_b;
			
			//get the left side (GNLS ;)
			for(uint8_t i=0; i<BASIC_EXPR_LEN; i++)
				left[i]=0;
			for(uint8_t i=0; i<start; i++)
				left[i]=expr[i];
				
			//get the right side
			for(uint8_t i=0; i<BASIC_EXPR_LEN; i++)
				right[i]=0;
			for(uint8_t i=stop+1; i<BASIC_EXPR_LEN; i++)
				right[i-(stop+1)]=expr[i];
				
			//after calculation
			for(uint8_t i=0; i<BASIC_EXPR_LEN; i++)
				tmp[i]=0;
			sprintf(tmp, "%.5f", res);
			sprintf(expr, "%s%s%s", left, tmp, right);
			
			i=1;
		}
		
		if(expr[i]=='+' && i<strlen(expr))
		{
			mark = i;
			
			//negative 2nd argument?
			if(expr[i+1]=='-')
			{
				expr[i+1]='0';
				neg_arg=1;
			}
			else
			{
				neg_arg=0;
			}
		
			//where does it start?
			for(uint8_t i=mark-1; i>=0; i--)
			{
				if(i==0)
				{
					start=0;
					arg_a=0;
					break;
				}
				if(expr[i]=='+' || expr[i]=='-' || expr[i]=='(')
				{
					start=i+1;
					break;
				}
				
			}
			
			//...and where does it end?
			for(uint8_t i=mark+1; i<BASIC_EXPR_LEN; i++)
			{
				if(expr[i]=='+' || expr[i]=='-' || expr[i]==')' || expr[i]==0)
				{
					stop=i-1;
					break;
				}
				
			}
			
			//take the left argument
			for(uint8_t i=0; i<BASIC_EXPR_LEN; i++)
				tmp[i]=0;
			for(uint8_t i=start; i<mark; i++)
				tmp[i-start]=expr[i];
			sscanf(tmp, "%f", &arg_a);
			
			//take the right argument
			for(uint8_t i=0; i<BASIC_EXPR_LEN; i++)
				tmp[i]=0;
			for(uint8_t i=mark+1; i<=stop; i++)
				tmp[i-(mark+1)]=expr[i];
			sscanf(tmp, "%f", &arg_b);
	       
			//calculate the result
			if(neg_arg)
				res=arg_a-arg_b;
			else
				res=arg_a+arg_b;
			
			//get the left side (GNLS ;)
			for(uint8_t i=0; i<BASIC_EXPR_LEN; i++)
				left[i]=0;
			for(uint8_t i=0; i<start; i++)
				left[i]=expr[i];
				
			//get the right side
			for(uint8_t i=0; i<BASIC_EXPR_LEN; i++)
				right[i]=0;
			for(uint8_t i=stop+1; i<BASIC_EXPR_LEN; i++)
				right[i-(stop+1)]=expr[i];
				
			//after calculation
			for(uint8_t i=0; i<BASIC_EXPR_LEN; i++)
				tmp[i]=0;
			sprintf(tmp, "%.5f", res);
			sprintf(expr, "%s%s%s", left, tmp, right);

			i=1;
		}
	}
}

//--------------------------------------REPLACE "--"------------------------------
void replaceMin(char *expr)
{
	char tmp[BASIC_EXPR_LEN];
	char left[BASIC_EXPR_LEN];
	char right[BASIC_EXPR_LEN];

	uint8_t mark=0;
	
	//look for "--"
	for(uint8_t i=0; i<BASIC_EXPR_LEN; i++)
	{
		if(expr[i]==0)
			break;
			
		if(expr[i]=='-' && expr[i+1]=='-' && i<strlen(expr))
		{
			mark = i;

			//get the left side (GNLS ;)
			for(uint8_t i=0; i<BASIC_EXPR_LEN; i++)
				left[i]=0;
			for(uint8_t i=0; i<mark; i++)
				left[i]=expr[i];
				
			//get the right side
			for(uint8_t i=0; i<BASIC_EXPR_LEN; i++)
				right[i]=0;
			for(uint8_t i=mark+2; i<BASIC_EXPR_LEN; i++)
				right[i-(mark+2)]=expr[i];
			
			sprintf(expr, "%s%c%s", left, '+', right);
			
			i=mark;
		}
	}	
}

void evaluate(char *expr)
{
	if(strlen(expr)!=0)
	{
		while(strchr(expr, '*')!=NULL || strchr(expr, '/')!=NULL || strchr(expr, '^')!=NULL)
		{
			solvePwr(expr);
			solveMult(expr);
			solveDiv(expr);
		}
		
		replaceMin(expr);
		solveAddSub(expr);
	}
}

void solvePar(char *expr, char *buffer)
{
	uint8_t start=0, stop=0;
	char left[BASIC_EXPR_LEN];
	char right[BASIC_EXPR_LEN];
	
	//find first ")" occurence and preceding "(" index
	for(uint8_t i=0; i<strlen(expr); i++)
	{
		if(expr[i]==')')
		{
			stop=i;
			
			for(uint8_t j=i-1; j>=0; j--)
				if(expr[j]=='(')
				{
					start=j;
					break;
				}
			
			break;
		}
	}

	//copy E to the buffer
	for(uint8_t i=0; i<EXPR_BUFF_LEN; i++)
		buffer[i]=0;
	for(uint8_t i=start+1; i<stop; i++)
		buffer[i-(start+1)]=expr[i];

	//eval E
	evaluate(buffer);
	
	//get the left side (GNLS ;)
	for(uint8_t i=0; i<BASIC_EXPR_LEN; i++)
		left[i]=0;
	for(uint8_t i=0; i<start; i++)
		left[i]=expr[i];
		
	//get the right side
	for(uint8_t i=0; i<BASIC_EXPR_LEN; i++)
		right[i]=0;
	for(uint8_t i=stop+1; i<BASIC_EXPR_LEN; i++)
		right[i-(stop+1)]=expr[i];
		
	//concatenate
	sprintf(expr, "%s%s%s", left, buffer, right);
}

void exprEval(char *expr, char *buffer)
{
	substConst(expr);

	while(strchr(expr, '(')!=NULL)
	{
		solvePar(expr, buffer);
	}
	
	evaluate(expr);
}
