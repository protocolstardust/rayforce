# :material-dots-horizontal: Other Types

This section covers special utility types used throughout RayforceDB

## :material-null: Null

!!! note ""
    Type Code: `126`. Internal Name: `NULL`

Null represents the absence of a value or an uninitialized state. It's a special type used throughout RayforceDB to indicate missing or undefined data.


## :material-alert-circle: Error

!!! note ""
    Type Code: `127`. Internal Name: `ERROR`

An Error is a special type that represents an error condition in RayforceDB. Errors contain an error code and an error message, providing detailed information about what went wrong.

Errors are automatically created and returned when operations fail. They can be checked using error-handling functions `try`.

### :material-arrow-right: See Error handling in [:material-function: Functions](./functions.md) context
