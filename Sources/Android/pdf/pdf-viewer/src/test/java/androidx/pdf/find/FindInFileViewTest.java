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

package androidx.pdf.find;

import static android.view.View.GONE;
import static android.view.View.VISIBLE;

import static com.google.common.truth.Truth.assertThat;

import static org.mockito.ArgumentMatchers.anyInt;
import static org.mockito.Mockito.doNothing;

import androidx.pdf.ActivityUtils;
import androidx.pdf.viewer.ImmersiveModeRequester;
import androidx.pdf.viewer.PaginatedView;
import androidx.pdf.viewer.loader.PdfLoader;

import com.google.android.material.floatingactionbutton.FloatingActionButton;

import junit.framework.TestCase;

import org.junit.After;
import org.junit.Before;
import org.junit.Ignore;
import org.junit.Test;
import org.junit.runner.RunWith;
import org.mockito.Mock;
import org.mockito.MockitoAnnotations;
import org.robolectric.RobolectricTestRunner;

/**
 * Unit tests for {@link FindInFileView}
 */
@RunWith(RobolectricTestRunner.class)
public class FindInFileViewTest extends TestCase {
    @Mock
    private PdfLoader mPdfLoader;
    @Mock
    private PaginatedView mPaginatedView;
    @Mock
    private FloatingActionButton mAnnotationButton;
    @Mock
    private ImmersiveModeRequester mImmersiveModeRequester;
    private FindInFileView mFindInFileView;
    private AutoCloseable mOpenMocks;

    @Before
    public void setUp() throws Exception {
        mOpenMocks = MockitoAnnotations.openMocks(this);
        mFindInFileView = new FindInFileView(ActivityUtils.INSTANCE.getThemedActivity());
        mFindInFileView.setPdfLoader(mPdfLoader);
        mFindInFileView.setPaginatedView(mPaginatedView);
        mFindInFileView.setAnnotationButton(mAnnotationButton, mImmersiveModeRequester);
    }

    @After
    public void tearDown() throws Exception {
        mOpenMocks.close();
    }

    @Ignore // b/376314114
    @Test
    public void testSetFindInFileView_visibilityTrue() {
        doNothing().when(mAnnotationButton).setVisibility(anyInt());
        mFindInFileView.setFindInFileView(true);
        assertThat(mFindInFileView.getVisibility()).isEqualTo(VISIBLE);
    }

    @Ignore // b/376314114
    @Test
    public void testSetFindInFileView_visibilityFalse() {
        mFindInFileView.setFindInFileView(false);
        assertThat(mFindInFileView.getVisibility()).isEqualTo(GONE);
    }
}
