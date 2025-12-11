# :material-window-restore: Window Join Benchmark

<script src="https://cdn.jsdelivr.net/npm/echarts@5.4.3/dist/echarts.min.js"></script>

<div id="windowjoin-chart" style="width: 100%; height: 400px;"></div>

<script>
document.addEventListener('DOMContentLoaded', function() {
  var chart = echarts.init(document.getElementById('windowjoin-chart'));
  // Convert ~33 min to milliseconds (33 * 60 * 1000 = 1,980,000 ms)
  chart.setOption({
    title: { text: 'Window Join Benchmark', left: 'center', textStyle: { fontSize: 18, fontWeight: 'bold', color: '#666' } },
    tooltip: { trigger: 'axis', axisPointer: { type: 'shadow' }, formatter: function(params) {
      var value = params[0].value;
      if (value > 1000000) {
        return params[0].name + '<br/>' + (value / 60000).toFixed(1) + ' min';
      }
      return params[0].name + '<br/>' + value.toFixed(2) + ' ms';
    }},
    legend: { top: 30, data: ['? (4.0)', 'RayforceDB'], textStyle: { color: '#666' } },
    grid: { left: '3%', right: '4%', bottom: '3%', top: 80, containLabel: true },
    xAxis: { type: 'category', data: ['Q1 (Window Join)'], axisLabel: { textStyle: { color: '#666' } } },
    yAxis: { type: 'value', name: 'Time', nameTextStyle: { color: '#666' }, axisLabel: { textStyle: { color: '#666' }, formatter: function(value) {
      if (value >= 60000) {
        return (value / 60000).toFixed(1) + ' min';
      }
      return (value / 1000).toFixed(1) + ' s';
    }}},
    series: [
      { name: '? (4.0)', type: 'bar', data: [1980000], itemStyle: { color: '#FFA726' } },
      { name: 'RayforceDB', type: 'bar', data: [59145.60], itemStyle: { color: '#1565C0', borderColor: '#0D47A1', borderWidth: 2 } }
    ]
  });
  window.addEventListener('resize', function() { chart.resize(); });
});
</script>

| DB                              | Q1    |
| ------------------------------- | ----- |
| **RayforceDB**                    | **59145.60 ms** |
| ? (4.0)                         | ~33 min |

!!! note ""
    *Q1 = Window Join query. Lower values are better. Dataset: Custom generated (n=10,000,000).*

**RayforceDB** demonstrates exceptional performance in window join operations, delivering a dramatic **33.5x** speedup


### RayforceDB Queries

```lisp
(set winj (read-csv [Symbol Time I64 I64 I64] "wj1.csv"))
(set n (count winj))
(set tsym (take n (concat (take 99 'AAPL) (take 1 'MSFT))))
(set ttime (+ 09:00:00 (as 'Time (/ (* (til n) 3) 10))))
(set price (+ 10 (til n)))
(set bsym (take (* 2 n) (concat (concat (take 3 'AAPL) (take 2 'MSFT)) (take 1 'GOOG))))
(set btime (+ 09:00:00 (as 'Time (/ (* (til (* 2 n)) 2) 10))))
(set bid (+ 8 (/ (til (* 2 n))2)))
(set ask (+ 12 (/ (til (* 2 n))2)))
(set trades (table [Sym Ts Price] (list tsym ttime price)))
(set quotes (table [Sym Ts Bid Ask] (list bsym btime bid ask)))
(set intervals (map-left + [-10000 10000] (at trades 'Ts)))
(println "wj1 % time: % ms" n (timeit (set wj (window-join1 [Sym Ts] intervals trades quotes {Bid: (min Bid) Ask: (max Ask)}))))  ;; Q1
```

### ? (4.0) Queries

```q
n: 10000000;
tsym:n#(99#`AAPL),1#`MSFT;
ttime:09:00:00.000+(3 * til n) div 10;
price:10+til n;
trades:([]Sym:tsym;Ts:ttime;Price:price);
bsym:(2*n)#`AAPL`AAPL`AAPL`MSFT`MSFT`GOOG;
btime:09:00:00.000+(2 * til 2*n) div 10;
bid:8+(til 2*n) div 2;
ask:12+(til 2*n) div 2;
quotes:([]Sym:bsym;Ts:btime;Bid:bid;Ask:ask);
w:-10000 10000+\:trades.Ts;
\t [`Sym`Ts xasc `quotes; winj: wj1[w;`Sym`Ts;trades;(quotes;(min;`Bid);(max;`Ask))]]  // Q1
:wj1.csv 0: "," 0: winj;
```
