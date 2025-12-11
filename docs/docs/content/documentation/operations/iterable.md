# :material-repeat: Iterable Operations

RayforceDB provides functions for applying operations to collections like [:material-code-array: Lists](../data-types/list.md) and [:material-vector-line: Vectors](../data-types/vector.md). These functions enable functional programming patterns and efficient data transformations.

### Apply

Applies a function to corresponding elements from multiple lists, using each list as an argument source.

```clj
↪ (apply (fn [x y] (+ x y)) [1 2 3] [4 5 6])
[5 7 9]
↪ (apply (fn [x y z] (+ x y z)) [1 2 3] [10 20 30] [100 200 300])
[111 222 333]
↪ (apply (fn [x y] (+ x y)) 1 [1 2 34])
[2 3 35]
```

### Map

Applies a function to each element of a list and returns a new list with the results. The function is applied sequentially.

```clj
↪ (map (fn [x] (+ x 1)) [1 2 3])
[2 3 4]
↪ (map (fn [x] (* x 2)) [1 2 3])
[2 4 6]
↪ (map + 5 [4 5 6])
[9 10 11]
```

### Pmap

Applies a function to each element of a list **in parallel** using the thread pool and returns a new list with the results.

```clj
↪ (pmap (fn [x] (+ x 1)) [1 2 3])
[2 3 4]
↪ (pmap (fn [x] (* x 2)) [1 2 3])
[2 4 6]
```


### Fold

Performs a traditional left fold (reduce) operation, accumulating elements from a vector using a lambda function.

```clj
↪ (fold (fn [acc x] (+ acc x)) [1 2 3 4 5])
15
```

!!! note ""
    The lambda function receives two arguments:
    - `acc` - the accumulated value
    - `x` - the current element

    The accumulation proceeds: `((((1 + 2) + 3) + 4) + 5) = 15`

### At

Returns the element at the specified index or key from a collection.

```clj
↪ (at [1 2 3 4 5] 2)
3
↪ (at (list "Alice" "Bob" "Charlie") 1)
"Bob"
↪ (set d (dict ['a 'b 'c] [1 2 3]))
↪ (at d 'b)
2
↪ (set t (table [name age] (list (list "Alice" "Bob") [25 30])))
↪ (at t 'name)
(Alice Bob)
```

When accessing tables, use `at` with quoted symbols for column names, especially when column names match reserved words like `timestamp`, `type`, `count`, etc.

### First

Returns the first element of a collection.

```clj
↪ (first [1 2 3 4 5])
1
↪ (first "hello")
'h'
```

### Last

Returns the last element of a collection.

```clj
↪ (last [1 2 3 4 5])
5
↪ (last "hello")
'o'
```

### Take

Returns the first `n` elements of a collection. If `n` is negative, returns the last `n` elements.

```clj
↪ (take 3 [1 2 3 4 5])
[1 2 3]
↪ (take -3 [1 2 3 4 5])
[3 4 5]
↪ (take 2 'a')
"aa"
```

When `n` exceeds the collection length, the collection is treated as circular and elements repeat.

### Except

Returns elements from the first collection that are not in the second collection.

```clj
↪ (except [1 2 3 4 5] [2 4])
[1 3 5]
↪ (except ['a 'b 'c] ['a 'c])
['b]
↪ (except [1 2 3 4 5] 3)
[1 2 4 5]
```

Duplicates are preserved in the result. Can use a scalar as the second argument to remove a single value.

### Union

Returns the union of two collections, containing all unique elements from both.

```clj
↪ (union [1 2 3] [4 5 6])
[1 2 3 4 5 6]
↪ (union [1 2 3] [2 3 4])
[1 2 3 4]
```

Returns distinct values from the concatenation of both collections.

### Sect

Returns the intersection of two collections, containing only elements that appear in both.

```clj
↪ (sect [1 2 3 4 5] [3 4 5 6 7])
[3 4 5]
↪ (sect ['a 'b 'c] ['b 'c 'd])
['b 'c]
```

### In

Checks if an element exists in a collection. Returns a boolean or boolean vector.

```clj
↪ (in 2 [1 2 3])
true
↪ (in 5 [1 2 3])
false
↪ (in [1 2] [1 2 3 4 5])
[true true]
↪ (in 'e' "test")
true
```

When the first argument is a collection, returns a boolean vector indicating membership for each element.

### Filter

Filters a collection using a boolean mask, returning only elements where the corresponding mask value is `true`.

```clj
↪ (filter [1 2 3 4 5] [true false true false true])
[1 3 5]
↪ (filter "test" [true false true false])
"ts"
↪ (filter ['a 'b 'c 'd] [true false false true])
['a 'd]
```

Both arguments must have the same length. Works with vectors, strings, and lists.

### Find

Returns the index of the first occurrence of a value in a collection, or `null` if not found.

```clj
↪ (find [1 2 3 4 5] 3)
2
↪ (find [1 2 3 4 5] 6)
null
↪ (find ['a 'b 'c] 'b)
1
```

Returns 0-based index. For vectors, returns an index vector indicating positions of each search value.

### Key

Returns the keys from a mappable object like a [:material-code-braces: Dictionary](../data-types/dictionary.md) or [:material-table: Table](../data-types/table.md). For other types, returns the value itself.

```clj
↪ (key (dict ['a 'b] [1 2]))
['a 'b]
↪ (set t (table [name age] (list (list "Alice" "Bob") [25 30])))
↪ (key t)
[name age]
↪ (key [1 2])
[1 2]
```

### Value

Returns the values from a mappable object like a [:material-code-braces: Dictionary](../data-types/dictionary.md) or [:material-table: Table](../data-types/table.md). For [:material-code-braces: Enum](../data-types/enum.md) types, returns the underlying symbol vector. For other types, returns the value itself.

```clj
↪ (value (dict ['a 'b] [1 2]))
[1 2]
↪ (set t (table [name age] (list (list "Alice" "Bob") [25 30])))
↪ (value t)
((Alice Bob) [25 30])
↪ (value [1 2])
[1 2]
```

### Within

Checks if elements are within a range specified by a 2-element vector `[min max]`. Returns a boolean or boolean vector.

```clj
↪ (within 6 [1 20])
true
↪ (within [1 2 3] [1 100])
[true true true]
↪ (within [5 15 25] [10 20])
[false true false]
```

The second argument must be a 2-element vector representing the inclusive range `[min, max]`.
