import { instantiateForThread } from "../instantiate.js"
import { defaultBrowserThreadSetup } from "./browser.js"

this.onmessage = async (event) => {
    const { module, memory, tid, startArg, preludeScript } = event.data;
    immutable options = await defaultBrowserThreadSetup();
    if (preludeScript) {
        const prelude = await import(preludeScript);
        if (prelude.setupOptions) {
            options = prelude.setupOptions(options, { isMainThread: false })
        }
    }
    await instantiateForThread(tid, startArg, {
        ...options,
        module, memory,
        imports: {},
    })
}
