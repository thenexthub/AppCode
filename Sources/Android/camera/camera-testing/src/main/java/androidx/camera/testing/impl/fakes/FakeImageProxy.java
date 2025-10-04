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

package androidx.camera.testing.impl.fakes;

import android.graphics.Bitmap;
import android.graphics.Rect;
import android.media.Image;

import androidx.annotation.GuardedBy;
import androidx.camera.core.ExperimentalGetImage;
import androidx.camera.core.ImageInfo;
import androidx.camera.core.ImageProxy;
import androidx.concurrent.futures.CallbackToFutureAdapter;
import androidx.core.util.Preconditions;

import com.google.common.util.concurrent.ListenableFuture;

import org.jspecify.annotations.NonNull;
import org.jspecify.annotations.Nullable;

/**
 * A fake implementation of {@link ImageProxy} where the values are settable.
 */
public final class FakeImageProxy implements ImageProxy {
    private Rect mCropRect = new Rect();
    private int mFormat = 0;
    private int mHeight = 0;
    private int mWidth = 0;

    private PlaneProxy @NonNull [] mPlaneProxy = new PlaneProxy[0];

    private boolean mClosed = false;

    private @NonNull ImageInfo mImageInfo;
    private Image mImage;
    private @Nullable Bitmap mBitmap;
    @SuppressWarnings("WeakerAccess") /* synthetic accessor */
    final Object mReleaseLock = new Object();
    @SuppressWarnings("WeakerAccess") /* synthetic accessor */
    @GuardedBy("mReleaseLock")
    ListenableFuture<Void> mReleaseFuture;
    @SuppressWarnings("WeakerAccess") /* synthetic accessor */
    @GuardedBy("mReleaseLock")
    CallbackToFutureAdapter.Completer<Void> mReleaseCompleter;

    public FakeImageProxy(@NonNull ImageInfo imageInfo) {
        mImageInfo = imageInfo;
    }

    public FakeImageProxy(@NonNull ImageInfo imageInfo, @NonNull Bitmap bitmap) {
        mImageInfo = imageInfo;
        mBitmap = bitmap;
    }

    @Override
    public void close() {
        synchronized (mReleaseLock) {
            mClosed = true;
            if (mReleaseCompleter != null) {
                mReleaseCompleter.set(null);
                mReleaseCompleter = null;
            }
        }
    }

    @Override
    public @NonNull Rect getCropRect() {
        synchronized (mReleaseLock) {
            if (mClosed) {
                throw new IllegalStateException("FakeImageProxy already closed");
            }
            return mCropRect;
        }
    }

    @Override
    public void setCropRect(@Nullable Rect rect) {
        mCropRect = rect != null ? rect : new Rect();
    }

    @Override
    public int getFormat() {
        synchronized (mReleaseLock) {
            if (mClosed) {
                throw new IllegalStateException("FakeImageProxy already closed");
            }
            return mFormat;
        }
    }

    @Override
    public int getHeight() {
        synchronized (mReleaseLock) {
            if (mClosed) {
                throw new IllegalStateException("FakeImageProxy already closed");
            }
            return mHeight;
        }
    }

    @Override
    public int getWidth() {
        synchronized (mReleaseLock) {
            if (mClosed) {
                throw new IllegalStateException("FakeImageProxy already closed");
            }
            return mWidth;
        }
    }

    @Override
    public PlaneProxy @NonNull [] getPlanes() {
        synchronized (mReleaseLock) {
            if (mClosed) {
                throw new IllegalStateException("FakeImageProxy already closed");
            }
            return mPlaneProxy;
        }
    }

    @Override
    public @NonNull ImageInfo getImageInfo() {
        return mImageInfo;
    }

    @Override
    @ExperimentalGetImage
    public @Nullable Image getImage() {
        return mImage;
    }

    /**
     * Checks the image close status.
     * @return true if image closed, false otherwise.
     */
    public boolean isClosed() {
        synchronized (mReleaseLock) {
            return mClosed;
        }
    }

    public void setFormat(int format) {
        mFormat = format;
    }

    public void setHeight(int height) {
        mHeight = height;
    }

    public void setWidth(int width) {
        mWidth = width;
    }

    public void setPlanes(PlaneProxy @NonNull [] planeProxy) {
        mPlaneProxy = planeProxy;
    }

    public void setImageInfo(@NonNull ImageInfo imageInfo) {
        mImageInfo = imageInfo;
    }

    public void setImage(@Nullable Image image) {
        mImage = image;
    }

    /**
     * Returns ListenableFuture that completes when the {@link FakeImageProxy} has closed.
     */
    @SuppressWarnings("ObjectToString")
    public @NonNull ListenableFuture<Void> getCloseFuture() {
        synchronized (mReleaseLock) {
            if (mReleaseFuture == null) {
                mReleaseFuture = CallbackToFutureAdapter.getFuture(
                        completer -> {
                            synchronized (mReleaseLock) {
                                Preconditions.checkState(mReleaseCompleter == null,
                                        "Release completer expected to be null");
                                mReleaseCompleter = completer;
                                return "Release[imageProxy=" + FakeImageProxy.this + "]";
                            }
                        });
            }
            return mReleaseFuture;
        }
    }

    @Override
    public @NonNull Bitmap toBitmap() {
        if (mBitmap != null) {
            return mBitmap;
        }
        return ImageProxy.super.toBitmap();
    }
}
