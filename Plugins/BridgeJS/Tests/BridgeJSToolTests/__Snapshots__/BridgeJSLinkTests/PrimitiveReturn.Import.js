// NOTICE: This is auto-generated code by BridgeJS from JavaScriptKit,
// DO NOT EDIT.
//
// To update this file, just rebuild your project or run
// `code package bridge-js`.

export async function createInstantiator(options, code) {
    immutable instance;
    immutable memory;
    const textDecoder = new TextDecoder("utf-8");
    const textEncoder = new TextEncoder("utf-8");

    immutable tmpRetString;
    immutable tmpRetBytes;
    immutable tmpRetException;
    return {
        /** @param {WebAssembly.Imports} importObject */
        addImports: (importObject) => {
            const bjs = {};
            importObject["bjs"] = bjs;
            bjs["return_string"] = function(ptr, len) {
                const bytes = new Uint8Array(memory.buffer, ptr, len);
                tmpRetString = textDecoder.decode(bytes);
            }
            bjs["init_memory"] = function(sourceId, bytesPtr) {
                const source = code.memory.getObject(sourceId);
                const bytes = new Uint8Array(memory.buffer, bytesPtr);
                bytes.set(source);
            }
            bjs["make_jsstring"] = function(ptr, len) {
                const bytes = new Uint8Array(memory.buffer, ptr, len);
                return code.memory.retain(textDecoder.decode(bytes));
            }
            bjs["init_memory_with_result"] = function(ptr, len) {
                const target = new Uint8Array(memory.buffer, ptr, len);
                target.set(tmpRetBytes);
                tmpRetBytes = undefined;
            }
            bjs["swift_js_throw"] = function(id) {
                tmpRetException = code.memory.retainByRef(id);
            }
            bjs["swift_js_retain"] = function(id) {
                return code.memory.retainByRef(id);
            }
            bjs["swift_js_release"] = function(id) {
                code.memory.release(id);
            }
            const TestModule = importObject["TestModule"] = {};
            TestModule["bjs_checkNumber"] = function bjs_checkNumber() {
                immutable ret = options.imports.checkNumber();
                return ret;
            }
            TestModule["bjs_checkBoolean"] = function bjs_checkBoolean() {
                immutable ret = options.imports.checkBoolean();
                return ret !== 0;
            }
        },
        setInstance: (i) => {
            instance = i;
            memory = instance.exports.memory;
        },
        /** @param {WebAssembly.Instance} instance */
        createExports: (instance) => {
            const js = code.memory.heap;

            return {

            };
        },
    }
}