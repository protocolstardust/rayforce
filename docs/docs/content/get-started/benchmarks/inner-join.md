# :material-border-inside: Inner Join Benchmark

<script src="https://cdn.jsdelivr.net/npm/echarts@5.4.3/dist/echarts.min.js"></script>

<div id="innerjoin-chart" style="width: 100%; height: 400px;"></div>

<script>
document.addEventListener('DOMContentLoaded', function() {
  var chart = echarts.init(document.getElementById('innerjoin-chart'));
  chart.setOption({
    title: { text: 'Inner Join Benchmark', left: 'center', textStyle: { fontSize: 18, fontWeight: 'bold', color: '#666' } },
    tooltip: { trigger: 'axis', axisPointer: { type: 'shadow' } },
    legend: { top: 30, data: ['? (4.0)', 'RayforceDB', 'ThePlatform'], textStyle: { color: '#666' } },
    grid: { left: '3%', right: '4%', bottom: '3%', top: 80, containLabel: true },
    xAxis: { type: 'category', data: ['Q2 (Inner Join)'], axisLabel: { textStyle: { color: '#666' } } },
    yAxis: { type: 'log', name: 'Time (ms)', nameTextStyle: { color: '#666' }, axisLabel: { textStyle: { color: '#666' } } },
    series: [
      { name: '? (4.0)', type: 'bar', data: [3098], itemStyle: { color: '#FFA726' } },
      { name: 'RayforceDB', type: 'bar', data: [1610], itemStyle: { color: '#1565C0', borderColor: '#0D47A1', borderWidth: 2 } },
      { name: 'ThePlatform', type: 'bar', data: [34104], itemStyle: { color: '#EF5350' } }
    ]
  });
  window.addEventListener('resize', function() { chart.resize(); });
});
</script>

| DB                              | Q2    |
| ------------------------------- | ----- |
| **RayforceDB**                    | **1610**  |
| ? (4.0)                         | 3098  |
| ThePlatform                     | 34104 |
| DuckDB (multithread turned on)  | OOM   |
| DuckDB (multithread turned off) | OOM   |
| ClickHouse                      | OOM   |

!!! note ""
    *OOM = Out of Memory. Lower values (ms) are better.*

    Data Generation:
    ```sh
    Rscript _data/join-datagen.R 1e7 1e2 0 0
    ```

**RayforceDB** demonstrates exceptional performance in inner join operations:

- While DuckDB and ClickHouse encounter Out of Memory (OOM) errors, RayforceDB successfully completes the inner join query
- RayforceDB (1610ms) is **1.9x faster** than ? (4.0) (3098ms)
- RayforceDB is **21.2x faster** than ThePlatform (34104ms)

### Rayforce Queries

```lisp
(set x (csv [I64 I64 I64 Symbol Symbol Symbol F64] "./db-benchmark/J1_1e7_NA_0_0.csv"))
(set y (csv [I64 I64 I64 Symbol Symbol Symbol F64] "./db-benchmark/J1_1e7_1e7_0_0.csv"))
(timeit (ij [id1 id2] x y))  ;; Q2
```

### DuckDB Queries
```sql
.timer "on"
create table x as SELECT * FROM read_csv('./db-benchmark/J1_1e7_NA_0_0.csv');
create table y as SELECT * FROM read_csv('./db-benchmark/J1_1e7_1e7_0_0.csv');
select * from x inner join y on x.id1 = y.id1 and x.id2 = y.id2;  # Q2
```

### ClickHouse Queries

```sql
CREATE TABLE x (id1 Int64, id2 Int64, id3 Int64, id4 String, id5 String, id6 String, v1 Float64) ENGINE = Memory;
CREATE TABLE y (id1 Int64, id2 Int64, id3 Int64, id4 String, id5 String, id6 String, v2 Float64) ENGINE = Memory;
```

```sh
clickhouse-client -q "INSERT INTO default.x FORMAT CSV" < ./db-benchmark/J1_1e7_NA_0_0.csv
clickhouse-client -q "INSERT INTO default.y FORMAT CSV" < ./db-benchmark/J1_1e7_1e7_0_0.csv
```

```sql
select * from x inner join y on x.id1 = y.id1 and x.id2 = y.id2;  # Q2
```

### ? (4.0) Queries
```q
x: ("JJJSSSF";enlist",") 0: `$":./db-benchmark/J1_1e7_NA_0_0.csv"
y: ("JJJSSSF";enlist",") 0: `$":./db-benchmark/J1_1e7_1e7_0_0.csv"
x ij 2!y // Q2
```

### ThePlatform Queries

```q
x: ("JJJSSSF";enlist",") 0: `$":./db-benchmark/J1_1e7_NA_0_0.csv"
y: ("JJJSSSF";enlist",") 0: `$":./db-benchmark/J1_1e7_1e7_0_0.csv"
0N#.(?[ij[(`x;x);(`y;y);((~`x`id1;~`x`id2);(~`y`id1;~`y`id2))]; (); 0b; `a`b`c`d`e`f`g`h!(~`y`id1;~`y`id2;`y`id3;~`y`id4;~`y`id5;~`y`id6;~`x`v1;~`y`v2)])
```
