#include "cinq.h"
#include <stdio.h>

void main(void){
	int *numbers = NULL;
	for(int i=0; i < 10; i++) { cinq_append(numbers,i); }

	cinq_each(numbers, x, i, {
   		printf("%zu: %d\n", i, x);
	});

	printf("n * 3: \n");
	cinq_each(
		numbers = cinq_select(numbers, y, (y * 3)),
	z, printf("%d ", z));
	printf("\n");

	printf("List contains evens? %s\n", cinq_any(numbers, x, x % 2 == 0) ? "Yes" : "No");
	printf("List contains only evens? %s\n", cinq_all(numbers, x, x % 2 == 0) ? "Yes" : "No");

	printf("evens: \n");
	cinq_each(cinq_where(numbers, x, x % 2 == 0), y, printf("%d ", y));
	printf("\n");

	printf("first 5:\n");
	cinq_each(cinq_take(numbers,5), y, printf("%d ", y));
	printf("\n");

	printf("slice 4-6:\n");
    cinq_each(cinq_slice(numbers,4,6), y, printf("%d ", y));
    printf("\n");

	printf("a & b:\n");
	int* a = NULL;
	int* b = NULL;
	int* c = NULL;

	cinq_append(a, 1);
	cinq_append(a, 1);
	cinq_append(a, 1);
	cinq_append(b, 2);
	cinq_append(b, 2);
	cinq_append(b, 2);

	cinq_each(
		c = cinq_union(a,b),
	y, printf("%d ", y));
	printf("\n");

	cinq_each(cinq_distinct(c), y, printf("%d ", y));
	printf("\n");

}
