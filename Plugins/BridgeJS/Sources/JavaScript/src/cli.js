// @ts-check
import * as fs from 'fs';
import { TypeProcessor } from './processor.js';
import { parseArgs } from 'util';
import ts from 'typescript';
import path from 'path';

class DiagnosticEngine {
    /**
     * @param {string} level
     */
    constructor(level) {
        const levelInfo = DiagnosticEngine.LEVELS[level];
        if (!levelInfo) {
            throw new Error(`Invalid log level: ${level}`);
        }
        this.minLevel = levelInfo.level;
        /** @type {ts.FormatDiagnosticsHost} */
        this.formattHost = {
            getCanonicalFileName: (fileName) => fileName,
            getNewLine: () => ts.sys.newLine,
            getCurrentDirectory: () => ts.sys.getCurrentDirectory(),
        };
    }
    
    /**
     * @param {readonly ts.Diagnostic[]} diagnostics
     */
    tsDiagnose(diagnostics) {
        const message = ts.formatDiagnosticsWithColorAndContext(diagnostics, this.formattHost);
        console.log(message);
    }

    static LEVELS = {
        "verbose": {
            color: '\x1b[34m',
            level: 0,
        },
        "info": {
            color: '\x1b[32m',
            level: 1,
        },
        "warning": {
            color: '\x1b[33m',
            level: 2,
        },
        "error": {
            color: '\x1b[31m',
            level: 3,
        },
    }

    /**
     * @param {keyof typeof DiagnosticEngine.LEVELS} level
     * @param {string} message
     * @param {ts.Node | undefined} node
     */
    print(level, message, node = undefined) {
        const levelInfo = DiagnosticEngine.LEVELS[level];
        if (levelInfo.level < this.minLevel) {
            return;
        }
        const color = levelInfo.color;
        if (node) {
            const sourceFile = node.getSourceFile();
            const { line, character } = sourceFile.getLineAndCharacterOfPosition(node.getStart());
            const location = sourceFile.fileName + ":" + (line + 1) + ":" + (character);
            process.stderr.write(`${location}: ${color}${level}\x1b[0m: ${message}\n`);
        } else {
            process.stderr.write(`${color}${level}\x1b[0m: ${message}\n`);
        }
    }
}

function printUsage() {
    console.error('Usage: ts2skeleton <d.ts file path> -p <tsconfig.json path> [-o output.json]');
}

/**
 * Main function to run the CLI
 * @param {string[]} args - Command-line arguments
 * @returns {void}
 */
export function main(args) {
    // Parse command line arguments
    const options = parseArgs({
        args,
        options: {
            output: {
                type: 'string',
                short: 'o',
            },
            project: {
                type: 'string',
                short: 'p',
            },
            "log-level": {
                type: 'string',
                default: 'info',
            },
        },
        allowPositionals: true
    })

    if (options.positionals.length !== 1) {
        printUsage();
        process.exit(1);
    }

    const tsconfigPath = options.values.project;
    if (!tsconfigPath) {
        printUsage();
        process.exit(1);
    }

    const filePath = options.positionals[0];
    const diagnosticEngine = new DiagnosticEngine(options.values["log-level"] || "info");

    diagnosticEngine.print("verbose", `Processing ${filePath}...`);

    // Create TypeScript program and process declarations
    const configFile = ts.readConfigFile(tsconfigPath, ts.sys.readFile);
    const configParseResult = ts.parseJsonConfigFileContent(
        configFile.config,
        ts.sys,
        path.dirname(path.resolve(tsconfigPath))
    );

    if (configParseResult.errors.length > 0) {
        diagnosticEngine.tsDiagnose(configParseResult.errors);
        process.exit(1);
    }

    const program = TypeProcessor.createProgram(filePath, configParseResult.options);
    const diagnostics = program.getSemanticDiagnostics();
    if (diagnostics.length > 0) {
        diagnosticEngine.tsDiagnose(diagnostics);
        process.exit(1);
    }

    const processor = new TypeProcessor(program.getTypeChecker(), diagnosticEngine);
    const results = processor.processTypeDeclarations(program, filePath);

    // Write results to file or stdout
    const jsonOutput = JSON.stringify(results, null, 2);
    if (options.values.output) {
        fs.writeFileSync(options.values.output, jsonOutput);
    } else {
        process.stdout.write(jsonOutput, "utf-8");
    }
}
