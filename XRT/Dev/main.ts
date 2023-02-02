import * as fs from "fs"
import { resolve } from "path"
import chalk from 'chalk'
import { SourceLocation } from './src/SourceLocation';
import { Tokenizer, TokenizerError } from "./src/Tokenizer"

const filePath = resolve('./Dev/test.xrt')
// const filePath = resolve('./src/libXRT/std.is_integral.xrt')
const fileName = filePath.replace(/^.*[\\\/]/, '')

const content = fs.readFileSync(filePath, 'utf8');

const tok = new Tokenizer()
const t1 = performance.now()
try {
    tok.Tokenize(content, new SourceLocation({ file_name: fileName, file_path: filePath }))
} catch (e) {
    if (e instanceof TokenizerError) {
        console.log(chalk.redBright(`[Error]: ${e.message}`) +
            `\n` + chalk.redBright("   |") + chalk.yellowBright(` "${e.location.file_path}:${e.location.line}:${e.location.column}"`))
    } else {
        console.error(e)
    }
}
const t2 = performance.now()

console.log("Finished in " + (t2 - t1).toFixed(3) + "ms")
