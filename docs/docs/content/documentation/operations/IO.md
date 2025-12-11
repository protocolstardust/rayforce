# :material-pen: Input-Output Operations

RayforceDB provides functions for reading and writing data to files, IPC connections, and managing table storage formats. These functions enable data persistence, inter-process communication, and efficient table storage.

### Get

Gets a variable by symbol or reads a serialized object from a file path.

```clj
↪ (get 'myvar)
[1 2 3]
↪ (get "/path/to/file")
[1 2 3]
```

When called with a symbol, returns the value of that variable. When called with a string path, reads and deserializes a file containing a RayforceDB object.

### Get Splayed

Loads a splayed table from disk. A splayed table stores each column as a separate file for efficient columnar access.

```clj
↪ (get-splayed "/tmp/db/tab/" "/tmp/db/tab.sym")
```

The second argument (symfile path) is optional. If not provided, the symfile will be inferred from the table path.

!!! note ""
    Pay attention to the trailing slash in the path to the table. It is required.

### Set Splayed

Stores a [:material-table: Table](../data-types/table.md) as a splayed table on disk. Each column is saved as a separate file.

```clj
↪ (set-splayed "/tmp/db/tab/" t)
↪ (set-splayed "/tmp/db/tab/" t "/tmp/db/sym")
```

Optionally accepts a string path to a symfile. If provided, symbol columns will use this shared symfile for enumeration.

!!! note ""
    The table path must end with a trailing slash to indicate it's a directory.

### Get Parted

Loads a parted table from disk. A parted table is organized by partitions.

```clj
↪ (get-parted "/tmp/db/" 'tab)
```

Takes two arguments: the root path to the parted tables directory and a symbol representing the table name.

### Set Parted

Stores a [:material-table: Table](../data-types/table.md) as a parted table on disk, organized by partitions.

```clj
↪ (set-parted "/tmp/db/" t)
```

### Read CSV

Reads a CSV file and converts it into a [:material-table: Table](../data-types/table.md).

```clj
↪ (read-csv [I64 Symbol F64] "/path/to/file.csv")
↪ (read-csv [I64 Symbol F64] "/path/to/file.csv" ';')
```

Takes a vector of type symbols specifying the column types, a file path, and optionally a separator character (defaults to comma).

### Read

Reads a file from disk. When given a file path string, returns the file contents as a string. When given a file handle, reads IPC messages.

```clj
↪ (read "/path/to/file.txt")
"file contents"
↪ (read handle)
```

For file handles, returns a dictionary with `items`, `read`, and `total` keys indicating how many messages were read.

### Write

Writes data to a file handle or IPC connection.

```clj
↪ (write handle "(+ 1 2)")
3
↪ (write handle (list (+ 1 2)))
3
↪ (write fd data)
```

When writing to an IPC handle, the data is sent as a message and evaluated on the remote side. When writing to a file handle, the data is serialized and written to the file.
