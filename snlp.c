#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "jrb.h"
#include "fields.h"

typedef struct Place
{
	int line;
	struct Place *next;
}place;

typedef struct word_node
{
	int counter;
	place *p;
	char *str;
}word;

void delete_char(char *s, int loc){
	memmove(&s[loc], &s[loc+1], strlen(s)-loc);
}

void low_c(char *s){
	for (int i = 0; i<strlen(s);++i){
		tolower(s[i]);
	}
}

int c_format(char *s){
	int re;

	re = 1;
	for (int i = 0; i < strlen(s); ++i) {
		if (isalpha(s[i]) == 0)
		{
			delete_char(s,i); 
		} 
		if ((isdigit(s[i]) != 0) || (isupper(s[i]) != 0))
		{ 
			re = 0; break;
		}
		// if (isupper(s[i])){
		// 	tolower(s[i]);
		// 	re = 0;
		// }
	}
	return re;
}

JRB Read_S_word(){
	JRB tree;
	IS is;
	FILE *f;
	char *s;

	is = new_inputstruct("completed/stopw.txt");
	tree = make_jrb();

	if (is == NULL) {
		printf("stopw.txt not found\n");
		exit(1);
	}

	while(get_line(is)>=0){
		for (int i=0;i < is->NF; ++i){

			s = malloc(sizeof(char)*strlen(is->fields[i]));
			strcpy(s, is->fields[i]);

			jrb_insert_str(tree, s, new_jval_s(NULL));
		}
	}
	return tree;
}

int main(int argc, char const *argv[])
{
	JRB sw_tree, w_tree, trv, tmp;
	IS is;
	word *w,*token;
	place *pl;
	char *str,*str2;

	sw_tree = Read_S_word();
	w_tree = make_jrb();
	tmp = make_jrb();

	if (argc < 2) { fprintf(stderr, "no input text?\n"); exit(1); }

  	is = new_inputstruct(argv[1]);

  	while(get_line(is)>=0){
  		for (int i = 0; i < is->NF; i++)
  		{
  			str = malloc(sizeof(char) * strlen(is->fields[i]));
  			strcpy(str,is->fields[i]);

  			// before formatting, checking if there is ".", if there is, next word 
  			if (((str[strlen(str)-1] == '.') && (i+1<is->NF)) || (i==0)) low_c(is->fields[i+1]);

  			if ((c_format(str) != 0) && (jrb_find_str(sw_tree,str) == NULL))
  			{

  				trv = jrb_find_str(tmp, str);
	  			if (trv == NULL) 
	  			{
	  				w = malloc(sizeof(word));

  					w->str = malloc(sizeof(char) * strlen(str));
	  				strcpy(w->str,str);
	  				w->counter = 1;

	 				pl = malloc(sizeof(place));
	 				pl->next = NULL;
	 				pl->line = is->line;
	 				w->p = pl;

		  			jrb_insert_str(tmp, w->str, new_jval_v(w));
	  			} else{
	  				token = (word*) trv->val.v; 
	  				token->counter++;

	  				pl = malloc(sizeof(place));
	 				pl->line = is->line;

	 				pl->next = token->p;
	 				token->p = pl;
	  			}	
  			}
  		}
  	}

  	jrb_traverse(trv, tmp){
  		token = (word*) trv->val.v; 
  		printf("%10s\t%d\t ",token->str,token->counter);
  		while (token->p !=NULL){
  			printf("%d ", token->p->line);
  			token->p = token->p->next;
  		}
  		printf("\n");
  	}

	return 0;
}