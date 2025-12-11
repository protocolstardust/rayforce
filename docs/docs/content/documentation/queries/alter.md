# :material-pencil-outline: Alter Query

The `alter` function modifies elements at specific indices in [:material-vector-line: Vectors](../data-types/vector.md), [:material-code-array: Lists](../data-types/list.md), or [:material-table: Tables](../data-types/table.md) using a [:material-function: function](../data-types/functions.md). It applies the function to the element at the specified index with a given value.

```clj
↪ (alter obj func value)
```

The `alter` function takes 3-4 arguments: the object to modify, a binary [:material-function: function](../data-types/functions.md), an optional index, and a value.


## Working with [:material-table: Tables](../data-types/table.md)

```clj
↪ (set t (table [price volume] (list [100 200] [50 60])))
↪ (alter t + 'price 10)  ;; Add 10 to all values in price column
```

## In-Place Modification with [:simple-scalar: Symbol](../data-types/symbol.md)

To modify the object in place, pass the object name as a quoted [:simple-scalar: Symbol](../data-types/symbol.md):

```clj
↪ (alter 'arr set 0 99)  ;; Modifies arr directly
```

!!! warning ""
    By default, `alter` returns a modified copy. To persist changes, either reassign the result or use in-place modification with a quoted symbol.
