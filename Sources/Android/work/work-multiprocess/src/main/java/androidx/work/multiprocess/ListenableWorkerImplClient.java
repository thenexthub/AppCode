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

package androidx.work.multiprocess;

import static android.content.Context.BIND_AUTO_CREATE;

import android.annotation.SuppressLint;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.ServiceConnection;
import android.os.IBinder;

import androidx.annotation.RestrictTo;
import androidx.annotation.VisibleForTesting;
import androidx.work.Logger;
import androidx.work.impl.utils.futures.SettableFuture;

import com.google.common.util.concurrent.ListenableFuture;

import org.jspecify.annotations.NonNull;
import org.jspecify.annotations.Nullable;

import java.util.concurrent.Executor;

/***
 * A client for {@link IListenableWorkerImpl}.
 *
 */
@RestrictTo(RestrictTo.Scope.LIBRARY_GROUP)
public class ListenableWorkerImplClient {
    // Synthetic access
    static final String TAG = Logger.tagWithPrefix("ListenableWorkerImplClient");

    // Synthetic access
    final Context mContext;

    // Synthetic access
    final Executor mExecutor;

    private final Object mLock;
    private Connection mConnection;

    public ListenableWorkerImplClient(
            @NonNull Context context,
            @NonNull Executor executor) {
        mContext = context;
        mExecutor = executor;
        mLock = new Object();
    }

    /**
     * @return a {@link ListenableFuture} of {@link IListenableWorkerImpl} after a
     * {@link ServiceConnection} is established.
     */
    public @NonNull ListenableFuture<IListenableWorkerImpl> getListenableWorkerImpl(
            @NonNull ComponentName component) {

        synchronized (mLock) {
            if (mConnection == null) {
                Logger.get().debug(TAG, "Binding to " + component.getPackageName() + ", "
                        + component.getClassName());

                mConnection = new Connection();
                try {
                    Intent intent = new Intent();
                    intent.setComponent(component);
                    boolean bound = mContext.bindService(intent, mConnection, BIND_AUTO_CREATE);
                    if (!bound) {
                        unableToBind(mConnection,
                                new RuntimeException("Unable to bind to service"));
                    }
                } catch (Throwable throwable) {
                    unableToBind(mConnection, throwable);
                }
            }
            return mConnection.mFuture;
        }
    }

    /**
     * Executes a method on an instance of {@link IListenableWorkerImpl} using the instance of
     * {@link RemoteDispatcher}.
     */
    public @NonNull ListenableFuture<byte[]> execute(
            @NonNull ComponentName componentName,
            @NonNull RemoteDispatcher<IListenableWorkerImpl> dispatcher) {

        ListenableFuture<IListenableWorkerImpl> session = getListenableWorkerImpl(componentName);
        return execute(session, dispatcher);
    }

    /**
     * Executes a method on an instance of {@link IListenableWorkerImpl} using the instance of
     * {@link RemoteDispatcher}
     */
    @SuppressLint("LambdaLast")
    public @NonNull ListenableFuture<byte[]> execute(
            @NonNull ListenableFuture<IListenableWorkerImpl> session,
            final @NonNull RemoteDispatcher<IListenableWorkerImpl> dispatcher) {
        return RemoteExecuteKt.execute(mExecutor, session, dispatcher);
    }

    /**
     * Unbinds the {@link ServiceConnection}.
     */
    public void unbindService() {
        synchronized (mLock) {
            if (mConnection != null) {
                mContext.unbindService(mConnection);
                mConnection = null;
            }
        }
    }

    /**
     * @return the {@link ServiceConnection} instance.
     */
    @VisibleForTesting
    public @Nullable Connection getConnection() {
        return mConnection;
    }

    /**
     * The implementation of {@link ServiceConnection} that handles changes in the connection.
     *
     */
    @RestrictTo(RestrictTo.Scope.LIBRARY_GROUP)
    public static class Connection implements ServiceConnection {
        private static final String TAG = Logger.tagWithPrefix("ListenableWorkerImplSession");

        final SettableFuture<IListenableWorkerImpl> mFuture;

        public Connection() {
            mFuture = SettableFuture.create();
        }

        @Override
        public void onServiceConnected(
                @NonNull ComponentName componentName,
                @NonNull IBinder iBinder) {
            Logger.get().debug(TAG, "Service connected");
            IListenableWorkerImpl iListenableWorkerImpl =
                    IListenableWorkerImpl.Stub.asInterface(iBinder);
            mFuture.set(iListenableWorkerImpl);
        }

        @Override
        public void onServiceDisconnected(@NonNull ComponentName componentName) {
            Logger.get().warning(TAG, "Service disconnected");
            mFuture.setException(new RuntimeException("Service disconnected"));
        }

        @Override
        public void onBindingDied(@NonNull ComponentName name) {
            Logger.get().warning(TAG, "Binding died");
            mFuture.setException(new RuntimeException("Binding died"));
        }

        @Override
        public void onNullBinding(@NonNull ComponentName name) {
            Logger.get().error(TAG, "Unable to bind to service");
            mFuture.setException(
                    new RuntimeException("Cannot bind to service " + name));
        }
    }

    private static void unableToBind(@NonNull Connection session, @NonNull Throwable throwable) {
        Logger.get().error(TAG, "Unable to bind to service", throwable);
        session.mFuture.setException(throwable);
    }
}
