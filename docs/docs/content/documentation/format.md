# :material-format-text: Format Functions

RayforceDB provides functions for converting objects to string representations and printing formatted output. These functions enable data display, debugging, and user interaction.

### Format

Converts objects to string representations. When given a single argument, formats that object. When given multiple arguments, treats the first as a format string with `%` placeholders that are replaced by subsequent arguments.

```clj
↪ (format 1)
"1"
↪ (format [1 2 3])
"[1 2 3]"
↪ (format "% %" 1 3)
"1 3"
↪ (format "% %" 1 [1 2 3])
"1 [1 2 3]"
↪ (format "Value: % Count: %" 42 5)
"Value: 42 Count: 5"
```

Returns a string representation of the formatted result. Format strings use `%` as placeholders that are replaced by the formatted representation of subsequent arguments.

### Print

Prints formatted output to stdout without a trailing newline.

```clj
↪ (print "% %" 1 [2 3 4])
1 [2 3 4]
↪ (print "Hello")
Hello
```

Takes the same arguments as `format` but writes directly to stdout instead of returning a string. Multiple calls to `print` will output on the same line.

### Println

Prints formatted output to stdout with a trailing newline.

```clj
↪ (println "% %" 1 [2 3 4])
1 [2 3 4]
↪ (println "Hello")
Hello
```

Similar to `print`, but automatically appends a newline character after the output. Useful for printing complete lines of text.

### Show

Prints the full representation of an object to stdout without terminal width constraints. Displays the complete object structure regardless of display width settings.

```clj
↪ (show (til 100))
[0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31 32 33 34 35 36 37 38 39 40 41 42 43 44 45 46 47 48 49 50 51 52 53 54 55 56 57 58 59 60 61 62 63 64 65 66 67 68 69 70 71 72 73 74 75 76 77 78 79 80 81 82 83 84 85 86 87 88 89 90 91 92 93 94 95 96 97 98 99]
```

Takes a single object argument and prints its complete representation. Unlike regular formatting, `show` ignores display width limits and shows the entire object structure.

!!! note ""
    `show` uses full formatting mode, displaying all details of complex objects. Useful for debugging and inspecting large data structures.
