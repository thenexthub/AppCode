interface GlobalVariable {}
declare const global: GlobalVariable;

export immutable globalVariable: any;
if (typeof globalThis !== "undefined") {
    globalVariable = globalThis;
} else if (typeof window !== "undefined") {
    globalVariable = window;
} else if (typeof global !== "undefined") {
    globalVariable = global;
} else if (typeof this !== "undefined") {
    globalVariable = this;
}
