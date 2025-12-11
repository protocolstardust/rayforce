# :material-exponent: Math Operations

RayforceDB provides comprehensive mathematical operations for working with numeric and temporal data types. These functions operate on [:material-vector-line: Vectors](../data-types/vector.md) and [:simple-scalar: Scalar](./overview.md).

## Binary Arithmetic Operations

### :material-plus: Add `+`

Performs addition on its arguments.

```clj
↪ (+ 1 2)
3
↪ (+ [1 2 3] 3) ;; Add scalar to Vector
[4 5 6]
↪ (+ 2024.03.20 5) ;; Adding different types
2024.03.25
```

### :material-minus: Sub `-`

Performs subtraction on its arguments.

```clj
↪ (- 1 2)
-1
↪ (- [1 2 3] 3) ;; Substract value from Vector
[-2 -1 0]
↪ (- 2024.03.20 5) ;; Substract different types
2024.03.15
```

### :material-close: Mul `*`

Performs multiplication on its arguments.

```clj
↪ (* 1 2)
2
↪ (* [1 2 3] 3) ;; Multiply value to Vector
[3 6 9]
↪ (* 3i 02:15:07.000) ;; Multiply different types
06:45:21.000
```

### :material-division: Div `/`

Performs integer division, returning the quotient.

```clj
↪ (/ 10 3)
3
↪ (/ [10 -10 3] 2)
[5 -5 1]
```

!!! note ""
    Division by zero returns null (`0Nl`). For floating-point division, use `div` function.

### :material-percent: Mod `%`

Performs modulo operation, returning the remainder of division.

```clj
↪ (% 10 3)
1
↪ (% [10 -10 3] 3)
[1 2 0]
```

## Aggregation Functions

Aggregation functions operate on [:material-vector-line: Vectors](../data-types/vector.md) and return scalar values. They are commonly used in [:simple-googlebigquery: Select Queries](../queries/select.md).

### :material-sigma: Sum

Calculates the sum of values in a [:material-vector-line: Vector](../data-types/vector.md).

```clj
↪ (sum [1 2 3])
6
↪ (sum [1.2 2.2 3.2])
6.6
```

### :material-function-variant: Avg

Calculates the average value of a [:material-vector-line: Vector](../data-types/vector.md).

```clj
↪ (avg [1.0 2.0 3.0])
2.0
↪ (avg [-24 12 6])
-2.0
```

### :material-chart-line: Med

Calculates the median of a [:material-vector-line: Vector](../data-types/vector.md).

```clj
↪ (med [3 1 2])
2.0
↪ (med [3 1 2 4])
2.5
```


### :material-chart-bell-curve: Dev

Calculates the standard deviation of a [:material-vector-line: Vector](../data-types/vector.md).

```clj
↪ (dev [1 2 3 4 50])
19.03
↪ (dev [1i 2i])
0.5
```


### :material-arrow-down: Min

Returns the minimum value in a [:material-vector-line: Vector](../data-types/vector.md).

```clj
↪ (min [1 2 3])
1
↪ (min [10.5 2.3 7.8])
2.3
```

### :material-arrow-up: Max

Returns the maximum value in a [:material-vector-line: Vector](../data-types/vector.md).

```clj
↪ (max [1 2 3])
3
↪ (max [10.5 2.3 7.8])
10.5
```

## Rounding Functions

### :material-arrow-up-bold: Ceil

Rounds a value up to the nearest integer greater than or equal to the given value.

```clj
↪ (ceil 1.5)
2.0
↪ (ceil [1.1 2.5 -1.1])
[2.0 3.0 -1.0]
```

### :material-arrow-down-bold: Floor

Returns the largest integer less than or equal to a given number.

```clj
↪ (floor 1.5)
1.0
↪ (floor [1.1 2.5 -1.1])
[1.0 2.0 -2.0]
```


### :material-circle-outline: Round

Rounds a number to the nearest integer. If halfway between two integers, rounds away from zero.

```clj
↪ (round 1.5)
2.0
↪ (round [-1.5 -1.1 0.0 1.1 1.5])
[-2.0 -1.0 0.0 1.0 2.0]
```


## Special Functions

### :material-timer: Xbar

Rounds values down to the nearest multiple of a specified bin width. Commonly used for bucketing or binning data into fixed-size intervals.

```clj
↪ (xbar 17 5)
15
↪ (xbar [10 11 12 13 14] 3)
[9 9 12 12 12]
↪ (xbar (at trades 'timestamp) 60000)  ;; 1-minute buckets
```
