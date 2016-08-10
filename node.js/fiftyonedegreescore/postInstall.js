var reset = '\x1b[0m';
var Underscore = "\x1b[4m"

var fgRed = "\x1b[31m"
var fgBlack = "\x1b[30m"
var fgCyan = "\x1b[36m"

var bgWhite = "\x1b[47m"
var bgRed = "\x1b[41m"
var bold = "\x1b[1m"

console.log("                    " + bgWhite + "                ", reset);
console.log("                    " + bgWhite + fgRed + bold + "    51" + fgBlack + "Degrees    " + reset)
console.log("                    " + bgWhite + "                 " + reset);
console.log(       " #-----------------------------------------------------------#");
console.log(       " |  To use the 51Degrees API with the free Lite data files   |");
console.log(       " |  run either:                                              |");
console.log(" |" + bold + "    npm install fiftyonedegreeslitepattern                 " + reset + "|");
console.log(       " |  or                                                       |");
console.log(" |" + bold + "    npm install fiftyonedegreeslitetrie                    " + reset + "|");
console.log(       " |                                                           |");
console.log(       " |  For more information on data file options, see:          |");
console.log(       " |   " + bold + "https://51degrees.com/compare-data-options              " + reset + "|")
console.log(       " #-----------------------------------------------------------#");
