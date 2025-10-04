/*
 * Copyright 2023 The Android Open Source Project
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

package androidx.window.extensions.embedding;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertFalse;
import static org.junit.Assert.assertNotEquals;
import static org.junit.Assert.assertThrows;

import android.graphics.Color;

import org.junit.Test;
import org.junit.runner.RunWith;
import org.robolectric.RobolectricTestRunner;

/** Test for {@link AnimationBackground} */
@RunWith(RobolectricTestRunner.class)
public class AnimationBackgroundTest {

    @Test
    public void testDefaultBackground() {
        assertEquals(AnimationBackground.ANIMATION_BACKGROUND_DEFAULT,
                AnimationBackground.ANIMATION_BACKGROUND_DEFAULT);
        assertFalse(AnimationBackground.ANIMATION_BACKGROUND_DEFAULT
                instanceof AnimationBackground.ColorBackground);
    }

    @Test
    public void testCreateColorBackground() {
        final AnimationBackground.ColorBackground background =
                AnimationBackground.createColorBackground(Color.BLUE);

        assertEquals(Color.BLUE, background.getColor());
        assertEquals(background, AnimationBackground.createColorBackground(
                Color.BLUE));
        assertNotEquals(background, AnimationBackground.createColorBackground(
                Color.GREEN));
        assertThrows(IllegalArgumentException.class,
                () -> AnimationBackground.createColorBackground(Color.TRANSPARENT));
    }
}
