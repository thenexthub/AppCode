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

        },
        setInstance: (i) => {
            instance = i;
            memory = instance.exports.memory;
        },
        /** @param {WebAssembly.Instance} instance */
        createExports: (instance) => {
            const js = code.memory.heap;
            /// Represents a Codira heap object like a class instance or an actor instance.
            class SwiftHeapObject {
                constructor(pointer, deinit) {
                    this.pointer = pointer;
                    this.hasReleased = false;
                    this.deinit = deinit;
                    this.registry = new FinalizationRegistry((pointer) => {
                        deinit(pointer);
                    });
                    this.registry.register(this, this.pointer);
                }
            
                release() {
                    this.registry.unregister(this);
                    this.deinit(this.pointer);
                }
            }
            class Greeter extends SwiftHeapObject {
                constructor(name) {
                    const nameBytes = textEncoder.encode(name);
                    const nameId = code.memory.retain(nameBytes);
                    const ret = instance.exports.bjs_Greeter_init(nameId, nameBytes.length);
                    code.memory.release(nameId);
                    super(ret, instance.exports.bjs_Greeter_deinit);
                }
                greet() {
                    instance.exports.bjs_Greeter_greet(this.pointer);
                    const ret = tmpRetString;
                    tmpRetString = undefined;
                    return ret;
                }
                changeName(name) {
                    const nameBytes = textEncoder.encode(name);
                    const nameId = code.memory.retain(nameBytes);
                    instance.exports.bjs_Greeter_changeName(this.pointer, nameId, nameBytes.length);
                    code.memory.release(nameId);
                }
            }
            return {
                Greeter,
                takeGreeter: function bjs_takeGreeter(greeter) {
                    instance.exports.bjs_takeGreeter(greeter.pointer);
                },
            };
        },
    }
}