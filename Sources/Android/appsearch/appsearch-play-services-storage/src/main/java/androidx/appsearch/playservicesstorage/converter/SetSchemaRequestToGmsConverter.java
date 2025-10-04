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

package androidx.appsearch.playservicesstorage.converter;

import androidx.annotation.RestrictTo;
import androidx.appsearch.app.AppSearchSchema;
import androidx.appsearch.app.GenericDocument;
import androidx.appsearch.app.Migrator;
import androidx.appsearch.app.PackageIdentifier;
import androidx.appsearch.app.SchemaVisibilityConfig;
import androidx.appsearch.app.SetSchemaRequest;
import androidx.appsearch.app.SetSchemaResponse;
import androidx.core.util.Preconditions;

import org.jspecify.annotations.NonNull;

import java.util.List;
import java.util.Map;
import java.util.Set;

/**
 * Translates between Gms and Jetpack versions of {@link SetSchemaRequest}.
 */
@RestrictTo(RestrictTo.Scope.LIBRARY_GROUP)
public final class SetSchemaRequestToGmsConverter {
    private SetSchemaRequestToGmsConverter() {
    }

    /**
     * Translates a jetpack {@link SetSchemaRequest} into a googleGms
     * {@link com.google.android.gms.appsearch.SetSchemaRequest}.
     */
    public static com.google.android.gms.appsearch.@NonNull SetSchemaRequest toGmsSetSchemaRequest(
            @NonNull SetSchemaRequest jetpackRequest) {
        Preconditions.checkNotNull(jetpackRequest);
        com.google.android.gms.appsearch.SetSchemaRequest.Builder gmsBuilder =
                new com.google.android.gms.appsearch.SetSchemaRequest.Builder();
        for (AppSearchSchema jetpackSchema : jetpackRequest.getSchemas()) {
            gmsBuilder.addSchemas(SchemaToGmsConverter.toGmsSchema(jetpackSchema));
        }
        for (String schemaNotDisplayedBySystem : jetpackRequest.getSchemasNotDisplayedBySystem()) {
            gmsBuilder.setSchemaTypeDisplayedBySystem(
                    schemaNotDisplayedBySystem, /*displayed=*/ false);
        }
        for (Map.Entry<String, Set<PackageIdentifier>> jetpackSchemaVisibleToPackage :
                jetpackRequest.getSchemasVisibleToPackagesInternal().entrySet()) {
            for (PackageIdentifier jetpackPackageIdentifier :
                    jetpackSchemaVisibleToPackage.getValue()) {
                gmsBuilder.setSchemaTypeVisibilityForPackage(
                        jetpackSchemaVisibleToPackage.getKey(),
                        /*visible=*/ true,
                        new com.google.android.gms.appsearch.PackageIdentifier(
                                jetpackPackageIdentifier.getPackageName(),
                                jetpackPackageIdentifier.getSha256Certificate()));
            }
        }
        if (!jetpackRequest.getRequiredPermissionsForSchemaTypeVisibility().isEmpty()) {
            for (Map.Entry<String, Set<Set<Integer>>> entry :
                    jetpackRequest.getRequiredPermissionsForSchemaTypeVisibility().entrySet()) {
                for (Set<Integer> permissionGroup : entry.getValue()) {
                    gmsBuilder.addRequiredPermissionsForSchemaTypeVisibility(
                            entry.getKey(), permissionGroup);
                }
            }
        }
        if (!jetpackRequest.getPubliclyVisibleSchemas().isEmpty()) {
            for (Map.Entry<String, PackageIdentifier> entry :
                    jetpackRequest.getPubliclyVisibleSchemas().entrySet()) {
                PackageIdentifier publiclyVisibleTargetPackage = entry.getValue();
                gmsBuilder.setPubliclyVisibleSchema(
                        entry.getKey(),
                        new com.google.android.gms.appsearch.PackageIdentifier(
                                publiclyVisibleTargetPackage.getPackageName(),
                                publiclyVisibleTargetPackage.getSha256Certificate()));
            }
        }

        if (!jetpackRequest.getSchemasVisibleToConfigs().isEmpty()) {
            for (Map.Entry<String, Set<SchemaVisibilityConfig>> entry :
                    jetpackRequest.getSchemasVisibleToConfigs().entrySet()) {
                for (SchemaVisibilityConfig jetpackConfig : entry.getValue()) {
                    com.google.android.gms.appsearch.SchemaVisibilityConfig gmsConfig =
                            toGmsSchemaVisibilityConfig(jetpackConfig);
                    gmsBuilder.addSchemaTypeVisibleToConfig(entry.getKey(), gmsConfig);
                }
            }
        }
        for (Map.Entry<String, Migrator> entry : jetpackRequest.getMigrators().entrySet()) {
            Migrator jetpackMigrator = entry.getValue();
            com.google.android.gms.appsearch.Migrator gmsMigrator =
                    new com.google.android.gms.appsearch.Migrator() {
                        @Override
                        public boolean shouldMigrate(int currentVersion, int finalVersion) {
                            return jetpackMigrator.shouldMigrate(currentVersion, finalVersion);
                        }

                        @Override
                        public com.google.android.gms.appsearch.@NonNull GenericDocument onUpgrade(
                                int currentVersion,
                                int finalVersion,
                                com.google.android.gms.appsearch.@NonNull GenericDocument
                                        inGmsDocument) {
                            GenericDocument inJetpackDocument =
                                    GenericDocumentToGmsConverter
                                            .toJetpackGenericDocument(
                                                    inGmsDocument);
                            GenericDocument outJetpackDocument = jetpackMigrator.onUpgrade(
                                    currentVersion, finalVersion, inJetpackDocument);
                            if (inJetpackDocument.equals(outJetpackDocument)) {
                                return inGmsDocument; // Same object; no conversion occurred.
                            }
                            return GenericDocumentToGmsConverter
                                    .toGmsGenericDocument(
                                            outJetpackDocument);
                        }

                        @Override
                        public com.google.android.gms.appsearch.@NonNull GenericDocument
                                onDowngrade(
                                        int currentVersion,
                                        int finalVersion,
                                        com.google.android.gms.appsearch.@NonNull GenericDocument
                                                inGmsDocument) {
                            GenericDocument inJetpackDocument =
                                    GenericDocumentToGmsConverter
                                            .toJetpackGenericDocument(
                                                    inGmsDocument);
                            GenericDocument outJetpackDocument = jetpackMigrator.onDowngrade(
                                    currentVersion, finalVersion, inJetpackDocument);
                            if (inJetpackDocument.equals(outJetpackDocument)) {
                                return inGmsDocument; // Same object; no conversion occurred.
                            }
                            return GenericDocumentToGmsConverter
                                    .toGmsGenericDocument(
                                            outJetpackDocument);
                        }
                    };
            gmsBuilder.setMigrator(entry.getKey(), gmsMigrator);
        }
        return gmsBuilder
                .setForceOverride(jetpackRequest.isForceOverride())
                .setVersion(jetpackRequest.getVersion())
                .build();
    }

    /**
     * Translates a gms
     * {@link com.google.android.gms.appsearch.SetSchemaResponse} into a jetpack
     * {@link SetSchemaResponse}.
     */
    public static @NonNull SetSchemaResponse toJetpackSetSchemaResponse(
            com.google.android.gms.appsearch.@NonNull SetSchemaResponse
                    gmsResponse) {
        Preconditions.checkNotNull(gmsResponse);
        SetSchemaResponse.Builder jetpackBuilder = new SetSchemaResponse.Builder()
                .addDeletedTypes(gmsResponse.getDeletedTypes())
                .addIncompatibleTypes(gmsResponse.getIncompatibleTypes())
                .addMigratedTypes(gmsResponse.getMigratedTypes());
        for (com.google.android.gms.appsearch.SetSchemaResponse.MigrationFailure migrationFailure :
                gmsResponse.getMigrationFailures()) {
            jetpackBuilder.addMigrationFailure(new SetSchemaResponse.MigrationFailure(
                    migrationFailure.getNamespace(),
                    migrationFailure.getDocumentId(),
                    migrationFailure.getSchemaType(),
                    AppSearchResultToGmsConverter.gmsAppSearchResultToJetpack(
                            migrationFailure.getAppSearchResult(), /* valueMapper= */ i -> i))
            );
        }
        return jetpackBuilder.build();
    }

    /**
     * Translates a jetpack {@link SchemaVisibilityConfig} into a gms
     * {@link com.google.android.gms.appsearch.SchemaVisibilityConfig}.
     */
    private static com.google.android.gms.appsearch.@NonNull SchemaVisibilityConfig
            toGmsSchemaVisibilityConfig(@NonNull SchemaVisibilityConfig jetpackConfig) {
        Preconditions.checkNotNull(jetpackConfig);
        com.google.android.gms.appsearch.SchemaVisibilityConfig.Builder gmsBuilder =
                new com.google.android.gms.appsearch.SchemaVisibilityConfig.Builder();

        // Translate allowedPackages
        List<PackageIdentifier> allowedPackages = jetpackConfig.getAllowedPackages();
        for (int i = 0; i < allowedPackages.size(); i++) {
            gmsBuilder.addAllowedPackage(new com.google.android.gms.appsearch.PackageIdentifier(
                    allowedPackages.get(i).getPackageName(),
                    allowedPackages.get(i).getSha256Certificate()));
        }

        // Translate requiredPermissions
        for (Set<Integer> requiredPermissions : jetpackConfig.getRequiredPermissions()) {
            gmsBuilder.addRequiredPermissions(requiredPermissions);
        }

        // Translate publiclyVisibleTargetPackage
        PackageIdentifier publiclyVisibleTargetPackage =
                jetpackConfig.getPubliclyVisibleTargetPackage();
        if (publiclyVisibleTargetPackage != null) {
            gmsBuilder.setPubliclyVisibleTargetPackage(
                    new com.google.android.gms.appsearch.PackageIdentifier(
                            publiclyVisibleTargetPackage.getPackageName(),
                            publiclyVisibleTargetPackage.getSha256Certificate()));
        }

        return gmsBuilder.build();
    }
}
