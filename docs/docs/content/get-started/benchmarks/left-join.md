# :material-set-left-right: Left Join Benchmark

<script src="https://cdn.jsdelivr.net/npm/echarts@5.4.3/dist/echarts.min.js"></script>

<div id="leftjoin-chart" style="width: 100%; height: 400px;"></div>

<script>
document.addEventListener('DOMContentLoaded', function() {
  var chart = echarts.init(document.getElementById('leftjoin-chart'));
  chart.setOption({
    title: { text: 'Left Join Benchmark (lower is better)', left: 'center', textStyle: { fontSize: 18, fontWeight: 'bold', color: '#666' } },
    tooltip: { trigger: 'axis', axisPointer: { type: 'shadow' } },
    legend: { top: 30, data: ['? (4.0)', 'RayforceDB', 'ThePlatform'], textStyle: { color: '#666' } },
    grid: { left: '3%', right: '4%', bottom: '3%', top: 80, containLabel: true },
    xAxis: { type: 'category', data: ['Q1 (Left Join)'], axisLabel: { textStyle: { color: '#666' } } },
    yAxis: { type: 'log', name: 'Time (ms)', nameTextStyle: { color: '#666' }, axisLabel: { textStyle: { color: '#666' } } },
    series: [
      { name: '? (4.0)', type: 'bar', data: [3174], itemStyle: { color: '#FFA726' } },
      { name: 'RayforceDB', type: 'bar', data: [3149], itemStyle: { color: '#1565C0', borderColor: '#0D47A1', borderWidth: 2 } },
      { name: 'ThePlatform', type: 'bar', data: [23987], itemStyle: { color: '#EF5350' } }
    ]
  });
  window.addEventListener('resize', function() { chart.resize(); });
});
</script>

| DB                              | Q1    |
| ------------------------------- | ----- |
| **RayforceDB**                    | **3149**  |
| ? (4.0)                         | 3174  |
| ThePlatform                     | 23987 |
| ClickHouse                      | OOM   |
| DuckDB (multithread turned on)  | OOM   |
| DuckDB (multithread turned off) | OOM   |

!!! note ""
    *OOM = Out of Memory. Lower values (ms) are better.*

    Data generation:
    ```sh
    Rscript _data/join-datagen.R 1e7 1e2 0 0
    ```

**RayforceDB** excels in left join operations, demonstrating superior performance where other systems fail:

- While DuckDB and ClickHouse encounter Out of Memory (OOM) errors, RayforceDB successfully completes the left join query
- RayforceDB (3149ms) performs nearly identically to ? (4.0) (3174ms), with only a 0.8% difference
- RayforceDB is **7.6x faster** than ThePlatform (23987ms)


### RayforceDB Queries

```lisp
(set x (csv [I64 I64 I64 SYMBOL SYMBOL SYMBOL F64] "./db-benchmark/J1_1e7_NA_0_0.csv"))
(set y (csv [I64 I64 I64 SYMBOL SYMBOL SYMBOL F64] "./db-benchmark/J1_1e7_1e7_0_0.csv"))
(timeit (lj [id1 id2] x y))  ;; Q1
```

### DuckDB Queries
```sql
.timer "on"
create table x as SELECT * FROM read_csv('./db-benchmark/J1_1e7_NA_0_0.csv');
create table y as SELECT * FROM read_csv('./db-benchmark/J1_1e7_1e7_0_0.csv');
select * from x left join y on x.id1 = y.id1 and x.id2 = y.id2;  # Q1
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
select * from x left join y on x.id1 = y.id1 and x.id2 = y.id2;  # Q1
```

### ? (4.0) Queries
```q
x: ("JJJSSSF";enlist",") 0: `$":./db-benchmark/J1_1e7_NA_0_0.csv"
y: ("JJJSSSF";enlist",") 0: `$":./db-benchmark/J1_1e7_1e7_0_0.csv"
x lj 2!y  // Q1
```

### ThePlatform Queries

```q
x: ("JJJSSSF";enlist",") 0: `$":./db-benchmark/J1_1e7_NA_0_0.csv"
y: ("JJJSSSF";enlist",") 0: `$":./db-benchmark/J1_1e7_1e7_0_0.csv"
0N#.(?[lj[(`x;x);(`y;y);((~`x`id1;~`x`id2);(~`y`id1;~`y`id2))]; (); 0b; `a`b`c`d`e`f`g`h!(~`y`id1;~`y`id2;`y`id3;~`y`id4;~`y`id5;~`y`id6;~`x`v1;~`y`v2)])
```
