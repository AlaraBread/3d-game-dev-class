#include "gfc_hashmap.h"

void hashmap_test() {
	GFC_HashMap *map = gfc_hashmap_new();
	gfc_hashmap_insert(map, "a", (void *)1);
	gfc_hashmap_insert(map, "ab", (void *)2);
	gfc_hashmap_insert(map, "a", (void *)2);
	gfc_hashmap_insert(map, "a", (void *)2);
	gfc_hashmap_insert(map, "a", (void *)2);
	gfc_hashmap_insert(map, "a", (void *)2);
	gfc_hashmap_insert(map, "a", (void *)2);
	gfc_hashmap_insert(map, "a", (void *)3);
	long edgeCount = (long)gfc_hashmap_get(map, "a");
	gfc_hashmap_insert(map, "a", (void *)(edgeCount + 1));
}
