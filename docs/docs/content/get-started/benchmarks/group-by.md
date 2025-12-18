# :material-group: Group By Benchmark

<script src="https://cdn.jsdelivr.net/npm/echarts@5.4.3/dist/echarts.min.js"></script>

<div id="groupby-chart" style="width: 100%; height: 500px;"></div>

<script>
document.addEventListener('DOMContentLoaded', function() {
  var chart = echarts.init(document.getElementById('groupby-chart'));
  chart.setOption({
    title: { text: 'Group By Benchmark', left: 'center', textStyle: { fontSize: 18, fontWeight: 'bold', color: '#666' } },
    tooltip: { trigger: 'axis', axisPointer: { type: 'shadow' } },
    legend: { top: 30, data: ['DuckDB (MT on)', 'DuckDB (MT off)', 'ClickHouse', '? (4.0)', 'Rayforce', 'ThePlatform'], textStyle: { color: '#666' } },
    grid: { left: '3%', right: '4%', bottom: '3%', top: 80, containLabel: true },
    xAxis: { type: 'category', data: ['Q1', 'Q2', 'Q3', 'Q4', 'Q5', 'Q6', 'Q7'], axisLabel: { textStyle: { color: '#666' } } },
    yAxis: { type: 'log', name: 'Time (ms)', nameTextStyle: { color: '#666' }, axisLabel: { textStyle: { color: '#666' } } },
    series: [
      { name: 'DuckDB (MT on)', type: 'bar', data: [63, 153, 360, 23, 322, 330, 878], itemStyle: { color: '#42A5F5' } },
      { name: 'DuckDB (MT off)', type: 'bar', data: [347, 690, 601, 108, 440, 528, 3269], itemStyle: { color: '#90CAF9' } },
      { name: 'ClickHouse', type: 'bar', data: [51, 189, 235, 47, 265, 249, 1732], itemStyle: { color: '#66BB6A' } },
      { name: '? (4.0)', type: 'bar', data: [59, 143, 166, 99, 156, 551, 4497], itemStyle: { color: '#FFA726' } },
      { name: 'Rayforce', type: 'bar', data: [60, 74, 118, 72, 122, 104, 1394], itemStyle: { color: '#1565C0', borderColor: '#0D47A1', borderWidth: 2 } },
      { name: 'ThePlatform', type: 'bar', data: [213, 723, 507, 285, 488, 465, 15712], itemStyle: { color: '#EF5350' } }
    ]
  });
  window.addEventListener('resize', function() { chart.resize(); });
});
</script>

| DB                              | Q1  | Q2  | Q3  | Q4  | Q5  | Q6  | Q7    |
| ------------------------------- | --- | --- | --- | --- | --- | --- | ----- |
| **Rayforce**                    | **60**  | **74**  | **118** | **72**  | **122** | **104** | **1394**  |
| ClickHouse                      | 51  | 189 | 235 | 47  | 265 | 249 | 1732  |
| ? (4.0)                         | 59  | 143 | 166 | 99  | 156 | 551 | 4497  |
| ThePlatform                     | 213 | 723 | 507 | 285 | 488 | 465 | 15712 |
| DuckDB (multithread turned on)  | 63  | 153 | 360 | 23  | 322 | 330 | 878   |
| DuckDB (multithread turned off) | 347 | 690 | 601 | 108 | 440 | 528 | 3269  |

!!! note ""
    *Dataset: `G1_1e7_1e2_0_0.csv` (10 million rows). Lower values (ms) are better.*

**RayforceDB** demonstrates competitive performance across all 7 group by queries, consistently ranking among the top performers:

- **Q1-Q2**: Rayforce (60-74ms) performs comparably to ClickHouse (51-189ms) and ? (4.0) (59-143ms)
- **Q3**: At 118ms, Rayforce is **2.0x faster** than DuckDB MT-off (601ms) and **2.0x faster** than ? (4.0) (166ms)
- **Q4**: Rayforce (72ms) is competitive, though ClickHouse leads at 47ms
- **Q5-Q6**: Rayforce (122-104ms) shows strong performance, **2.1x faster** than DuckDB MT-off on Q5 and **5.3x faster** than ? (4.0) on Q6
- **Q7**: Rayforce (1394ms) is **2.3x faster** than ? (4.0) (4497ms) and **11.3x faster** than ThePlatform (15712ms)

### Rayforce Queries

```lisp
(set t (csv [SYMBOL SYMBOL SYMBOL I64 I64 I64 I64 I64 F64] "./db-benchmark/G1_1e7_1e2_0_0.csv"))
(timeit (select {v1: (sum v1) from: t by: id1}))  ;; Q1
(timeit (select {v1: (sum v1) from: t by: {id1: id1 id2: id2}}))  ;; Q2
(timeit (select {v1: (sum v1) v3: (avg v3) from: t by: id3}))  ;; Q3
(timeit (select {v1: (avg v1) v2: (avg v2) v3: (avg v3) from: t by: id4}))  ;; Q4
(timeit (select {v1: (sum v1) v2: (sum v2) v3: (sum v3) from: t by: id6}))  ;; Q5
(timeit (select {range_v1_v2: (- (max v1) (min v2)) from: t by: id3}))  ;; Q6
(timeit (select {v3: (sum v3) count: (map count v3) from: t by: {id1: id1 id2: id2 id3: id3 id4: id4 id5: id5 id6: id6}}))  ;; Q7
```

### DuckDB Queries

```sql
.timer "on"
create table t as SELECT * FROM read_csv('./db-benchmark/G1_1e7_1e2_0_0.csv');
select id1, sum(v1) AS v1 from t group by id1;  # Q1
select id1, id2, sum(v1) AS v1 from t group by id1, id2;  # Q2
select id3, sum(v1) AS v1, mean(v3) AS v3 from t group by id3;  # Q3
select id4, mean(v1) AS v1, mean(v2) AS v2, mean(v3) AS v3 from t group by id4;  # Q4
select id6, sum(v1) AS v1, sum(v2) AS v2, sum(v3) AS v3 from t group by id6;  # Q5
select id3, max(v1)-min(v2) AS range_v1_v2 from t group by id3;  # Q6
select id1, id2, id3, id4, id5, id6, sum(v3) AS v3, count(*) AS count from t group by id1, id2, id3, id4, id5, id6;  # Q7
```

### ClickHouse Queries

```sql
CREATE TABLE t (id1 String, id2 String, id3 String, id4 Int64, id5 Int64, id6 Int64, v1 Int64, v2 Int64, v3 Float64) ENGINE = Memory;
```

```bash
clickhouse-client -q "INSERT INTO default.t FORMAT CSV" < ./db-benchmark/G1_1e7_1e2_0_0.csv
```

```sql
select id1, sum(v1) AS v1 from t group by id1;  # Q1
select id1, id2, sum(v1) AS v1 from t group by id1, id2;  # Q2
select id3, sum(v1) AS v1, avg(v3) AS v3 from t group by id3;  # Q3
select id4, avg(v1) AS v1, avg(v2) AS v2, avg(v3) AS v3 from t group by id4;  # Q4
select id6, sum(v1) AS v1, sum(v2) AS v2, sum(v3) AS v3 from t group by id6;  # Q5
select id3, max(v1)-min(v2) AS range_v1_v2 from t group by id3;  # Q6
select id1, id2, id3, id4, id5, id6, sum(v3) AS v3, count(*) AS count from t group by id1, id2, id3, id4, id5, id6;  # Q7
```

### ? (4.0) Queries

```q
t: ("SSSJJJJJF";enlist",") 0: hsym `$":./db-benchmark/G1_1e7_1e2_0_0.csv"
select v1: sum v1 by id1 from t  // Q1
select v1: sum v1 by id1, id2 from t  // Q2
select v1: sum v1, v3: avg v3 by id3 from t  // Q3
select v1: avg v1, v2: avg v2, v3: avg v3 by id4 from t  // Q4
select v1: sum v1, v2: sum v2, v3: sum v3 by id6 from t  // Q5
select range_v1_v2: (max v1) - (min v2) by id3 from t  // Q6
select v3: sum v3, cnt: count i by id1, id2, id3, id4, id5, id6 from t  // Q7
```

### ThePlatform Queries

```q
t: ("SSSJJJJJF";enlist",") 0: `$":./db-benchmark/G1_1e7_1e2_0_0.csv"
0N#.?[t;();`id1!`id1;`v1!(sum;`v1)]
0N#.?[t;();`id1`id2!`id1`id2;`v1!(sum;`v1)]
0N#.?[t;();`id3!`id3;`v2`v3!((sum;`v2);(avg;`v3))]
0N#.?[t;();`id4!`id4;`v1`v2`v3!((avg;`v1);(avg;`v2);(avg;`v3))]
0N#.?[t;();`id6!`id6;`v1`v2`v3!((sum;`v1);(sum;`v2);(sum;`v3))]
0N#.?[?[t;();`id3!`id3;`v1`v2!((max;`v1);(min;`v2))];();0b;`id3`range_v1_v2!(`id3;(-;`v1;`v2))]
0N#.?[t;();`id1`id2`id3`id4`id5`id6!`id1`id2`id3`id4`id5`id6;`v3`count!((sum;`v3);(count;`v3))]
```
