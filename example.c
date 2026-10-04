#define STRIP

#include "cinq.h"
#include <stdio.h>

void main(void){
	int *numbers = NULL;
	for(int i=0; i < 10; i++) { append(numbers,i); }

	each(numbers, x, i, {
   		printf("%zu: %d\n", i, x);
	});

	printf("n * 3: \n");
	each(
		numbers = select(numbers, y, (y * 3)),
	z, printf("%d ", z));
	printf("\n");

	printf("List contains evens? %s\n", any(numbers, x, x % 2 == 0) ? "Yes" : "No");
	printf("List contains only evens? %s\n", all(numbers, x, x % 2 == 0) ? "Yes" : "No");

	printf("evens: \n");
	each(where(numbers, x, x % 2 == 0), y, printf("%d ", y));
	printf("\n");

	printf("first 5:\n");
	each(take(numbers,5), y, printf("%d ", y));
	printf("\n");

	printf("slice 4-6:\n");
    each(slice(numbers,4,6), y, printf("%d ", y));
    printf("\n");

	printf("a & b:\n");
	int* a = NULL;
	int* b = NULL;
	int* c = NULL;

	append(a, 1);
	append(a, 1);
	append(a, 1);
	append(b, 2);
	append(b, 2);
	append(b, 2);

	each(
		c = union(a,b),
	y, printf("%d ", y));
	printf("\n");

	each(distinct(c), y, printf("%d ", y));
	printf("\n");

}
