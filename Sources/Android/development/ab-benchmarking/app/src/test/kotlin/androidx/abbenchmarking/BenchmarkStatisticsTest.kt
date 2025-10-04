/*
 * Copyright 2025 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package androidx.abbenchmarking

import com.google.common.truth.Truth.assertThat
import java.io.ByteArrayOutputStream
import java.io.PrintStream
import java.nio.file.Path
import kotlin.io.path.exists
import kotlin.io.path.name
import org.junit.jupiter.api.Test
import org.junit.jupiter.api.io.TempDir

class BenchmarkStatisticsTest {

    // A small tolerance for comparing floating-point numbers.
    private val tolerance = 0.01

    @Test
    fun calculateBootstrapCIMedianDifference_withClearDifference() {
        // GIVEN two datasets with clearly different medians
        // Median of data1 is 100.0, Median of data2 is 120.0
        val data1 = doubleArrayOf(98.0, 99.0, 100.0, 101.0, 102.0)
        val data2 = doubleArrayOf(118.0, 119.0, 120.0, 121.0, 122.0)

        // WHEN the confidence interval is calculated
        val ci = calculateBootstrapCIMedianDifference(data1, data2)

        // THEN the resulting interval should not contain zero, indicating a significant difference
        // The expected values are deterministic due to the fixed random seed.
        assertThat(ci.lower).isWithin(0.01).of(17.0)
        assertThat(ci.upper).isWithin(0.01).of(23.0)
        assertThat(ci.lower).isGreaterThan(0.0)
        assertThat(ci.upper).isGreaterThan(0.0)
    }

    @Test
    fun calculateBootstrapCIMedianDifference_withOverlappingData() {
        // GIVEN two datasets with similar medians and overlapping values
        // Median of data1 is 100.0, Median of data2 is 101.0
        val data1 = doubleArrayOf(98.0, 99.0, 100.0, 101.0, 102.0)
        val data2 = doubleArrayOf(99.0, 100.0, 101.0, 102.0, 103.0)

        // WHEN the confidence interval is calculated
        val ci = calculateBootstrapCIMedianDifference(data1, data2)

        // THEN the resulting interval should contain zero, suggesting no significant difference
        // The expected values are deterministic due to the fixed random seed.
        assertThat(ci.lower).isWithin(0.01).of(-2.0)
        assertThat(ci.upper).isWithin(0.01).of(4.0)
        assertThat(ci.lower).isLessThan(0.0)
        assertThat(ci.upper).isGreaterThan(0.0)
    }

    @Test
    fun calculateBCaCIMedianDifference_withClearDifference() {
        // GIVEN two datasets with clearly different medians
        val data1 = doubleArrayOf(98.0, 99.0, 100.0, 101.0, 102.0) // Median: 100.0
        val data2 = doubleArrayOf(118.0, 119.0, 120.0, 121.0, 122.0) // Median: 120.0
        val observedMedianDiff = 20.0

        // WHEN the BCa confidence interval is calculated
        val ci = calculateBCaCIMedianDifference(data1, data2, observedMedianDiff)

        // THEN the interval should not contain zero
        // With simple symmetric data, BCa can be close to percentile, which is fine.
        assertThat(ci.lower).isWithin(0.01).of(16.76)
        assertThat(ci.upper).isWithin(0.01).of(22.0)
        assertThat(ci.lower).isGreaterThan(0.0)
    }

    @Test
    fun calculateBCaCIMedianDifference_withOverlappingData() {
        // GIVEN two datasets with similar medians
        val data1 = doubleArrayOf(98.0, 99.0, 100.0, 101.0, 102.0) // Median: 100.0
        val data2 = doubleArrayOf(99.0, 100.0, 101.0, 102.0, 103.0) // Median: 101.0
        val observedMedianDiff = 1.0

        // WHEN the BCa confidence interval is calculated
        val ci = calculateBCaCIMedianDifference(data1, data2, observedMedianDiff)

        // THEN the interval should contain zero
        assertThat(ci.lower).isWithin(0.01).of(-2.24)
        assertThat(ci.upper).isWithin(0.01).of(3.0)
        assertThat(ci.lower).isLessThan(0.0)
        assertThat(ci.upper).isGreaterThan(0.0)
    }

    @Test
    fun calculateBCaCIMedianDifference_fallsBackToBootstrapCI_whenBiasIsUndefined() {
        // GIVEN two datasets where all elements are identical within each set.
        // This creates a scenario where all bootstrap replicates of the median difference
        // are identical to the observed median difference.
        val data1 = doubleArrayOf(10.0, 10.0, 10.0, 10.0, 10.0) // Median: 10.0
        val data2 = doubleArrayOf(20.0, 20.0, 20.0, 20.0, 20.0) // Median: 20.0
        val observedMedianDiff = 10.0

        // In this case, every bootstrap median difference will be exactly 10.0.
        // This makes the proportion of bootstrap replicates less than the observed median diff
        // (10.0)
        // equal to 0. The bias-correction factor (z₀) becomes infinite, and the BCa method
        // must fall back to the standard percentile bootstrap method.

        // and GIVEN the standard output is redirected to capture the warning
        val originalOut = System.out
        val outContent = ByteArrayOutputStream()
        System.setOut(PrintStream(outContent))

        // WHEN the BCa confidence interval is calculated
        val bcaResult = calculateBCaCIMedianDifference(data1, data2, observedMedianDiff)

        // THEN the system should print a fallback warning
        val consoleOutput = outContent.toString()
        assertThat(consoleOutput)
            .contains(
                "Warning: Bias-correction factor is undefined (z₀ is infinite). " +
                    "Falling back to the standard percentile confidence interval."
            )

        // AND the result from BCa should be identical to a direct call to the percentile method
        val bootstrapResult = calculateBootstrapCIMedianDifference(data1, data2)

        assertThat(bcaResult.lower).isEqualTo(bootstrapResult.lower)
        assertThat(bcaResult.upper).isEqualTo(bootstrapResult.upper)

        // AND the result itself should be a single point, since there's no variation
        assertThat(bcaResult.lower).isWithin(tolerance).of(10.0)
        assertThat(bcaResult.upper).isWithin(tolerance).of(10.0)

        // FINALLY, restore the original standard output
        System.setOut(originalOut)
    }

    @Test
    fun calculateBCaCIMedianDifference_withEmptyInput() {
        // GIVEN one empty dataset
        val data1 = doubleArrayOf(1.0, 2.0, 3.0)
        val data2 = doubleArrayOf()

        // WHEN the confidence interval is calculated
        val ci = calculateBCaCIMedianDifference(data1, data2, Double.NaN)

        // THEN the result should be NaN for both bounds
        assertThat(ci.lower).isNaN()
        assertThat(ci.upper).isNaN()
    }

    @Test
    fun calculateStatistics_withSignificantDifference() {
        // GIVEN two datasets where set B is consistently ~10% slower than set A.
        val dataA = doubleArrayOf(100.0, 101.0, 99.0, 102.0, 98.0) // mean ~100
        val dataB = doubleArrayOf(110.0, 111.0, 109.0, 112.0, 108.0) // mean ~110

        // WHEN statistics are calculated
        val stats = calculateStatistics(dataA, dataB)

        // THEN the results should reflect the difference
        assertThat(stats.count1).isEqualTo(5)
        assertThat(stats.mean1).isWithin(tolerance).of(100.0)
        assertThat(stats.mean2).isWithin(tolerance).of(110.0)
        assertThat(stats.meanDiffPercent).isWithin(tolerance).of(10.0) // B is 10% slower

        assertThat(stats.median1).isWithin(tolerance).of(100.0)
        assertThat(stats.median2).isWithin(tolerance).of(110.0)
        assertThat(stats.medianDiff).isWithin(tolerance).of(10.0)
        assertThat(stats.medianDiffPercent).isWithin(tolerance).of(10.0)

        // The p-value should be very low, indicating a significant difference.
        assertThat(stats.pValue).isLessThan(0.05)
        // AND confidence intervals should not contain zero
        assertThat(stats.medianDiffCI.lower).isGreaterThan(0.0)
    }

    @Test
    fun calculateStatistics_withNoSignificantDifference() {
        // GIVEN two nearly identical datasets.
        val dataA = doubleArrayOf(100.0, 101.0, 99.0, 102.0, 98.0)
        val dataB = doubleArrayOf(100.1, 101.1, 99.1, 102.1, 98.1)

        // WHEN statistics are calculated
        val stats = calculateStatistics(dataA, dataB)

        // THEN the differences should be minimal
        assertThat(stats.meanDiffPercent).isWithin(tolerance).of(0.1)
        assertThat(stats.medianDiff).isWithin(tolerance).of(0.1)

        // The p-value should be high, indicating no significant difference.
        assertThat(stats.pValue).isGreaterThan(0.05)
        // AND confidence intervals should contain zero
        assertThat(stats.medianDiffCI.lower).isLessThan(0.0)
        assertThat(stats.medianDiffCI.upper).isGreaterThan(0.0)
    }

    @Test
    fun calculateStatistics_handlesImprovement() {
        // GIVEN two datasets where set B is consistently faster than set A.
        val dataA = doubleArrayOf(110.0, 111.0, 109.0, 112.0, 108.0) // mean ~110
        val dataB = doubleArrayOf(100.0, 101.0, 99.0, 102.0, 98.0) // mean ~100

        // WHEN statistics are calculated
        val stats = calculateStatistics(dataA, dataB)

        // THEN the differences should be negative
        assertThat(stats.meanDiffPercent).isWithin(tolerance).of(-9.09) // (100-110)/110
        assertThat(stats.medianDiff).isWithin(tolerance).of(-10.0)
        assertThat(stats.medianDiffPercent).isWithin(tolerance).of(-9.09)
    }

    @Test
    fun confidenceInterval_shouldContainTheMedianDifference() {
        // GIVEN two datasets
        val dataA = doubleArrayOf(100.0, 101.0, 99.0, 102.0, 98.0, 105.0, 95.0)
        val dataB = doubleArrayOf(110.0, 111.0, 109.0, 112.0, 108.0, 115.0, 105.0)

        // WHEN statistics are calculated
        val stats = calculateStatistics(dataA, dataB)

        // THEN the calculated median difference should fall within the confidence interval
        assertThat(stats.medianDiff).isAtLeast(stats.medianDiffCI.lower)
        assertThat(stats.medianDiff).isAtMost(stats.medianDiffCI.upper)
    }

    @Test
    fun printSummary_formatsOutputCorrectly_withSignificantDifference() {
        // GIVEN a StatisticsResult object with known values indicating a significant difference
        val stats =
            StatisticsResult(
                count1 = 10,
                min1 = 95.5,
                mean1 = 100.0,
                median1 = 99.0,
                std1 = 5.0,
                count2 = 10,
                min2 = 105.5,
                mean2 = 110.0,
                median2 = 109.0,
                std2 = 6.0,
                minDiff = 10.0,
                minDiffPercent = 10.47,
                meanDiff = 10.0,
                meanDiffPercent = 10.0,
                medianDiff = 10.0,
                medianDiffPercent = 10.101,
                pValue = 0.04,
                medianDiffCI = ConfidenceInterval(8.1, 12.2),
                medianDiffCIPercent = ConfidenceInterval(8.18, 12.32),
            )
        val benchmarkName = "MyBenchmarkTest"

        // and GIVEN the standard output is redirected to capture the result
        val originalOut = System.out
        val outContent = ByteArrayOutputStream()
        System.setOut(PrintStream(outContent))

        // WHEN the printSummary function is called
        printSummary(benchmarkName, stats)

        // THEN the output should be a correctly formatted multi-line string
        val expectedOutput =
            """
--- Comparison for: MyBenchmarkTest ---
                       Dataset 1 (Branch A) | Dataset 2 (Branch B)
----------------------------------------------------------------
Count                | 10                   | 10
Min (ns)             | 95.50                | 105.50
Mean (ns)            | 100.00               | 110.00
Median (ns)          | 99.00                | 109.00
Std. Dev. (ns)       | 5.00                 | 6.00
Min Difference:      | 10.00 ns (10.47%)
Mean Difference:     | 10.00 ns (10.00%)
Median Difference:   | 10.00 ns (10.10%)
95% CI of Diff:      | [8.10, 12.20] ns ([8.18%, 12.32%])

The confidence interval does not contain zero, suggesting a statistically significant difference exists between the medians.

--- MannWhitneyUTest Results (Branch B vs. Branch A) ---
P-value:                 0.0400
Result:                  Statistically significant difference.

-------------------------------------------------------

"""

        val actualOutput = outContent.toString()
        assertThat(actualOutput).isEqualTo(expectedOutput)

        // FINALLY, restore the original standard output
        System.setOut(originalOut)
    }

    @Test
    fun printCsvOutput_formatsOutputCorrectly() {
        // GIVEN a StatisticsResult object with known values
        val stats =
            StatisticsResult(
                count1 = 10,
                min1 = 95.5,
                mean1 = 100.0,
                median1 = 99.0,
                std1 = 5.0,
                count2 = 10,
                min2 = 105.5,
                mean2 = 110.0,
                median2 = 109.0,
                std2 = 6.0,
                minDiff = 10.0,
                minDiffPercent = 10.47,
                meanDiff = 10.0,
                meanDiffPercent = 10.0,
                medianDiff = 10.0,
                medianDiffPercent = 10.101,
                pValue = 0.04567,
                medianDiffCI = ConfidenceInterval(8.1, 12.2),
                medianDiffCIPercent = ConfidenceInterval(8.18, 12.32),
            )
        val benchmarkName = "MyBenchmarkTest"

        // and GIVEN the standard output is redirected to capture the result
        val originalOut = System.out
        val outContent = ByteArrayOutputStream()
        System.setOut(PrintStream(outContent))

        // WHEN the printCsvOutput function is called
        printCsvOutput(benchmarkName, stats)

        // THEN the output should be a correctly formatted multi-line CSV string
        val expectedOutput =
            """
        --- Machine-Readable CSV ---
        benchmarkName,count,min1,min2,min_diff,min_diff_%,mean1,mean2,mean_diff_%,median1,median2,p-value,median_diff_%,median_diff,median_diff_ci_lower,median_diff_ci_upper,median_diff_ci_lower_%,median_diff_ci_upper_%
        MyBenchmarkTest,10,95.50,105.50,10.00,10.47%,100.00,110.00,10.00%,99.00,109.00,0.0457,10.10%,10.00,8.10,12.20,8.18%,12.32%
    """
                .trimIndent() + System.lineSeparator() // printCsvOutput adds a final newline

        val actualOutput = outContent.toString()
        assertThat(actualOutput).isEqualTo(expectedOutput)

        // FINALLY, restore the original standard output
        System.setOut(originalOut)
    }

    @Test
    fun createHistogramPlot_savesFileToTempDir(@TempDir tempPath: Path) {
        // GIVEN two datasets, a benchmark name, and a temporary output path
        val data1 = doubleArrayOf(100.0, 102.0, 105.0)
        val data2 = doubleArrayOf(110.0, 112.0, 115.0)
        val benchmarkName = "MyPlotTest"

        // and GIVEN the standard output is redirected to capture console messages
        val originalOut = System.out
        val outContent = ByteArrayOutputStream()
        System.setOut(PrintStream(outContent))

        // WHEN the createHistogramPlot function is called
        createHistogramPlot(benchmarkName, data1, data2, tempPath)

        // THEN a file with the correct name should exist in the temporary directory
        val expectedFileName = "${benchmarkName}_histogram.png"
        val expectedFile = tempPath.resolve(expectedFileName)

        assertThat(expectedFile.exists()).isTrue()
        assertThat(expectedFile.name).isEqualTo(expectedFileName)

        // AND the console output should confirm the file was saved
        val consoleOutput = outContent.toString()
        assertThat(consoleOutput).contains("--- Graphical Plot ---")
        assertThat(consoleOutput).contains("Saved histogram to: $expectedFile")

        // FINALLY, restore the original standard output
        System.setOut(originalOut)
    }
}
