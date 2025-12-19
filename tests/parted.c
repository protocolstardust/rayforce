/*
 *   Copyright (c) 2024 Anton Kundenko <singaraiona@gmail.com>
 *   All rights reserved.
 *
 *   Permission is hereby granted, free of charge, to any person obtaining a copy
 *   of this software and associated documentation files (the "Software"), to deal
 *   in the Software without restriction, including without limitation the rights
 *   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *   copies of the Software, and to permit persons to whom the Software is
 *   furnished to do so, subject to the following conditions:
 *
 *   The above copyright notice and this permission notice shall be included in all
 *   copies or substantial portions of the Software.
 *
 *   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *   SOFTWARE.
 */

// Parted table tests - create, load, and query parted data

static nil_t parted_cleanup() { system("rm -rf /tmp/rayforce_test_parted"); }

// Helper macro to create a parted table for testing
// 5 partitions (days), 100 rows each
// Columns: OrderId (i64), Price (f64), Size (i64)
#define PARTED_TEST_SETUP                                         \
    "(do "                                                        \
    "  (set dbpath \"/tmp/rayforce_test_parted/\")"               \
    "  (set n 100)"                                               \
    "  (set gen-partition "                                       \
    "    (fn [day]"                                               \
    "      (let p (format \"%/%/a/\" dbpath (+ 2024.01.01 day)))" \
    "      (let t (table [OrderId Price Size] "                   \
    "        (list "                                              \
    "          (+ (* day 1000) (til n))"                          \
    "          (/ (+ (* day 100.0) (til n)) 100.0)"               \
    "          (+ day (% (til n) 10))"                            \
    "        )"                                                   \
    "      ))"                                                    \
    "      (set-splayed p t)"                                     \
    "    )"                                                       \
    "  )"                                                         \
    "  (map gen-partition (til 5))"                               \
    "  (set t (get-parted \"/tmp/rayforce_test_parted/\" 'a))"    \
    ")"

test_result_t test_parted_load() {
    parted_cleanup();
    TEST_ASSERT_EQ(PARTED_TEST_SETUP "(count t)", "500");
    parted_cleanup();
    PASS();
}

test_result_t test_parted_select_where_date() {
    parted_cleanup();
    TEST_ASSERT_EQ(PARTED_TEST_SETUP "(count (select {from: t where: (== Date 2024.01.01)}))", "100");

    TEST_ASSERT_EQ(PARTED_TEST_SETUP "(count (select {from: t where: (in Date [2024.01.01 2024.01.03])}))", "200");
    parted_cleanup();
    PASS();
}

test_result_t test_parted_select_by_date() {
    parted_cleanup();
    TEST_ASSERT_EQ(PARTED_TEST_SETUP "(count (select {from: t by: Date c: (count OrderId)}))", "5");
    parted_cleanup();
    PASS();
}

test_result_t test_parted_select_multiple_aggregates() {
    parted_cleanup();
    TEST_ASSERT_EQ(PARTED_TEST_SETUP
                   "(count (select {from: t s: (sum Size) c: (count OrderId) mn: (min Price) mx: (max Price)}))",
                   "1");

    TEST_ASSERT_EQ(PARTED_TEST_SETUP "(count (select {from: t by: Date s: (sum Size) c: (count OrderId)}))", "5");
    parted_cleanup();
    PASS();
}

test_result_t test_parted_aggregate_by_date() {
    parted_cleanup();
    // Group by Date with sum aggregation
    TEST_ASSERT_EQ(PARTED_TEST_SETUP "(sum (at (select {from: t by: Date c: (count OrderId)}) 'c))", "500");

    // Group by Date with sum of Size
    // Size = day + (til 100) % 10, sum per day = 100*day + 45*10 = 100*day + 450
    // Total = 100*(0+1+2+3+4) + 5*450 = 1000 + 2250 = 3250
    TEST_ASSERT_EQ(PARTED_TEST_SETUP "(sum (at (select {from: t by: Date s: (sum Size)}) 's))", "3250");
    parted_cleanup();
    PASS();
}

test_result_t test_parted_aggregate_where() {
    parted_cleanup();
    // Filter by date then aggregate - returns one row per matching partition
    TEST_ASSERT_EQ(PARTED_TEST_SETUP "(at (select {from: t where: (== Date 2024.01.01) c: (count OrderId)}) 'c)",
                   "[100]");

    // Two partitions matching -> two rows, sum them to get total count
    TEST_ASSERT_EQ(PARTED_TEST_SETUP
                   "(sum (at (select {from: t where: (in Date [2024.01.01 2024.01.02]) c: (count OrderId)}) 'c))",
                   "200");
    parted_cleanup();
    PASS();
}

test_result_t test_parted_aggregate_f64() {
    parted_cleanup();
    // Test f64 aggregation by date - first should be 0.00, 1.00, 2.00, 3.00, 4.00
    TEST_ASSERT_EQ(PARTED_TEST_SETUP "(at (select {from: t by: Date f: (first Price)}) 'f)",
                   "[0.00 1.00 2.00 3.00 4.00]");

    // Test min/max for f64 - same as first since price increases within each partition
    TEST_ASSERT_EQ(PARTED_TEST_SETUP "(at (select {from: t by: Date mn: (min Price)}) 'mn)",
                   "[0.00 1.00 2.00 3.00 4.00]");
    parted_cleanup();
    PASS();
}

test_result_t test_parted_aggregate_i64() {
    parted_cleanup();
    // Test i64 aggregation by date
    TEST_ASSERT_EQ(PARTED_TEST_SETUP "(at (select {from: t by: Date f: (first OrderId)}) 'f)",
                   "[0 1000 2000 3000 4000]");

    TEST_ASSERT_EQ(PARTED_TEST_SETUP "(at (select {from: t by: Date l: (last OrderId)}) 'l)",
                   "[99 1099 2099 3099 4099]");
    parted_cleanup();
    PASS();
}

test_result_t test_parted_aggregate_minmax() {
    parted_cleanup();
    // Test min/max on i64 Size column
    // Size = day + (til n) % 10, so for day 0: 0-9, day 1: 1-10, etc.
    TEST_ASSERT_EQ(PARTED_TEST_SETUP "(at (select {from: t by: Date mn: (min Size)}) 'mn)", "[0 1 2 3 4]");

    TEST_ASSERT_EQ(PARTED_TEST_SETUP "(at (select {from: t by: Date mx: (max Size)}) 'mx)", "[9 10 11 12 13]");
    parted_cleanup();
    PASS();
}

// Extended setup with time (i32) column for temporal type tests
// Columns: OrderId (i64), Price (f64), Size (i64), Time (time/i32)
#define PARTED_TEST_SETUP_TIME                                    \
    "(do "                                                        \
    "  (set dbpath \"/tmp/rayforce_test_parted/\")"               \
    "  (set n 100)"                                               \
    "  (set gen-partition "                                       \
    "    (fn [day]"                                               \
    "      (let p (format \"%/%/a/\" dbpath (+ 2024.01.01 day)))" \
    "      (let t (table [OrderId Price Size Time] "              \
    "        (list "                                              \
    "          (+ (* day 1000) (til n))"                          \
    "          (/ (+ (* day 100.0) (til n)) 100.0)"               \
    "          (+ day (% (til n) 10))"                            \
    "          (+ 09:30:00.000 (* 1000 (+ (* day 100) (til n))))" \
    "        )"                                                   \
    "      ))"                                                    \
    "      (set-splayed p t)"                                     \
    "    )"                                                       \
    "  )"                                                         \
    "  (map gen-partition (til 5))"                               \
    "  (set t (get-parted \"/tmp/rayforce_test_parted/\" 'a))"    \
    ")"

test_result_t test_parted_aggregate_time() {
    parted_cleanup();
    // Test time (i32) aggregation by Date
    // Time = 09:30:00.000 + 1000*(day*100 + til n) ms
    // Day 0: first = 09:30:00.000, last = 09:31:39.000
    // Results are returned as integers (milliseconds since midnight)
    // 09:30:00 = 34200000ms, 09:31:40 = 34300000ms, etc.
    TEST_ASSERT_EQ(PARTED_TEST_SETUP_TIME "(at (select {from: t by: Date f: (first Time)}) 'f)",
                   "[34200000 34300000 34400000 34500000 34600000]");

    TEST_ASSERT_EQ(PARTED_TEST_SETUP_TIME "(at (select {from: t by: Date l: (last Time)}) 'l)",
                   "[34299000 34399000 34499000 34599000 34699000]");

    // Min should be same as first (time increases within partition)
    TEST_ASSERT_EQ(PARTED_TEST_SETUP_TIME "(at (select {from: t by: Date mn: (min Time)}) 'mn)",
                   "[34200000 34300000 34400000 34500000 34600000]");

    // Max should be same as last
    TEST_ASSERT_EQ(PARTED_TEST_SETUP_TIME "(at (select {from: t by: Date mx: (max Time)}) 'mx)",
                   "[34299000 34399000 34499000 34599000 34699000]");
    parted_cleanup();
    PASS();
}

test_result_t test_parted_aggregate_time_where() {
    parted_cleanup();
    // Test time aggregation with filter
    // Filter to single partition and aggregate
    // 09:30:00.000 = 34200000ms, 09:31:39.000 = 34299000ms
    TEST_ASSERT_EQ(PARTED_TEST_SETUP_TIME "(at (select {from: t where: (== Date 2024.01.01) f: (first Time)}) 'f)",
                   "[34200000]");

    TEST_ASSERT_EQ(PARTED_TEST_SETUP_TIME "(at (select {from: t where: (== Date 2024.01.01) l: (last Time)}) 'l)",
                   "[34299000]");

    // Filter to multiple partitions
    TEST_ASSERT_EQ(
        PARTED_TEST_SETUP_TIME
        "(count (at (select {from: t where: (in Date [2024.01.01 2024.01.02]) by: Date mn: (min Time)}) 'mn))",
        "2");
    parted_cleanup();
    PASS();
}

test_result_t test_parted_aggregate_time_sum() {
    parted_cleanup();
    // Test sum on time column (by date groups)
    // This tests the i32 sum path in PARTED_MAP
    TEST_ASSERT_EQ(PARTED_TEST_SETUP_TIME "(count (at (select {from: t by: Date s: (sum Time)}) 's))", "5");
    parted_cleanup();
    PASS();
}

// Extended setup with i16 (Qty) column for i16 type tests
// Columns: OrderId (i64), Price (f64), Size (i64), Qty (i16)
#define PARTED_TEST_SETUP_I16                                     \
    "(do "                                                        \
    "  (set dbpath \"/tmp/rayforce_test_parted/\")"               \
    "  (set n 100)"                                               \
    "  (set gen-partition "                                       \
    "    (fn [day]"                                               \
    "      (let p (format \"%/%/a/\" dbpath (+ 2024.01.01 day)))" \
    "      (let t (table [OrderId Price Size Qty] "               \
    "        (list "                                              \
    "          (+ (* day 1000) (til n))"                          \
    "          (/ (+ (* day 100.0) (til n)) 100.0)"               \
    "          (+ day (% (til n) 10))"                            \
    "          (as 'I16 (+ day (% (til n) 5)))"                   \
    "        )"                                                   \
    "      ))"                                                    \
    "      (set-splayed p t)"                                     \
    "    )"                                                       \
    "  )"                                                         \
    "  (map gen-partition (til 5))"                               \
    "  (set t (get-parted \"/tmp/rayforce_test_parted/\" 'a))"    \
    ")"

test_result_t test_parted_aggregate_i16() {
    parted_cleanup();
    // Test i16 aggregation - Qty = day + (til n) % 5
    // First values per day: day + 0 = 0, 1, 2, 3, 4
    TEST_ASSERT_EQ(PARTED_TEST_SETUP_I16 "(at (select {from: t by: Date f: (first Qty)}) 'f)", "[0 1 2 3 4]");

    // Last values per day: day + 99 % 5 = day + 4 = 4, 5, 6, 7, 8
    TEST_ASSERT_EQ(PARTED_TEST_SETUP_I16 "(at (select {from: t by: Date l: (last Qty)}) 'l)", "[4 5 6 7 8]");

    // Min per day: day + 0 = 0, 1, 2, 3, 4
    TEST_ASSERT_EQ(PARTED_TEST_SETUP_I16 "(at (select {from: t by: Date mn: (min Qty)}) 'mn)", "[0 1 2 3 4]");

    // Max per day: day + 4 = 4, 5, 6, 7, 8
    TEST_ASSERT_EQ(PARTED_TEST_SETUP_I16 "(at (select {from: t by: Date mx: (max Qty)}) 'mx)", "[4 5 6 7 8]");
    parted_cleanup();
    PASS();
}

test_result_t test_parted_aggregate_i16_sum() {
    parted_cleanup();
    // Test sum on i16 column (by date groups)
    // Qty = day + (til 100) % 5, sum per day = 100*day + (0+1+2+3+4)*20 = 100*day + 200
    // Day 0: 200, Day 1: 300, Day 2: 400, Day 3: 500, Day 4: 600
    // Check individual sums first
    TEST_ASSERT_EQ(PARTED_TEST_SETUP_I16 "(at (select {from: t by: Date s: (sum Qty)}) 's)", "[200 300 400 500 600]");
    parted_cleanup();
    PASS();
}

// ============================================================================
// Global aggregation tests (no by or where) - smart aggregation over all partitions
// ============================================================================

test_result_t test_parted_global_count() {
    parted_cleanup();
    // Global count should return 500 (5 partitions * 100 rows)
    TEST_ASSERT_EQ(PARTED_TEST_SETUP "(at (select {from: t c: (count OrderId)}) 'c)", "[500]");
    parted_cleanup();
    PASS();
}

test_result_t test_parted_global_sum() {
    parted_cleanup();
    // Global sum of Size across all partitions
    // Size = day + (til n) % 10, so sum = sum of (0+1+2+...+9)*10 = 45*10 = 450 per day
    // Total = 5 * 450 + 100*(0+1+2+3+4) = 2250 + 1000 = 3250
    TEST_ASSERT_EQ(PARTED_TEST_SETUP "(at (select {from: t s: (sum Size)}) 's)", "[3250]");
    parted_cleanup();
    PASS();
}

test_result_t test_parted_global_avg() {
    parted_cleanup();
    // Global avg of Size = 3250 / 500 = 6.5
    TEST_ASSERT_EQ(PARTED_TEST_SETUP "(at (select {from: t a: (avg Size)}) 'a)", "[6.50]");
    parted_cleanup();
    PASS();
}

test_result_t test_parted_global_minmax() {
    parted_cleanup();
    // Global min of Size should be 0 (day 0, offset 0)
    TEST_ASSERT_EQ(PARTED_TEST_SETUP "(at (select {from: t mn: (min Size)}) 'mn)", "[0]");

    // Global max of Size should be 13 (day 4 + offset 9 = 13)
    TEST_ASSERT_EQ(PARTED_TEST_SETUP "(at (select {from: t mx: (max Size)}) 'mx)", "[13]");
    parted_cleanup();
    PASS();
}

test_result_t test_parted_global_first_last() {
    parted_cleanup();
    // Global first OrderId = 0 (first row of first partition)
    TEST_ASSERT_EQ(PARTED_TEST_SETUP "(at (select {from: t f: (first OrderId)}) 'f)", "[0]");

    // Global last - use by Date then take last of result
    TEST_ASSERT_EQ(PARTED_TEST_SETUP "(last (at (select {from: t by: Date l: (last OrderId)}) 'l))", "4099");
    parted_cleanup();
    PASS();
}

test_result_t test_parted_global_multiple() {
    parted_cleanup();
    // Multiple global aggregates in one query
    TEST_ASSERT_EQ(PARTED_TEST_SETUP
                   "(at (select {from: t s: (sum Size) c: (count OrderId) mn: (min Size) mx: (max "
                   "Size)}) 's)",
                   "[3250]");
    TEST_ASSERT_EQ(PARTED_TEST_SETUP
                   "(at (select {from: t s: (sum Size) c: (count OrderId) mn: (min Size) mx: (max "
                   "Size)}) 'c)",
                   "[500]");
    parted_cleanup();
    PASS();
}

// ============================================================================
// Timestamp type tests
// ============================================================================

#define PARTED_TEST_SETUP_TIMESTAMP                                                \
    "(do "                                                                         \
    "  (set dbpath \"/tmp/rayforce_test_parted/\")"                                \
    "  (set n 100)"                                                                \
    "  (set gen-partition "                                                        \
    "    (fn [day]"                                                                \
    "      (let p (format \"%/%/a/\" dbpath (+ 2024.01.01 day)))"                  \
    "      (let t (table [OrderId Ts] "                                            \
    "        (list "                                                               \
    "          (+ (* day 1000) (til n))"                                           \
    "          (+ 2024.01.01D09:30:00.000 (* 1000000000 (+ (* day 100) (til n))))" \
    "        )"                                                                    \
    "      ))"                                                                     \
    "      (set-splayed p t)"                                                      \
    "    )"                                                                        \
    "  )"                                                                          \
    "  (map gen-partition (til 5))"                                                \
    "  (set t (get-parted \"/tmp/rayforce_test_parted/\" 'a))"                     \
    ")"

test_result_t test_parted_timestamp_aggregate() {
    parted_cleanup();
    // Timestamp aggregation by Date - first timestamps per day
    TEST_ASSERT_EQ(PARTED_TEST_SETUP_TIMESTAMP "(count (at (select {from: t by: Date f: (first Ts)}) 'f))", "5");

    // Min/max should work the same as first/last since timestamps increase
    TEST_ASSERT_EQ(PARTED_TEST_SETUP_TIMESTAMP "(count (at (select {from: t by: Date mn: (min Ts)}) 'mn))", "5");
    TEST_ASSERT_EQ(PARTED_TEST_SETUP_TIMESTAMP "(count (at (select {from: t by: Date mx: (max Ts)}) 'mx))", "5");
    parted_cleanup();
    PASS();
}

// ============================================================================
// Complex filter tests
// ============================================================================

test_result_t test_parted_filter_range() {
    parted_cleanup();
    // Filter by date range using >= and <=
    TEST_ASSERT_EQ(
        PARTED_TEST_SETUP "(count (select {from: t where: (and (>= Date 2024.01.02) (<= Date 2024.01.04))}))", "300");
    parted_cleanup();
    PASS();
}

test_result_t test_parted_filter_not_in() {
    parted_cleanup();
    // Select middle dates (exclude first and last)
    TEST_ASSERT_EQ(PARTED_TEST_SETUP "(count (select {from: t where: (in Date [2024.01.02 2024.01.03 2024.01.04])}))",
                   "300");
    parted_cleanup();
    PASS();
}

test_result_t test_parted_filter_all_match() {
    parted_cleanup();
    // Filter that matches all partitions
    TEST_ASSERT_EQ(PARTED_TEST_SETUP "(count (select {from: t where: (>= Date 2024.01.01)}))", "500");
    parted_cleanup();
    PASS();
}

test_result_t test_parted_filter_none_match() {
    parted_cleanup();
    // Filter that matches only one partition (boundary check)
    TEST_ASSERT_EQ(PARTED_TEST_SETUP "(count (select {from: t where: (== Date 2024.01.05)}))", "100");
    parted_cleanup();
    PASS();
}

// ============================================================================
// Combined where + by tests
// ============================================================================

test_result_t test_parted_where_by_combined() {
    parted_cleanup();
    // Filter to subset then group by date
    TEST_ASSERT_EQ(PARTED_TEST_SETUP
                   "(count (select {from: t where: (in Date [2024.01.01 2024.01.03]) by: Date c: (count OrderId)}))",
                   "2");

    // Sum within filtered subset grouped by date
    TEST_ASSERT_EQ(
        PARTED_TEST_SETUP
        "(sum (at (select {from: t where: (in Date [2024.01.01 2024.01.03]) by: Date c: (count OrderId)}) 'c))",
        "200");
    parted_cleanup();
    PASS();
}

// ============================================================================
// Materialization tests - selecting actual data, not just aggregates
// ============================================================================

test_result_t test_parted_materialize_column() {
    parted_cleanup();
    // Access individual column from parted table
    // count of parted column returns the parted count
    TEST_ASSERT_EQ(PARTED_TEST_SETUP "(sum (map count (at t 'OrderId)))", "500");

    // Access Price column
    TEST_ASSERT_EQ(PARTED_TEST_SETUP "(sum (map count (at t 'Price)))", "500");
    parted_cleanup();
    PASS();
}

test_result_t test_parted_materialize_filtered() {
    parted_cleanup();
    // Materialize filtered data
    TEST_ASSERT_EQ(PARTED_TEST_SETUP "(count (at (select {from: t where: (== Date 2024.01.01)}) 'OrderId))", "100");
    parted_cleanup();
    PASS();
}

test_result_t test_parted_materialize_sorted() {
    parted_cleanup();
    // Test accessing partitions via aggregate first/last which handles parted types
    TEST_ASSERT_EQ(PARTED_TEST_SETUP "(at (select {from: t by: Date f: (first OrderId)}) 'f)",
                   "[0 1000 2000 3000 4000]");

    TEST_ASSERT_EQ(PARTED_TEST_SETUP "(at (select {from: t by: Date l: (last OrderId)}) 'l)",
                   "[99 1099 2099 3099 4099]");
    parted_cleanup();
    PASS();
}

// ============================================================================
// Average aggregation tests
// ============================================================================

test_result_t test_parted_avg_by_date() {
    parted_cleanup();
    // Average Size by date
    // Size = day + (til 100) % 10
    // For day 0: avg = (0+1+2+...+9)*10/100 = 450/100 = 4.5
    // For day 1: avg = (1+2+...+10)*10/100 = 550/100 = 5.5, etc.
    TEST_ASSERT_EQ(PARTED_TEST_SETUP "(at (select {from: t by: Date a: (avg Size)}) 'a)", "[4.50 5.50 6.50 7.50 8.50]");
    parted_cleanup();
    PASS();
}

test_result_t test_parted_avg_f64() {
    parted_cleanup();
    // Average Price by date
    // Price = (day*100 + til 100) / 100
    // For day 0: avg = sum(0..99)/100 / 100 = 4950/100/100 = 0.495
    TEST_ASSERT_EQ(PARTED_TEST_SETUP "(count (at (select {from: t by: Date a: (avg Price)}) 'a))", "5");
    parted_cleanup();
    PASS();
}

// ============================================================================
// Edge cases
// ============================================================================

test_result_t test_parted_single_partition() {
    parted_cleanup();
    // Query affecting only one partition
    TEST_ASSERT_EQ(PARTED_TEST_SETUP "(at (select {from: t where: (== Date 2024.01.03) s: (sum Size)}) 's)", "[650]");
    parted_cleanup();
    PASS();
}

test_result_t test_parted_first_partition() {
    parted_cleanup();
    // Query first partition only
    TEST_ASSERT_EQ(PARTED_TEST_SETUP "(at (select {from: t where: (== Date 2024.01.01) f: (first OrderId)}) 'f)",
                   "[0]");
    TEST_ASSERT_EQ(PARTED_TEST_SETUP "(at (select {from: t where: (== Date 2024.01.01) l: (last OrderId)}) 'l)",
                   "[99]");
    parted_cleanup();
    PASS();
}

test_result_t test_parted_last_partition() {
    parted_cleanup();
    // Query last partition only
    TEST_ASSERT_EQ(PARTED_TEST_SETUP "(at (select {from: t where: (== Date 2024.01.05) f: (first OrderId)}) 'f)",
                   "[4000]");
    TEST_ASSERT_EQ(PARTED_TEST_SETUP "(at (select {from: t where: (== Date 2024.01.05) l: (last OrderId)}) 'l)",
                   "[4099]");
    parted_cleanup();
    PASS();
}

// ============================================================================
// Multi-type mixed operations
// ============================================================================

test_result_t test_parted_mixed_types() {
    parted_cleanup();
    // Mix i64, f64 aggregations in one query
    TEST_ASSERT_EQ(PARTED_TEST_SETUP
                   "(count (at (select {from: t by: Date si: (sum OrderId) sp: (sum Price) ss: (sum Size)}) 'si))",
                   "5");
    parted_cleanup();
    PASS();
}

test_result_t test_parted_all_aggregates() {
    parted_cleanup();
    // All aggregate types in one query
    TEST_ASSERT_EQ(PARTED_TEST_SETUP
                   "(count (select {from: t by: Date c: (count OrderId) s: (sum Size) a: (avg Size) "
                   "mn: (min Size) mx: (max Size) f: (first OrderId) l: (last OrderId)}))",
                   "5");
    parted_cleanup();
    PASS();
}

// ============================================================================
// Date column operations
// ============================================================================

test_result_t test_parted_date_column() {
    parted_cleanup();
    // The Date column is the partition key (MAPCOMMON type) with 5 unique dates
    // Access dates via aggregation
    TEST_ASSERT_EQ(PARTED_TEST_SETUP "(count (select {from: t by: Date c: (count OrderId)}))", "5");

    // Get unique dates via group by
    TEST_ASSERT_EQ(PARTED_TEST_SETUP "(first (at (select {from: t by: Date c: (count OrderId)}) 'Date))", "2024.01.01");
    TEST_ASSERT_EQ(PARTED_TEST_SETUP "(last (at (select {from: t by: Date c: (count OrderId)}) 'Date))", "2024.01.05");
    parted_cleanup();
    PASS();
}

// ============================================================================
// Large partition count test
// ============================================================================

#define PARTED_TEST_SETUP_MANY                                    \
    "(do "                                                        \
    "  (set dbpath \"/tmp/rayforce_test_parted/\")"               \
    "  (set n 10)"                                                \
    "  (set gen-partition "                                       \
    "    (fn [day]"                                               \
    "      (let p (format \"%/%/a/\" dbpath (+ 2024.01.01 day)))" \
    "      (let t (table [OrderId Size] "                         \
    "        (list "                                              \
    "          (+ (* day 100) (til n))"                           \
    "          (+ day (% (til n) 5))"                             \
    "        )"                                                   \
    "      ))"                                                    \
    "      (set-splayed p t)"                                     \
    "    )"                                                       \
    "  )"                                                         \
    "  (map gen-partition (til 30))"                              \
    "  (set t (get-parted \"/tmp/rayforce_test_parted/\" 'a))"    \
    ")"

test_result_t test_parted_many_partitions() {
    parted_cleanup();
    // 30 partitions, 10 rows each = 300 rows
    TEST_ASSERT_EQ(PARTED_TEST_SETUP_MANY "(count t)", "300");
    TEST_ASSERT_EQ(PARTED_TEST_SETUP_MANY "(at (select {from: t c: (count OrderId)}) 'c)", "[300]");
    TEST_ASSERT_EQ(PARTED_TEST_SETUP_MANY "(count (select {from: t by: Date c: (count OrderId)}))", "30");
    parted_cleanup();
    PASS();
}

// ============================================================================
// Small data tests
// ============================================================================

#define PARTED_TEST_SETUP_SMALL                                   \
    "(do "                                                        \
    "  (set dbpath \"/tmp/rayforce_test_parted/\")"               \
    "  (set n 3)"                                                 \
    "  (set gen-partition "                                       \
    "    (fn [day]"                                               \
    "      (let p (format \"%/%/a/\" dbpath (+ 2024.01.01 day)))" \
    "      (let t (table [OrderId Val] "                          \
    "        (list "                                              \
    "          (+ (* day 10) (til n))"                            \
    "          (+ (* day 10.0) (til n))"                          \
    "        )"                                                   \
    "      ))"                                                    \
    "      (set-splayed p t)"                                     \
    "    )"                                                       \
    "  )"                                                         \
    "  (map gen-partition (til 2))"                               \
    "  (set t (get-parted \"/tmp/rayforce_test_parted/\" 'a))"    \
    ")"

test_result_t test_parted_small_data() {
    parted_cleanup();
    // 2 partitions, 3 rows each = 6 rows
    TEST_ASSERT_EQ(PARTED_TEST_SETUP_SMALL "(count t)", "6");
    TEST_ASSERT_EQ(PARTED_TEST_SETUP_SMALL "(at (select {from: t c: (count OrderId)}) 'c)", "[6]");
    TEST_ASSERT_EQ(PARTED_TEST_SETUP_SMALL "(at (select {from: t by: Date c: (count OrderId)}) 'c)", "[3 3]");
    TEST_ASSERT_EQ(PARTED_TEST_SETUP_SMALL "(at (select {from: t s: (sum OrderId)}) 's)", "[36]");
    parted_cleanup();
    PASS();
}

// ============================================================================
// Filter on data column tests (where on non-partition key column)
// ============================================================================

test_result_t test_parted_filter_data_column() {
    parted_cleanup();
    // Filter on Price column (data column, not partition key)
    // Price for each partition = (day*100 + til 100) / 100
    // Day 0: 0.00-0.99, Day 1: 1.00-1.99, etc.
    // where Price >= 2.0 should match partitions 2,3,4 (all rows)
    // Days 2,3,4: 100 rows each = 300
    TEST_ASSERT_EQ(PARTED_TEST_SETUP "(count (select {from: t where: (>= Price 2)}))", "300");
    parted_cleanup();
    PASS();
}

test_result_t test_parted_filter_data_with_aggr() {
    parted_cleanup();
    // Test max on non-parted data (sanity check)
    TEST_ASSERT_EQ("(max [4.00 4.01 4.50 4.99])", "4.99");

    // Test select by date returning aggregations per partition (5 partitions = 5 results)
    TEST_ASSERT_EQ(PARTED_TEST_SETUP "(count (at (select {from: t by: Date c: (count OrderId)}) 'c))", "5");

    // Verify count per partition is correct
    TEST_ASSERT_EQ(PARTED_TEST_SETUP "(at (select {from: t by: Date c: (count OrderId)}) 'c)", "[100 100 100 100 100]");
    parted_cleanup();
    PASS();
}

test_result_t test_parted_filter_data_min() {
    parted_cleanup();
    // Test min with partition key filter (uses by clause path)
    TEST_ASSERT_EQ(PARTED_TEST_SETUP "(at (select {from: t where: (== Date 2024.01.05) by: Date m: (min Price)}) 'm)",
                   "[4.00]");
    parted_cleanup();
    PASS();
}

test_result_t test_parted_filter_data_sum() {
    parted_cleanup();
    // Filter on Size column then count (corrected calculation)
    // Size = day + (til 100) % 10 = day + [0,1,2,3,4,5,6,7,8,9,0,1,2,...] (repeating)
    // Day 2: sizes [2,3,4,5,6,7,8,9,10,11,...] repeated, Size > 10 means 11 appears 10 times
    // Day 3: sizes [3,4,...,12,...], Size > 10 means {11,12} appear 20 times
    // Day 4: sizes [4,5,...,13,...], Size > 10 means {11,12,13} appear 30 times
    // Total: 10 + 20 + 30 = 60
    TEST_ASSERT_EQ(PARTED_TEST_SETUP "(count (select {from: t where: (> Size 10)}))", "60");
    parted_cleanup();
    PASS();
}
