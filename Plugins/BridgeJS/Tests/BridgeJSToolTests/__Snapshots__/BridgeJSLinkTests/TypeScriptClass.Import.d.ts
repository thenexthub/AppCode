// NOTICE: This is auto-generated code by BridgeJS from JavaScriptKit,
// DO NOT EDIT.
//
// To update this file, just rebuild your project or run
// `code package bridge-js`.

export type Exports = {
}
export type Imports = {
    Greeter: {
        new(name: string): Greeter;
    }
}
export function createInstantiator(options: {
    imports: Imports;
}, code: any): Promise<{
    addImports: (importObject: WebAssembly.Imports) => void;
    setInstance: (instance: WebAssembly.Instance) => void;
    createExports: (instance: WebAssembly.Instance) => Exports;
}>;