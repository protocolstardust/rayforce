# :material-plus-circle: Insert Query

The `insert` function adds new rows to a [:material-table: Table](../data-types/table.md). It can insert a single row or multiple rows at once.

```clj
↪ (insert table (list "Charlie" 35))
```

The `insert` function takes two arguments: the table to insert into and the data to insert. The data can be provided as a [:material-code-array: List](../data-types/list.md), [:material-code-braces: Dictionary](../data-types/dictionary.md), or another [:material-table: Table](../data-types/table.md).

## Inserting Multiple Rows with [:material-vector-line: Vectors](../data-types/vector.md)

Insert multiple rows using a list of [:material-vector-line: Vectors](../data-types/vector.md):

```clj
↪ (insert t (list (list "David" "Eve") [40 25]))
```

## Inserting with [:material-code-braces: Dictionary](../data-types/dictionary.md)

Insert rows using a [:material-code-braces: Dictionary](../data-types/dictionary.md) for clearer column mapping:

```clj
↪ (insert t (dict [name age] (list "Frank" 45)))
```

## Inserting with [:material-table: Table](../data-types/table.md)

Insert rows from another [:material-table: Table](../data-types/table.md):

```clj
↪ (insert t (table [name age] (list (list "Kate" "Leo") [65 70])))
```

## In-Place Insertion with [:simple-scalar: Symbol](../data-types/symbol.md)

To modify the table in place, pass the table name as a quoted [:simple-scalar: Symbol](../data-types/symbol.md):

```clj
↪ (insert 't (list "Mike" 75))  ;; Modifies t directly
```

!!! warning ""
    By default, `insert` returns a new table. To persist changes, either reassign the result or use in-place insertion with a quoted symbol:

!!! note ""
    Column types must match the [:material-table: table's](../data-types/table.md) column types. If a column name matches a built-in [:material-function: function](../data-types/functions.md) or env var, use `(at table 'column)` to access it.
