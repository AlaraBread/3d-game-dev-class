#include "asserts.h"
#include "lists.h"

void list_test() {
	IntList *list = newIntList();
	for(int i = 0; i < 20; i++) {
		intListAppend(list, i);
	}
	for(int i = 0; i < 20; i++) {
		ASSERT_EQ_I(intListGet(list, i), i);
	}
	intListFree(list);
}
