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
package androidx.pdf.viewer

import android.view.View
import android.widget.ProgressBar
import android.widget.TextView
import androidx.pdf.ActivityUtils
import androidx.pdf.R
import com.google.common.truth.Truth
import org.junit.Assert
import org.junit.Before
import org.junit.Ignore
import org.junit.Test
import org.junit.runner.RunWith
import org.robolectric.RobolectricTestRunner

/** Tests for [LoadingView]. */
@RunWith(RobolectricTestRunner::class)
class LoadingViewTest {
    private lateinit var loadingView: LoadingView

    @Before
    fun setUp() {
        loadingView = LoadingView(ActivityUtils.getThemedActivity())
    }

    @Ignore // b/376314114
    @Test
    fun testLoadingView_isNotNull() {
        Assert.assertNotNull(loadingView)
    }

    @Ignore // b/376314114
    @Test
    fun showLoadingView_showsLoadingSpinner() {
        val progressBar = loadingView.findViewById<ProgressBar>(R.id.loadingProgressBar)
        val errorMessage = loadingView.findViewById<TextView>(R.id.errorTextView)

        loadingView.showLoadingView()

        Assert.assertEquals(View.VISIBLE.toLong(), progressBar.visibility.toLong())
        Assert.assertEquals(View.GONE.toLong(), errorMessage.visibility.toLong())
    }

    @Ignore // b/376314114
    @Test
    fun showErrorView_showsErrorMessage() {
        val dummyErrorMessage = "testing error"
        val progressBar = loadingView.findViewById<ProgressBar>(R.id.loadingProgressBar)
        val errorMessage = loadingView.findViewById<TextView>(R.id.errorTextView)

        loadingView.showErrorView(dummyErrorMessage)

        Assert.assertEquals(View.GONE.toLong(), progressBar.visibility.toLong())
        Assert.assertEquals(View.VISIBLE.toLong(), errorMessage.visibility.toLong())
        Truth.assertThat(errorMessage.text).isEqualTo(dummyErrorMessage)
    }
}
