# :material-numeric: Enum

!!! note ""
    Type Code: `20`. Internal Name: `ENUM`

An Enum (enumeration) is a special type that maps a [:material-vector-line: Vector](./vector.md) of [:simple-scalar: Symbols](./symbol.md) to a more compact representation. It consists of a symbol key and a vector of indices that reference the original symbol vector.

Enums are useful for saving space when storing repeated symbols, especially in [:material-table: Table](./table.md) columns with many duplicate symbol values.

Enums are created using the `enum` function, which takes a symbol key and a vector of symbols:

```clj
↪ (set a [a b c])
[a b c]
↪ (enum 'a [a b c a c c c c a])
'a#[a b c a c c c c a]
```

### :material-book-open-page-variant: For a comprehensive guide on why enums are essential for storage, how they work internally, and their role in parted tables with symfiles, see the [:material-alphabetical-variant: Symbols, Enums, and Symfiles Guide](../symbols-and-enums.md).
