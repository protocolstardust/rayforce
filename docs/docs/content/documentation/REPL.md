# :material-console: REPL Functions

RayforceDB provides functions for parsing, evaluating, and loading code, as well as controlling REPL behavior and formatting. These functions enable code introspection, dynamic execution, and customization of the interactive environment.

### Parse

Parses a string into an expression without evaluating it. Returns the parsed abstract syntax tree.

```clj
↪ (parse "(+ 1 2)")
(
  +
  1
  2
)
↪ (parse "(fn [x] (* x 2))")
@(null)
```

### Eval

Evaluates a string or expression. When given a string, parses and evaluates it. When given an expression, evaluates it directly.

```clj
↪ (eval "(+ 1 2)")
3
↪ (set expr (list + 1 2))
↪ (eval expr)
3
```

### Load

Loads and evaluates a RayforceDB source file. If the path ends with a slash, loads a table from that directory. Otherwise, reads the file as code and evaluates it.

```clj
↪ (load "myfile.rfl")
↪ (load "/path/to/table/")
```

When loading a table directory, the table is bound to a symbol derived from the directory name.

### Args

Returns the command-line arguments passed to the RayforceDB process.

```clj
↪ (args)
["arg1" "arg2" "arg3"]
```

### Quote

Prevents evaluation of an expression, returning it as-is. This is a special form.

```clj
↪ (quote (+ 1 2))
(
  +
  1
  2
)
↪ (quote x)
x
```

### Resolve

Resolves a symbol to its value in the current environment. Returns the value if found, `null` otherwise.

```clj
↪ (set x 42)
↪ (resolve 'x)
42
↪ (resolve 'nonexistent)
null
```

### Display Width

Sets the maximum display width for table output formatting. Use the command `:set-display-width` in the REPL.

```clj
:set-display-width 120
```

Controls how wide tables can be before wrapping or truncating columns.

### FPR

Sets the floating-point precision for number formatting. Use the command `:set-fpr` in the REPL.

```clj
:set-fpr 6
```

Controls how many decimal places are shown when displaying floating-point numbers.

### Use Unicode Format

Enables or disables Unicode characters for table formatting. Use the command `:use-unicode` in the REPL.

```clj
:use-unicode 1
:use-unicode 0
```

When enabled, uses Unicode box-drawing characters for tables. When disabled, uses ASCII characters.

## REPL Commands

The REPL supports several built-in commands prefixed with `:`:

- `:?` - Displays help and list of available commands
- `:u [0|1]` - Toggle Unicode formatting for tables
- `:t [0|1]` - Toggle time measurement of expressions
- `:q [exit code]` - Exit the application with optional exit code
