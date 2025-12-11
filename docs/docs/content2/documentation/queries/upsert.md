# :material-swap-horizontal: Upsert Query

The `upsert` function updates existing rows or inserts new ones in a [:material-table: Table](../data-types/table.md) based on key column(s). If a row with the same key exists, it is updated. Otherwise, a new row is inserted.

```clj
↪ (upsert table 1 data)
```

The `upsert` function takes three arguments: the table to upsert into, the number of key columns, and the data to upsert. The data can be provided as a [:material-code-array: List](../data-types/list.md), [:material-code-braces: Dictionary](../data-types/dictionary.md), or another [:material-table: Table](../data-types/table.md).

## Key Columns

The first N columns (where N = `key_count`) form the key used to identify rows. If a row with the same key exists, it is updated; otherwise, a new row is inserted.

```clj
↪ (set t (table [id name] (list [1 2] (list "Alice" "Bob"))))

↪ (set t (upsert t 1 (list [2 3] (list "Bob-updated" "Charlie"))))
```

!!! note ""
    In this example, `id` is the key column (first argument is `1`). Row with `id=2` is updated, and a new row with `id=3` is inserted.

## Upserting Single Row

```clj
↪ (set t (upsert t 1 (list 4 "David")))
```

## Upserting Multiple Rows with [:material-vector-line: Vectors](../data-types/vector.md)

```clj
↪ (set t (upsert t 1 (list [5 6] (list "Eve" "Frank"))))
```

## Upserting with [:material-code-braces: Dictionary](../data-types/dictionary.md)

```clj
↪ (set t (upsert t 1 (dict [id name] (list 7 "Grace"))))
```

## Upserting with [:material-table: Table](../data-types/table.md)

```clj
↪ (set t (upsert t 1 (table [id name] (list [8 9] (list "Henry" "Ivy")))))
```

## In-Place Upsertion with [:simple-scalar: Symbol](../data-types/symbol.md)

To modify the table in place, pass the table name as a quoted [:simple-scalar: Symbol](../data-types/symbol.md):

```clj
↪ (upsert 't 1 (list 11 "Kate"))  ;; Modifies t directly
```

!!! warning ""
    By default, `upsert` returns a new table. To persist changes, either reassign the result or use in-place upsertion with a quoted symbol:

!!! note ""
    Column types must match the [:material-table: Table's](../data-types/table.md) column types. The key columns must be compatible types for comparison.
    
    If a column name matches a built-in [:material-function: function](../data-types/functions.md) or env var, use `(at table 'column)` to access it.
