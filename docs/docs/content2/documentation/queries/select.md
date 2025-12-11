# :simple-googlebigquery: Select Query

The `select` function performs data selection, filtering, and aggregation on [:material-table: Tables](../data-types/table.md). `select` operations are optimized for performance and automatically parallelize aggregations when possible. It uses [:material-code-braces: Dictionary](../data-types/dictionary.md) syntax to specify columns and clauses.

## Syntax

```clj
↪ (select {column1: expr1
         column2: expr2
         ...
         from: source
         [where: condition]
         [by: grouping]})
```

All `select` parameters are provided as a single [:material-code-braces: Dictionary](../data-types/dictionary.md). The `from` clause is required.

## Filtering with `where` using [:material-vector-line: Vector](../data-types/vector.md) of [:simple-scalar: Booleans](../data-types/boolean.md)

Use the `where` clause to filter rows based on conditions:

```clj
↪ (select {name: name salary: salary from: employees where: (> salary 70000)})
```

The `where` clause accepts any boolean expression that evaluates to a [:material-vector-line: Vector](../data-types/vector.md) of [:simple-scalar: Booleans](../data-types/boolean.md) or a single boolean value.

## Aggregation with [:material-function: Functions](../data-types/functions.md)

Use aggregation [:material-function: functions](../data-types/functions.md) to compute columns:

```clj
↪ (select {total_salary: (sum salary) 
        avg_salary: (avg salary) 
        headcount: (count name) 
        from: employees})
```


## Grouping with `by` using [:simple-scalar: Symbol](../data-types/symbol.md) or [:material-code-braces: Dictionary](../data-types/dictionary.md)

You can group rows using the `by` keyword, which accepts a [:simple-scalar: Symbol](../data-types/symbol.md) or a [:material-code-braces: Dictionary](../data-types/dictionary.md) of symbols:

```clj
↪ (select {dept: dept 
           avg_salary: (avg salary) 
           headcount: (count name) 
           earliest_hire: (min hire_date) 
           from: employees 
           by: dept})
```

### Multiple Grouping Columns with [:material-code-braces: Dictionary](../data-types/dictionary.md)

Group by multiple columns using a [:material-code-braces: Dictionary](../data-types/dictionary.md):

```clj
↪ (select {dept: dept 
           region: region 
           avg_salary: (avg salary) 
           from: employees 
           by: {dept: dept region: region}})  ;; Multiple columns
```

!!! note ""
    If a column name matches a built-in [:material-function: function](../data-types/functions.md) or env var, use `(at table 'column)` to access it.
