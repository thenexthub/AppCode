/*
 * Copyright 2024 The Android Open Source Project
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

package androidx.appsearch.cts.ast.query;

import static com.google.common.truth.Truth.assertThat;

import static org.junit.Assert.assertThrows;

import androidx.appsearch.app.SearchSpec;
import androidx.appsearch.ast.query.SemanticSearchNode;
import androidx.appsearch.flags.Flags;
import androidx.appsearch.testutil.AppSearchTestUtils;
import androidx.appsearch.testutil.flags.RequiresFlagsEnabled;

import org.junit.Rule;
import org.junit.Test;
import org.junit.rules.RuleChain;

@RequiresFlagsEnabled(Flags.FLAG_ENABLE_ABSTRACT_SYNTAX_TREES)
public class SemanticSearchNodeCtsTest {
    @Rule
    public final RuleChain mRuleChain = AppSearchTestUtils.createCommonTestRules();

    @Test
    public void testEquals_identical() {
        SemanticSearchNode semanticSearchNodeOne = new SemanticSearchNode(0, -1.5f, 3,
                SearchSpec.EMBEDDING_SEARCH_METRIC_TYPE_DOT_PRODUCT);
        SemanticSearchNode semanticSearchNodeTwo = new SemanticSearchNode(0, -1.5f, 3,
                SearchSpec.EMBEDDING_SEARCH_METRIC_TYPE_DOT_PRODUCT);

        assertThat(semanticSearchNodeOne).isEqualTo(semanticSearchNodeTwo);
        assertThat(semanticSearchNodeOne.hashCode()).isEqualTo(semanticSearchNodeTwo.hashCode());
    }

    @Test
    public void testConstructor_throwsOnNegativeIndex() {
        assertThrows(IllegalArgumentException.class, () -> new SemanticSearchNode(-1));
    }

    @Test
    public void testConstructor_throwsOnInvalidBounds() {
        IllegalArgumentException thrown =  assertThrows(IllegalArgumentException.class,
                () -> new SemanticSearchNode(0, 1, -1));

        assertThat(thrown).hasMessageThat().contains("lower bound must be less than or equal");
    }

    @Test
    public void testConstructor_throwsOnInvalidDistanceMetric() {
        assertThrows(IllegalArgumentException.class,
                () -> new SemanticSearchNode(0, -1, 1, -1));

        assertThrows(IllegalArgumentException.class,
                () -> new SemanticSearchNode(0, -1, 1, 4));
    }

    @Test
    public void testConstructor_allDefaultValues() {
        SemanticSearchNode semanticSearchNode = new SemanticSearchNode(0);

        assertThat(semanticSearchNode.getLowerBound()).isEqualTo(Float.NEGATIVE_INFINITY);
        assertThat(semanticSearchNode.getUpperBound()).isEqualTo(Float.POSITIVE_INFINITY);
        assertThat(semanticSearchNode.getDistanceMetric())
                .isEqualTo(SearchSpec.EMBEDDING_SEARCH_METRIC_TYPE_DEFAULT);
    }

    @Test
    public void testConstructor_lowerBoundSet_defaultValues() {
        SemanticSearchNode semanticSearchNode = new SemanticSearchNode(0, -1);

        assertThat(semanticSearchNode.getUpperBound()).isEqualTo(Float.POSITIVE_INFINITY);
        assertThat(semanticSearchNode.getDistanceMetric())
                .isEqualTo(SearchSpec.EMBEDDING_SEARCH_METRIC_TYPE_DEFAULT);
    }

    @Test
    public void testConstructor_boundsSet_defaultValues() {
        SemanticSearchNode semanticSearchNode = new SemanticSearchNode(0, -1, 1);

        assertThat(semanticSearchNode.getDistanceMetric())
                .isEqualTo(SearchSpec.EMBEDDING_SEARCH_METRIC_TYPE_DEFAULT);
    }

    @Test
    public void testGetFunctionName_functionNameCorrect() {
        SemanticSearchNode semanticSearchNode = new SemanticSearchNode(0);
        assertThat(semanticSearchNode.getFunctionName()).isEqualTo("semanticSearch");
    }

    @Test
    public void testGetChildren_returnsEmptyList() {
        SemanticSearchNode semanticSearchNode = new SemanticSearchNode(0);
        assertThat(semanticSearchNode.getChildren()).isEmpty();
    }

    @Test
    public void testSetDistanceMetric_setDistanceMetricCorrectly() {
        SemanticSearchNode semanticSearchNode = new SemanticSearchNode(0);
        semanticSearchNode.setDistanceMetric(SearchSpec.EMBEDDING_SEARCH_METRIC_TYPE_DOT_PRODUCT);
        assertThat(semanticSearchNode.getDistanceMetric())
                .isEqualTo(SearchSpec.EMBEDDING_SEARCH_METRIC_TYPE_DOT_PRODUCT);
    }

    @Test
    public void testSetDistanceMetric_throwsOnInvalidDistanceMetric() {
        assertThrows(IllegalArgumentException.class,
                () -> new SemanticSearchNode(0).setDistanceMetric(-1));

        assertThrows(IllegalArgumentException.class,
                () -> new SemanticSearchNode(0).setDistanceMetric(4));
    }

    @Test
    public void testSetBounds_setLowerBoundUpperBoundCorrectly() {
        SemanticSearchNode semanticSearchNode = new SemanticSearchNode(0);
        semanticSearchNode.setBounds(0, 1);
        assertThat(semanticSearchNode.getLowerBound()).isEqualTo(0);
        assertThat(semanticSearchNode.getUpperBound()).isEqualTo(1);
    }

    @Test
    public void testSetBounds_throwsOnInvalidInputs() {
        SemanticSearchNode semanticSearchNode = new SemanticSearchNode(0);

        IllegalArgumentException thrown = assertThrows(IllegalArgumentException.class,
                () -> semanticSearchNode.setBounds(1, -1));
        assertThat(thrown).hasMessageThat().contains("lower bound must be less than or equal");
    }

    @Test
    public void testSetVectorIndex_setVectorIndexCorrectly() {
        SemanticSearchNode semanticSearchNode = new SemanticSearchNode(0);
        semanticSearchNode.setVectorIndex(1);
        assertThat(semanticSearchNode.getVectorIndex()).isEqualTo(1);
    }

    @Test
    public void testSetVectorIndex_throwsOnNegativeInput() {
        SemanticSearchNode semanticSearchNode = new SemanticSearchNode(0);
        assertThrows(IllegalArgumentException.class, () -> semanticSearchNode.setVectorIndex(-1));
    }

    @Test
    public void testToString_allDefaults_returnsCorrectString() {
        SemanticSearchNode semanticSearchNode = new SemanticSearchNode(0);
        assertThat(semanticSearchNode.toString())
                .isEqualTo("semanticSearch(getEmbeddingParameter(0))");
    }

    @Test
    public void testToString_lowerBoundSet_returnsCorrectString() {
        SemanticSearchNode semanticSearchNode = new SemanticSearchNode(0, -0.5f);
        assertThat(semanticSearchNode.toString())
                .isEqualTo("semanticSearch(getEmbeddingParameter(0), -0.5)");
    }

    @Test
    public void testToString_BoundsSet_returnsCorrectString() {
        SemanticSearchNode semanticSearchNode = new SemanticSearchNode(0, -0.5f, 1);
        assertThat(semanticSearchNode.toString())
                .isEqualTo("semanticSearch(getEmbeddingParameter(0), -0.5, 1.0)");
    }

    @Test
    public void testToString_noDefaults_returnsCorrectString() {
        SemanticSearchNode semanticSearchNode = new SemanticSearchNode(0,
                -0.5f, 1, SearchSpec.EMBEDDING_SEARCH_METRIC_TYPE_DOT_PRODUCT);
        assertThat(semanticSearchNode.toString())
                .isEqualTo("semanticSearch(getEmbeddingParameter(0), -0.5, 1.0, \"DOT_PRODUCT\")");
    }

    @Test
    public void testToString_distanceMetricSet_defaultBounds_returnsCorrectString() {
        SemanticSearchNode semanticSearchNode = new SemanticSearchNode(0,
                -0.5f, 1, SearchSpec.EMBEDDING_SEARCH_METRIC_TYPE_COSINE);
        semanticSearchNode.setBounds(Float.NEGATIVE_INFINITY, Float.POSITIVE_INFINITY);
        assertThat(semanticSearchNode.toString()).isEqualTo(
                "semanticSearch(getEmbeddingParameter(0), -" + Float.MAX_VALUE + ", "
                        + Float.MAX_VALUE + ", " + "\"COSINE\")");
    }

    @Test
    public void testToString_upperBoundSet_defaultLowerBound_returnsCorrectString() {
        SemanticSearchNode semanticSearchNode = new SemanticSearchNode(0,
                -0.5f, 1);
        semanticSearchNode.setBounds(Float.NEGATIVE_INFINITY, 1);
        assertThat(semanticSearchNode.toString()).isEqualTo(
                "semanticSearch(getEmbeddingParameter(0), -" + Float.MAX_VALUE + ", " + "1.0)");
    }
}
