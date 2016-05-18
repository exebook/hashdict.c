#include "hashdict.h"

#define hash_func hash_mur_mur

uint32_t hash_mur_mur(const char* key, int count) {
	#define muR(x, r) (((x) << r) | ((x) >> (32 - r)))
	int n = count >> 2;
	uint32_t k, seed = 0x8e96a8f2, mur = 0x8e96a8f2, c1 = 0xcc9e2d51, c2 = 0x1b873593,
	*b = (uint32_t*) (key + n * 4);
	for(int i = -n; i != 0; i++) {
		k = (muR(b[i] * c1, 15) * c2);
		mur = muR(mur^k, 13)*5+0xe6546b64;
	}
	uint8_t * tail = (uint8_t*)(key + (n<<2));
	k = 0;
	switch(count & 3) {
		case 3: k ^= tail[2] << 16;
		case 2: k ^= tail[1] << 8;
		case 1: k ^= tail[0];
		k = mur ^ muR(k*c1,15)*c2;
	}
	mur ^= count;
	mur ^= (mur ^ mur >> 16) * 0x85ebca6b >> 13;
	return mur ^ (mur * 0xc2b2ae35 >> 16);
	#undef muR
}

struct keynode *keynode_new(char*k, int l) {
	struct keynode *node = malloc(sizeof(struct keynode));
	node->len = l;
	node->key = malloc(l);
	memcpy(node->key, k, l);
	node->next = 0;
	node->value = -1;
	return node;
}

void keynode_delete(struct keynode *node) {
	free(node->key);
	if (node->next) keynode_delete(node->next);
	free(node);
}

int node_cmp(struct keynode *node, char*k, int l) {
	if (node->len != l) return 0;
	return memcmp(node->key, k, l) == 0;
}

struct dictionary* dic_new(int initial_size) {
	struct dictionary* dic = malloc(sizeof(struct dictionary*));
	if (initial_size == 0) initial_size = 1024;
	dic->length = initial_size;
	dic->count = 0;
	dic->table = calloc(sizeof(struct keynode*), initial_size);
	return dic;
}

void dic_delete(struct dictionary* dic) {
	for (int i = 0; i < dic->length; i++) {
		if (dic->table[i])
			keynode_delete(dic->table[i]);
	}
	free(dic->table);
	dic->table = 0;
	free(dic);
}

void dic_reinsert_when_resizing(struct dictionary* dic, struct keynode *k2) {
	int n = hash_func(k2->key, k2->len) % dic->length;
	if (dic->table[n] == 0) {
		dic->table[n] = k2;
		dic->result = &dic->table[n]->value;
		return;
	}
	struct keynode *k = dic->table[n];
	k2->next = k;
	dic->table[n] = k2;
	dic->result = &k2->value;
}

void dic_resize(struct dictionary* dic, int newsize) {
	int o = dic->length;
	struct keynode **old = dic->table;
	dic->table = calloc(sizeof(struct keynode*), newsize);
	dic->length = newsize;
	for (int i = 0; i < o; i++) {
		struct keynode *k = old[i];
		while (k) {
			struct keynode *next = k->next;
			k->next = 0;
			dic_reinsert_when_resizing(dic, k);
			k = next;
		}
	}
	free(old);
}

int dic_add(struct dictionary* dic, void *key, int keyn) {
	int n = hash_func((const char*)key, keyn) % dic->length;
	if (dic->table[n] == 0) {
		double f = (double)dic->count / (double)dic->length;
		if (f > HASHDICT_GROWTH_THRESHOLD) {
			dic_resize(dic, dic->length * HASHDICT_GROWTH_FACTOR);
			return dic_add(dic, key, keyn);
		}
		dic->table[n] = keynode_new((char*)key, keyn);
		dic->result = &dic->table[n]->value;
		dic->count++;
		return 0;
	}
	struct keynode *k = dic->table[n];
	while (k) {
		if (node_cmp(k, (char*)key, keyn)) {
			dic->result = &k->value;
			return 1;
		}
		k = k->next;
	}
	dic->count++;
	struct keynode *k2 = keynode_new((char*)key, keyn);
	k2->next = dic->table[n];
	dic->table[n] = k2;
	dic->result = &k2->value;
	return 0;
}

int dic_find(struct dictionary* dic, void *key, int keyn) {
	int n = hash_func((const char*)key, keyn) % dic->length;
	if (dic->table[n] == 0) return 0;
	struct keynode *k = dic->table[n];
	while (k) {
		if (node_cmp(k, (char*)key, keyn)) {
			dic->result = &k->value;
			return 1;
		}
		k = k->next;
	}
	return 0;
}

void dic_forEach(struct dictionary* dic, enumFunc f, void *user) {
	for (int i = 0; i < dic->length; i++) {
		if (dic->table[i] != 0) {
			struct keynode *k = dic->table[i];
			while (k) {
				if (!f(k->key, k->len, &k->value, user)) return;
				k = k->next;
			}
		}
	}
}
