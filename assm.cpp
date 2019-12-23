#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

const int MAXLEN = 255;

typedef double Elem_t;

enum AssmErrors
{
  FileDetectErr = 2,
  MemchrErr,
  ReadArg,
  SyntaxErr,
  MemoryLimit,
  LabelErr
};

enum Registers
{
  AX,
  BX,
  CX,
  DX
};

struct AssmCommand
{
  const char* name;
  const int numArg;
};

struct Labels
{
  void* head;
  char* name;
  size_t ptr_place;
};

Labels* label_init();
Labels* label_push(Labels* label, char* code_place, char* name, size_t ptr_place);
long label_get_address(Labels* labels, const char* nameLabel);

int destroyLabels(Labels* list);

const AssmCommand Commands[] = {
  {"end",  1}, //0
  {"in",   1}, //1
  {"out",  1}, //2
  {"push", 2}, //3
  {"pop",  1}, //4
  {"add",  1}, //5
  {"sub",  1}, //6
  {"mul",  1}, //7
  {"div",  1}, //8
  {"sqr",  1}, //9
  {"sqrt", 1}, //10
  {"push", 2}, //11
  {"pop",  2}, //12
  {"sin",  1}, //13
  {"cos",  1}, //14
  {"jmp",  2}  //15
};

enum Coms {
  Com_end,
  Com_in,
  Com_out,
  Com_push,
  Com_pop,
  Com_add,
  Com_sub,
  Com_mul,
  Com_div,
  Com_sqr,
  Com_sqrt,
  Com_pushRx,
  Com_popRx,
  Com_sin,
  Com_cos,
  Com_jmp,
  ComLastEmpty
};

int translator(char* origin, FILE* fbin);
Elem_t detArg(char* arg);

int main()
{
  FILE * asml;
  if (!(asml = fopen("assm.in", "r")))
  {
    printf("%i:: Error file detecting" "\n", __LINE__);
    return FileDetectErr; // Pleeeease, destroy it and THINK ABOUT FUNCTIONS!!!
  }

  FILE * bytecode;
  if (!(bytecode = fopen("bytecode.din", "w")))
  {
    printf("%i:: Error file creating/writting" "\n", __LINE__);
  }
  long asmLen = 0;
  fseek(asml, 0, SEEK_END);

  if ((asmLen = ftell(asml)) < 0)
  {
    printf("%i:: Void file" "\n", __LINE__);
    return 1;
  }
  fseek(asml, 0, SEEK_SET);

  char* origin = (char*)(calloc((size_t)asmLen + 1, sizeof(*origin)));
  fread(origin, sizeof(*origin), (size_t)asmLen, asml);

  if (translator(origin, bytecode))
  {
    return 0;
  }

  free(origin);
  fclose(asml);
  fclose(bytecode);
}

int translator(char* origin, FILE* fbin)
{
  size_t slen = strlen(origin);
  char command[MAXLEN];
  char arg[MAXLEN];
  int num_arg = 0;
  Elem_t val_arg = 0;
  int curLen = 1;
  size_t progcnt = 0;
  Labels* labels;
  if (!(labels = label_init()))
  {
    return MemoryLimit;
  }
  Labels* startLabel = labels;

  for (char* s = origin; s < origin + slen; s ++, curLen ++)
  {
    num_arg = sscanf(s, "%s%*[ ]%s", command, arg);
    progcnt += (size_t)num_arg;
    switch (num_arg)
    {
      case 0:
        return ReadArg;
      case 1:
        if (strlen(command) >= 2 && command[strlen(command) - 1] == ':')
        {
          if (!(labels = label_push(labels, s, command, progcnt)))
          {
            printf("%i:: Error! Memory Limit?" "\n", __LINE__);
            return MemoryLimit;
          }
          break;
        }
        for (int el = 0; el < 16; el ++)
        {
          if (!strcmp(command, Commands[el].name))
          {
            fwrite(&el, sizeof(int), 1, fbin);
            if (!el)
            {
              printf("%i:: Compiled!" "\n", __LINE__);
              destroyLabels(startLabel);
              free(startLabel); // <- destroyList(startLabel);
              return 0;
            }
            break;
          }
          if (el == ComLastEmpty - 1)
          {
            printf("::assm.cpp::%i::->:: assm.in::%i:: Wrong command" "\n", __LINE__, curLen); // assm.in -> FILENAME(FILE * file)
            return SyntaxErr;
          }
        }
        break;
      case 2:
        for (int el = 0; el < ComLastEmpty; el ++)
        {
          if (!strcmp(command, Commands[el].name))
          {
            if ((strlen(arg) == 2 && arg[1] == 'x') && el < 10) continue;
            fwrite(&(el), sizeof(int), 1, fbin);
            val_arg = (el != Com_jmp) ? detArg(arg) : label_get_address(startLabel, arg);
            if (val_arg < 0)
            {
              printf("%i:: Wrong label" "\n", __LINE__);
              return LabelErr;
            }
            fwrite(&val_arg, sizeof(Elem_t), 1, fbin);
            break;
          }
          if (el == ComLastEmpty - 1)
          {
            printf("::assm.cpp::%i::->:: assm.in::%i:: Wrong command" "\n", __LINE__, curLen); // assm.in -> FILENAME(FILE * file)
            return SyntaxErr;
          }

        }
        break;
      default:
        printf("::assm.cpp::%i::->:: assm.in::%i:: Wrong command" "\n", __LINE__, curLen); // assm.in -> FILENAME(FILE * file)
        return SyntaxErr;
    }
    if ((s = (char*)(memchr(s, '\n', slen))) == NULL)
    {
      printf("%i:: Wrong memchr\n%s" "\n", __LINE__, s);
      return MemchrErr;
    }
  }
  return 0;
}

Elem_t detArg(char* arg)
{
  if ('0' <= arg[0] && arg[0] <= '9')
  {
    return atof(arg);
  }
  else
    return arg[0] - 'a';
}

Labels* label_init()
{
  Labels* label = (Labels*)(calloc(32, sizeof(*label)));
  if (!label)
    return NULL;

  for (int el = 0; el < 31; el ++)
  {
    label[el].head = (void*)(&(label[el + 1]));
  }

  return label;
}

Labels* label_push(Labels* label, char* code_place, char* name, size_t ptr_place)
{
  if (label->head)
  {
    label->name = (char*)(calloc(strlen(name) + 1, sizeof(*(label->name))));
    sscanf(code_place, "%s", label->name);
    label->ptr_place = ptr_place;
    return (Labels*)label->head;
  }
  else
  {
    static Labels* next_labels;
    if (!(next_labels = (Labels*)(calloc(64, sizeof(*next_labels)))))
    {
      return 0;
      // return MemoryLimit;
    }
    for (int el = 0; el < 63; el ++)
    {
      next_labels[el].head = (void*)(&(next_labels[el + 1]));
    }
    label->head = (void*)next_labels;
    next_labels[0].name = name;
    next_labels[0].ptr_place = ptr_place;
    return next_labels;
  }
  // return 0;
}

long label_get_address(Labels* labels, const char* nameLabel)
{
  assert(labels);

  for (Labels* label = labels; label && label->name; label = (Labels*)(label->head))
  {
    if (!(strcmp(label->name, nameLabel)))
    {
      return (long)label->ptr_place;
    }
  }
  return -1;
}

int destroyLabels(Labels* list)
{
  for (Labels* label = list; label; label = (Labels*)(label->head))
  {
    free(label->name);
  }
  return 1;
}
