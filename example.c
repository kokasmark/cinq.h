#include <stdio.h>

#define CINQ_STRIP

#include "cinq.h"

int main(void){
	int *numbers = NULL;
	for(int i = 0; i < 10; i++) { append(numbers, i); }

	each(numbers, x, i, {
		printf("%zu: %d\n", i, x);
	});

	printf("n * 3: \n");
	chain(numbers,
		select(y, y * 3),
		each(z, printf("%d ", z))
	);
	printf("\n");

	printf("List contains evens? %s\n", any(numbers, x, x % 2 == 0) ? "Yes" : "No");
	printf("List contains only evens? %s\n", all(numbers, x, x % 2 == 0) ? "Yes" : "No");
	printf("List contains 7? %s\n", chain(numbers, contains(7)) ? "Yes" : "No");

	printf("evens: \n");
	chain(numbers,
		where(x, x % 2 == 0),
		each(y, printf("%d ", y))
	);
	printf("\n");

	printf("first 5:\n");
	chain(numbers,
		take(5),
		each(y, printf("%d ", y))
	);
	printf("\n");

	printf("slice 4-6:\n");
	chain(numbers,
		slice(4, 6),
		each(y, printf("%d ", y))
	);
	printf("\n");

	printf("a & b:\n");
	int *a = NULL;
	int *b = NULL;

	append(a, 1);
	append(a, 1);
	append(a, 1);
	append(b, 2);
	append(b, 2);
	append(b, 2);

	chain(a,
		union(b),
		each(y, printf("%d ", y)));
	printf("\n");

	printf("distinct a & b:\n");
	chain(a,
		union(b),
		distinct(),
		each(y, printf("%d ", y)));
	printf("\n");

	printf("even numbers * 3, first 3:\n");
	int *result = chain(numbers,
		where(x, x % 2 == 0),
		select(x, x * 3),
		take(3));
	each(result, y, printf("%d ", y));
	printf("\n");

	list_free(result);
	list_free(numbers);
	list_free(a);
	list_free(b);
	return 0;
}