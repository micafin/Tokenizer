/*
 * tokenizer.c
 */
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

//declaring enum to be used to identify the state of a token
 enum State{NONE,MAL,DECIMAL,HEX,FLOA,OCTAL,ZERO};

//a pointer for the separate tokens
 char * ftoken;
int isFloat=0,isE=0;
 typedef enum State State;

//initializing current state of the token
 State currstate=NONE,prevstate=NONE;

/*
 * Tokenizer type.  You need to fill in the type as part of your implementation.
 */
struct TokenizerT_ {
	char * token;
	int token_pos;
};

typedef struct TokenizerT_ TokenizerT;

/*
 * TKCreate creates a new TokenizerT object for a given token stream
 * (given as a string).
 * 
 * TKCreate should copy the arguments so that it is not dependent on
 * them staying immutable after returning.  (In the future, this may change
 * to increase efficiency.)
 *
 * If the function succeeds, it returns a non-NULL TokenizerT.
 * Else it returns NULL.
 *
 * You need to fill in this function as part of your implementation.
 */

TokenizerT *TKCreate( char * ts ) {
	TokenizerT * t=malloc(sizeof(TokenizerT));
	t->token_pos=0;
	t->token=ts;

  return t;
}

/*
 * TKDestroy destroys a TokenizerT object.  It should free all dynamically
 * allocated memory that is part of the object being destroyed.
 *
 * You need to fill in this function as part of your implementation.
 */

void TKDestroy( TokenizerT * tk ) {
	free(tk->token);
	free(tk);
}

//checks if the token is a float
//returns float and malformed
//has checks for various error casses for e
State stateFloat(TokenizerT * tk){
	if(isdigit(tk->token[tk->token_pos])){
		return FLOA;
	}
	else if(((tk->token[tk->token_pos]=='E')||(tk->token[tk->token_pos]=='e'))&&(!isdigit(tk->token[tk->token_pos+1])&&(tk->token[tk->token_pos+1]!='-')&&(tk->token[tk->token_pos+1]!='+'))){
		tk->token_pos--;
		return MAL;
	}
	else if((isdigit(tk->token[tk->token_pos-1]))&&((tk->token[tk->token_pos]=='E')||(tk->token[tk->token_pos]=='e'))){
		if(isE==0){
			isE=1;
		}
		else{
			tk->token_pos--;
			return MAL;
		}
		return FLOA;
	}
	else if(!isdigit(tk->token[tk->token_pos+1])&&((tk->token[tk->token_pos]=='-')||(tk->token[tk->token_pos]=='+'))&&((tk->token[tk->token_pos-1]=='E')||(tk->token[tk->token_pos-1]=='e'))){
		if(isFloat==1){
			prevstate=FLOA;
		}
		else{
			prevstate=DECIMAL;
		}
		tk->token_pos=tk->token_pos-2;
		return MAL;
	}
	else if(((tk->token[tk->token_pos]=='-')||(tk->token[tk->token_pos]=='+'))&&((tk->token[tk->token_pos-1]=='E')||(tk->token[tk->token_pos-1]=='e'))){
		if(isE==0){
			isE=1;
		}
		else{
			tk->token_pos=tk->token_pos-2;
			return MAL;
		}
		return FLOA;
	}
	else{
		tk->token_pos--;
		return MAL;
	}

}

//checks if the token is a decimal
//returns decimal, float, and malformed
//has checks for various error casses for e
State stateDecimal(TokenizerT * tk){

	if(isdigit(tk->token[tk->token_pos])){
		return DECIMAL;
	}
	else if(((tk->token[tk->token_pos]=='E')||(tk->token[tk->token_pos]=='e'))&&(!isdigit(tk->token[tk->token_pos+1])&&(tk->token[tk->token_pos+1]!='-')&&(tk->token[tk->token_pos+1]!='+'))){
		tk->token_pos--;
		return MAL;
	}
	else if((isdigit(tk->token[tk->token_pos-1]))&&((tk->token[tk->token_pos]=='E')||(tk->token[tk->token_pos]=='e'))){
		return FLOA;
	}
	else if(!isdigit(tk->token[tk->token_pos+1])&&((tk->token[tk->token_pos]=='-')||(tk->token[tk->token_pos]=='+'))&&((tk->token[tk->token_pos-1]=='E')||(tk->token[tk->token_pos-1]=='e'))){
		tk->token_pos=tk->token_pos-2;
		return MAL;
	}
	else if(((tk->token[tk->token_pos]=='-')||(tk->token[tk->token_pos]=='+'))&&((tk->token[tk->token_pos-1]=='E')||(tk->token[tk->token_pos-1]=='e'))){
		return FLOA;
	}
	else if(tk->token[tk->token_pos]=='.'&&isdigit(tk->token[tk->token_pos+1])){
		isFloat=1;
		return FLOA;
	}
	else{
		tk->token_pos--;
		return MAL;
	}

}

//base case for most tokens, since they start off as a none state
//because all of the tokens must start as a digit it returns decimal or malformed
State stateNone(TokenizerT * tk){
	
	if(isdigit(tk->token[tk->token_pos])){
		return DECIMAL;
	}
	else{
		return MAL;
	}

}

//checks if the token is an octal and returns the state
State stateOctal(TokenizerT * tk){
	
	if((tk->token[tk->token_pos]>='0'&&tk->token[tk->token_pos]<='7')){
		//printf("%c\n",tk->token[tk->token_pos]);
		return OCTAL;
	}
	else{
		tk->token_pos--;
		return MAL;
	}
}

//checks if the token is a hex and returns the state
State stateHex(TokenizerT * tk){
	
	if(isdigit(tk->token[tk->token_pos])||isxdigit(tk->token[tk->token_pos])){
		return HEX;
	}
	else{
		tk->token_pos--;
		return MAL;
	}
}

/*
 * TKGetNextToken returns the next token from the token stream as a
 * character string.  Space for the returned token should be dynamically
 * allocated.  The caller is responsible for freeing the space once it is
 * no longer needed.
 *
 * If the function succeeds, it returns a C string (delimited by '\0')
 * containing the token.  Else it returns 0.
 *
 * You need to fill in this function as part of your implementation.
 */

char *TKGetNextToken( TokenizerT * tk ) { 

	int length=strlen(tk->token), starttoke=0;

	while((isspace(tk->token[tk->token_pos]))){
		tk->token_pos++;
	}

	//loops until end of tokenizer object
	while(tk->token_pos<length){

			if(currstate==NONE){
				starttoke=tk->token_pos;
			}

			prevstate=currstate;
			
			//checks if the first digit of the token is a 0 and then checks if it would fall under a hex, octal, or float state (special case for 0)
			if(tk->token[tk->token_pos]=='0'&&currstate==NONE){
				tk->token_pos++;
				//checks that there is a digit after the x for a token in 0x form
				if(((tk->token[tk->token_pos]=='.')||(tk->token[tk->token_pos]=='x'||tk->token[tk->token_pos]=='X'))&&(!isxdigit(tk->token[tk->token_pos+1]))){
					tk->token_pos--;
					prevstate=ZERO;
					currstate=MAL;
				}
				else if(tk->token[tk->token_pos]=='.'){
					tk->token_pos++;
					currstate=stateFloat(tk);
				}
				else if(tk->token[tk->token_pos]=='x'||tk->token[tk->token_pos]=='X'){
					tk->token_pos++;
					currstate=stateHex(tk);
				}
				
				else if(tk->token_pos>=length||!isdigit(tk->token[tk->token_pos])){
					currstate=ZERO;
				}
				else if(tk->token[tk->token_pos]>='0'&&tk->token[tk->token_pos]<='7'){
					tk->token_pos--;
					currstate=stateOctal(tk);
				}
			}
			
			//checks the state of the token
			switch(currstate){
				case NONE:
					currstate=stateNone(tk);
					break;
				case DECIMAL:
					currstate=stateDecimal(tk);
					break;
				case OCTAL:
					currstate=stateOctal(tk);
					break;
				case HEX:
					currstate=stateHex(tk);
					break;
				case FLOA:
					currstate=stateFloat(tk);
					break;
				case ZERO:
					break;
				case MAL:
					break;
			}

		tk->token_pos++;

		//if the case is malformed then it finds the token up to the escape character and returns that
		if(currstate==MAL||currstate==ZERO){
			if(prevstate!=MAL&&prevstate!=NONE){
				currstate=prevstate;
				ftoken=malloc(sizeof(char)*(tk->token_pos-starttoke)+1);
				strncpy(ftoken,&tk->token[starttoke],tk->token_pos-starttoke);
				return ftoken;
			}
			break;
		}	

	}

	ftoken=malloc(sizeof(char)*(tk->token_pos-starttoke)+1);
	strncpy(ftoken,&tk->token[starttoke],tk->token_pos-starttoke);
 	return ftoken;

}

/*
 * main will have a string argument (in argv[1]).
 * The string argument contains the tokens.
 * Print out the tokens in the second string in left-to-right order.
 * Each token should be printed on a separate line.
 */

int main(int argc, char **argv) {
	//Check to see if the correct amount of arguments was entered
	if(argc!=2){
		printf("ERROR\n");
		return 0;
	}

	//initializes a pointer to the Tokenizer object
	TokenizerT * tk = TKCreate(argv[1]);
	
	int length=strlen(tk->token);

	//loops until the end of the tokenizer string
	while(tk->token_pos<length){

		//calls the TKGetNextToken function and returns the seperate tokens of the tokenizer object
		ftoken=TKGetNextToken(tk);

		//Prints the state of the token
		switch(currstate){
			case ZERO:
				printf("ZERO        ");
				break;
			case NONE:
				break;
			case MAL:
				printf("MALFORMED   ");
				break;
			case DECIMAL:
				printf("DECIMAL     ");
				break;
			case HEX:
				printf("HEX         ");
				break;
			case FLOA:
				printf("FLOAT       ");
				break;
			case OCTAL:
				printf("OCTAL       ");
				break;
		}

		//If the state is malformed, it prints the escape character
		if(currstate==MAL){
			printf("[%#2x]\n",*ftoken);
		}
		//otherwise it prints the token
		else if(currstate!=NONE){
			printf("%s\n",ftoken);
		}
		isE=0;
		isFloat=0;
		currstate=NONE;
		prevstate=NONE;
	}
	free(ftoken);
  	return 0;
}