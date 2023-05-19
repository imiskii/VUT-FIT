<?php
/**
 * IPP project, parse.php
 * @Author: Michal Ľaš
 * @Date:   2023-02-08
 */


 ini_set('display_errors', 'stderr');



// ********************** CONSTANTS AND GLOBAL VARIABLES ********************** //


// Language
const CODE_LANGUAGE = "IPPcode23";

// flag for epty line
const EMPTY_LINE = 1;

// flag for HEADER
const HEADER_LINE = 2;

// Reguler expressions
const VARR = "/^(GF|TF|LF)@([a-zA-Z_\-$&%*!?][a-zA-Z0-9_\-$&%*!?]*)$/";
const SYMB = "/^((GF|TF|LF)@([a-zA-Z_\-$&%*!?][a-zA-Z0-9_\-$&%*!?]*)|(int)@([+-]?([1-9][0-9]*|0x[0-9a-fA-F]+|0o[0-7]+|0+))|(float)@([+-]?([1-9][0-9]*(.[0-9]+)?|0x[0-9a-fA-F]+(.[0-9a-fA-F]+)?|0+(.[0-9]+)?)(p[+-][0-9]+)?)|(bool)@(true|false)|(nil)@(nil)|(string)@((\\\\[0-9]{3}|[^#\\\\])*))$/";
const SYMBL = "/^((GF|TF|LF)@([a-zA-Z_\-$&%*!?][a-zA-Z0-9_\-$&%*!?]*)|([a-zA-Z_\-$&%*!?][a-zA-Z0-9_\-$&%*!?]*)|(int)@([+-]?([1-9][0-9]*|0x[0-9a-fA-F]+|0o[0-7]+|0+))|(float)@([+-]?([1-9][0-9]*(.[0-9]+)?|0x[0-9a-fA-F]+(.[0-9a-fA-F]+)?|0+(.[0-9]+)?)(p[+-][0-9]+)?)|(bool)@(true|false)|(nil)@(nil)|(string)@((\\\\[0-9]{3}|[^#\\\\])*))$/";
#const SYMB_BOOL = "/^(?:(GF|TF|LF)@([a-zA-Z_\-$&%*!?][a-zA-Z0-9_\-$&%*!?]*)|(bool)@(true|false))$/";
#const SYMB_INT = "/(?:(GF|TF|LF)@([a-zA-Z_\-$&%*!?][a-zA-Z0-9_\-$&%*!?]*)|(int)@([+-]?[0-9]+))/";
#const SYMB_STRING = "/^(?:(GF|TF|LF)@([a-zA-Z_\-$&%*!?][a-zA-Z0-9_\-$&%*!?]*)|(string)@((?:\\\\[0-9]{3}|[^\s#\\\\])*))$/";
const LABEL = "/^([a-zA-Z_\-$&%*!?][a-zA-Z0-9_\-$&%*!?]*)$/";
const TYPE = "/^(int|string|bool|float)$/";

// IPPcode23 Instructions
// For each instruction there is number of arguments and regular expression for its operands
const INSTRUCTIONS = array(
    "MOVE" => array("argc" => 2, "argv" => array(VARR, SYMB)),
    "CREATEFRAME" => array("argc" => 0, "argv" => array()),
    "PUSHFRAME" => array("argc" => 0, "argv" => array()),
    "POPFRAME" => array("argc" => 0, "argv" => array()),
    "DEFVAR" => array("argc" => 1, "argv" => array(VARR)),
    "CALL" => array("argc" => 1, "argv" => array(LABEL)),
    "RETURN" => array("argc" => 0, "argv" => array()),
    "PUSHS" => array("argc" => 1, "argv" => array(SYMBL)),
    "POPS" => array("argc" => 1, "argv" => array(VARR)),
    "ADD" => array("argc" => 3, "argv" => array(VARR, SYMB, SYMB)),
    "SUB" => array("argc" => 3, "argv" => array(VARR, SYMB, SYMB)),
    "MUL" => array("argc" => 3, "argv" => array(VARR, SYMB, SYMB)),
    "IDIV" => array("argc" => 3, "argv" => array(VARR, SYMB, SYMB)),
    "LT" => array("argc" => 3, "argv" => array(VARR, SYMB, SYMB)),
    "GT" => array("argc" => 3, "argv" => array(VARR, SYMB, SYMB)),
    "EQ" => array("argc" => 3, "argv" => array(VARR, SYMB, SYMB)),
    "AND" => array("argc" => 3, "argv" => array(VARR, SYMB, SYMB)),
    "OR" => array("argc" => 3, "argv" => array(VARR, SYMB, SYMB)),
    "NOT" => array("argc" => 2, "argv" => array(VARR, SYMB)),
    "INT2CHAR" => array("argc" => 2, "argv" => array(VARR, SYMB)),
    "STRI2INT" => array("argc" => 3, "argv" => array(VARR, SYMB, SYMB)),
    "READ" => array("argc" => 2, "argv" => array(VARR, TYPE)),
    "WRITE" => array("argc" => 1, "argv" => array(SYMB)),
    "CONCAT" => array("argc" => 3, "argv" => array(VARR, SYMB, SYMB)),
    "STRLEN" => array("argc" => 2, "argv" => array(VARR, SYMB)),
    "GETCHAR" => array("argc" => 3, "argv" => array(VARR, SYMB, SYMB)),
    "SETCHAR" => array("argc" => 3, "argv" => array(VARR, SYMB, SYMB)),
    "TYPE" => array("argc" => 2, "argv" => array(VARR, SYMB)),
    "LABEL" => array("argc" => 1, "argv" => array(LABEL)),
    "JUMP" => array("argc" => 1, "argv" => array(LABEL)),
    "JUMPIFEQ" => array("argc" => 3, "argv" => array(LABEL, SYMB, SYMB)),
    "JUMPIFNEQ" => array("argc" => 3, "argv" => array(LABEL, SYMB, SYMB)),
    "EXIT" => array("argc" => 1, "argv" => array(SYMB)),
    "DPRINT" => array("argc" => 1, "argv" => array(SYMB)),
    "BREAK" => array("argc" => 0, "argv" => array()),
    "INT2FLOAT" => array("argc" => 2, "argv" => array(VARR, SYMB)),
    "FLOAT2INT" => array("argc" => 2, "argv" => array(VARR, SYMB)),
    "CLEARS" => array("argc" => 0, "argv" => array()),
    "ADDS" => array("argc" => 0, "argv" => array()), 
    "SUBS" => array("argc" => 0, "argv" => array()),
    "MULS" => array("argc" => 0, "argv" => array()), 
    "IDIVS" => array("argc" => 0, "argv" => array()), 
    "LTS" => array("argc" => 0, "argv" => array()), 
    "GTS" => array("argc" => 0, "argv" => array()), 
    "EQS" => array("argc" => 0, "argv" => array()), 
    "ANDS" => array("argc" => 0, "argv" => array()), 
    "ORS" => array("argc" => 0, "argv" => array()), 
    "NOTS" => array("argc" => 0, "argv" => array()), 
    "INT2CHARS" => array("argc" => 0, "argv" => array()), 
    "STRI2INTS" => array("argc" => 0, "argv" => array()), 
    "JUMPIFEQS" => array("argc" => 0, "argv" => array()), 
    "JUMPIFNEQS" => array("argc" => 0, "argv" => array())
);


// Global variable for holding statistics values
$STATS = array(
    "--loc" => 0,
    "--comments" => 0,
    "--labels" => 0,
    "--jumps" => 0,
    "--fwjumps" => 0,
    "--backjumps" => 0,
    "--badjumps" => 0,
    "--frequent" => array()
);

 // ******************************** FUNCTIONS ******************************** //


 /**
  * Function print help for program and exit the script
  */
function printHelp()
{
    echo("This program reads IPPcode23 from standard input (stdin), checks it lexically and syntactically, and returns an XML representation of IPPcode23 to standard output (stdout).
    Possible parameters:
    * --stats=file: the program will write statistics to the file according to the parameters placed after this --stats
    * --loc: writes to the statistics number of lines with instructions
    * --comments: writes to the statistics the number of lines with comment
    * --jumps: writes to the statistics number of JUMPs, CALL and RETURN instructions
    * --fwjumps: writes the number of forward jumps
    * --backjumps: writes the number of back jumps
    * --badjumps: writes the number of jumps on a non-existent sign
    * --frequent: writes the names of the instructions that are most frequent in the source code
    * --print=string: writes the 'string' to the statistics
    * --eol: writes new line to the statistics
    Possible exit codes:
    - 0 : if program ran correctly
    - 10: missing script parameter or using a prohibited combination of parameters
    - 12: error opening output files for writing
    - 21: wrong or missing header in the source code written in IPPcode23
    - 22: unknown or incorrect operation code in the source code written in IPPcode23
    - 23: other lexical or syntactic error of the source code written in IPPcode23\n");
    
    exit(0);
}


/**
 * Function process program arguments
 * @param int $argc number of arguments
 * @param array $args array of arguments
 * @return array of statistic and files names where statistic will write
 */
function processArgs(int $argc, array $args) : array
{
    $STATS_TO_DO = array();

    if ($argc > 1){
        foreach($args as $arg)
        {
            switch ($arg)
            {
                case "parse.php":
                    break;

                case "--help":
                    if ($argc > 2)
                    {
                        fwrite(STDERR, "--help cannot be used with other parameters.\n");
                        exit(10);
                    }
                    printHelp();

                case preg_match("/^(--stats=.+)/", $arg) ? $arg : !$arg:
                    // Add file to STATS_TO_DO
                    if (($EQpos = strpos($arg, "=")) !== false)
                    {
                        $file = substr($arg, ($EQpos + 1), null);

                        // Check for duplicit files
                        foreach ($STATS_TO_DO as $set)
                        {
                            if (!strcmp($file, $set["file"]))
                            {
                                fwrite(STDERR, "ERROR: Attempt to write multiple groups of statistics to the same file !\n");
                                exit(12);
                            }
                        }
                    }
                    array_push($STATS_TO_DO, array("file" => $file, "params" => array()));
                    break;
                
                case "--loc":
                case "--comments":
                case "--labels":
                case "--jumps":
                case "--fwjumps":
                case "--backjumps":
                case "--badjumps":
                case "--frequent":
                case preg_match("/^(--print=)/", $arg) ? $arg : !$arg:
                case "--eol":
                    $STATS_TO_DO = processStatsArgs($STATS_TO_DO, $arg);
                    break;
                
                default:
                    fwrite(STDERR, "ERROR: Unknow program argument !\n");
                    exit(10);
            }
        }
    }
    return $STATS_TO_DO;
}


/**
 * Function splits a line into an array of words. At the same time, the function removes redundant white characters and comments.
 * @param string $line line to split
 * @param ?bool $DO_STATS decide if statistics shoud be calculated
 * @return array|int: array of words (strings) | number 1 (int) if line is empty (EMPTY_LINE) | number 2 (int) if line is header line (HEADER_LINE)
 */
function parseLine(string $line, ?bool $DO_STATS=false) : array | int
{
    // Remove comments
    if (($commentPos = strpos($line, '#')) !== false)
    {
        $line = substr($line, 0, $commentPos);
        if ($DO_STATS) doStatistic("--comments");
    }
    
    // Remove white spaces in the beginning and at the end of line
    $line = trim($line);

    // Check for empty line
    if (strlen($line) === 0)
    {
        return EMPTY_LINE;
    }

    // Check for header
    if (strcasecmp($line, "." . CODE_LANGUAGE) === 0)
    {
        return HEADER_LINE;
    }

    // Replace tabulators (\t) with spaces
    $line = str_replace("\t", " ", $line);

    // Choose individual words from the line
    $word = "";
    $wordArray = [];
    for ($i = 0; $i < strlen($line); $i++)
    {
        if ($line[$i] === " " && $word !== "")
        {
            array_push($wordArray, $word);
            $word = "";
        }
        elseif ($line[$i] !== " ")
        {
            $word = $word . $line[$i];
        }
    }

    // Add last word
    array_push($wordArray, $word);

    // Upper instruction name
    $wordArray[0] = strtoupper($wordArray[0]);

    if ($DO_STATS) doStatistic("--loc");

    // return array of words
    return $wordArray;
}


/**
 * Function reple symbols &, < and > with &amp; &lt; &gt
 * @param string $word word in which symbol will be replaced
 */
function replaceSpecialSymbols(string $word) : string
{
    $word = htmlspecialchars($word, ENT_SUBSTITUTE | ENT_XML1 | ENT_QUOTES, "UTF-8");

    return $word;
}


/**
 * Function return instruction argument type
 * @param string $arg instruction argument
 * @param string $type type of argument
 * @return string type of given argument
 */
function getArgType(string $arg, string $type) : string
{
    switch ($type)
    {
        case VARR:
            return "var";
        case TYPE:
            return "type";
        case LABEL:
            return "label";
        // SYMB
        default:
            if (preg_match(VARR, $arg))
            {
                return "var";
            }
            elseif (preg_match(LABEL, $arg))
            {
                return "label";
            }
            else
            {
                return substr($arg, 0, strpos($arg, '@'));
            }
    }
}


/**
 * Function return instruction argument content
 * @param string $arg instruction argument
 * @param string $type type of argument
 * @return string content of given argument
 */
function getArgContent(string $arg, string $type) : string
{
    if (preg_match(VARR, $arg) || preg_match(LABEL, $arg) || $type === LABEL || $type === TYPE)
    {
        return replaceSpecialSymbols($arg);
    }
    else
    {
        $pos = strpos($arg, "@");
        return replaceSpecialSymbols(substr($arg, $pos + 1, null));
    }
}


/**
 * Function return incstuction argument on index i
 * @param string $instruction
 * @param int $i index of instruction argument
 * @return string regular expresion with name of instruction type
 */
function getInstructionArgType(string $instruction ,int $i) : string
{
    return INSTRUCTIONS[$instruction]["argv"][$i];
}


/**
 * Function check line of code in IPPcode23 and write XML code
 * @param XMLWriter $writer
 * @param array $line line of code
 * @param $order number of instruction
 * @param ?bool $DO_STATS decide if statistics shoud be calculated
 */
function check_and_write(XMLWriter $writer, array $line, int $order, ?bool $DO_STATS=false)
{
    // Check if instruction exist
    if (!array_key_exists($line[0], INSTRUCTIONS)){
        fwrite(STDERR, "ERROR: Unknown or faulty operation code in the source code !\n");
        exit(22);
    }

    // Start new instruction
    $writer->startElement("instruction");
    $writer->writeAttribute("order", "$order");
    $writer->writeAttribute("opcode", "$line[0]");

    //Check number of arguments
    if (INSTRUCTIONS[$line[0]]["argc"] !== (count($line) - 1))
    {
        fwrite(STDERR, "ERROR: Lexical or syntax error in the source code !\n");
        exit(23);
    }

    // Statistics
    if ($DO_STATS)
    {
        doStatistic("--frequent", $line[0]);
        if (!strcmp($line[0], "LABEL")) doStatistic("--labels", $line[1]);
        if (!strcmp($line[0], "RETURN")) doStatistic("--jumps");
        if (!strcmp($line[0], "JUMP") || !strcmp($line[0], "JUMPIFEQ") || !strcmp($line[0], "JUMPIFNEQ") || !strcmp($line[0], "CALL"))
        {
            doStatistic("--jumps");
            doStatistic("--fwjumps", $line[1]);
            doStatistic("--backjumps", $line[1]);
            doStatistic("--badjumps", $line[1]);
        }
    }

    // Process arguments
    for ($i = 1; $i <= INSTRUCTIONS[$line[0]]["argc"]; $i++)
    {
        if (preg_match(getInstructionArgType($line[0], $i-1), $line[$i]))
        {
            // new argument
            $writer->startElement("arg" . $i);
            // write type of argument
            $writer->writeAttribute("type", getArgType($line[$i], getInstructionArgType($line[0], $i-1)));
            // write content of argument
            $writer->writeRaw(getArgContent($line[$i], getInstructionArgType($line[0], $i-1)));
            // close argument
            $writer->endElement();
        }
        else
        {
            fwrite(STDERR, "ERROR: Lexical or syntax error in the source code !\n");
            exit(23);
        }
    }

    // End instruction
    $writer->endElement();

}

// ********** STATS FUNCTIONS ********** //


/**
 * Function add statistic parameter to last statistic set
 * if no statistic set was created then function exit program with error code 10
 * @param array $STATS_TO_DO array with statistci information
 * @param string $param parameter that will be add to statistic
 * @return array modified array $STATS_TO_DO
 */
function processStatsArgs(array $STATS_TO_DO, string $param) : array
{
    if (!empty($STATS_TO_DO))
    {
        array_push($STATS_TO_DO[count($STATS_TO_DO)-1]["params"], $param);
        return $STATS_TO_DO;
    }
    else
    {
        fwrite(STDERR, "ERROR: Statistic parameter before '--stats' !\n");
        exit(10);
    }
}

/**
 * Function performs statistics for all parameters. The statistics are in the global variable $STATS
 * if the function parameter $param is not set, then the function evaluates the statistics for '--badjumps'
 * @param string|bool $param parameter for which the statistics are to be performed
 * @param string|bool $value this parameter is required for statistics: '--fwjumps', '--backjumps', '--badjumps', --labels and '--frequent'
 * with parameters '--fwjumps', '--backjumps', '--badjumps', --labels $value should be name of LABEL
 * with parameter '--frequent' $value should be name of instruction
 */
function doStatistic(string|bool $param=false, string|bool $value=false)
{
    global $STATS;
    // Variable stores the expected label names
    static $expectedLabels = array();
    // Variable stores existing label names
    static $existingLabels = array();

    if ($param)
    {   
        switch ($param)
        {
            case "--labels":
                if ($value)
                {
                    // Check fwjumps
                    if (!in_array($value, $existingLabels)) array_push($existingLabels, $value);
                    if (array_key_exists($value, $expectedLabels))
                    {
                        $STATS["--fwjumps"] += $expectedLabels[$value];
                        unset($expectedLabels[$value]);
                    }
                }
                $STATS[$param]++; 
                break;
            case "--loc":
            case "--comments":
            case "--jumps":
                $STATS[$param]++; 
                break;
            case "--fwjumps":
                if ($value)
                {
                    if (!in_array($value, $existingLabels))
                    {
                        // Increase counter
                        if (array_key_exists($value, $expectedLabels))
                        {
                            $expectedLabels[$value]++;
                        }
                        // Create record
                        else
                        {
                            $expectedLabels[$value] = 1;
                        }
                    }
                }
                break;
            case "--backjumps":
                if ($value)
                {
                    if (in_array($value, $existingLabels)) $STATS["--backjumps"]++;
                }
                break;
            case "--frequent":
                if ($value)
                {
                    // Increase counter
                    if (array_key_exists($value, $STATS[$param]))
                    {
                        $STATS[$param][$value]++;
                    }
                    // Create record
                    else
                    {
                        $STATS[$param][$value] = 1;
                    }
                }
                break;
            default:
                break;
        }
    }
    // Evaluates the statistics for '--badjumps'
    else
    {
        $count = 0;
        foreach ($expectedLabels as $value)
        {
            $count += $value;
        }
        $STATS["--badjumps"] = $count;
    }
}


/**
 * Function write most frequented instruction names to file
 * @param array $instructionFreqStats instructions and the number of their occurrences
 * @param $file stream 
 */
function writeFreqInstruction(array $instructionFreqStats, $file)
{
    // sort by occurrences
    arsort($instructionFreqStats);

    $commaFlag = false;

    $instrKeys = array_keys($instructionFreqStats);
    $maxOc = $instructionFreqStats[$instrKeys[0]];

    foreach ($instrKeys as $instruction)
    {
        if ($instructionFreqStats[$instruction] === $maxOc)
        {
            if ($commaFlag)
            {
                fwrite($file, ",");
            }
            $commaFlag = true;
            fwrite($file, $instruction);
        }
        else
        {
            break;
        }
    }
}


/**
 * Function write statistic to file/s
 * @param array $STATS_TO_DO information about statistic
 */
function writeStats(array $STATS_TO_DO)
{
    global $STATS;
    // Evaluates the statistics
    doStatistic();

    foreach ($STATS_TO_DO as $set)
    {
        // Open file
        $file = fopen($set["file"], "w");
        if (!$file) {
            fwrite(STDERR, "ERROR: Couldn't open file file for statistics !");
            exit(12);
        }

        foreach($set["params"] as $param)
        {
            switch ($param)
            {
                case "--loc":
                    fwrite($file, $STATS["--loc"]);
                    break;
                case "--comments":
                    fwrite($file, $STATS["--comments"]);
                    break;
                case "--labels":
                    fwrite($file, $STATS["--labels"]);
                    break;
                case "--jumps":
                    fwrite($file, $STATS["--jumps"]);
                    break;
                case "--fwjumps":
                    fwrite($file, $STATS["--fwjumps"]);
                    break;
                case "--backjumps":
                    fwrite($file, $STATS["--backjumps"]);
                    break;
                case "--badjumps":
                    fwrite($file, $STATS["--badjumps"]);
                    break;
                case "--frequent":
                    writeFreqInstruction($STATS["--frequent"], $file);
                    break;                        
                case preg_match("/^(--print=)/", $param) ? $param : !$param:
                    fwrite($file, substr($param, (strpos($param, "=") + 1)), null);
                    break;
                case "--eol":
                    break;
            }
            fwrite($file, "\n");
        }

        if (!fclose($file)) {
            fwrite(STDERR, "ERROR: closing statstic file !");
            exit(12);
        }
    }
}

 // *********************************** SCRIPT ******************************** //


// Argument processing
$STATS_TO_DO = processArgs($argc, $argv);

// Check if do statistics
$DO_STATS = !empty($STATS_TO_DO);

// Stores the values ​​of whether the code header was specified
$HEADER = false;

// Initialize instruction counter
$INSTRUCTION_COUNT = 1;

// Generate XML
$xml = new XMLWriter();
$xml->openUri("php://output");
$xml->startDocument("1.0", "UTF-8");
$xml->setIndent(true);


while($line = fgets(STDIN))
{

    // process the string (from now is line separated to words that are stored in array)
    $line = parseLine($line, $DO_STATS);

    // Check if header is included
    if (!$HEADER && $line === HEADER_LINE)
    {
        $xml->startElement("program");
        $xml->writeAttribute("language", CODE_LANGUAGE);
        $HEADER = true; 
    }
    // Normall line
    elseif ($HEADER && $line !== EMPTY_LINE && $line !== HEADER_LINE)
    {
        check_and_write($xml ,$line, $INSTRUCTION_COUNT++, $DO_STATS);
    }
    // Empty line
    elseif ($line === EMPTY_LINE)
    {
        continue;
    }
    // Double header
    elseif ($HEADER && $line === HEADER_LINE)
    {
        fwrite(STDERR, "ERROR: Double header !\n");
        exit(22);
    }
    else
    {
        fwrite(STDERR, "ERROR: Missing or incorrect program header !\n");
        exit(21);
    }
}

// END program
$xml->endElement();

// Write Statistic
if ($DO_STATS) writeStats($STATS_TO_DO);

?>