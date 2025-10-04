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
package androidx.emoji2.text;

import static androidx.annotation.RestrictTo.Scope.LIBRARY;

import android.content.res.AssetManager;
import android.graphics.Typeface;
import android.util.SparseArray;

import androidx.annotation.AnyThread;
import androidx.annotation.RestrictTo;
import androidx.annotation.VisibleForTesting;
import androidx.core.os.TraceCompat;
import androidx.core.util.Preconditions;
import androidx.emoji2.text.flatbuffer.MetadataList;

import org.jspecify.annotations.NonNull;

import java.io.IOException;
import java.io.InputStream;
import java.nio.ByteBuffer;

/**
 * Class to hold the emoji metadata required to process and draw emojis.
 */
@AnyThread
public final class MetadataRepo {
    /**
     * The default children size of the root node.
     */
    private static final int DEFAULT_ROOT_SIZE = 1024;
    private static final String S_TRACE_CREATE_REPO = "EmojiCompat.MetadataRepo.create";

    /**
     * MetadataList that contains the emoji metadata.
     */
    private final @NonNull MetadataList mMetadataList;

    /**
     * char presentation of all TypefaceEmojiRasterizer's in a single array. All emojis we have are
     * mapped to Private Use Area A, in the range U+F0000..U+FFFFD. Therefore each emoji takes 2
     * chars.
     */
    private final char @NonNull [] mEmojiCharArray;

    /**
     * Empty root node of the trie.
     */
    private final @NonNull Node mRootNode;

    /**
     * Typeface to be used to render emojis.
     */
    private final @NonNull Typeface mTypeface;

    /**
     * Private constructor that is called by one of {@code create} methods.
     *
     * @param typeface Typeface to be used to render emojis
     * @param metadataList MetadataList that contains the emoji metadata
     */
    private MetadataRepo(final @NonNull Typeface typeface,
            final @NonNull MetadataList metadataList) {
        mTypeface = typeface;
        mMetadataList = metadataList;
        mRootNode = new Node(DEFAULT_ROOT_SIZE);
        mEmojiCharArray = new char[mMetadataList.listLength() * 2];
        constructIndex(mMetadataList);
    }

    /**
     * Construct MetadataRepo with empty metadata.
     *
     * This should only be used from tests.
     */
    @RestrictTo(LIBRARY)
    @VisibleForTesting
    public static @NonNull MetadataRepo create(final @NonNull Typeface typeface) {
        try {
            TraceCompat.beginSection(S_TRACE_CREATE_REPO);
            return new MetadataRepo(typeface, new MetadataList());
        } finally {
            TraceCompat.endSection();
        }
    }

    /**
     * Construct MetadataRepo from an input stream. The library does not close the given
     * InputStream, therefore it is caller's responsibility to properly close the stream.
     *
     * @param typeface Typeface to be used to render emojis
     * @param inputStream InputStream to read emoji metadata from
     */
    public static @NonNull MetadataRepo create(final @NonNull Typeface typeface,
            final @NonNull InputStream inputStream) throws IOException {
        try {
            TraceCompat.beginSection(S_TRACE_CREATE_REPO);
            return new MetadataRepo(typeface, MetadataListReader.read(inputStream));
        } finally {
            TraceCompat.endSection();
        }
    }

    /**
     * Construct MetadataRepo from a byte buffer. The position of the ByteBuffer will change, it is
     * caller's responsibility to reposition the buffer if required.
     *
     * @param typeface Typeface to be used to render emojis
     * @param byteBuffer ByteBuffer to read emoji metadata from
     */
    public static @NonNull MetadataRepo create(final @NonNull Typeface typeface,
            final @NonNull ByteBuffer byteBuffer) throws IOException {
        try {
            TraceCompat.beginSection(S_TRACE_CREATE_REPO);
            return new MetadataRepo(typeface, MetadataListReader.read(byteBuffer));
        } finally {
            TraceCompat.endSection();
        }
    }

    /**
     * Construct MetadataRepo from an asset.
     *
     * @param assetManager AssetManager instance
     * @param assetPath asset manager path of the file that the Typeface and metadata will be
     *                  created from
     */
    public static @NonNull MetadataRepo create(final @NonNull AssetManager assetManager,
            final @NonNull String assetPath) throws IOException {
        try {
            TraceCompat.beginSection(S_TRACE_CREATE_REPO);
            final Typeface typeface = Typeface.createFromAsset(assetManager, assetPath);
            return new MetadataRepo(typeface,
                    MetadataListReader.read(assetManager, assetPath));
        } finally {
            TraceCompat.endSection();
        }
    }

    /**
     * Read emoji metadata list and construct the trie.
     */
    private void constructIndex(final MetadataList metadataList) {
        int length = metadataList.listLength();
        for (int i = 0; i < length; i++) {
            final TypefaceEmojiRasterizer metadata = new TypefaceEmojiRasterizer(this, i);
            //since all emojis are mapped to a single codepoint in Private Use Area A they are 2
            //chars wide
            //noinspection ResultOfMethodCallIgnored
            Character.toChars(metadata.getId(), mEmojiCharArray, i * 2);
            put(metadata);
        }
    }

    /**
     */
    @RestrictTo(RestrictTo.Scope.LIBRARY)
    @NonNull Typeface getTypeface() {
        return mTypeface;
    }

    /**
     */
    @RestrictTo(RestrictTo.Scope.LIBRARY)
    int getMetadataVersion() {
        return mMetadataList.version();
    }

    /**
     */
    @RestrictTo(RestrictTo.Scope.LIBRARY)
    @NonNull Node getRootNode() {
        return mRootNode;
    }

    /**
     */
    @RestrictTo(RestrictTo.Scope.LIBRARY)
    public char @NonNull [] getEmojiCharArray() {
        return mEmojiCharArray;
    }

    /**
     */
    @RestrictTo(RestrictTo.Scope.LIBRARY)
    public @NonNull MetadataList getMetadataList() {
        return mMetadataList;
    }

    /**
     * Add a TypefaceEmojiRasterizer to the index.
     *
     */
    @RestrictTo(RestrictTo.Scope.LIBRARY)
    @VisibleForTesting
    void put(final @NonNull TypefaceEmojiRasterizer data) {
        Preconditions.checkNotNull(data, "emoji metadata cannot be null");
        Preconditions.checkArgument(data.getCodepointsLength() > 0,
                "invalid metadata codepoint length");

        mRootNode.put(data, 0, data.getCodepointsLength() - 1);
    }

    /**
     * Trie node that holds mapping from emoji codepoint(s) to TypefaceEmojiRasterizer.
     *
     * A single codepoint emoji is represented by a child of the root node.
     *
     */
    @RestrictTo(RestrictTo.Scope.LIBRARY)
    static class Node {
        private final SparseArray<Node> mChildren;
        private TypefaceEmojiRasterizer mData;

        private Node() {
            this(1);
        }

        @SuppressWarnings("WeakerAccess") /* synthetic access */
        Node(final int defaultChildrenSize) {
            mChildren = new SparseArray<>(defaultChildrenSize);
        }

        Node get(final int key) {
            return mChildren == null ? null : mChildren.get(key);
        }

        final TypefaceEmojiRasterizer getData() {
            return mData;
        }

        @SuppressWarnings("WeakerAccess") /* synthetic access */
        void put(final @NonNull TypefaceEmojiRasterizer data, final int start, final int end) {
            Node node = get(data.getCodepointAt(start));
            if (node == null) {
                node = new Node();
                mChildren.put(data.getCodepointAt(start), node);
            }

            if (end > start) {
                node.put(data, start + 1, end);
            } else {
                node.mData = data;
            }
        }
    }
}
