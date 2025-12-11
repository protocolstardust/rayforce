# :material-code-braces: Compose Operations

RayforceDB provides functions for creating, combining, and transforming data structures. These functions enable data construction, type conversion, and collection manipulation.

### Casting

Converts a value to a specified type. Takes a type symbol and a value to convert.

```clj
↪ (as 'I64 3.14)
3
↪ (as 'F64 5)
5.0
↪ (as 'String 42)
"42"
↪ (as 'Symbol "hello")
'hello
```

The first argument must be a symbol representing a type name (e.g., `'I64`, `'F64`, `'String`, `'Symbol`, `'Date`, `'Time`, `'Timestamp`).

### Concat

Concatenates two vectors or strings, combining them into a single vector or string.

```clj
↪ (concat [1 2] [3 4])
[1 2 3 4]
↪ (concat "hello" "world")
"helloworld"
↪ (concat ['a 'b] ['c 'd])
['a 'b 'c 'd]
```

Works with vectors of the same type and strings. Returns a new vector or string containing all elements from both arguments.

### Create Dict

Creates a [:material-code-braces: Dictionary](../data-types/dictionary.md) from keys and values vectors.

```clj
↪ (dict ['a 'b 'c] [1 2 3])
{a: 1, b: 2, c: 3}
↪ (dict [name age] (list ["Alice" "Bob"] [25 30]))
{name: (Alice Bob), age: (25 30)}
```

Takes two arguments: a vector of keys and a vector of values. Both vectors must have the same length.

### Create Table

Creates a [:material-table: Table](../data-types/table.md) from column names and column data.

```clj
↪ (table [name age] (list (list "Alice" "Bob") [25 30]))
↪ (table [x y] (list [1 2 3] [4 5 6]))
```

Takes two arguments: a symbol vector of column names and a list of column vectors. All column vectors must have the same length. Scalar values are automatically expanded to match column length.

### Create List

Creates a [:material-code-array: List](../data-types/list.md) from multiple arguments.

```clj
↪ (list 1 2 3)
(1 2 3)
↪ (list "hello" "world" 42)
("hello" "world" 42)
↪ (list [1 2] [3 4])
([1 2] [3 4])
```

Takes any number of arguments and returns a list containing all of them. Lists can contain heterogeneous types.

### Create Vector

Vectors are created using literal syntax with square brackets. There is no separate `vector` function.

```clj
↪ [1 2 3 4 5]
[1 2 3 4 5]
↪ ['a 'b 'c]
['a 'b 'c]
↪ [2025.01.01 2025.01.02]
[2025.01.01 2025.01.02]
```

Vectors must contain elements of the same type. Use `list` for heterogeneous collections.

### Group

Groups elements of a vector by value, returning a dictionary mapping unique values to lists of indices where they occur.

```clj
↪ (group [1 2 1 3 2 1])
{1: (0 2 5), 2: (1 4), 3: (3)}
```

Returns a dictionary where keys are distinct values from the input vector, and values are lists of indices where each key appears.

### GUID

Generates a vector of GUIDs (Globally Unique Identifiers).

```clj
↪ (guid 3)
[<guid1> <guid2> <guid3>]
```

Takes an integer argument specifying how many GUIDs to generate. Returns a vector of GUID values.

### Rand

Generates a vector of random integers between 0 (inclusive) and an upper bound (exclusive).

```clj
↪ (rand 5 10)
[3 7 2 9 1]
```

Takes two integer arguments: count and upper bound. Returns a vector of `count` random integers in the range `[0, upper_bound)`.

### Raze

Flattens a list of vectors into a single vector, or flattens nested lists.

```clj
↪ (raze (list [1 2] [3 4] [5 6]))
[1 2 3 4 5 6]
↪ (raze (list (list 1 2) (list 3 4)))
(1 2 3 4)
```

If all elements are vectors of the same type, returns a single concatenated vector. Otherwise, flattens to a list.

### Reverse

Reverses the order of elements in a vector or list.

```clj
↪ (reverse [1 2 3 4])
[4 3 2 1]
↪ (reverse "hello")
"olleh"
↪ (reverse (list "a" "b" "c"))
("c" "b" "a")
```

Returns a new vector or list with elements in reverse order. Preserves the type of the input.

### Til

Generates a vector of integers from 0 to n-1.

```clj
↪ (til 5)
[0 1 2 3 4]
↪ (til 0)
[]
```

Takes a non-negative integer and returns a vector containing `[0, 1, 2, ..., n-1]`. The result has `ATTR_ASC` and `ATTR_DISTINCT` attributes set.

### Distinct

Returns a vector containing only unique values from the input, preserving order of first occurrence.

```clj
↪ (distinct [1 2 1 3 2 1])
[1 2 3]
↪ (distinct ['a 'b 'a 'c 'b])
['a 'b 'c]
```

Removes duplicate values while maintaining the order in which values first appear. Works with vectors of various types including Enum.
