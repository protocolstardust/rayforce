# :material-code-array: List

!!! note ""
    Type Code: `0`. Internal Name: `LIST`

A List is a series of objects that contain elements that are not necessarily of the same type.

Essentially, consider a List as a [:material-vector-line: Vector](./vector.md), but the type of elements is not strictly enforced.

Any Vector that happens to have different value types becomes a List.

```clj
(list 1 2 3 4)  ;; List of integers
(list 'a 'b 'c)  ;; List of symbols
(list 1 'a "asd" 2025.10.01)  ;; List of different types
```

Lists are defined using the `list` function.

### :material-arrow-right: See List usage in [:material-table: Table](./table.md) and [:material-code-braces: Dictionary](./dictionary.md) contexts
