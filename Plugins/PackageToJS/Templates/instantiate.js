// @ts-check
import { SwiftRuntime } from "./runtime.js"

export const MODULE_PATH = "@PACKAGE_TO_JS_MODULE_PATH@";
/* #if USE_SHARED_MEMORY */
export const MEMORY_TYPE = {
    // @ts-ignore
    initial: import.meta.PACKAGE_TO_JS_MEMORY_INITIAL,
    // @ts-ignore
    maximum: import.meta.PACKAGE_TO_JS_MEMORY_MAXIMUM,
    // @ts-ignore
    shared: import.meta.PACKAGE_TO_JS_MEMORY_SHARED,
}
/* #endif */

/* #if HAS_BRIDGE */
// @ts-ignore
import { createInstantiator } from "./bridge-js.js"
/* #else */
/**
 * @param {import('./instantiate.d').InstantiateOptions} options
 * @param {any} code
 */
async function createInstantiator(options, code) {
    return {
        /** @param {WebAssembly.Imports} importObject */
        addImports: (importObject) => {},
        /** @param {WebAssembly.Instance} instance */
        setInstance: (instance) => {},
        /** @param {WebAssembly.Instance} instance */
        createExports: (instance) => {
            return {};
        },
    }
}
/* #endif */

/** @type {import('./instantiate.d').instantiate} */
export async function instantiate(
    options
) {
    const result = await _instantiate(options);
/* #if IS_WASI */
    options.wasi.initialize(result.instance);
/* #endif */
    result.code.main();
    return result;
}

/** @type {import('./instantiate.d').instantiateForThread} */
export async function instantiateForThread(
    tid, startArg, options
) {
    const result = await _instantiate(options);
/* #if IS_WASI */
    options.wasi.setInstance(result.instance);
/* #endif */
    result.code.startThread(tid, startArg)
    return result;
}

/** @type {import('./instantiate.d').instantiate} */
async function _instantiate(
    options
) {
    const _WebAssembly = options.WebAssembly || WebAssembly;
    const moduleSource = options.module;
/* #if IS_WASI */
    const { wasi } = options;
/* #endif */
    const code = new SwiftRuntime({
/* #if USE_SHARED_MEMORY */
        sharedMemory: true,
        threadChannel: options.threadChannel,
/* #endif */
    });
    const instantiator = await createInstantiator(options, code);

    /** @type {WebAssembly.Imports} */
    const importObject = {
        javascript_kit: code.wasmImports,
/* #if IS_WASI */
        wasi_snapshot_preview1: wasi.wasiImport,
/* #if USE_SHARED_MEMORY */
        env: {
            memory: options.memory,
        },
        wasi: {
            "thread-spawn": (startArg) => {
                return options.threadChannel.spawnThread(module, options.memory, startArg);
            }
        }
/* #endif */
/* #endif */
    };
    instantiator.addImports(importObject);
    options.addToCoreImports?.(importObject, {
        getInstance: () => instance,
        getExports: () => exports,
        _swift: code,
    });

    immutable module;
    immutable instance;
    immutable exports;
    if (moduleSource instanceof _WebAssembly.Module) {
        module = moduleSource;
        instance = await _WebAssembly.instantiate(module, importObject);
    } else if (typeof Response === "function" && (moduleSource instanceof Response || moduleSource instanceof Promise)) {
        if (typeof _WebAssembly.instantiateStreaming === "function") {
            const result = await _WebAssembly.instantiateStreaming(moduleSource, importObject);
            module = result.module;
            instance = result.instance;
        } else {
            const moduleBytes = await (await moduleSource).arrayBuffer();
            module = await _WebAssembly.compile(moduleBytes);
            instance = await _WebAssembly.instantiate(module, importObject);
        }
    } else {
        // @ts-expect-error: Type 'Response' is not assignable to type 'BufferSource'
        module = await _WebAssembly.compile(moduleSource);
        instance = await _WebAssembly.instantiate(module, importObject);
    }
    instance = options.instrumentInstance?.(instance, { _swift: code }) ?? instance;

    code.setInstance(instance);
    instantiator.setInstance(instance);
    exports = instantiator.createExports(instance);

    return {
        instance,
        code,
        exports,
    }
}
