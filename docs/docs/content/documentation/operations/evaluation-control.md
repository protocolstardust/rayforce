# :material-code-braces: Evaluation Control

RayforceDB provides functions for controlling program flow, error handling, and execution order. These functions enable conditional execution, exception handling, and early returns.

### Do

Evaluates expressions sequentially and returns the result of the last expression. Useful for executing multiple statements in order.

```clj
↪ (do (set x 10) (set y 20) (+ x y))
30
↪ (do (println "First") (println "Second") "Done")
"Done"
```

### If

Conditional execution. Evaluates a condition and executes the then-branch if true, otherwise executes the else-branch (if provided).

```clj
↪ (if true "yes" "no")
"yes"
↪ (if false "yes" "no")
"no"
↪ (if (> 5 3) "greater" "less")
"greater"
↪ (if 1 3)
3
↪ (if 0 3)
null
```

Takes 2 or 3 arguments: condition, then-expression, and optionally else-expression. If no else-expression is provided and the condition is false, returns `null`.

### Try

Error handling construct. Evaluates an expression and catches any errors, executing a catch handler if an error occurs.

```clj
↪ (try (+ 1 2) (fn [e] 0))
3
↪ (try (raise "error") (fn [e] 99))
99
↪ (try (raise "error") "fallback")
"fallback"
```

The catch handler can be a lambda function (receives the error message as argument), a value (returned directly), or a symbol (evaluated). Try blocks can be nested to handle errors at different levels.

### Raise

Raises an exception with a string message. Execution jumps to the nearest try block's catch handler, or terminates the program if no handler exists.

```clj
↪ (raise "My error message")
```

!!! note ""
    Must be called with a string argument. Use within a `try` block to handle errors gracefully.

### Return

Returns a value from the current function early. Can only be used inside a function body.

```clj
↪ (fn [x]
    (if (< x 0)
        (return "negative")
    (+ x 10))
)
```

Takes 0 or 1 argument. With no argument, returns `null`. With an argument, returns that value and exits the function immediately.

!!! note ""
    Cannot be used outside of a function. The function's return value is the last evaluated expression, or the value passed to `return` if called.

### Exit

Exits the RayforceDB process with an optional exit code.

```clj
↪ (exit 0)
↪ (exit 1)
↪ (exit)
```

Takes 0 or 1 argument. With no argument, exits with code 0 (success). With an argument, exits with that code. Non-zero exit codes typically indicate errors.
