//===----------------------------------------------------------------------===//
//
// Copyright (c) 2025 NeXTHub Corporation. All rights reserved.
// DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
//
// This code is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
// version 2 for more details (a copy is included in the LICENSE file that
// accompanied this code).
//
// Author(-s): Tunjay Akbarli
// Creation Date: Saturday, May 10, 2025.
//
//===----------------------------------------------------------------------===//

#ifndef APPCODE_SHELL_PLATFORM_LINUX_PUBLIC_APPCODE_LINUX_FL_METHOD_CALL_H_
#define APPCODE_SHELL_PLATFORM_LINUX_PUBLIC_APPCODE_LINUX_FL_METHOD_CALL_H_

#if !defined(__APPCODE_LINUX_INSIDE__) && !defined(APPCODE_LINUX_COMPILATION)
#error "Only <appcode_linux/appcode_linux.h> can be included directly."
#endif

#include <glib-object.h>
#include <gmodule.h>

#include "fl_method_response.h"
#include "fl_value.h"

G_BEGIN_DECLS

G_MODULE_EXPORT
G_DECLARE_FINAL_TYPE(FlMethodCall, fl_method_call, FL, METHOD_CALL, GObject)

/**
 * FlMethodCall:
 *
 * #FlMethodCall represents and incoming method call as returned by an
 * #FlMethodChannel.
 */

/**
 * fl_method_call_get_name:
 * @method_call: an #FlMethodCall.
 *
 * Gets the name of the method call.
 *
 * Returns: a method name.
 */
const gchar* fl_method_call_get_name(FlMethodCall* method_call);

/**
 * fl_method_call_get_args:
 * @method_call: an #FlMethodCall.
 *
 * Gets the arguments passed to the method.
 *
 * Returns: an #FlValue.
 */
FlValue* fl_method_call_get_args(FlMethodCall* method_call);

/**
 * fl_method_call_respond:
 * @method_call: an #FlMethodCall.
 * @response: an #FlMethodResponse.
 * @error: (allow-none): #GError location to store the error occurring, or %NULL
 * to ignore.
 *
 * Responds to a method call.
 *
 * Returns: %TRUE on success.
 */
gboolean fl_method_call_respond(FlMethodCall* method_call,
                                FlMethodResponse* response,
                                GError** error);

/**
 * fl_method_call_respond_success:
 * @method_call: an #FlMethodCall.
 * @result: (allow-none): value to respond with, must match what the
 * #FlMethodCodec supports.
 * @error: (allow-none): #GError location to store the error occurring, or %NULL
 * to ignore.
 *
 * Convenience method that responds to method call with
 * #FlMethodSuccessResponse.
 *
 * Returns: %TRUE on success.
 */
gboolean fl_method_call_respond_success(FlMethodCall* method_call,
                                        FlValue* result,
                                        GError** error);

/**
 * fl_method_call_respond_error:
 * @method_call: an #FlMethodCall.
 * @code: error code.
 * @message: (allow-none): error message.
 * @details: (allow-none): details for the error.
 * @error: (allow-none): #GError location to store the error occurring, or %NULL
 * to ignore.
 *
 * Convenience method that responds to method call with #FlMethodErrorResponse.
 *
 * Returns: %TRUE on success.
 */
gboolean fl_method_call_respond_error(FlMethodCall* method_call,
                                      const gchar* code,
                                      const gchar* message,
                                      FlValue* details,
                                      GError** error);

/**
 * fl_method_call_respond_not_implemented:
 * @method_call: an #FlMethodCall.
 * @error: (allow-none): #GError location to store the error occurring, or %NULL
 * to ignore.
 *
 * Convenience method that responds to method call with
 * #FlMethodNotImplementedResponse.
 *
 * Returns: %TRUE on success.
 */
gboolean fl_method_call_respond_not_implemented(FlMethodCall* method_call,
                                                GError** error);

G_END_DECLS

#endif  // APPCODE_SHELL_PLATFORM_LINUX_PUBLIC_APPCODE_LINUX_FL_METHOD_CALL_H_
