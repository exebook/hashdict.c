# hashdict.c

This is my REALLY FAST implementation of a hash table in C, in 150 lines of code.

For some reason it is more than twice as fast on my benchmarks as the hash table used in Redis.

The hash function used is my adaptation of MurMur.

Hash slot duplicates are stored as linked list.

There are few defines in the header file that you can edit to tune the table for your case, although the defaults are quite good.

The key is a combination of a pointer to bytes and a count of bytes.

The value type is defined as `int` in the header file, you can redefine it to the required type.
```c
#define HASHDICT_VALUE_TYPE int
```

##example

```c
#include <sys/time.h>
#include <stdio.h>
#include "hashdict.h"

int main() {
	struct dictionary* dic = dic_new(0);
	
	dic_add(dic, "ABC", 3);
	*dic->result = 100;
	
	dic_add(dic, "DE", 2);
	*dic->result = 200;
	
	dic_add(dic, "HJKL", 4);
	*dic->result = 300;
	
	if (dic_find(dic, "ABC", 3))
		printf("ABC found: %i\n", *dic->result);
	else printf("error\n");
	
	if (dic_find(dic, "DE", 2))
		printf("DE found: %i\n", *dic->result);
	else printf("error\n");
	
	if (dic_find(dic, "HJKL", 4))
		printf("HJKL found: %i\n", *dic->result);
	else printf("error\n");
	
	dic_delete(dic);
	return 0;
}
```

##dic_new()

Creates the hash table.

`struct dictionary* dic_new(int initial_size);`

set `initial_size` to 0 for the initial size of the table, which is 1024 items. Useful when you know how much keys you will store and want to preallocate, in which case use N/2 as the initial_size.

##dic_delete()

Deletes the hash table and frees all occupied memory.

`void dic_delete(struct dictionary* dic);`

##dic_add()

Adds a new key to the hash table.

Unlike most of implementations, you do NOT supply the value as the argument for the `add()` function. Instead after `dic_add()` returns, set the value like this: `*mydic->result = <VALUE>`.

`int dic_add(struct dictionary* dic, void *key, int keyn);`

Returns `true`(`1`) if the key was already in the table, otherwise `false`(`0`). In both cases you can change the associated value by referencing the `*dic->result` field.

##dic_find()

Lookup the key in the hash table. Return `true`(`1`) if found, the you can get the value like this: `myvalue = *dic->result`. 

`int dic_find(struct dictionary* dic, void *key, int keyn);`

##forEach()

Iterates over all keys in the table and calls the specified callback for each of them.

`void dic_forEach(struct dictionary* dic, enumFunc f, void *user);`

`typedef int (*enumFunc)(void *key, int count, int *value, void *user);`



