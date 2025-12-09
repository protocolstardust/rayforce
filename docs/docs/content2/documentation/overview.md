# :material-file-document: Welcome to RayforceDB Documentation!

RayforceDB is a powerful database which utilises LISP-like syntax. It's quick, efficient, and weights under 1MB, which is a great engineering achievement.

(set employees (table [name dept salary hire_date last_login] (list (list "Alice" "Bob" "Charlie") ['IT 'HR 'IT] [75000 65000 85000] [2021.01.15 2020.03.20 2019.11.30] [2024.03.15T09:30:00 2024.03.15T10:15:00 2024.03.15T11:45:00])))

(set employees (table [name dept salary hire_date last_login] (list (list "Alice" "Bob" "Charlie") ['IT 'HR 'IT] [75000 65000 85000] [2021.01.15 2020.03.20 2019.11.30] [2024.03.15D09:30:00.012 2024.03.16D09:30:00.012 2024.03.17D09:30:00.012])))

```
(set employees (
    table [
        name dept salary hire_date last_login
    ] (
        list 
            ["Alice" "Bob" "Charlie"]
            ['IT 'HR 'IT]
            [75000 65000 85000]
            [2021.01.15 2020.03.20 2019.11.30]
            [2024.03.15T09:30:00 2024.03.15T10:15:00 2024.03.15T11:45:00]
        )
    )
)

(select {name: name dept: dept salary: salary from: employees})
┌─────────┬──────┬────────┐
│ name    │ dept │ salary │
├─────────┼──────┼────────┤
│ Alice   │ IT   │ 75000  │
│ Bob     │ HR   │ 65000  │
│ Charlie │ IT   │ 85000  │
└─────────┴──────┴────────┘

(select {dept: dept avg_salary: (avg salary) headcount: (count name) earliest_hire: (min hire_date) from: employees by: dept})
```

This documentation outlines the main ways to interact with the database, store, access and aggregate the values.

We recommend you start from discovering data types RayforceDB has to offer: [::]




