#include <stdio.h>
#include <string.h>
#include "../core/storm.h"
#include "../core/format.h"
#include "../core/monad.h"
#include "../core/alloc.h"
#include "../core/vm.h"
#include "parse.h"
#include <time.h>

#include <stdlib.h>

#define LINE_SIZE 2048

int main()
{
    storm_alloc_init();

    i8_t run = 1;
    str_t line = (str_t)storm_malloc(LINE_SIZE), ptr;
    memset(line, 0, LINE_SIZE);
    value_t value;
    vm_t vm;
    u8_t *code;

    vm = vm_create();

    while (run)
    {

        printf(">");
        ptr = fgets(line, LINE_SIZE, stdin);
        UNUSED(ptr);

        value = parse("REPL", line);

        str_t buf = value_fmt(&value);
        printf("%s\n", buf);

        code = compile(value);
        vm_exec(vm, code);

        value_free(&value);
    }

    storm_free(line);
    vm_free(vm);

    storm_alloc_deinit();

    return 0;
}
