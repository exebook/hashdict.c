#include <stdlib.h> // malloc/calloc
#include <stdint.h>
#include <string.h> // memcpy/memcmp

/*
	Your callback for iteration of all keys
*/
typedef int (*enumFunc)(void *key, int count, int *value, void *user);

/*
	When to resize, for example 0.5 means <if number of inserted keys is half
	of the table length then resize>.
	
	My experiments on english dictionary shows balanced
	performance/memory savings with 1.0 to 2.0.
*/
#define HASHDICT_GROWTH_THRESHOLD 2.0

/*
	grow the size of hash table by N, suggested number is between 2 (conserve memory) and 10 (faster 		insertions).
*/
#define HASHDICT_GROWTH_FACTOR 10

typedef int (*enumFunc)(void *key, int count, int *value, void *user);

#define HASHDICT_VALUE_TYPE int

struct keynode {
	char *key;
	short len;
	struct keynode *next;
	HASHDICT_VALUE_TYPE value;
};
		
struct dictionary {
	struct keynode **table;
	int length, count;
	HASHDICT_VALUE_TYPE *result;
};

struct dictionary* dic_new(int initial_size);
void dic_delete(struct dictionary* dic);
int dic_add(struct dictionary* dic, void *key, int keyn);
int dic_find(struct dictionary* dic, void *key, int keyn);
void dic_forEach(struct dictionary* dic, enumFunc f, void *user);
