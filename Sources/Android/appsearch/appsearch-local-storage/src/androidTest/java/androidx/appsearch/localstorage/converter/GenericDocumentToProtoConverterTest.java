/*
 * Copyright 2020 The Android Open Source Project
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

package androidx.appsearch.localstorage.converter;

import static androidx.appsearch.testutil.AppSearchTestUtils.calculateDigest;

import static com.google.common.truth.Truth.assertThat;

import static org.junit.Assume.assumeFalse;
import static org.junit.Assume.assumeTrue;

import androidx.appsearch.app.AppSearchBlobHandle;
import androidx.appsearch.app.EmbeddingVector;
import androidx.appsearch.app.GenericDocument;
import androidx.appsearch.flags.Flags;
import androidx.appsearch.localstorage.AppSearchConfigImpl;
import androidx.appsearch.localstorage.LocalStorageIcingOptionsConfig;
import androidx.appsearch.localstorage.SchemaCache;
import androidx.appsearch.localstorage.UnlimitedLimitConfig;
import androidx.appsearch.localstorage.util.PrefixUtil;

import com.google.android.icing.proto.DocumentProto;
import com.google.android.icing.proto.PropertyConfigProto;
import com.google.android.icing.proto.PropertyProto;
import com.google.android.icing.proto.SchemaTypeConfigProto;
import com.google.android.icing.protobuf.ByteString;
import com.google.common.collect.ImmutableMap;

import org.junit.Test;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collections;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.TreeMap;

public class GenericDocumentToProtoConverterTest {
    private static final byte[] BYTE_ARRAY_1 = new byte[]{(byte) 1, (byte) 2, (byte) 3};
    private static final byte[] BYTE_ARRAY_2 = new byte[]{(byte) 4, (byte) 5, (byte) 6, (byte) 7};
    private static final String SCHEMA_TYPE_1 = "sDocumentPropertiesSchemaType1";
    private static final String SCHEMA_TYPE_2 = "sDocumentPropertiesSchemaType2";
    private static final EmbeddingVector sEmbedding1 = new EmbeddingVector(
            new float[]{1.1f, 2.2f, 3.3f}, "my_model_v1");
    private static final EmbeddingVector sEmbedding2 = new EmbeddingVector(
            new float[]{4.4f, 5.5f, 6.6f, 7.7f}, "my_model_v2");
    private static final GenericDocument DOCUMENT_PROPERTIES_1 =
            new GenericDocument.Builder<GenericDocument.Builder<?>>(
                    "namespace", "sDocumentProperties1", SCHEMA_TYPE_1)
                    .setCreationTimestampMillis(12345L)
                    .build();
    private static final GenericDocument DOCUMENT_PROPERTIES_2 =
            new GenericDocument.Builder<GenericDocument.Builder<?>>(
                    "namespace", "sDocumentProperties2", SCHEMA_TYPE_2)
                    .setCreationTimestampMillis(6789L)
                    .build();
    private static final String PREFIX = "package$databaseName/";
    private static final SchemaTypeConfigProto SCHEMA_PROTO_1 = SchemaTypeConfigProto.newBuilder()
            .setSchemaType(PREFIX + SCHEMA_TYPE_1)
            .build();
    private static final SchemaTypeConfigProto SCHEMA_PROTO_2 = SchemaTypeConfigProto.newBuilder()
            .setSchemaType(PREFIX + SCHEMA_TYPE_2)
            .build();
    private static final Map<String, Map<String, SchemaTypeConfigProto>> SCHEMA_MAP =
            ImmutableMap.of(PREFIX, ImmutableMap.of(
                    PREFIX + SCHEMA_TYPE_1, SCHEMA_PROTO_1,
                    PREFIX + SCHEMA_TYPE_2, SCHEMA_PROTO_2));

    @Test
    public void testDocumentProtoConvert() throws Exception {
        GenericDocument document =
                new GenericDocument.Builder<GenericDocument.Builder<?>>("namespace", "id1",
                        SCHEMA_TYPE_1)
                        .setCreationTimestampMillis(5L)
                        .setScore(1)
                        .setTtlMillis(1L)
                        .setPropertyLong("longKey1", 1L)
                        .setPropertyDouble("doubleKey1", 1.0)
                        .setPropertyBoolean("booleanKey1", true)
                        .setPropertyString("stringKey1", "test-value1")
                        .setPropertyBytes("byteKey1", BYTE_ARRAY_1, BYTE_ARRAY_2)
                        .setPropertyDocument("documentKey1", DOCUMENT_PROPERTIES_1)
                        .setPropertyDocument("documentKey2", DOCUMENT_PROPERTIES_2)
                        .build();

        // Create the Document proto. Need to sort the property order by key.
        DocumentProto.Builder documentProtoBuilder = DocumentProto.newBuilder()
                .setUri("id1")
                .setSchema(SCHEMA_TYPE_1)
                .setCreationTimestampMs(5L)
                .setScore(1)
                .setTtlMs(1L)
                .setNamespace("namespace");
        HashMap<String, PropertyProto.Builder> propertyProtoMap = new HashMap<>();
        propertyProtoMap.put("longKey1",
                PropertyProto.newBuilder().setName("longKey1").addInt64Values(1L));
        propertyProtoMap.put("doubleKey1",
                PropertyProto.newBuilder().setName("doubleKey1").addDoubleValues(1.0));
        propertyProtoMap.put("booleanKey1",
                PropertyProto.newBuilder().setName("booleanKey1").addBooleanValues(true));
        propertyProtoMap.put("stringKey1",
                PropertyProto.newBuilder().setName("stringKey1").addStringValues("test-value1"));
        propertyProtoMap.put("byteKey1",
                PropertyProto.newBuilder().setName("byteKey1")
                        .addBytesValues(ByteString.copyFrom(BYTE_ARRAY_1))
                        .addBytesValues(ByteString.copyFrom(BYTE_ARRAY_2)));
        propertyProtoMap.put("documentKey1",
                PropertyProto.newBuilder().setName("documentKey1").addDocumentValues(
                        GenericDocumentToProtoConverter.toDocumentProto(DOCUMENT_PROPERTIES_1)));
        propertyProtoMap.put("documentKey2",
                PropertyProto.newBuilder().setName("documentKey2").addDocumentValues(
                        GenericDocumentToProtoConverter.toDocumentProto(DOCUMENT_PROPERTIES_2)));
        List<String> sortedKey = new ArrayList<>(propertyProtoMap.keySet());
        Collections.sort(sortedKey);
        for (String key : sortedKey) {
            documentProtoBuilder.addProperties(propertyProtoMap.get(key));
        }
        DocumentProto documentProto = documentProtoBuilder.build();

        GenericDocument convertedGenericDocument =
                GenericDocumentToProtoConverter.toGenericDocument(documentProto, PREFIX,
                        new SchemaCache(SCHEMA_MAP),
                        new AppSearchConfigImpl(new UnlimitedLimitConfig(),
                                new LocalStorageIcingOptionsConfig()));
        DocumentProto convertedDocumentProto =
                GenericDocumentToProtoConverter.toDocumentProto(document);

        assertThat(convertedDocumentProto).isEqualTo(documentProto);
        assertThat(convertedGenericDocument).isEqualTo(document);
    }

    @Test
    public void testConvertDocument_whenPropertyHasEmptyList() throws Exception {
        // Build original GenericDocument
        GenericDocument document =
                new GenericDocument.Builder<GenericDocument.Builder<?>>("namespace", "id1",
                        SCHEMA_TYPE_1)
                        .setCreationTimestampMillis(5L)
                        .setScore(1)
                        .setTtlMillis(1L)
                        .setPropertyLong("emptyLongKey")
                        .setPropertyDouble("emptyDoubleKey")
                        .setPropertyBoolean("emptyBooleanKey")
                        .setPropertyString("emptyStringKey")
                        .setPropertyBytes("emptyByteKey")
                        .setPropertyDocument("emptyDocumentKey")
                        .build();

        // Build original DocumentProto
        DocumentProto.Builder documentProtoBuilder = DocumentProto.newBuilder()
                .setUri("id1")
                .setSchema(SCHEMA_TYPE_1)
                .setCreationTimestampMs(5L)
                .setScore(1)
                .setTtlMs(1L)
                .setNamespace("namespace");
        TreeMap<String, PropertyProto.Builder> propertyProtoMap = new TreeMap<>();
        propertyProtoMap.put("emptyLongKey",
                PropertyProto.newBuilder().setName("emptyLongKey"));
        propertyProtoMap.put("emptyDoubleKey",
                PropertyProto.newBuilder().setName("emptyDoubleKey"));
        propertyProtoMap.put("emptyBooleanKey",
                PropertyProto.newBuilder().setName("emptyBooleanKey"));
        propertyProtoMap.put("emptyStringKey",
                PropertyProto.newBuilder().setName("emptyStringKey"));
        propertyProtoMap.put("emptyByteKey",
                PropertyProto.newBuilder().setName("emptyByteKey"));
        propertyProtoMap.put("emptyDocumentKey",
                PropertyProto.newBuilder().setName("emptyDocumentKey"));
        for (Map.Entry<String, PropertyProto.Builder> entry : propertyProtoMap.entrySet()) {
            documentProtoBuilder.addProperties(entry.getValue());
        }
        DocumentProto documentProto = documentProtoBuilder.build();

        // Build schema proto used for find empty property type
        PropertyConfigProto emptyLongListProperty = PropertyConfigProto.newBuilder()
                .setCardinality(PropertyConfigProto.Cardinality.Code.REPEATED)
                .setDataType(PropertyConfigProto.DataType.Code.INT64)
                .setPropertyName("emptyLongKey")
                .build();
        PropertyConfigProto emptyDoubleListProperty = PropertyConfigProto.newBuilder()
                .setCardinality(PropertyConfigProto.Cardinality.Code.REPEATED)
                .setDataType(PropertyConfigProto.DataType.Code.DOUBLE)
                .setPropertyName("emptyDoubleKey")
                .build();
        PropertyConfigProto emptyBooleanListProperty = PropertyConfigProto.newBuilder()
                .setCardinality(PropertyConfigProto.Cardinality.Code.REPEATED)
                .setDataType(PropertyConfigProto.DataType.Code.BOOLEAN)
                .setPropertyName("emptyBooleanKey")
                .build();
        PropertyConfigProto emptyStringListProperty = PropertyConfigProto.newBuilder()
                .setCardinality(PropertyConfigProto.Cardinality.Code.REPEATED)
                .setDataType(PropertyConfigProto.DataType.Code.STRING)
                .setPropertyName("emptyStringKey")
                .build();
        PropertyConfigProto emptyByteListProperty = PropertyConfigProto.newBuilder()
                .setCardinality(PropertyConfigProto.Cardinality.Code.REPEATED)
                .setDataType(PropertyConfigProto.DataType.Code.BYTES)
                .setPropertyName("emptyByteKey")
                .build();
        PropertyConfigProto emptyDocumentListProperty = PropertyConfigProto.newBuilder()
                .setCardinality(PropertyConfigProto.Cardinality.Code.REPEATED)
                .setDataType(PropertyConfigProto.DataType.Code.DOCUMENT)
                .setPropertyName("emptyDocumentKey")
                .build();
        SchemaTypeConfigProto schemaTypeConfigProto = SchemaTypeConfigProto.newBuilder()
                .addProperties(emptyLongListProperty)
                .addProperties(emptyDoubleListProperty)
                .addProperties(emptyBooleanListProperty)
                .addProperties(emptyStringListProperty)
                .addProperties(emptyByteListProperty)
                .addProperties(emptyDocumentListProperty)
                .setSchemaType(PREFIX + SCHEMA_TYPE_1)
                .build();
        Map<String, Map<String, SchemaTypeConfigProto>> schemaMap =
                ImmutableMap.of(PREFIX,
                        ImmutableMap.of(PREFIX + SCHEMA_TYPE_1, schemaTypeConfigProto));

        // Convert to the other type and check if they are matched.
        GenericDocument convertedGenericDocument =
                GenericDocumentToProtoConverter.toGenericDocument(documentProto, PREFIX,
                        new SchemaCache(schemaMap),
                        new AppSearchConfigImpl(new UnlimitedLimitConfig(),
                                new LocalStorageIcingOptionsConfig()));
        DocumentProto convertedDocumentProto =
                GenericDocumentToProtoConverter.toDocumentProto(document);
        assertThat(convertedDocumentProto).isEqualTo(documentProto);
        assertThat(convertedGenericDocument).isEqualTo(document);
    }

    @Test
    public void testConvertDocument_whenNestedDocumentPropertyHasEmptyList() throws Exception {
        // Build original nested document in type 1 and outer document in type2
        GenericDocument nestedDocument =
                new GenericDocument.Builder<GenericDocument.Builder<?>>("namespace", "id1",
                        SCHEMA_TYPE_1)
                        .setCreationTimestampMillis(5L)
                        .setScore(1)
                        .setTtlMillis(1L)
                        .setPropertyLong("emptyLongKey")
                        .setPropertyDouble("emptyDoubleKey")
                        .setPropertyBoolean("emptyBooleanKey")
                        .setPropertyString("emptyStringKey")
                        .setPropertyBytes("emptyByteKey")
                        .setPropertyDocument("emptyDocumentKey")
                        .build();
        GenericDocument outerDocument =
                new GenericDocument.Builder<GenericDocument.Builder<?>>("namespace", "id2",
                        SCHEMA_TYPE_2)
                        .setCreationTimestampMillis(5L)
                        .setScore(1)
                        .setTtlMillis(1L)
                        .setPropertyDocument("documentKey", nestedDocument)
                .build();

        // Build original nested Document proto in type 1 and outer DocumentProto in type 2.
        DocumentProto.Builder documentProtoBuilder = DocumentProto.newBuilder()
                .setUri("id1")
                .setSchema(SCHEMA_TYPE_1)
                .setCreationTimestampMs(5L)
                .setScore(1)
                .setTtlMs(1L)
                .setNamespace("namespace");
        TreeMap<String, PropertyProto.Builder> propertyProtoMap = new TreeMap<>();
        propertyProtoMap.put("emptyLongKey",
                PropertyProto.newBuilder().setName("emptyLongKey"));
        propertyProtoMap.put("emptyDoubleKey",
                PropertyProto.newBuilder().setName("emptyDoubleKey"));
        propertyProtoMap.put("emptyBooleanKey",
                PropertyProto.newBuilder().setName("emptyBooleanKey"));
        propertyProtoMap.put("emptyStringKey",
                PropertyProto.newBuilder().setName("emptyStringKey"));
        propertyProtoMap.put("emptyByteKey",
                PropertyProto.newBuilder().setName("emptyByteKey"));
        propertyProtoMap.put("emptyDocumentKey",
                PropertyProto.newBuilder().setName("emptyDocumentKey"));
        for (Map.Entry<String, PropertyProto.Builder> entry : propertyProtoMap.entrySet()) {
            documentProtoBuilder.addProperties(entry.getValue());
        }
        DocumentProto nestedDocumentProto = documentProtoBuilder.build();
        DocumentProto outerDocumentProto = DocumentProto.newBuilder()
                .setUri("id2")
                .setSchema(SCHEMA_TYPE_2)
                .setScore(1)
                .setTtlMs(1L)
                .setCreationTimestampMs(5L)
                .setNamespace("namespace")
                .addProperties(
                        PropertyProto.newBuilder()
                                .addDocumentValues(nestedDocumentProto)
                                .setName("documentKey")
                                .build()
                ).build();

        // Build nested schema proto in type 1 and outer schema proto in type 2.
        PropertyConfigProto emptyLongListProperty = PropertyConfigProto.newBuilder()
                .setCardinality(PropertyConfigProto.Cardinality.Code.REPEATED)
                .setDataType(PropertyConfigProto.DataType.Code.INT64)
                .setPropertyName("emptyLongKey")
                .build();
        PropertyConfigProto emptyDoubleListProperty = PropertyConfigProto.newBuilder()
                .setCardinality(PropertyConfigProto.Cardinality.Code.REPEATED)
                .setDataType(PropertyConfigProto.DataType.Code.DOUBLE)
                .setPropertyName("emptyDoubleKey")
                .build();
        PropertyConfigProto emptyBooleanListProperty = PropertyConfigProto.newBuilder()
                .setCardinality(PropertyConfigProto.Cardinality.Code.REPEATED)
                .setDataType(PropertyConfigProto.DataType.Code.BOOLEAN)
                .setPropertyName("emptyBooleanKey")
                .build();
        PropertyConfigProto emptyStringListProperty = PropertyConfigProto.newBuilder()
                .setCardinality(PropertyConfigProto.Cardinality.Code.REPEATED)
                .setDataType(PropertyConfigProto.DataType.Code.STRING)
                .setPropertyName("emptyStringKey")
                .build();
        PropertyConfigProto emptyByteListProperty = PropertyConfigProto.newBuilder()
                .setCardinality(PropertyConfigProto.Cardinality.Code.REPEATED)
                .setDataType(PropertyConfigProto.DataType.Code.BYTES)
                .setPropertyName("emptyByteKey")
                .build();
        PropertyConfigProto emptyDocumentListProperty = PropertyConfigProto.newBuilder()
                .setCardinality(PropertyConfigProto.Cardinality.Code.REPEATED)
                .setDataType(PropertyConfigProto.DataType.Code.DOCUMENT)
                .setPropertyName("emptyDocumentKey")
                .build();
        SchemaTypeConfigProto nestedSchemaTypeConfigProto = SchemaTypeConfigProto.newBuilder()
                .addProperties(emptyLongListProperty)
                .addProperties(emptyDoubleListProperty)
                .addProperties(emptyBooleanListProperty)
                .addProperties(emptyStringListProperty)
                .addProperties(emptyByteListProperty)
                .addProperties(emptyDocumentListProperty)
                .setSchemaType(PREFIX + SCHEMA_TYPE_1)
                .build();
        PropertyConfigProto nestedDocumentProperty = PropertyConfigProto.newBuilder()
                .setCardinality(PropertyConfigProto.Cardinality.Code.REPEATED)
                .setDataType(PropertyConfigProto.DataType.Code.DOCUMENT)
                .setPropertyName("documentKey")
                .setSchemaType(PREFIX + SCHEMA_TYPE_1)
                .build();
        SchemaTypeConfigProto outerSchemaTypeConfigProto = SchemaTypeConfigProto.newBuilder()
                .addProperties(nestedDocumentProperty)
                .setSchemaType(PREFIX + SCHEMA_TYPE_2)
                .build();
        Map<String, Map<String, SchemaTypeConfigProto>> schemaMap =
                ImmutableMap.of(PREFIX,
                        ImmutableMap.of(PREFIX + SCHEMA_TYPE_1, nestedSchemaTypeConfigProto,
                                PREFIX + SCHEMA_TYPE_2, outerSchemaTypeConfigProto));

        // Convert to the other type and check if they are matched.
        GenericDocument convertedGenericDocument =
                GenericDocumentToProtoConverter.toGenericDocument(outerDocumentProto, PREFIX,
                        new SchemaCache(schemaMap),
                        new AppSearchConfigImpl(new UnlimitedLimitConfig(),
                                new LocalStorageIcingOptionsConfig()));
        DocumentProto convertedDocumentProto =
                GenericDocumentToProtoConverter.toDocumentProto(outerDocument);
        assertThat(convertedDocumentProto).isEqualTo(outerDocumentProto);
        assertThat(convertedGenericDocument).isEqualTo(outerDocument);
    }
    // @exportToFramework:startStrip()
    // TODO(b/274157614): setParentTypes is hidden
    @Test
    @SuppressWarnings("deprecation")
    public void testConvertDocument_withParentTypes() throws Exception {
        assumeFalse(Flags.enableSearchResultParentTypes());
        // Create a type with a parent type.
        SchemaTypeConfigProto schemaProto1 = SchemaTypeConfigProto.newBuilder()
                .setSchemaType(PREFIX + SCHEMA_TYPE_1)
                .addParentTypes(PREFIX + SCHEMA_TYPE_2)
                .build();
        Map<String, Map<String, SchemaTypeConfigProto>> schemaMap =
                ImmutableMap.of(PREFIX, ImmutableMap.of(
                        PREFIX + SCHEMA_TYPE_1, schemaProto1,
                        PREFIX + SCHEMA_TYPE_2, SCHEMA_PROTO_2));

        // Create a document proto for the above type.
        DocumentProto.Builder documentProtoBuilder = DocumentProto.newBuilder()
                .setUri("id1")
                .setSchema(SCHEMA_TYPE_1)
                .setCreationTimestampMs(5L)
                .setScore(1)
                .setTtlMs(1L)
                .setNamespace("namespace");
        HashMap<String, PropertyProto.Builder> propertyProtoMap = new HashMap<>();
        propertyProtoMap.put("longKey1",
                PropertyProto.newBuilder().setName("longKey1").addInt64Values(1L));
        propertyProtoMap.put("doubleKey1",
                PropertyProto.newBuilder().setName("doubleKey1").addDoubleValues(1.0));
        for (Map.Entry<String, PropertyProto.Builder> entry : propertyProtoMap.entrySet()) {
            documentProtoBuilder.addProperties(entry.getValue());
        }
        DocumentProto documentProto = documentProtoBuilder.build();

        // Check if the parent types list is properly wrapped, either as a property or a meta field.
        GenericDocument expectedDocWithParentAsMetaField =
                new GenericDocument.Builder<GenericDocument.Builder<?>>("namespace", "id1",
                        SCHEMA_TYPE_1)
                        .setParentTypes(Collections.singletonList(SCHEMA_TYPE_2))
                        .setCreationTimestampMillis(5L)
                        .setScore(1)
                        .setTtlMillis(1L)
                        .setPropertyLong("longKey1", 1L)
                        .setPropertyDouble("doubleKey1", 1.0)
                        .build();
        GenericDocument expectedDocWithParentAsSyntheticProperty =
                new GenericDocument.Builder<GenericDocument.Builder<?>>("namespace", "id1",
                        SCHEMA_TYPE_1)
                        .setPropertyString(
                                GenericDocument.PARENT_TYPES_SYNTHETIC_PROPERTY, SCHEMA_TYPE_2)
                        .setCreationTimestampMillis(5L)
                        .setScore(1)
                        .setTtlMillis(1L)
                        .setPropertyLong("longKey1", 1L)
                        .setPropertyDouble("doubleKey1", 1.0)
                        .build();

        GenericDocument actualDocWithParentAsMetaField =
                GenericDocumentToProtoConverter.toGenericDocument(documentProto, PREFIX,
                        new SchemaCache(schemaMap),
                        new AppSearchConfigImpl(new UnlimitedLimitConfig(),
                                new LocalStorageIcingOptionsConfig(),
                                /* storeParentInfoAsSyntheticProperty= */ false,
                                /* shouldRetrieveParentInfo= */ true,
                                /* persistToDiskRecoveryProof=*/false));
        GenericDocument actualDocWithParentAsSyntheticProperty =
                GenericDocumentToProtoConverter.toGenericDocument(documentProto, PREFIX,
                        new SchemaCache(schemaMap),
                        new AppSearchConfigImpl(new UnlimitedLimitConfig(),
                                new LocalStorageIcingOptionsConfig(),
                                /* storeParentInfoAsSyntheticProperty= */ true,
                                /* shouldRetrieveParentInfo= */ true,
                                /* persistToDiskRecoveryProof=*/false));

        assertThat(actualDocWithParentAsMetaField).isEqualTo(expectedDocWithParentAsMetaField);
        assertThat(actualDocWithParentAsMetaField).isNotEqualTo(
                expectedDocWithParentAsSyntheticProperty);

        assertThat(actualDocWithParentAsSyntheticProperty).isEqualTo(
                expectedDocWithParentAsSyntheticProperty);
        assertThat(actualDocWithParentAsSyntheticProperty).isNotEqualTo(
                expectedDocWithParentAsMetaField);
    }
    // @exportToFramework:endStrip()

    @Test
    public void testConvertDocument_withoutParentTypes() throws Exception {
        assumeTrue(Flags.enableSearchResultParentTypes());
        // Create a type with a parent type.
        SchemaTypeConfigProto schemaProto1 = SchemaTypeConfigProto.newBuilder()
                .setSchemaType(PREFIX + SCHEMA_TYPE_1)
                .addParentTypes(PREFIX + SCHEMA_TYPE_2)
                .build();
        Map<String, Map<String, SchemaTypeConfigProto>> schemaMap =
                ImmutableMap.of(PREFIX, ImmutableMap.of(
                        PREFIX + SCHEMA_TYPE_1, schemaProto1,
                        PREFIX + SCHEMA_TYPE_2, SCHEMA_PROTO_2));

        // Create a document proto for the above type.
        DocumentProto.Builder documentProtoBuilder = DocumentProto.newBuilder()
                .setUri("id1")
                .setSchema(SCHEMA_TYPE_1)
                .setCreationTimestampMs(5L)
                .setScore(1)
                .setTtlMs(1L)
                .setNamespace("namespace");
        HashMap<String, PropertyProto.Builder> propertyProtoMap = new HashMap<>();
        propertyProtoMap.put("longKey1",
                PropertyProto.newBuilder().setName("longKey1").addInt64Values(1L));
        propertyProtoMap.put("doubleKey1",
                PropertyProto.newBuilder().setName("doubleKey1").addDoubleValues(1.0));
        for (Map.Entry<String, PropertyProto.Builder> entry : propertyProtoMap.entrySet()) {
            documentProtoBuilder.addProperties(entry.getValue());
        }
        DocumentProto documentProto = documentProtoBuilder.build();

        // Check that the parent types list is not wrapped anywhere in GenericDocument, neither
        // as a synthetic property nor as a meta field, since Flags.enableSearchResultParentTypes()
        // is true.
        GenericDocument expectedDoc =
                new GenericDocument.Builder<GenericDocument.Builder<?>>("namespace", "id1",
                        SCHEMA_TYPE_1)
                        .setCreationTimestampMillis(5L)
                        .setScore(1)
                        .setTtlMillis(1L)
                        .setPropertyLong("longKey1", 1L)
                        .setPropertyDouble("doubleKey1", 1.0)
                        .build();
        GenericDocument actualDoc1 =
                GenericDocumentToProtoConverter.toGenericDocument(documentProto, PREFIX,
                        new SchemaCache(schemaMap),
                        new AppSearchConfigImpl(new UnlimitedLimitConfig(),
                                new LocalStorageIcingOptionsConfig(),
                                /* storeParentInfoAsSyntheticProperty= */ false,
                                /* shouldRetrieveParentInfo= */ true,
                                /* persistToDiskRecoveryProof=*/false));
        GenericDocument actualDoc2 =
                GenericDocumentToProtoConverter.toGenericDocument(documentProto, PREFIX,
                        new SchemaCache(schemaMap),
                        new AppSearchConfigImpl(new UnlimitedLimitConfig(),
                                new LocalStorageIcingOptionsConfig(),
                                /* storeParentInfoAsSyntheticProperty= */ true,
                                /* shouldRetrieveParentInfo= */ true,
                                /* persistToDiskRecoveryProof=*/false));
        assertThat(actualDoc1).isEqualTo(expectedDoc);
        assertThat(actualDoc2).isEqualTo(expectedDoc);
    }

    @Test
    public void testDocumentProtoConvert_EmbeddingProperty() throws Exception {
        GenericDocument document =
                new GenericDocument.Builder<GenericDocument.Builder<?>>("namespace", "id1",
                        SCHEMA_TYPE_1)
                        .setCreationTimestampMillis(5L)
                        .setScore(1)
                        .setTtlMillis(1L)
                        .setPropertyLong("longKey1", 1L)
                        .setPropertyDocument("documentKey1", DOCUMENT_PROPERTIES_1)
                        .setPropertyEmbedding("embeddingKey1", sEmbedding1, sEmbedding2)
                        .build();

        // Create the Document proto. Need to sort the property order by key.
        DocumentProto.Builder documentProtoBuilder = DocumentProto.newBuilder()
                .setUri("id1")
                .setSchema(SCHEMA_TYPE_1)
                .setCreationTimestampMs(5L)
                .setScore(1)
                .setTtlMs(1L)
                .setNamespace("namespace");
        HashMap<String, PropertyProto.Builder> propertyProtoMap = new HashMap<>();
        propertyProtoMap.put("longKey1",
                PropertyProto.newBuilder().setName("longKey1").addInt64Values(1L));
        propertyProtoMap.put("documentKey1",
                PropertyProto.newBuilder().setName("documentKey1").addDocumentValues(
                        GenericDocumentToProtoConverter.toDocumentProto(DOCUMENT_PROPERTIES_1)));
        propertyProtoMap.put("embeddingKey1",
                PropertyProto.newBuilder().setName("embeddingKey1")
                        .addVectorValues(PropertyProto.VectorProto.newBuilder()
                                .addAllValues(Arrays.asList(1.1f, 2.2f, 3.3f))
                                .setModelSignature("my_model_v1")
                        )
                        .addVectorValues(PropertyProto.VectorProto.newBuilder()
                                .addAllValues(Arrays.asList(4.4f, 5.5f, 6.6f, 7.7f))
                                .setModelSignature("my_model_v2")
                        ));
        List<String> sortedKey = new ArrayList<>(propertyProtoMap.keySet());
        Collections.sort(sortedKey);
        for (String key : sortedKey) {
            documentProtoBuilder.addProperties(propertyProtoMap.get(key));
        }
        DocumentProto documentProto = documentProtoBuilder.build();

        GenericDocument convertedGenericDocument =
                GenericDocumentToProtoConverter.toGenericDocument(documentProto, PREFIX,
                        new SchemaCache(SCHEMA_MAP),
                        new AppSearchConfigImpl(new UnlimitedLimitConfig(),
                                new LocalStorageIcingOptionsConfig()));
        DocumentProto convertedDocumentProto =
                GenericDocumentToProtoConverter.toDocumentProto(document);

        assertThat(convertedDocumentProto).isEqualTo(documentProto);
        assertThat(convertedGenericDocument).isEqualTo(document);
    }

    @Test
    public void testDocumentProtoConvert_blobHandleProperty() throws Exception {
        byte[] data1 = {(byte) 1};
        byte[] data2 = {(byte) 2};
        byte[] digest1 = calculateDigest(data1);
        byte[] digest2 = calculateDigest(data2);
        AppSearchBlobHandle blobHandle1 = AppSearchBlobHandle.createWithSha256(
                digest1, "package1", "db1", "namespace");
        AppSearchBlobHandle blobHandle2 = AppSearchBlobHandle.createWithSha256(
                digest2, "package1", "db1", "namespace");

        GenericDocument document =
                new GenericDocument.Builder<GenericDocument.Builder<?>>("namespace", "id1",
                        SCHEMA_TYPE_1)
                        .setCreationTimestampMillis(5L)
                        .setScore(1)
                        .setTtlMillis(1L)
                        .setPropertyBlobHandle("blobKey1", blobHandle1)
                        .setPropertyBlobHandle("blobKey2", blobHandle2)
                        .build();

        // Create the Document proto. Need to sort the property order by key.
        DocumentProto.Builder documentProtoBuilder = DocumentProto.newBuilder()
                .setUri("id1")
                .setSchema(SCHEMA_TYPE_1)
                .setCreationTimestampMs(5L)
                .setScore(1)
                .setTtlMs(1L)
                .setNamespace("namespace");

        // Create the BlobHandle proto.
        HashMap<String, PropertyProto.Builder> propertyProtoMap = new HashMap<>();
        String prefix = PrefixUtil.createPrefix("package1", "db1");

        PropertyProto.BlobHandleProto.Builder blobHandleProto1 =
                PropertyProto.BlobHandleProto.newBuilder()
                        .setNamespace(prefix + "namespace")
                        .setDigest(ByteString.copyFrom(blobHandle1.getSha256Digest()));

        PropertyProto.BlobHandleProto.Builder blobHandleProto2 =
                PropertyProto.BlobHandleProto.newBuilder()
                        .setNamespace(prefix + "namespace")
                        .setDigest(ByteString.copyFrom(blobHandle2.getSha256Digest()));

        propertyProtoMap.put("blobKey1", PropertyProto.newBuilder()
                .setName("blobKey1").addBlobHandleValues(blobHandleProto1));
        propertyProtoMap.put("blobKey2", PropertyProto.newBuilder()
                .setName("blobKey2").addBlobHandleValues(blobHandleProto2));

        // Sort property by keys
        List<String> sortedKey = new ArrayList<>(propertyProtoMap.keySet());
        Collections.sort(sortedKey);
        for (String key : sortedKey) {
            documentProtoBuilder.addProperties(propertyProtoMap.get(key));
        }
        DocumentProto documentProto = documentProtoBuilder.build();

        GenericDocument convertedGenericDocument =
                GenericDocumentToProtoConverter.toGenericDocument(documentProto, PREFIX,
                        new SchemaCache(SCHEMA_MAP),
                        new AppSearchConfigImpl(new UnlimitedLimitConfig(),
                                new LocalStorageIcingOptionsConfig()));
        DocumentProto convertedDocumentProto =
                GenericDocumentToProtoConverter.toDocumentProto(document);

        assertThat(convertedDocumentProto).isEqualTo(documentProto);
        assertThat(convertedGenericDocument).isEqualTo(document);
    }
}
