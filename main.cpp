#define INIT_STRUCT(Struct_t, structure) Struct_t structure = {#structure}
#ifndef INIT_STRUCT
	#define INIT_STRUCT(Struct_t, structure) Struct_t structure = {};
#endif
#define NAME_OF_ARG(ARG) #ARG
#define STACK_ANALYSIS

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "stack.h"
#include "dump.c"

int main()
{
	INIT_STRUCT(Stack_t, st);
  st.Create();
  for (int el = 0; el <= 21; el ++)
  {
		stackDump(&st);
    if (st.Push(el) >= 0)
		{
			printf("%i::Warning::%s" "\n", __LINE__, strErr(st.err));
			break;
		}
  }

		stackDump(&st);
  int* pops = (int*)(calloc(21, sizeof(*pops)));
	int num_el = 0;

  int temp = 0;
  for (int act = 0; act <= 21; act ++)
  {
    temp = st.Top();

    if (!st.Pop(&(pops[act])))
    {
      printf("%i::Warning::%s" "\n", __LINE__, strErr(st.err));
			break;
    }
		stackDump(&st);
		num_el ++;
  }

	for (int el = 0; el < num_el; el ++)
	{
		printf("%i:: Pops[%d] = %d" "\n", __LINE__, el, pops[el]);
	}

  st.Destruct();
  free(pops);

  //end_main
}



