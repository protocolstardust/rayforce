#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include "../core/storm.h"
#include "../core/format.h"
#include "../core/monad.h"
#include "../core/alloc.h"
#include "parse.h"

#define LINE_SIZE 2048

int main()
{
    a0_init();

    int run = 1;
    char *line = (char *)a0_malloc(LINE_SIZE);
    char *ptr;
    g0 value;
    Result res;

    while (run)
    {

        printf(">");
        ptr = fgets(line, LINE_SIZE, stdin);
        UNUSED(ptr);
        value = parse("REPL", line);

        res = g0_fmt(&line, value);
        if (res == Ok)
        {
            printf("%s\n", line);
        }

        result_fmt(&line, res);
        printf("Result: %s\n", line);

        g0_free(value);
    }

    a0_deinit();

    return 0;
}
