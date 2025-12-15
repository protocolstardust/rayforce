# :material-beaker-question-outline: Logic Operations

RayforceDB provides logical and comparison operations for working with boolean values. These functions enable conditional logic, filtering, and pattern matching.

### And

Performs a logical AND operation on its arguments. Returns `true` only if all arguments are `true`.

```clj
↪ (and true false)
false
↪ (and true true)
true
↪ (and [true false true] [false true false])
[false false false]
↪ (and [true false true] true)
[true false true]
```


### Or

Performs a logical OR operation on its arguments. Returns `true` if at least one argument is `true`.

```clj
↪ (or true false)
true
↪ (or false false)
false
↪ (or [true false true] [false true false])
[true true true]
↪ (or [true false true] false)
[true false true]
```

### Not

Performs logical negation. Returns `true` if the argument is `false`, and `false` if the argument is `true`.

```clj
↪ (not true)
false
↪ (not false)
true
↪ (not [true false true])
[false true false]
```


### Like

Performs pattern matching on strings using wildcard patterns. Supports `*` (matches any sequence), `?` (matches any single character), and `[chars]` (matches any character in the set).

```clj
↪ (like "hello" "hello")
true
↪ (like "hello" "he*")
true
↪ (like "hello" "h?llo")
true
↪ (like "hello" "h[ae]llo")
true
↪ (like ["hello" "world" "test"] "he*")
[true false false]
```

### Nil

Checks if a value is [:material-code-braces: Null](../data-types/other.md#null). Returns `true` if the value is `null`, `false` otherwise.

```clj
↪ (nil? null)
true
↪ (nil? 5)
false
↪ (nil? "hello")
false
```

### Equal

Compares two values and returns `true` if they are equal.

```clj
↪ (== 1 1)
true
↪ (== 1 2)
false
↪ (== [1 2 3] 1)
[true false false]
↪ (== [1 2 3] [1 2 3])
[true true true]
↪ (== "hello" "hello")
true
```

### Not Equal

Compares two values and returns `true` if they are not equal.

```clj
↪ (!= 1 2)
true
↪ (!= 1 1)
false
↪ (!= [1 2 3] 2)
[true false true]
↪ (!= [1 2 3] [1 2 4])
[false false true]
↪ (!= "hello" "world")
true
```

### Greater Than

Compares two values and returns `true` if the first value is greater than the second value.

```clj
↪ (> 2 1)
true
↪ (> 1 2)
false
↪ (> [3 2 1] 2)
[true false false]
↪ (> [3 2 1] [2 2 0])
[true false true]
↪ (> "zoo" "apple")
true
```

### Greater or Equal

Compares two values and returns `true` if the first value is greater than or equal to the second value.

```clj
↪ (>= 2 1)
true
↪ (>= 1 1)
true
↪ (>= 1 2)
false
↪ (>= [3 2 1] 2)
[true true false]
↪ (>= [3 2 1] [3 1 1])
[true true true]
```


### Lower Than

Compares two values and returns `true` if the first value is less than the second value.

```clj
↪ (< 1 2)
true
↪ (< 2 1)
false
↪ (< [1 2 3] 2)
[true false false]
↪ (< [1 2 3] [2 2 4])
[true false true]
↪ (< "apple" "zoo")
true
```

### Lower or Equal

Compares two values and returns `true` if the first value is less than or equal to the second value.

```clj
↪ (<= 1 1)
true
↪ (<= 1 2)
true
↪ (<= 2 1)
false
↪ (<= [1 2 3] 1)
[true false false]
↪ (<= [1 2 3] [1 2 3])
[true true true]
↪ (<= [1 2 3] [1 2 4])
[true true true]
```
