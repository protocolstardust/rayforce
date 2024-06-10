/*
 *   Copyright (c) 2024 Anton Kundenko <singaraiona@gmail.com>
 *   All rights reserved.

 *   Permission is hereby granted, free of charge, to any person obtaining a copy
 *   of this software and associated documentation files (the "Software"), to deal
 *   in the Software without restriction, including without limitation the rights
 *   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *   copies of the Software, and to permit persons to whom the Software is
 *   furnished to do so, subject to the following conditions:

 *   The above copyright notice and this permission notice shall be included in all
 *   copies or substantial portions of the Software.

 *   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *   SOFTWARE.
 */

#include <stdio.h>
#include <unistd.h>
#include "term.h"
#include "heap.h"
#include "string.h"
#include "error.h"
#include "ops.h"
#include "runtime.h"

#define COMMANDS_LIST "\
  :?  - Displays help.\n\
  :g  - Use rich graphic formatting: [0|1].\n\
  :q  - Exits the application: [exit code]."

term_p term_create()
{
    term_p term = (term_p)heap_mmap(sizeof(struct term_t));

    if (term == NULL)
        return NULL;

    // Set the terminal to non-canonical mode
    tcgetattr(STDIN_FILENO, &term->oldattr);
    term->newattr = term->oldattr;
    term->newattr.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &term->newattr);

    // Initialize the input buffer
    term->bufpos = 0;

    return term;
}

nil_t term_destroy(term_p term)
{
    // Restore the terminal attributes
    tcsetattr(STDIN_FILENO, TCSANOW, &term->oldattr);

    // Unmap the terminal structure
    heap_unmap(term, sizeof(struct term_t));
}

obj_p term_read(term_p term)
{
    c8_t c;
    obj_p res = NULL;
    i64_t exit_code;

    if (read(STDIN_FILENO, &c, 1) == 1)
    {
        // Process the character
        switch (c)
        {
        case '\n':
            // Enter key pressed, process the input buffer
            term->buf[term->bufpos] = '\0';

            if (strncmp(term->buf, ":q", 2) == 0)
            {
                if (term->bufpos > 2)
                    exit_code = i64_from_str(term->buf + 2, term->bufpos - 3);
                else
                    exit_code = 0;

                poll_exit(runtime_get()->poll, exit_code);
            }
            else if (strncmp(term->buf, ":?", 2) == 0)
            {
                printf("\n%s** Commands list:\n%s%s", YELLOW, COMMANDS_LIST, RESET);
                fflush(stdout);
                res = NULL_OBJ;
            }
            else
                res = (term->bufpos) ? cstring_from_str(term->buf, term->bufpos) : NULL_OBJ;

            term->bufpos = 0;
            printf("\n");
            fflush(stdout);

            return res;
        case '\b':
            // Backspace key pressed
            if (term->bufpos > 0)
            {
                term->bufpos--;
                printf("\b \b");
                fflush(stdout);
            }

            return res;

        default:
            // regular character
            if (term->bufpos < TERM_BUF_SIZE - 1)
            {
                term->buf[term->bufpos++] = c;
                printf("%c", c);
                fflush(stdout);
            }

            return res;
        }
    }

    return res;
}
