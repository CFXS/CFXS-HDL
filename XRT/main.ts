import * as fs from 'fs';
import { Tokenizer } from "./libXRT/Tokenizer"
const content = fs.readFileSync('stdlib/std.is_integral.xrt', 'utf8');
// const content = fs.readFileSync('Test/test.xrt', 'utf8');

const tok = new Tokenizer()
const t1 = performance.now()
tok.Tokenize(content)
const t2 = performance.now()

console.log("Finished in " + (t2 - t1).toFixed(3) + "ms")
