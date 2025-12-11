# :material-sort: Order Operations

RayforceDB provides functions for sorting and ordering [:material-vector-line: Vectors](../data-types/vector.md), [:material-code-array: Lists](../data-types/list.md), and [:material-table: Tables](../data-types/table.md). These functions enable data organization and ranking operations.

## Sorting Functions

### :material-sort-ascending: Asc

Sorts elements in ascending order. Returns a new sorted [:material-vector-line: Vector](../data-types/vector.md) or [:material-code-array: List](../data-types/list.md).

```clj
↪ (asc [3 1 4 1 5 9 2 6])
[1 1 2 3 4 5 6 9]
↪ (asc (list "banana" "apple" "cherry"))
(apple banana cherry)
```

### :material-sort-descending: Desc

Sorts elements in descending order. Returns a new sorted [:material-vector-line: Vector](../data-types/vector.md) or [:material-code-array: List](../data-types/list.md).

```clj
↪ (desc [3 1 4 1 5 9 2 6])
[9 6 5 4 3 2 1 1]
↪ (desc (list "banana" "apple" "cherry"))
(cherry banana apple)
```

## Index Functions

### :material-sort-ascending: Iasc

Returns indices that would sort the input in ascending order.

```clj
↪ (iasc [3 1 4 1 5])
[1 3 0 2 4]
↪ (iasc (list "banana" "apple" "cherry"))
[1 0 2]
```

### :material-sort-descending: Idesc

Returns indices that would sort the input in descending order.

```clj
↪ (idesc [3 1 4 1 5])
[4 2 0 1 3]
↪ (idesc (list "banana" "apple" "cherry"))
[2 0 1]
```


## Ranking Functions

### :material-numeric: Rank

Returns the rank (position in sorted order) of each element in a [:material-vector-line: Vector](../data-types/vector.md).

```clj
↪ (rank [30 10 20])
[2 0 1]
↪ (rank [5 3 1 4 2])
[4 2 0 3 1]
```


### :material-numeric-1-box: Xrank

Assigns each element to a bucket based on its rank, dividing the data into n equal-sized groups.

```clj
↪ (xrank [30 10 20 40 50 60] 3)
[1 0 0 1 2 2]
↪ (xrank [1 2 3 4] 2)
[0 0 1 1]
```


## Table Sorting Functions

### :material-sort-ascending: Xasc

Sorts a [:material-table: Table](../data-types/table.md) in ascending order based on specified columns.

```clj
↪ (set t (table [name age score] (list (list "Bob" "Alice" "Charlie") [25 30 20] [85 90 95])))
↪ (xasc [name] t)
```

The `xasc` function takes two arguments: a [:material-vector-line: Vector](../data-types/vector.md) of [:simple-scalar: Symbols](../data-types/symbol.md) representing column names, and the [:material-table: Table](../data-types/table.md) to sort.

### :material-sort-descending: Xdesc

Sorts a [:material-table: Table](../data-types/table.md) in descending order based on specified columns.

```clj
↪ (set t (table [name age score] (list (list "Bob" "Alice" "Charlie") [25 30 20] [85 90 95])))
↪ (xdesc t [score])
```

The `xdesc` function takes two arguments: a [:material-vector-line: Vector](../data-types/vector.md) of [:simple-scalar: Symbols](../data-types/symbol.md) representing column names, and the [:material-table: Table](../data-types/table.md) to sort.


## Numeric Operations

### :material-minus-circle: Neg

Returns the negative of a number or applies negation element-wise to [:material-vector-line: Vectors](../data-types/vector.md).

```clj
↪ (neg 5)
-5
↪ (neg -3)
3
↪ (neg [1 -2 3 -4])
[-1 2 -3 4]
```
