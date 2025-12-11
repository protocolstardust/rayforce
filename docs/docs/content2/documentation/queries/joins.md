# :material-table-network: Joins

Join operations combine rows from two [:material-table: Tables](../data-types/table.md) based on matching column values. 

## :material-set-left-right: Left Join

Keeping all rows from the left table and matching rows from the right table. Missing matches are filled with null values.

```clj
↪ (left-join columns left-table right-table)
```

The `left-join` function takes three arguments: a [:material-vector-line: Vector](../data-types/vector.md) of [:simple-scalar: Symbols](../data-types/symbol.md) representing join columns, the left [:material-table: Table](../data-types/table.md), and the right [:material-table: Table](../data-types/table.md).

## :material-border-inside: Inner Join

Keeping only rows where the join columns match in both tables.

```clj
↪ (inner-join columns left-table right-table)
```

The `inner-join` function takes three arguments: a [:material-vector-line: Vector](../data-types/vector.md) of [:simple-scalar: Symbols](../data-types/symbol.md) representing join columns, the left [:material-table: Table](../data-types/table.md), and the right [:material-table: Table](../data-types/table.md).

## :material-window-restore: Window Join

The `window-join` and `window-join1` functions are useful for time-series data. They join tables based on equality for all join columns except the last one, and use time windows for the last column.

```clj
↪ (window-join columns intervals left-table right-table aggregations)
↪ (window-join1 columns intervals left-table right-table aggregations)
```

Both functions take five arguments:

- `columns`: A [:material-vector-line: Vector](../data-types/vector.md) of [:simple-scalar: Symbols](../data-types/symbol.md) - the last column is used for window matching
- `intervals`: A [:material-code-array: List](../data-types/list.md) of two [:material-vector-line: Vectors](../data-types/vector.md) representing time windows
- `left-table`: The left [:material-table: Table](../data-types/table.md)
- `right-table`: The right [:material-table: Table](../data-types/table.md)
- `aggregations`: A [:material-code-braces: Dictionary](../data-types/dictionary.md) of aggregation [:material-function: functions](../data-types/functions.md)

### Creating Intervals

```clj
↪ (set intervals (map-left + [-1000 1000] (at trades 'Ts)))
```

!!! note ""
    This creates intervals of [-1000, +1000] milliseconds around each timestamp in the `Ts` column.

### Difference between `window-join` and `window-join1`

`window-join` excludes interval bounds from aggregation, while `window-join1` includes interval bounds in aggregation
