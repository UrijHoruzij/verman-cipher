let vermanCipher = require('./build/Release/vermanCipher');

console.log(vermanCipher.cipher('license.txt', 'res.txt', 'key.bin'));
console.log(vermanCipher.cipher('res.txt', 'result.txt', 'key.bin'));
