# hashdict.c

This is my REALLY FAST implementation of a hash table in C, in under 200 lines of code.

This is in fact a port of my [hashdic][cppversion] previously written in C++ for [jslike][jslike] project (which is a `var` class making programming in C++ as easy as in JavaScript).

[cppversion]: https://github.com/exebook/hashdic
[jslike]: https://github.com/exebook/jslike

For some reason it is more than twice as fast on my benchmarks as the [hash table][redisdictc] used in Redis. But unlike Redis version of a hash table there is no incremental resize.

The hash function used is my adaptation of [Meiyan][cmp2]/7zCRC, it is [better than MurMur3][cmp1].

[cmp1]: https://www.strchr.com/hash_functions
[cmp2]: http://www.sanmayce.com/Fastest_Hash/
[redisdictc]: https://github.com/antirez/redis/blob/unstable/src/dict.c

Hash slot duplicates are stored as linked list `node = node->next`.

##example

```c
#include <sys/time.h>
#include <stdio.h>
#include "hashdict.h"

int main() {
	struct dictionary* dic = dic_new(0);
	
	dic_add(dic, "ABC", 3);
	*dic->value = 100;
	
	dic_add(dic, "DE", 2);
	*dic->value = 200;
	
	dic_add(dic, "HJKL", 4);
	*dic->value = 300;
	
	if (dic_find(dic, "ABC", 3))
		printf("ABC found: %i\n", *dic->value);
	else printf("error\n");
	
	if (dic_find(dic, "DE", 2))
		printf("DE found: %i\n", *dic->value);
	else printf("error\n");
	
	if (dic_find(dic, "HJKL", 4))
		printf("HJKL found: %i\n", *dic->value);
	else printf("error\n");
	
	dic_delete(dic);
	return 0;
}
```
##dic_add()

Add a new key to the hash table.

Unlike most of implementations, you do NOT supply the value as the argument for the `add()` function. Instead after `dic_add()` returns, set the value like this: `*mydic->value = <VALUE>`.

`int dic_add(struct dictionary* dic, void *key, int keyn);`

Returns `true`(`1`) if the key was already in the table, otherwise `false`(`0`). In both cases you can change the associated value by referencing the `*dic->value` field after `dic_add()` returns.

##dic_find()

Lookup the key in the hash table. Return `true`(`1`) if found, the you can get the value like this: `myvalue = *dic->value`. 

`int dic_find(struct dictionary* dic, void *key, int keyn);`

##dic_new()

Create the hash table.

`struct dictionary* dic_new(int initial_size);`

Set `initial_size` to 0 for the initial size of the table, which is 1024 items. Useful when you know how much keys you will store and want to preallocate, in which case use N/growth_treshold as the initial_size. `growth_threshold` is 2.0 by default.

##dic_delete()

Delete the hash table and frees all occupied memory.

`void dic_delete(struct dictionary* dic);`

##forEach()

Iterates over all keys in the table and calls the specified callback for each of them.

`void dic_forEach(struct dictionary* dic, enumFunc f, void *user);`

`typedef int (*enumFunc)(void *key, int count, int *value, void *user);`


##tuning


#### growth (resize)
`struct dictionary` has tuning fields:

`growth_threshold`: when to resize, for example `0.5` means "if number of inserted keys is half of the table length then resize". Default: `2.0`;
	
My experiments on English dictionary shows balanced performance/memory savings with 1.0 to 2.0.

`growth_factor`: grow the size of hash table by N. Suggested number is between 2 (conserve memory) and 10 (faster insertions).

The key is a combination of a pointer to bytes and a count of bytes.

####value type

The value type is defined as `int` in the header file, you can redefine it to the required type, which could be `char` or `void*` or anything you need.

```c
#define HASHDICT_VALUE_TYPE int
```


