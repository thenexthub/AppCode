/*
 * Copyright 2021 The Android Open Source Project
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

package androidx.window.demo.embedding

import android.content.Intent
import android.graphics.Color
import android.graphics.drawable.ColorDrawable
import android.os.Bundle
import android.view.View
import android.widget.TextView
import androidx.lifecycle.Lifecycle
import androidx.lifecycle.lifecycleScope
import androidx.lifecycle.repeatOnLifecycle
import androidx.window.demo.common.EdgeToEdgeActivity
import androidx.window.demo.databinding.ActivitySplitActivityListLayoutBinding
import androidx.window.demo.embedding.SplitActivityDetail.Companion.EXTRA_SELECTED_ITEM
import androidx.window.embedding.SplitController
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.launch
import kotlinx.coroutines.withContext

private lateinit var viewBinding: ActivitySplitActivityListLayoutBinding

open class SplitActivityList : EdgeToEdgeActivity() {
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        val color = Color.parseColor("#e0f7fa")
        viewBinding = ActivitySplitActivityListLayoutBinding.inflate(layoutInflater)
        setContentView(viewBinding.root)
        viewBinding.root.setBackgroundColor(color)
        val splitController = SplitController.getInstance(this)

        window.setBackgroundDrawable(ColorDrawable(color))

        lifecycleScope.launch {
            // The block passed to repeatOnLifecycle is executed when the lifecycle
            // is at least STARTED and is cancelled when the lifecycle is STOPPED.
            // It automatically restarts the block when the lifecycle is STARTED again.
            lifecycle.repeatOnLifecycle(Lifecycle.State.STARTED) {
                splitController.splitInfoList(this@SplitActivityList).collect { newSplitInfos ->
                    withContext(Dispatchers.Main) {
                        viewBinding.infoButton.visibility =
                            if (newSplitInfos.isEmpty()) View.VISIBLE else View.GONE
                    }
                }
            }
        }
    }

    open fun onItemClick(view: View) {
        val text = (view as TextView).text ?: throw IllegalArgumentException()
        val startIntent = Intent(this, SplitActivityDetail::class.java)
        startIntent.putExtra(EXTRA_SELECTED_ITEM, text)
        startActivity(startIntent)
    }
}
