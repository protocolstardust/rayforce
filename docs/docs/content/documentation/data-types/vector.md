# :material-vector-line: Vector

!!! note ""
    Type Code: Positive integers (e.g., `1` for B8, `6` for Symbol). A Vector's type code is the positive representation of its corresponding [:simple-scalar: Scalar](./overview.md) type code.

A Vector is a series of objects of a certain type. Vectors can consist of any [:simple-scalar: Scalar](./overview.md) type.

Vector type codes are positive representations of their corresponding Scalar types. For example, a scalar Symbol has type code `-6`, while a Vector of Symbols has type code `6`.

## What is the usage?

Vectors are structures that enable efficient operations against datasets, which is why they are primarily utilized within the [:material-table: Table](./table.md) type. They provide fast, homogeneous data storage and operations.

```clj
[1 2 3 4 5]  ;; Vector of I64
['abc 'dca 'sed]  ;; Vector of Symbols
[2025.01.01 2025.01.02]  ;; Vector of Dates
```

Vectors are defined using square brackets `[` and `]`.

### :material-arrow-right: See Vector usage in [:material-table: Table](./table.md) and [:material-code-braces: Dictionary](./dictionary.md) contexts
