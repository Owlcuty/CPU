#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <limits.h>

enum {
  MAX_LENGTH_LEN = 256,
  MAX_INT_VALUE  = INT_MAX,
  EPS_POISON_VAL = 1000
};

void stackDump(Stack_t* st);

void stackDump(Stack_t* st)
{
  assert(st);

  printf("Stack_t '%s' [%p]\n", st->stack_name, st);
  printf("{\n");

  printf("\t" "errcode = %d\n", st->err);

  printf("\t" "size = %u\n", st->size);

  printf("\t" "data[%lu] = [%p]\n", st->max_size, st->data);
  printf("\t" "{\n");
  for (size_t el = 0; el < st->max_size; el ++)
  {
    printf("\t\t");
    if (el < st->size) printf("*");
    printf("[%lu] = %lf", el, st->data[el]);
    if (st->data[el] > MAX_INT_VALUE) printf(" (POISON ?)\n");
    else printf("\n");
  }
  printf("\t" "}\n");
  printf("}\n");
}
