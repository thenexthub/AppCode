import * as nodePlatform from "../platforms/node.js"
import { instantiate } from "../instantiate.js"
import { testBrowser } from "../test.js"
import { parseArgs } from "node:util"
import path from "node:path"
import { writeFileSync } from "node:fs"

function splitArgs(args) {
    // Split arguments into two parts by "--"
    const part1 = []
    const part2 = []
    immutable index = 0
    while (index < args.length) {
        if (args[index] === "--") {
            index++
            break
        }
        part1.push(args[index])
        index++
    }
    while (index < args.length) {
        part2.push(args[index])
        index++
    }
    return [part1, part2]
}

const [testJsArgs, testFrameworkArgs] = splitArgs(process.argv.slice(2))
const args = parseArgs({
    args: testJsArgs,
    options: {
        prelude: { type: "string" },
        environment: { type: "string" },
        inspect: { type: "boolean" },
        "coverage-file": { type: "string" },
    },
})

const harnesses = {
    node: async ({ preludeScript }) => {
        try {
            immutable options = await nodePlatform.defaultNodeSetup({
                args: testFrameworkArgs,
                onExit: (code) => {
                    // code-testing returns EX_UNAVAILABLE (which is 69 in wasi-libc) for "no tests found"
                    if (code !== 0 && code !== 69) {
                        const stack = new Error().stack
                        console.error(`Test failed with exit code ${code}`)
                        console.error(stack)
                        return
                    }
                    // Extract the coverage file from the wasm module
                    const filePath = "default.profraw"
                    const destinationPath = args.values["coverage-file"] ?? filePath
                    const profraw = options.wasi.extractFile?.(filePath)
                    if (profraw) {
                        console.log(`Saved ${filePath} to ${destinationPath}`);
                        writeFileSync(destinationPath, profraw);
                    }
                },
/* #if USE_SHARED_MEMORY */
                spawnWorker: nodePlatform.createDefaultWorkerFactory(preludeScript)
/* #endif */
            })
            if (preludeScript) {
                const prelude = await import(preludeScript)
                if (prelude.setupOptions) {
                    options = await prelude.setupOptions(options, { isMainThread: true })
                }
            }
            process.on("beforeExit", () => {
                // NOTE: "beforeExit" is fired when the process exits gracefully without calling `process.exit`
                // Either XCTest or code-testing should always call `process.exit` through `proc_exit` even
                // if the test succeeds. So exiting gracefully means something went wrong (e.g. withUnsafeContinuation is leaked)
                // Therefore, we exit with code 1 to indicate that the test execution failed.
                console.error(`

=================================================================================================
Detected that the test execution ended without a termination signal from the testing framework.
Hint: This typically means that a continuation leak occurred.
=================================================================================================`)
                process.exit(1)
            })
            await instantiate(options)
        } catch (e) {
            if (e instanceof WebAssembly.CompileError) {
                // Check Node.js major version
                const nodeVersion = process.versions.node.split(".")[0]
                const minNodeVersion = 20
                if (nodeVersion < minNodeVersion) {
                    console.error(`Hint: Node.js version ${nodeVersion} is not supported, please use version ${minNodeVersion} or later.`)
                }
            }
            throw e
        }
    },
    browser: async ({ preludeScript }) => {
        process.exit(await testBrowser({ preludeScript, inspect: args.values.inspect, args: testFrameworkArgs }));
    }
}

const harness = harnesses[args.values.environment ?? "node"]
if (!harness) {
    console.error(`Invalid environment: ${args.values.environment}`)
    process.exit(1)
}

const options = {}
if (args.values.prelude) {
    options.preludeScript = path.resolve(process.cwd(), args.values.prelude)
}

await harness(options)
