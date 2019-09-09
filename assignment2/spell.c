/* written by Soong Ze Shaun 900793
 * Assignment 2 COMP20007 2018 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "spell.h"
#include "hashtbl.h"
#include "strhash.h"


#define COST 		1			// cost of changing 1 character
#define HASHSIZE	1.5			// hashtable 1.5 times bigger than input file
#define MAX_LEN		3			// Max different in length of word for searching ED 3
#define NUM_ALPHA	26 			
#define MAX_ALPHA 	'z'			
#define MAX_ARRAY_SIZE 13594  	// max possible words for ED1 words (len 256)

/* ***********************************************************/
/*  Function prototypes  									 */
								 
int possibilities(int len);
int min(int x, int y, int z);
int edit_dist(char *word1, char *word2);
int search_prediction(char **A, int size, HashTable *H);
void free_array(char ** A, int size);
void fill_array(List *l, char *array[]);
void print_prediction(char *w1, char *w2);
void fill_hashtable(HashTable *H, List *l);
void deletion(char *word, int len, char **output, int *index);
void insertion(char *word, int len, char **output, int *index);
void subsitution(char *word, int len, char **output, int *index);
void brute_force(char **dic, char *word, int size, char **predict);
void methods_edit_dist(char *word, int len, char **output, int *index);
void search_edit_dist_1(char *word, char **A, HashTable *H, char **predict);

/* ***********************************************************/


// Assignment Task 1: Computing edit distance
void print_edit_distance(char *word1, char *word2) {

	// output
	int edit_distance = edit_dist(word1, word2);
	printf("%d\n", edit_distance);
}

// Assignment Task 2: Enumerating all possible edits
void print_all_edits(char *word) {
	
	int i, index = 0;
	int len = strlen(word);
	int size = possibilities(len);
	char **outputs = malloc(size * sizeof(char*));

	// methods of edits
	methods_edit_dist(word, len, outputs, &index);

	// print outputs
	for(i=0; i<size; i++){
		printf("%s\n", outputs[i]);
	}
	// free 
	free_array(outputs, size);
}

// Assignment Task 3: Spell checking
void print_checked(List *dictionary, List *document) {

	int size = dictionary->size;

	// create hashtable storing dictionary
	HashTable *table = new_hash_table(size * HASHSIZE);
	fill_hashtable(table, dictionary);

	// loop document
	Node *temp = document->head;
	assert(temp != NULL);

	while(temp != NULL){
		char *word = temp->data;

		// point to next word
		temp = temp->next;

		// search dictionary
		int ans = hash_table_has(table, word);

		// output, if not found in dictionary
		if(ans == 0){
			printf("%s?\n", word);
		}
		else{
			printf("%s\n", word);
		}
	} 
	//print_hash_table(table);
	free_hash_table(table);
}

// Assignment Task 4: Spelling correction
void print_corrected(List *dictionary, List *document) {
	int size = dictionary->size;

	// create array and hashtable to store dictionary
	// create array storing all possible predictions with ED 1
	HashTable *table  = new_hash_table(size * HASHSIZE);
	char **dic        = malloc(size * sizeof(char*));
	char **prediction = malloc(MAX_ARRAY_SIZE * sizeof(char*));

	fill_array(&*dictionary, &*dic);
	fill_hashtable(table, dictionary);

	// loop document
	Node *temp = document->head;
	assert(temp != NULL);

	while(temp != NULL){
		char *word = temp->data;
		temp = temp->next;

		// search dictionary (hashing)
		int ans = hash_table_has(table, word);

		// print valid words
		if(ans != 0){
			printf("%s\n", word);
		}
		// predictions
		else{
			char *predict = NULL; 

			// edit distance 1 predictions
			search_edit_dist_1(word, prediction, table, &predict);

			// edit distance 2 and 3 predictions
			brute_force(dic, word, size, &predict);

			// output
			print_prediction(predict, word);
		}

	}
	free_array(dic, size);
	free_array(prediction, MAX_ARRAY_SIZE);
	free_hash_table(table); 
}


// ************************************************************
/* Function prototypes 										 */

// get minimun number among the three
int min(int x, int y, int z){

	if(x<=y && x<=z){
		return x;
	}
	else if(y<=x && y<=z){
		return y;
	}
	else{
		return z;
	}
}

// get number of possibilities of 1 edit distance difference
int possibilities(int len){
	// sum of del + sub + inst (factorised formula)
	return (len*(1 + NUM_ALPHA + NUM_ALPHA) + NUM_ALPHA);
}

// fill up Levenshtein edit dist table, and return edit dist
int edit_dist(char *word1, char *word2){
	int i,j;

	// length of both words	
	int n = strlen(word1);
	int m = strlen(word2);

	// array storing all Levenshtein distances
	int array[n+1][m+1];
	
	// fill up known values 
	for(i=0; i <= n; i++){
		array[i][0] = i;
	}
	for(j=0; j <= m; j++){
		array[0][j] = j;
	}

	// calculate Levenshtein dist for remaining chars
	for(i=1; i <= n; i++){
		for(j=1; j <= m; j++){

			int sub_cost;
			if(word1[i-1] == word2[j-1]){
				sub_cost = 0;
			}else{
				sub_cost = COST;
			}

			// increment by lowest value
			array[i][j]= min(array[i-1][j-1] + sub_cost,
				array[i-1][j] + COST, array[i][j-1] + COST);
		}
	}
	// edit distance value
	return array[n][m];
}

// all the methods of edit distance = 1
void methods_edit_dist(char *word, int len, char **output, int *index){
	deletion   (word, len, output, index);
	subsitution(word, len, output, index);
	insertion  (word, len, output, index);
}

// deletion of char 
void deletion(char *word, int len, char **output, int *index){
	int i, j, k, idx = *index;
	char *newword = malloc((len-1)+1 * sizeof(char));

	// position of char (not to be copied)
	for(i=0; i < len ; i++){

		k = 0;
		// loop through word
		for(j=0; j< len-1 ; j++){

			// same pos, skip by 1 extra position
			if(k == i){
				k += 1;
			}
			newword[j] = word[k];
			k++;
		}
		// null terminator 
		newword[len-1] = '\0';

		// store output
		output[idx] = malloc((len-1) * sizeof(char));
		strcpy(output[idx], newword);
		idx ++;
	}
	//return new index
	*index = idx;

	free(newword);
}

// subsitution of a char
void subsitution(char *word, int len, char **output, int *index){
	int i, k, idx = *index;
	char *newword = (char *)malloc((len)+1 * sizeof(char));
	char c;

	// position of char (to be replaced)
	for(i=0; i < len ; i++){

		// letter to be changed into
		for(c = 'a'; c <= MAX_ALPHA; c++){

			// illiterate through word 
			for(k=0; k<len; k++){

				// replace
				if(k == i){
					newword[k] = c;
				}
				else{
					newword[k] = word[k];
				}
			}
			// null terminator 
			newword[len] = '\0';

			// store output
			output[idx] = malloc((len) * sizeof(char));
			strcpy(output[idx], newword);
			idx ++;
		}
	}
	//return new index
	*index = idx;

	free(newword);
}

// insertion
void insertion(char *word, int len, char **output, int *index){
	int i, k, offset, idx = *index;
	char *newword = (char *)malloc((len+1)+1 * sizeof(char));
	char c;

	// position of char (to be inserted)
	for(i=0; i<len+1; i++){

		// letter to be added
		for(c = 'a'; c <= MAX_ALPHA; c++){

			offset = 0;
			// illiterate through word 
			for(k=0; k< len+1 ; k++){

				// add
				if(k == i){
					newword[k] = c;
					offset = 1;
				}
				else{
					newword[k] = word[k-offset];
				}
			}
			// null terminator 
			newword[len+1] = '\0';

			// store output
			output[idx] = malloc((len+1) * sizeof(char));
			strcpy(output[idx], newword);
			idx ++;
		}
	}
	//return new index
	*index = idx;

	free(newword);
}

// fill up array
void fill_array(List *l, char *array[]){
	int i;
	Node *temp = l->head;
	
	for(i=0; i<l->size; i++){

		char *word = temp->data;
		array[i] = word;

		// illiterate through list
		temp = temp->next;
	}
}

// fill up hash table
void fill_hashtable(HashTable *H, List *l){
	// index of words in dictionary
	int index = 0;

	// loop List
	Node *temp = l->head;
	assert(temp != NULL);

	while(temp != NULL){
		char *word = temp->data;
		hash_table_put(H, word, index);

		// point to next word
		temp = temp->next;
		index ++;
	}
}

void print_prediction(char *w1, char *w2){

	// output for predictions
	if( w1 != NULL){
		printf("%s\n", w1);
	}

	// when no words matches
	else{
		printf("%s?\n", w2);
	}
}

// free array of chars
void free_array(char ** A, int size){
	int i;

	// free individual words
	for(i=0; i<size; i++){
		A[i] = NULL;
		free(A[i]);
	}
	// free the array
	free(A);
}

/* search all predictions, and get the word with lowest index,
 * returns -1 for no prediction found */
int search_prediction(char **A, int size, HashTable *H){
	int i, ans = -1;
	int newindex, oldindex;

	for(i=0; i<size; i++){
		if(hash_table_has(H, A[i])){

			newindex = hash_table_get(H, A[i]);

			// first value
			if(ans == -1){
				ans  = i;
				oldindex = newindex;
			}

			// when found a new word with lower index
			else if(oldindex > newindex){
				ans = i;
				oldindex = newindex;
			}
		}
	}
	return ans;
}

// search all words in dictionary for edit dist 2 and 3
void brute_force(char **dic, char *word, int size, char **predict){
	
	int i;
	char *newword = NULL;

	// if prediction is already found
	if(*predict != NULL){
		return;
	}

	// iterate through the dictionary
	for(i=0; i<size; i++){

		// ignore words that are too long or too short
		if(strlen(dic[i]) > strlen(word) + MAX_LEN || 
			strlen(dic[i]) < strlen(word)- MAX_LEN) {}

		else{
			int ed = edit_dist(word, dic[i]);	
			// edit distance 2
			if(ed == 2){
				newword = dic[i];
				break;
			}
			// edit distance 3
			else if(ed == 3 && newword == NULL){
				newword = dic[i];
			}
		}	
	}
	// return found word
	*predict = newword;
}

// search all possible words for edit distance 1
void search_edit_dist_1(char *word, char **A, HashTable *H, char **predict){

	int i = 0;
	int len  = strlen(word);
	int size = possibilities(len);

	// create all possible ED 1 words, stores it in A
	methods_edit_dist(word, len, A, &i);

	// search all ED 1 predictions, use the one with the smallest index
	int ans = search_prediction(A, size, H);

	// edit distance 1 word match
	if(ans != -1){
		*predict = A[ans];
	}
}

/* Algorithms are fun :D
 * imma try my luck anyway */