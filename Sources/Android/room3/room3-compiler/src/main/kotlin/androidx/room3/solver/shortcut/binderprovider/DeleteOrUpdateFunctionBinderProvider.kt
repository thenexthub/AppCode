/*
 * Copyright 2018 The Android Open Source Project
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

package androidx.room3.solver.shortcut.binderprovider

import androidx.room3.compiler.processing.XType
import androidx.room3.solver.shortcut.binder.DeleteOrUpdateFunctionBinder
import androidx.room3.solver.shortcut.result.DeleteOrUpdateFunctionAdapter

/** Provider for delete or update binders */
interface DeleteOrUpdateFunctionBinderProvider {

    /** Check whether the [XType] can be handled by the [DeleteOrUpdateFunctionBinder] */
    fun matches(declared: XType): Boolean

    /** Provider of [DeleteOrUpdateFunctionAdapter], based on the [XType] */
    fun provide(declared: XType): DeleteOrUpdateFunctionBinder
}
