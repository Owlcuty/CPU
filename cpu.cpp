#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "stack.h"
#include "dump.c"

typedef int Com_t;
typedef double Elem_t;

enum CpuErrors // to file progerrors.h
{
  FileDetectErr = 2,
  FileSeekErr,
  FileTellErr,
  CallocErr,
  InvalidInputData,
  NoDataInStack,
  StackOverflow,
  DivByZero,
  SqrtNegative
};

enum Registers {
  AX,
  BX,
  CX,
  DX
};

typedef struct
{
  const char* name;
  const int num_arg;
} AssmCommand;

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

const char* userFriendlyErrs[] =
{
  "",
  "",
  "%i::%s:: Error file detecting" "\n",
  "%i::%s:: Error fseek FILE{'%s'}" "\n",
  "%i::%s:: Error ftell FILE{'%s'}" "\n",
  "%i::%s:: Error calloc" "\n",
  "%i::%s:: Expected {int, ..., double}" "\n",
  "%i::%s:: Expected data in 'heap'(Stack_t cpu)" "\n",
  "%i::%s:: Can't add value. StackOverflow" "\n",
  "%i::%s:: Division by zero" "\n"
};

const char* FileName = "bytecode.din";

void retCodeErrDestr(FILE* fileToClose, const char* fileName, Stack_t* stToDestr, int codeErr, int LINE);

int run_code(char* code, char* end, Stack_t* cpu, Elem_t* ax, Elem_t* bx, Elem_t* cx, Elem_t* dx);
int cpu_in(Stack_t* cpu);
int cpu_out(Stack_t* cpu);
int cpu_push(Stack_t* cpu, Elem_t value);
int cpu_pop(Stack_t* cpu);
int cpu_add(Stack_t* cpu);
int cpu_sub(Stack_t* cpu);
int cpu_mul(Stack_t* cpu);
int cpu_div(Stack_t* cpu);
int cpu_sqr(Stack_t* cpu);
int cpu_sqrt(Stack_t* cpu);
int cpu_pushRx(Stack_t* cpu, const Elem_t registr);
int cpu_popRx(Stack_t* cpu, Elem_t* registr);
int cpu_sin(Stack_t* cpu);
int cpu_cos(Stack_t* cpu);
int cpu_jmp(Stack_t* cpu, char* code);
// Add prototypes of funcs (Asm commands)

int (*cpu_uno_func[])(Stack_t* cpu) = {
  NULL,
  cpu_in,
  cpu_out,
  NULL,
  cpu_pop,
  cpu_add,
  cpu_sub,
  cpu_mul,
  cpu_div,
  cpu_sqr,
  cpu_sqrt,
  NULL,
  NULL,
  cpu_sin,
  cpu_cos,
  NULL
};

int main(int argc, char* argv[])
{
  INIT_STRUCT(Stack_t, cpu);
  cpu.Create(10);

  const char* CodeFileName = argv[1]; // File with code

  FILE * bytecode;
  if (!(bytecode = fopen(CodeFileName, "r")))
  {
    retCodeErrDestr(bytecode, CodeFileName, &cpu, FileDetectErr, __LINE__);
  }

  size_t codeLen = 0;
  if (fseek(bytecode, 0, SEEK_END) < 0)
  {
    retCodeErrDestr(bytecode, CodeFileName, &cpu, FileSeekErr, __LINE__);
  }
  if ((codeLen = ftell(bytecode)) < 0)
  {
    retCodeErrDestr(bytecode, CodeFileName, &cpu, FileTellErr, __LINE__);
  }
  if (fseek(bytecode, 0, SEEK_SET) < 0)
  {
    retCodeErrDestr(bytecode, CodeFileName, &cpu, FileSeekErr, __LINE__);
  }

  char* code;
  if (!(code = (char*)(calloc(codeLen, sizeof(*code)))))
  {
    retCodeErrDestr(bytecode, CodeFileName, &cpu, CallocErr, __LINE__);
  }

  fread(code, sizeof(*code), codeLen, bytecode); // add checking later!!

  Elem_t ax, bx, cx, dx;

  if (run_code(code, code + codeLen, &cpu, &ax, &bx, &cx, &dx))
  {
    return -1;
    /* retCodeErrDestr */
  }

  cpu.Destruct();
}

int run_code(char* code, char* end, Stack_t* cpu, Elem_t* ax, Elem_t* bx, Elem_t* cx, Elem_t* dx)
{
  int command = 0;
  Elem_t arg = 0;
  while (*((Com_t*)code) || (end - code > sizeof(Com_t) && *((Elem_t*)code)))
  {
    printf("\n");
    printf("%i:: ------------------------" "\n", __LINE__);
    printf("%i:: ax{%lf}, bx{%lf}, cx{%lf}, dx{%lf}" "\n", __LINE__, *ax, *bx, *cx, *dx);
    printf("%i:: ---- END ---------------" "\n", __LINE__);
    stackDump(cpu);
    command = *((Com_t*)code);
    code += sizeof(Com_t);
    if (cpu_uno_func[command])
    {
      (cpu_uno_func[command]) (cpu);
    }
    else
    {
      arg = *((Elem_t*)code);
      code += sizeof(Elem_t);
      switch (command)
      {
        case Com_push:
          printf("%i:: PUSH VALUE" "\n", __LINE__);
          cpu_push(cpu, arg);
          break;
        case Com_pushRx:
          printf("%i:: We're here" "\n", __LINE__);
          switch ((int)arg)
          {
            case AX:
              printf("%i:: <-<-<-<- AX. {%lf}" "\n", __LINE__, *ax);
              cpu_pushRx(cpu, *ax);
              break;
            case BX:
              printf("%i:: <-<-<-<- BX. {%lf}" "\n", __LINE__, *bx);
              cpu_pushRx(cpu, *bx);
              break;
            case CX:
              printf("%i:: <-<-<-<- CX" "\n", __LINE__);
              cpu_pushRx(cpu, *cx);
              break;
            case DX:
              printf("%i:: <-<-<-<- DX" "\n", __LINE__);
              cpu_pushRx(cpu, *dx);
              break;
          }
          break;
        case Com_popRx:
          printf("%i:: POPRX. Arg = %lf. (int)Arg = %d" "\n", __LINE__, arg, (int)arg);
          switch ((int)arg)
          {
            case AX:
              printf("%i:: ->->->-> AX" "\n", __LINE__);
              cpu_popRx(cpu, ax);
              break;
            case BX:
              printf("%i:: ->->->-> BX" "\n", __LINE__);
              cpu_popRx(cpu, bx);
              break;
            case CX:
              printf("%i:: ->->->-> CX" "\n", __LINE__);
              cpu_popRx(cpu, cx);
              break;
            case DX:
              printf("%i:: ->->->-> DX" "\n", __LINE__);
              cpu_popRx(cpu, dx);
              break;
            default:
              printf("%i:: ARG = %lf" "\n", __LINE__, arg);
              break;
          }
          break;
        default:
          printf("%i:: Wrong switch. Command = %d" "\n", __LINE__, command);
          break;
      }
    }
  }

  printf("%i:: Compile successed" "\n", __LINE__);
  return 0;
}

int cpu_in(Stack_t* cpu)
{
  Elem_t in_value = 0;
  if (!scanf("%lf", &in_value))
  {
    return InvalidInputData;
  }

  cpu->Push(in_value);

  return 0;
}

int cpu_out(Stack_t* cpu)
{
  Elem_t out_value = 0;
  printf("%i:: Here" "\n", __LINE__);

  if (!cpu->Pop(&out_value))
  {
    return NoDataInStack;
  }

  printf("%lf\n", out_value);

  return 0;
}

int cpu_push(Stack_t* cpu, Elem_t value)
{
  printf("%i:: Arg = %lf" "\n", __LINE__, value);
  if (cpu->Push(value) != -1)
  {
    return StackOverflow;
  }

  stackDump(cpu);

  return 0;
}

int cpu_pop(Stack_t* cpu)
{
  Elem_t empty = 0;

  if (!cpu->Pop(&empty))
  {
    return NoDataInStack;
  }

  return 0;
}

int cpu_add(Stack_t* cpu)
{
  Elem_t var1 = 0,
         var2 = 0;

  if (!cpu->Pop(&var2))
  {
    return NoDataInStack;
  }

  if (!cpu->Pop(&var1))
  {
    return NoDataInStack;
  }

  if (cpu->Push(var1 + var2) != -1)
  {
    return StackOverflow;
  }

  return 0;
}

int cpu_sub(Stack_t* cpu)
{
  printf("%i:: Here" "\n", __LINE__);
  Elem_t var1 = 0,
         var2 = 0;

  if (!cpu->Pop(&var2))
  {
    return NoDataInStack;
  }

  printf("%i:: Var2 = %lf" "\n", __LINE__, var2);

  if (!cpu->Pop(&var1))
  {
    return NoDataInStack;
  }

  printf("%i:: Var1 = %lf" "\n", __LINE__, var1);

  if (cpu->Push(var1 - var2) != -1)
  {
    return StackOverflow;
  }

  return 0;
}

int cpu_mul(Stack_t* cpu)
{
  Elem_t var1 = 0,
         var2 = 0;

  if (!cpu->Pop(&var2))
  {
    return NoDataInStack;
  }

  if (!cpu->Pop(&var1))
  {
    return NoDataInStack;
  }

  if (cpu->Push(var1 * var2) != -1)
  {
    return StackOverflow;
  }

  return 0;
}

int cpu_div(Stack_t* cpu)
{
  Elem_t var1 = 0,
         var2 = 0;

  if (!cpu->Pop(&var2))
  {
    return NoDataInStack;
  }

  if (!cpu->Pop(&var1))
  {
    return NoDataInStack;
  }

  if (!var2)
  {
    return DivByZero;
  }

  if (cpu->Push(var1 / var2) != -1)
  {
    return StackOverflow;
  }

  return 0;
}

int cpu_sqr(Stack_t* cpu)
{
  Elem_t var = 0;

  if (!cpu->Pop(&var))
  {
    return NoDataInStack;
  }

  if (cpu->Push(var * var) != -1)
  {
    return StackOverflow;
  }

  return 0;
}

int cpu_pushRx(Stack_t* cpu, const Elem_t registr)
{
  if (!cpu->Push(registr))
  {
    return NoDataInStack;
  }

  return 0;
}

int cpu_sqrt(Stack_t* cpu)
{
  Elem_t var = 0;

  if (!cpu->Pop(&var))
  {
    return NoDataInStack;
  }

  if (var < 0)
  {
    return SqrtNegative;
  }

  if (cpu->Push(sqrt(var)) != -1)
  {
    return StackOverflow;
  }

  return 0;
}

int cpu_popRx(Stack_t* cpu, Elem_t* registr)
{
  Elem_t value = 0;
  printf("%i:: Here" "\n", __LINE__);

  if (!cpu->Pop(&value))
  {
    return NoDataInStack;
  }
  printf("%i:: POPVAL = %lf" "\n", __LINE__, value);

  *registr = value;

  return 0;
}

int cpu_sin(Stack_t* cpu)
{
  Elem_t value = 0;

  if (!cpu->Pop(&value))
  {
    return NoDataInStack;
  }

  if (cpu->Push(sin(value)) != -1)
  {
    return StackOverflow;
  }

  return 0;
}

int cpu_cos(Stack_t* cpu)
{
  Elem_t value = 0;

  if (!cpu->Pop(&value))
  {
    return NoDataInStack;
  }

  if (cpu->Push(cos(value)) != -1)
  {
    return StackOverflow;
  }

  return -1;
}

int cpu_jmp(Stack_t* cpu, char* code)
{
  //jmp to place in code
}






void retCodeErrDestr(FILE* fileToClose, const char* fileName, Stack_t* stToDestr, int codeErr, int LINE)
{
  printf(userFriendlyErrs[codeErr], LINE, __FILE__, fileName);

  if (fileToClose)
  {
    fclose(fileToClose);
  }
  stToDestr->Destruct();
}
