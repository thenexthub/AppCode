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

package androidx.appsearch.safeparcel;

import static androidx.appsearch.testutil.AppSearchTestUtils.calculateDigest;
import static androidx.appsearch.testutil.AppSearchTestUtils.generateRandomBytes;

import static com.google.common.truth.Truth.assertThat;

import static org.junit.Assert.assertThrows;

import android.os.Parcel;

import androidx.appsearch.app.AppSearchBlobHandle;
import androidx.appsearch.app.EmbeddingVector;
import androidx.appsearch.flags.Flags;
import androidx.appsearch.testutil.AppSearchTestUtils;
import androidx.appsearch.testutil.flags.RequiresFlagsEnabled;

import org.junit.Rule;
import org.junit.Test;
import org.junit.rules.RuleChain;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;
import java.util.Map;

/** Tests for {@link androidx.appsearch.app.GenericDocument} related SafeParcels. */
public class GenericDocumentParcelTest {

    @Rule
    public final RuleChain mRuleChain = AppSearchTestUtils.createCommonTestRules();

    @Test
    public void testPropertyParcel_onePropertySet_success() {
        String[] stringValues = {"a", "b"};
        long[] longValues = {1L, 2L};
        double[] doubleValues = {1.0, 2.0};
        boolean[] booleanValues = {true, false};
        byte[][] bytesValues = {new byte[1]};
        EmbeddingVector[] embeddingValues = {new EmbeddingVector(new float[1],
                "my_model")};
        GenericDocumentParcel[] docValues = {(new GenericDocumentParcel.Builder(
                "namespace", "id", "schemaType")).build()};

        assertThat(new PropertyParcel.Builder("name").setStringValues(
                stringValues).build().getStringValues()).isEqualTo(
                Arrays.copyOf(stringValues, stringValues.length));
        assertThat(new PropertyParcel.Builder("name").setLongValues(
                longValues).build().getLongValues()).isEqualTo(
                Arrays.copyOf(longValues, longValues.length));
        assertThat(new PropertyParcel.Builder("name").setDoubleValues(
                doubleValues).build().getDoubleValues()).isEqualTo(
                Arrays.copyOf(doubleValues, doubleValues.length));
        assertThat(new PropertyParcel.Builder("name").setBooleanValues(
                booleanValues).build().getBooleanValues()).isEqualTo(
                Arrays.copyOf(booleanValues, booleanValues.length));
        assertThat(new PropertyParcel.Builder("name").setBytesValues(
                bytesValues).build().getBytesValues()).isEqualTo(
                Arrays.copyOf(bytesValues, bytesValues.length));
        assertThat(new PropertyParcel.Builder("name").setEmbeddingValues(
                embeddingValues).build().getEmbeddingValues()).isEqualTo(
                Arrays.copyOf(embeddingValues, embeddingValues.length));
        assertThat(new PropertyParcel.Builder("name").setDocumentValues(
                docValues).build().getDocumentValues()).isEqualTo(
                Arrays.copyOf(docValues, docValues.length));
    }

    @Test
    @RequiresFlagsEnabled(Flags.FLAG_ENABLE_BLOB_STORE)
    public void testPropertyParcel_blobHandleSet_success() throws Exception {
        byte[] data1 = generateRandomBytes(10); // 10 Bytes
        byte[] digest1 = calculateDigest(data1);
        byte[] data2 = generateRandomBytes(10); // 10 Bytes
        byte[] digest2 = calculateDigest(data2);
        AppSearchBlobHandle blob1 = AppSearchBlobHandle.createWithSha256(
                digest1, "package1", "db1", "ns");
        AppSearchBlobHandle blob2 = AppSearchBlobHandle.createWithSha256(
                digest2, "package1", "db1", "ns");
        AppSearchBlobHandle[] blobHandles = {blob1, blob2};

        PropertyParcel parcel =  new PropertyParcel.Builder("name")
                .setBlobHandleValues(blobHandles).build();
        assertThat(parcel.getBlobHandleValues()).asList()
                .containsExactly(blob1, blob2).inOrder();
    }

    @Test
    public void testPropertyParcel_moreThanOnePropertySet_exceptionThrown() {
        String[] stringValues = {"a", "b"};
        long[] longValues = {1L, 2L};
        PropertyParcel.Builder propertyParcelBuilder =
                new PropertyParcel.Builder("name")
                        .setStringValues(stringValues)
                        .setLongValues(longValues);

        IllegalArgumentException exception = assertThrows(IllegalArgumentException.class,
                () -> propertyParcelBuilder.build());

        assertThat(exception.getMessage()).contains("One and only one type array");
    }

    @Test
    public void testGenericDocumentParcel_propertiesGeneratedCorrectly() {
        GenericDocumentParcel.Builder builder =
                new GenericDocumentParcel.Builder(
                        /*namespace=*/ "namespace",
                        /*id=*/ "id",
                        /*schemaType=*/ "schemaType");
        long[] longArray = new long[]{1L, 2L, 3L};
        String[] stringArray = new String[]{"hello", "world", "!"};
        builder.putInPropertyMap(/*name=*/ "longArray", /*values=*/ longArray);
        builder.putInPropertyMap(/*name=*/ "stringArray", /*values=*/ stringArray);
        GenericDocumentParcel genericDocumentParcel = builder.build();

        List<PropertyParcel> properties = genericDocumentParcel.getProperties();
        Map<String, PropertyParcel> propertyMap = genericDocumentParcel.getPropertyMap();
        PropertyParcel longArrayProperty = new PropertyParcel.Builder(
                /*name=*/ "longArray").setLongValues(longArray).build();
        PropertyParcel stringArrayProperty = new PropertyParcel.Builder(
                /*name=*/ "stringArray").setStringValues(stringArray).build();

        assertThat(properties).containsExactly(longArrayProperty, stringArrayProperty);
        assertThat(propertyMap).containsExactly("longArray", longArrayProperty,
                "stringArray", stringArrayProperty);
    }

    @Test
    public void testGenericDocumentParcel_buildFromAnotherDocumentParcelCorrectly() {
        GenericDocumentParcel.Builder builder =
                new GenericDocumentParcel.Builder(
                        /*namespace=*/ "namespace",
                        /*id=*/ "id",
                        /*schemaType=*/ "schemaType");
        long[] longArray = new long[]{1L, 2L, 3L};
        String[] stringArray = new String[]{"hello", "world", "!"};
        List<String> parentTypes = new ArrayList<>(Arrays.asList("parentType1", "parentType2"));
        builder.putInPropertyMap(/*name=*/ "longArray", /*values=*/ longArray);
        builder.putInPropertyMap(/*name=*/ "stringArray", /*values=*/ stringArray);
        builder.setParentTypes(parentTypes);
        GenericDocumentParcel genericDocumentParcel = builder.build();

        GenericDocumentParcel genericDocumentParcelCopy =
                new GenericDocumentParcel.Builder(genericDocumentParcel).build();

        assertThat(genericDocumentParcelCopy.getNamespace()).isEqualTo(
                genericDocumentParcel.getNamespace());
        assertThat(genericDocumentParcelCopy.getId()).isEqualTo(genericDocumentParcel.getId());
        assertThat(genericDocumentParcelCopy.getSchemaType()).isEqualTo(
                genericDocumentParcel.getSchemaType());
        assertThat(genericDocumentParcelCopy.getCreationTimestampMillis()).isEqualTo(
                genericDocumentParcel.getCreationTimestampMillis());
        assertThat(genericDocumentParcelCopy.getTtlMillis()).isEqualTo(
                genericDocumentParcel.getTtlMillis());
        assertThat(genericDocumentParcelCopy.getScore()).isEqualTo(
                genericDocumentParcel.getScore());
        assertThat(genericDocumentParcelCopy.getParentTypes()).isEqualTo(
                genericDocumentParcel.getParentTypes());

        // Check it is a copy.
        assertThat(genericDocumentParcelCopy).isNotSameInstanceAs(genericDocumentParcel);
        assertThat(genericDocumentParcelCopy.getProperties()).isEqualTo(
                genericDocumentParcel.getProperties());
    }

    @Test
    public void testGenericDocumentParcelWithParentTypes() {
        GenericDocumentParcel.Builder builder =
                new GenericDocumentParcel.Builder(
                        /*namespace=*/ "namespace",
                        /*id=*/ "id",
                        /*schemaType=*/ "schemaType");
        List<String> parentTypes = new ArrayList<>(Arrays.asList("parentType1", "parentType2"));

        builder.setParentTypes(parentTypes);
        GenericDocumentParcel genericDocumentParcel = builder.build();

        assertThat(genericDocumentParcel.getParentTypes()).isEqualTo(parentTypes);
    }

    @Test
    public void testGenericDocumentParcel_builderCanBeReused() {
        GenericDocumentParcel.Builder builder =
                new GenericDocumentParcel.Builder(
                        /*namespace=*/ "namespace",
                        /*id=*/ "id",
                        /*schemaType=*/ "schemaType");
        long[] longArray = new long[]{1L, 2L, 3L};
        String[] stringArray = new String[]{"hello", "world", "!"};
        List<String> parentTypes = new ArrayList<>(Arrays.asList("parentType1", "parentType2"));
        builder.putInPropertyMap(/*name=*/ "longArray", /*values=*/ longArray);
        builder.putInPropertyMap(/*name=*/ "stringArray", /*values=*/ stringArray);
        builder.setParentTypes(parentTypes);

        GenericDocumentParcel genericDocumentParcel = builder.build();
        builder.setParentTypes(new ArrayList<>(Arrays.asList("parentType3", "parentType4")));
        builder.clearProperty("longArray");
        builder.putInPropertyMap(/*name=*/ "stringArray", /*values=*/ new String[]{""});

        PropertyParcel longArrayProperty = new PropertyParcel.Builder(
                /*name=*/ "longArray").setLongValues(longArray).build();
        PropertyParcel stringArrayProperty = new PropertyParcel.Builder(
                /*name=*/ "stringArray").setStringValues(stringArray).build();
        assertThat(genericDocumentParcel.getParentTypes()).isEqualTo(parentTypes);
        assertThat(genericDocumentParcel.getPropertyMap()).containsExactly("longArray",
                longArrayProperty, "stringArray", stringArrayProperty);
    }

    @Test
    public void testRecreateFromParcelWithParentTypes() {
        String[] stringArray = new String[]{"Hello", "world"};
        long[] longArray = new long[]{1L, 2L};
        double[] doubleArray = new double[]{1.1, 2.2};
        boolean[] booleanArray = new boolean[]{true, false};
        byte[][] bytesArray = new byte[][]{{1, 2}};
        GenericDocumentParcel inDoc = new GenericDocumentParcel.Builder(
                "namespace1", "id1", "schema1")
                .setParentTypes(new ArrayList<>(Arrays.asList("Class1", "Class2")))
                .setScore(42)
                .setTtlMillis(43)
                .setCreationTimestampMillis(44)
                .putInPropertyMap("propStrings", stringArray)
                .putInPropertyMap("propLongs", longArray)
                .putInPropertyMap("propDoubles", doubleArray)
                .putInPropertyMap("propBytes", bytesArray)
                .putInPropertyMap("propBooleans", booleanArray)
                .putInPropertyMap(
                        "propDoc",
                        new GenericDocumentParcel[]{
                                new GenericDocumentParcel.Builder(
                                        "namespace2", "id2", "schema2")
                                        .putInPropertyMap("propStrings", new String[]{"Goodbye"})
                                        .putInPropertyMap("propBytes", new byte[][]{{3, 4}})
                                        .build()})
                .build();

        // Serialize the document
        Parcel inParcel = Parcel.obtain();
        inParcel.writeParcelable(inDoc, /*flags=*/ 0);
        byte[] data = inParcel.marshall();
        inParcel.recycle();

        // Deserialize the document
        Parcel outParcel = Parcel.obtain();
        outParcel.unmarshall(data, 0, data.length);
        outParcel.setDataPosition(0);
        @SuppressWarnings("deprecation")
        GenericDocumentParcel outDoc = outParcel.readParcelable(getClass().getClassLoader());
        outParcel.recycle();

        // Compare results
        assertThat(outDoc.getId()).isEqualTo("id1");
        assertThat(outDoc.getNamespace()).isEqualTo("namespace1");
        assertThat(outDoc.getSchemaType()).isEqualTo("schema1");
        assertThat(outDoc.getParentTypes()).isEqualTo(Arrays.asList("Class1", "Class2"));
        assertThat(outDoc.getScore()).isEqualTo(42);
        assertThat(outDoc.getTtlMillis()).isEqualTo(43);
        assertThat(outDoc.getCreationTimestampMillis()).isEqualTo(44);

        // Properties
        Map<String, PropertyParcel> propertyMap = outDoc.getPropertyMap();
        assertThat(propertyMap.get("propStrings").getStringValues()).isEqualTo(stringArray);
        assertThat(propertyMap.get("propLongs").getLongValues()).isEqualTo(longArray);
        assertThat(propertyMap.get("propDoubles").getDoubleValues()).isEqualTo(doubleArray);
        assertThat(propertyMap.get("propBytes").getBytesValues()).isEqualTo(bytesArray);
        assertThat(propertyMap.get("propBooleans").getBooleanValues()).isEqualTo(booleanArray);

        // Check inner doc.
        GenericDocumentParcel[] innerDocs = propertyMap.get("propDoc").getDocumentValues();
        assertThat(innerDocs).hasLength(1);
        assertThat(innerDocs[0].getNamespace()).isEqualTo("namespace2");
        assertThat(innerDocs[0].getId()).isEqualTo("id2");
        assertThat(innerDocs[0].getSchemaType()).isEqualTo("schema2");
        assertThat(innerDocs[0].getPropertyMap().get("propStrings").getStringValues()).isEqualTo(
                new String[]{"Goodbye"});
        assertThat(innerDocs[0].getPropertyMap().get("propBytes").getBytesValues()).isEqualTo(
                new byte[][]{{3, 4}});

        // Finally check equals and hashcode
        assertThat(inDoc).isEqualTo(outDoc);
        assertThat(inDoc.hashCode()).isEqualTo(outDoc.hashCode());
    }
}
