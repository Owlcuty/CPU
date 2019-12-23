#ifndef INIT_STRUCT
  #define INIT_STRUCT(Struct_t, structure) Struct_t structure = {#structure}
#endif
#ifndef INIT_STRUCT
  #define INIT_STRUCT(Struct_t, structure) Struct_t structure = {};
#endif
#ifndef NAME_OF_ARG_DEF
  #define NAME_OF_ARG_DEF(ARG) #ARG
#endif

#include <assert.h>

const unsigned long CANARY_VAL = 0xfff'acf'afc'daf'fcf;

enum Values: unsigned long
{
  POISON_VAL = 0xfcd'acf'fac'fff
};

typedef double Elem_t;
typedef unsigned long canary_t;

const int StartLenData = 255;
const int Up   = 1,
          Down = 0;

enum Stack_errors
{
  STACK_UNDERFLOW,
  STACK_OVERFLOW,
  INVALID_DATA
};

struct Stack_t
{
  const char* stack_name;

#ifdef STACK_ANALYSIS
  canary_t canary1_struct = CANARY_VAL;
#endif

  unsigned int size;
#ifdef STACK_ANALYSIS
  canary_t* canary1_buf;
  char* buf;
  canary_t* canary2_buf;
#endif
  Elem_t* data;
  size_t max_size;
  int err;

  int Ok();
  int Create(size_t max_size);
  int Resize(); // (int updown)
  int Push(Elem_t val);
  Elem_t Top();
  int Pop(Elem_t* val);
  int Destruct();

#ifdef STACK_ANALYSIS
  canary_t canary2_struct = CANARY_VAL;
#endif

};

int setPoisons(Elem_t* data, size_t num_elem);
const char* strErr(int err);

int setPoisons(Elem_t* data, size_t num_elem)
{
  for (size_t el = 0; el < num_elem; el ++)
  {
    data[el] = POISON_VAL;
  }
  return 1;
}

// -- Stack ----------------------------------------------------------

int Stack_t::Ok()
{
  if (!data)
    return 0;

  if (max_size < size)
    return 0;

#ifdef STACK_ANALYSIS
  assert(canary1_buf);
  assert(canary2_buf);
  assert(*(canary1_buf) == CANARY_VAL);
  assert(*(canary2_buf) == CANARY_VAL);
#endif

  return 1;
}

int Stack_t::Create(size_t max_size)
{
  if (data)
  {
    free(data);
  }

	printf("%i:: Max_size = %ld" "\n", __LINE__, max_size);
	if (!max_size)
  	max_size += !max_size * StartLenData;
	this->max_size = max_size;

#ifdef STACK_ANALYSIS
  if ( !(buf = (char*)(calloc(max_size * sizeof(*data) + sizeof(canary_t) * 2, sizeof(*buf)))) )
  {
    return -1;
  }
  canary1_buf  = (canary_t*) buf;
  *canary1_buf = CANARY_VAL;
  canary2_buf  = (canary_t*)(buf + sizeof(canary_t) + max_size * sizeof(*data));
  *canary2_buf = CANARY_VAL;
  data = (Elem_t*)(buf + sizeof(canary_t));
#else
  if ( !(data = (Elem_t*)(calloc(max_size, sizeof(*data)))))
  {
    return -1;
  }
#endif
  setPoisons(data, max_size);

  printf("%i:: Created stack[%u//%lu.<->.%zu]" "\n", __LINE__, size, max_size, (size_t)data);
  return 1;
}

int Stack_t::Resize() // (int updown)
{
  assert(Ok());

  return 1;
  /* if ( !(data = (Elem_t*)realloc(data, (updown ? (max_size *= 2) : (max_size /= 2)) * sizeof(*data)) ) )
  {
    return ERRCALLOC;
  } */

  assert(Ok());
}

int Stack_t::Push(Elem_t val)
{
  assert(Ok());

  if (size + 1 > max_size)
  {
    err = STACK_OVERFLOW;
    return STACK_OVERFLOW;
    // Resize(Up);
  }

  data[size++] = val;

  return -1;

  assert(Ok());
}

Elem_t Stack_t::Top()
{
  assert(Ok());

  return data[size - 1];
}

int Stack_t::Pop(Elem_t* val)
{
  assert(Ok());

  if (size < 1)
  {
    err = STACK_UNDERFLOW;
    return 0;
  }
  if (size - 1 < max_size / 2)
  {
    // Resize(Down);
  }

  *val = data[--size];

  assert(Ok());
  return 1;
}

int Stack_t::Destruct()
{
#ifdef STACK_ANALYSIS
  free(buf);
#else
  free(data);
#endif
  return 1;
}


const char* strErr(int err)
{
  switch(err)
  {
    case STACK_UNDERFLOW:
      return "STACK UNDERFLOW";
      break;
    case STACK_OVERFLOW:
      return "STACK OVERFLOW";
      break;
    case INVALID_DATA:
      return "INVALID DATA";
      break;
    default:
      return "UNKNOWN ERROR";
      break;
  }
}



