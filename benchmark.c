#include <sys/time.h>
#include <stdio.h>
#include "hashdict.h"

int time1000() {
	struct timeval val;
	gettimeofday(&val, 0);
	val.tv_sec &= 0xffff;
	return val.tv_sec * 1000 + val.tv_usec / 1000;
}

int main() {
	struct dictionary* dic = dic_new(0);
	int T = time1000(), i, KEY_SIZE = 8;
	__int64_t STEPS = 100000, STEP = 500000000, key;
	key = 0;
	for (i = 0; i < STEPS; i++) {
		key += STEP;
		dic_add(dic, &key, KEY_SIZE);
		*dic->result = (key+55) & 0xff;
	}
	printf("insert time:%i\n",time1000() - T); T = time1000();fflush(0);
	int err = 0;
	key = 0;
	for (i = 0; i < STEPS; i++) {
		key += STEP;
		if (dic_find(dic, &key, KEY_SIZE)) {
			if (*dic->result != ((key+55) & 0xff)) err++;
		}
		else err++;
	}
	printf("find time:%i\n",time1000() - T); T = time1000();
	printf("errors:%i\n", err);fflush(0);
	dic_delete(dic);
	return 0;
}
