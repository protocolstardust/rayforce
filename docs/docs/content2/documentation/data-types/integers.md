# :octicons-number-16: Integer Types

RayforceDB offers 4 types of different integers: [:material-numeric-8-box-outline: U8](./integers.md#unsigned-8-bit-integer), [:octicons-cpu-16: I16](./integers.md#signed-16-bit-integer), [:material-cpu-32-bit: I32](./integers.md#signed-32-bit-integer) and [:material-cpu-64-bit: I64](./integers.md#signed-64-bit-integer)


## :material-cpu-64-bit: Signed 64-bit Integer

!!! note ""
    Scalar Type Code: `-5`. Scalar Internal Name: `i64`. Vector Type Code: `5`. Vector Internal Name: `I64`

```clj
↪ (type 1) ;; Scalar
i64
↪ (type [1 2]) ;; Vector
I64
```


## :material-cpu-32-bit: Signed 32-bit Integer

!!! note ""
    Scalar Type Code: `-4`. Scalar Internal Name: `i32`. Vector Type Code: `4`. Vector Internal Name: `I32`

!!! warning ""
    Doesn't occur naturally in data types, but can be explicitly casted

```clj
↪ (type (as 'i32 1)) ;; Scalar
i32
↪ (type (as 'I32 (list 1 2)))  ;; Vector
I32
```

## :octicons-cpu-16: Signed 16-bit Integer

!!! note ""
    Scalar Type Code: `-3`. Scalar Internal Name: `i16`. Vector Type Code: `3`. Vector Internal Name: `I16`

!!! warning ""
    Doesn't occur naturally in data types, but can be explicitly casted

```clj
↪ (type (as 'i16 1)) ;; Scalar
i16
↪ (type (as 'I16 (list 1 2))) ;; Vector
I16
```

## :material-numeric-8-box-outline: Unsigned 8-bit Integer

!!! note ""
    Scalar Type Code: `-2`. Scalar Internal Name: `u8`. Vector Type Code: `2`. Vector Internal Name: `U8`.

!!! warning ""
    Doesn't occur naturally in data types, but can be explicitly casted

```clj
↪ (type (as 'u8 1)) ;; Scalar
u8
↪ (type (as 'U8 (list 1 2))) ;; Vector
U8
```