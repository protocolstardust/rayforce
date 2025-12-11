# :material-file-document: RayforceDB Documentation

RayforceDB is a powerful database which utilises LISP-like syntax. It's quick, efficient (see [:material-speedometer: Benchmarks](../get-started/benchmarks/overview.md)), and weighs right <b>under 1MB</b>!

```clj
(set employees (table [name dept salary hire_date last_login] (list (list "Alice" "Bob" "Charlie") ['IT 'HR 'IT] [75000 65000 85000] [2021.01.15 2020.03.20 2019.11.30] [2024.03.15D09:30:00.012 2024.03.16D09:30:00.012 2024.03.17D09:30:00.012])))

(select {name: name dept: dept salary: salary from: employees})
┌─────────┬──────┬────────┐
│ name    │ dept │ salary │
├─────────┼──────┼────────┤
│ Alice   │ IT   │ 75000  │
│ Bob     │ HR   │ 65000  │
│ Charlie │ IT   │ 85000  │
└─────────┴──────┴────────┘

(select {avg_salary: (avg salary) headcount: (count name) earliest_hire: (min hire_date) from: employees by: dept})
┌──────┬────────────┬───────────┬───────────────┐
│ dept │ avg_salary │ headcount │ earliest_hire │
├──────┼────────────┼───────────┼───────────────┤
│ IT   │ 80000.00   │ 2         │ 2019.11.30    │
│ HR   │ 65000.00   │ 1         │ 2020.03.20    │
├──────┴────────────┴───────────┴───────────────┤
│ 2 rows (2 shown) 4 columns (4 shown)          │
└───────────────────────────────────────────────┘
```

This documentation outlines the main ways to interact with the database, and to store, access, and aggregate values.

We recommend you start by discovering the data types RayforceDB has to offer: [:simple-databricks: Data Types](./data-types/overview.md)
