# :simple-databricks: Data Types

RayforceDB supports several categories of data types. Each category serves a specific purpose in the database system:

## :simple-scalar: Scalar

!!! note ""
    Represents a single value of a non-iterable type, such as numbers, booleans, dates, and symbols.

| Name      | Type Code    | Description |
| --------- | ------- | ----------- |
| [B8](./boolean.md)        | -1      | Boolean |
| [U8](./integers.md#unsigned-8-bit-integer)        | -2      | 8-bit unsigned integer |
| [I16](./integers.md#signed-16-bit-integer)       | -3      | 16-bit signed integer |
| [I32](./integers.md#signed-32-bit-integer)       | -4      | 32-bit signed integer |
| [I64](./integers.md#signed-64-bit-integer)       | -5      | 64-bit signed integer |
| [Symbol](./symbol.md)    | -6      | Interned string |
| [Date](./temporal.md#date)      | -7      | YYYY.MM.DD format |
| [Time](./temporal.md#time)      | -8      | HH:MM:SS.mmm format |
| [Timestamp](./temporal.md#timestamp) | -9      | YYYY.MM.DD\DHH:MM:SS.mmm format |
| [F64](./float.md)       | -10     | 64-bit signed float |
| [GUID](./guid.md)      | -11     | Globally Unique Identifier |
| [C8](./char.md)        | -12     | Char |


## [:material-vector-line: Vector](./vector.md)

!!! note ""
    Represents a collection of elements of a certain type, enabling efficient storage and operations on homogeneous data.

## [:material-text-shadow: String](./string.md)

!!! note ""
    Represents a [:material-vector-line: Vector](./vector.md) of [C8](./char.md) (characters), used for storing text values.

## [:material-code-array: List](./list.md)

!!! note ""
    Represents a collection of elements that are not necessarily of the same type.

## [:material-table: Table](./table.md)

!!! note ""
    An object that holds information about the columns and rows of a specific dataset, forming the core structure for relational operations.

## [:material-code-braces: Dictionary](./dictionary.md)

!!! note ""
    An object that holds information about keys and their corresponding values.

## [:material-function: Function](./functions.md)

!!! note ""
    Built-in or custom user-defined functions and lambdas that perform operations on data.

## [:material-numeric: Enum](./enum.md)

!!! note ""
    An enumeration of certain values, providing a way to represent a fixed set of named constants.

## [:material-dots-horizontal: Other](./other.md)

!!! note ""
    Special types including `Null` and `Error` used throughout the system.

---

## :octicons-code-24: What is a Type Code?

A type code is a specific integer that represents a particular data type. It's primarily used in internal object evaluations and is not necessary for regular database users to understand.
