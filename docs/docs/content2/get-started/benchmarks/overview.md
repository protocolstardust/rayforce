# :material-speedometer: Performance Benchmark

Our benchmarks are based on the [H2OAI Group By Benchmark](https://h2oai.github.io/db-benchmark) standard.

<script src="https://cdn.jsdelivr.net/npm/echarts@5.4.3/dist/echarts.min.js"></script>

<style>
tr:has(td:first-child:contains("Rayforce")),
table tbody tr:nth-child(5) {
  background-color: rgba(21, 101, 192, 0.15) !important;
}
.performance-cards {
  display: flex;
  flex-wrap: wrap;
  gap: 1.5em;
  margin: 2em 0;
  justify-content: center;
}
.performance-card {
  flex: 1;
  min-width: 200px;
  max-width: 300px;
  background: rgba(42, 42, 42, 0.95);
  border: 1px solid rgba(139, 111, 71, 0.6);
  border-radius: 12px;
  padding: 2em 1.5em;
  text-align: center;
  box-shadow: 0 4px 6px rgba(0, 0, 0, 0.3);
  transition: transform 0.3s ease, box-shadow 0.3s ease;
}
@media (prefers-color-scheme: light) {
  .performance-card {
    background: rgba(250, 250, 250, 0.95);
    border: 1px solid rgba(139, 111, 71, 0.4);
  }
}
.performance-card:hover {
  transform: translateY(-5px);
  box-shadow: 0 8px 16px rgba(0, 0, 0, 0.4);
}
.performance-card .metric {
  font-size: 3.5em;
  font-weight: 700;
  color: #ff9800;
  margin: 0;
  line-height: 1;
  text-shadow: 0 2px 4px rgba(255, 152, 0, 0.3);
  font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, sans-serif;
}
.performance-card .description {
  font-size: 1em;
  color: #b0b0b0;
  margin-top: 0.8em;
  line-height: 1.4;
  font-weight: 400;
}
@media (prefers-color-scheme: light) {
  .performance-card .description {
    color: #666;
  }
}
</style>

<script>
document.addEventListener('DOMContentLoaded', function() {
  document.querySelectorAll('table tbody tr').forEach(function(row) {
    if (row.cells[0] && row.cells[0].textContent.trim() === 'Rayforce') {
      row.style.backgroundColor = 'rgba(21, 101, 192, 0.15)';
      row.style.fontWeight = '500';
    }
  });
});
</script>

<div class="performance-cards">

<div class="performance-card">
<div class="metric">33.5x</div>
<div class="description">Faster Window Join</div>
</div>

<div class="performance-card">
<div class="metric">21.2x</div>
<div class="description">Faster Inner Join</div>
</div>

<div class="performance-card">
<div class="metric">11.3x</div>
<div class="description">Faster Group By</div>
</div>

</div>

[:material-group: Group By](./group-by.md) - demonstrates competitive performance across all 7 group by queries, consistently ranking among the top performers.

[:material-set-left-right: Left Join](./left-join.md) - excels in left join operations, successfully completing queries where DuckDB and ClickHouse encounter Out of Memory errors.

[:material-border-inside: Inner Join](./inner-join.md) - demonstrates exceptional performance in inner join operations, delivering the fastest execution time among all systems.

[:material-window-restore: Window Join](./window-join.md) - delivers dramatic performance improvements in window join operations, reducing execution time from over half an hour to under one minute.
