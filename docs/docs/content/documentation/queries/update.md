# :material-pencil: Update Query

The `update` function modifies columns in a [:material-table: Table](../data-types/table.md) using [:material-code-braces: Dictionary](../data-types/dictionary.md) syntax similar to [:simple-googlebigquery: Select Query](./select.md).

```clj
↪ (update {column1: expr1
          column2: expr2
          ...
          from: table
          [where: condition]
          [by: grouping]})
```

All `update` parameters are provided as a single [:material-code-braces: Dictionary](../data-types/dictionary.md). The `from` clause is required.

## Filtering with `where` using [:material-vector-line: Vector](../data-types/vector.md) of [:simple-scalar: Booleans](../data-types/boolean.md)

Use the `where` clause to update only rows that match a condition:

```clj
↪ (set t (update {salary: (* salary 1.1) from: t where: (> salary 55000)}))
```

The `where` clause accepts any boolean expression that evaluates to a [:material-vector-line: Vector](../data-types/vector.md) of [:simple-scalar: Booleans](../data-types/boolean.md) or a single boolean value.

## Grouping with `by` using [:simple-scalar: Symbol](../data-types/symbol.md) or [:material-code-braces: Dictionary](../data-types/dictionary.md)

You can group rows using the `by` keyword, which accepts a [:simple-scalar: Symbol](../data-types/symbol.md) or a [:material-code-braces: Dictionary](../data-types/dictionary.md) of symbols:

```clj
↪ (set t (update {salary: (+ salary 1000) from: t by: dept where: (> salary 55000)}))
```

### Multiple Grouping Columns with [:material-code-braces: Dictionary](../data-types/dictionary.md)

Group by multiple columns using a [:material-code-braces: Dictionary](../data-types/dictionary.md):

```clj
↪ (set t (update {price: (* price 1.05) 
                  from: trades 
                  by: {dept: dept region: region}})
```

## In-Place Update with [:simple-scalar: Symbol](../data-types/symbol.md)

To modify the table in place, pass the table name as a quoted [:simple-scalar: Symbol](../data-types/symbol.md) in the `from` clause:

```clj
↪ (update {salary: (* salary 1.1) from: 't where: (> salary 55000)})  ;; Modifies t directly
```

!!! warning ""
    By default, `update` returns a new table. To persist changes, either reassign the result or use in-place update with a quoted symbol:

!!! note ""
    If a column name matches a built-in [:material-function: function](../data-types/functions.md) or env var, use `(at table 'column)` to access it.
