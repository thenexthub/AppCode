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

package com.example.androidx.mediarouting.activities.systemrouting.source;

import android.content.Context;

import androidx.mediarouter.media.MediaControlIntent;
import androidx.mediarouter.media.MediaRouteSelector;
import androidx.mediarouter.media.MediaRouter;

import com.example.androidx.mediarouting.activities.systemrouting.SystemRouteItem;
import com.example.androidx.mediarouting.activities.systemrouting.SystemRoutesSourceItem;

import org.jspecify.annotations.NonNull;

import java.util.ArrayList;
import java.util.List;

/** Implements {@link SystemRoutesSource} using {@link MediaRouter}. */
public final class AndroidXMediaRouterSystemRoutesSource extends SystemRoutesSource {

    private final @NonNull MediaRouter mMediaRouter;

    private final MediaRouter.@NonNull Callback mMediaRouterCallback =
            new MediaRouter.Callback() {
                @Override
                public void onRouteAdded(
                        @NonNull MediaRouter router, MediaRouter.@NonNull RouteInfo route) {
                    mOnRoutesChangedListener.run();
                }

                @Override
                public void onRouteRemoved(
                        @NonNull MediaRouter router, MediaRouter.@NonNull RouteInfo route) {
                    mOnRoutesChangedListener.run();
                }

                @Override
                public void onRouteSelected(
                        @NonNull MediaRouter router,
                        MediaRouter.@NonNull RouteInfo selectedRoute,
                        int reason,
                        MediaRouter.@NonNull RouteInfo requestedRoute) {
                    mOnRoutesChangedListener.run();
                }

                @Override
                public void onRouteUnselected(
                        @NonNull MediaRouter router,
                        MediaRouter.@NonNull RouteInfo route,
                        int reason) {
                    mOnRoutesChangedListener.run();
                }

                @Override
                public void onRouteChanged(
                        @NonNull MediaRouter router, MediaRouter.@NonNull RouteInfo route) {
                    mOnRoutesChangedListener.run();
                }
            };

    /** Returns a new instance. */
    public static @NonNull AndroidXMediaRouterSystemRoutesSource create(@NonNull Context context) {
        MediaRouter mediaRouter = MediaRouter.getInstance(context);
        return new AndroidXMediaRouterSystemRoutesSource(mediaRouter);
    }

    AndroidXMediaRouterSystemRoutesSource(@NonNull MediaRouter mediaRouter) {
        mMediaRouter = mediaRouter;
    }

    @Override
    public void start() {
        MediaRouteSelector selector = new MediaRouteSelector.Builder()
                .addControlCategory(MediaControlIntent.CATEGORY_REMOTE_PLAYBACK)
                .addControlCategory(MediaControlIntent.CATEGORY_LIVE_AUDIO)
                .build();

        mMediaRouter.addCallback(selector, mMediaRouterCallback);
    }

    @Override
    public void stop() {
        mMediaRouter.removeCallback(mMediaRouterCallback);
    }

    @Override
    public @NonNull SystemRoutesSourceItem getSourceItem() {
        return new SystemRoutesSourceItem(/* name= */ "AndroidX MediaRouter");
    }

    @Override
    public @NonNull List<SystemRouteItem> fetchSourceRouteItems() {
        List<SystemRouteItem> out = new ArrayList<>();

        for (MediaRouter.RouteInfo routeInfo : mMediaRouter.getRoutes()) {
            if (!routeInfo.isDefaultOrBluetooth()) {
                continue;
            }

            out.add(createRouteItemFor(routeInfo));
        }

        return out;
    }

    @Override
    public boolean select(@NonNull SystemRouteItem item) {
        for (MediaRouter.RouteInfo routeInfo : mMediaRouter.getRoutes()) {
            if (routeInfo.getId().equals(item.mId)) {
                routeInfo.select();
                return true;
            }
        }
        return false;
    }

    private @NonNull SystemRouteItem createRouteItemFor(MediaRouter.@NonNull RouteInfo routeInfo) {
        SystemRouteItem.Builder builder =
                new SystemRouteItem.Builder(getSourceId(), routeInfo.getId())
                        .setName(routeInfo.getName());

        builder.setSelectionSupportState(
                routeInfo.isSelected()
                        ? SystemRouteItem.SelectionSupportState.RESELECTABLE
                        : SystemRouteItem.SelectionSupportState.SELECTABLE);
        String description = routeInfo.getDescription();
        if (description != null) {
            builder.setDescription(description);
        }

        return builder.build();
    }
}
