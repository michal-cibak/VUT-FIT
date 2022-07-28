<?php

# ------------------------------ #
# Handling of scripts arguments. #
# ------------------------------ #

if ($argc != 1)
{
    if ($argc == 2)
    {
        if (strcmp($argv[1], "--help") == 0)
        {
            echo "Skript typu filter v jazyku PHP 7.4, ktorý načíta zo štandardného vstupu zdrojový kód v jazyku IPPcode20,\n",
                 "skontroluje jeho lexikálnu a syntaktickú správnosť a na štandardný výstup vypíše jeho XML reprezentáciu.\n",
                 "\n",
                 "Podporované argumenty skriptu:\n",
                 "  --help - Nemožno kombinovať s iným argumentom.\n";
            exit(0);
        }
    }

    fwrite(STDERR, "Wrong program arguments. Use \"--help\" only to show help.\n");
    exit(10); # TODO - a different exit code may be requiered #
}

# ------------------------------------- #
# Searching for and checking of header. #
# ------------------------------------- #

do
// skips empty lines, spaces, tabs and/or comments until other non empty line is reached, which should be header:
// header must contain dot followed by a proper language name and can contain spaces, tabs or comment, anything more leads to an error
{
    $line = fgets(STDIN);
    if ($line === FALSE) // error or EOF
    {
        if (feof(STDIN))
        {
            fwrite(STDERR, "End of file reached before a valid language name could be found.\n");
            exit(21);
        }
        else
        {
            fwrite(STDERR, "Error in fgets() function.\n");
            exit(99);
        }
    }
    $line = preg_replace('/#.*/', '', $line); // comment trimming
    $line = trim($line, " \t\n\r"); // whitespace trimming from beggining and end of string
    if (strlen($line) == 0) // empty line - skip
        continue;
    elseif (strcasecmp($line, ".IPPcode20") == 0) // proper header - continue with code processing
        break;
    else // invalid header
    {
        fwrite(STDERR, "Invalid header.\n");
        exit(21);
    }
}
while (TRUE); // loops until proper header is encountered, exits if unexpected content or EOF is reached

echo "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
echo "<program language=\"IPPcode20\">\n";

# ------------------------- #
# Definitions of functions. #
# ------------------------- #

function err_regexpr()
// outputs error message and exits the script
{
    fwrite(STDERR, "Error occurred while finding match with regular expression.\n");
    exit(99);
}

function err_operandcnt($opcode)
// outputs error message and exits the script
{
    fwrite(STDERR, "Wrong number of operands of instruction $opcode\n");
    exit(23);
}

function err_wrongoperand($opcode)
// outputs error message and exits the script
{
    fwrite(STDERR, "Wrong operand of instruction $opcode\n");
    exit(23);
}

function checkvar_xmlout($string, $order)
// checks if $string can be a variable, and if so, outputs corresponding XML code and returns true, if it can't be, false is returned
// will exit() if error occurs in preg_match()
{
    $match = preg_match('/\A[GLT]F@[[:alpha:]_\-\$&%\*!\?][[:alnum:]_\-\$&%\*!\?]*\z/', $string);
    if ($match === false)
        err_regexpr(); // exit(99) with message
    if (!$match)
        return false;

    $string = preg_replace('/&/', '&amp;', $string); // replace & in variable name, which is a problematic character for XML
    echo "<arg$order type=\"var\">$string</arg$order>\n";
    return true;
}

function checkconst_xmlout($string, $order)
// checks if $string can be a constant, and if so, outputs corresponding XML code and returns true, if it can't be, false is returned
// will exit() if error occurs in preg_match()
{
    if (strcmp($string, "nil@nil") != 0 && strcmp($string, "bool@true") != 0 && strcmp($string, "bool@false") != 0) // it isn't nil nor bool
    {
        $match = preg_match('/\Aint@[+\-]?[[:digit:]]+\z/', $string);
        if ($match === false)
            err_regexpr(); // exit(99) with message
        elseif (!$match) // it isn't int
        {
            $match = preg_match('/\Astring@(?:[^\s#\\\\]|(?:\\\\[0-9]{3}))*\z/', $string);
            if ($match === false)
                err_regexpr(); // exit(99) with message
            elseif (!$match) // it isn't string
                return false;
        }
    }

    $arr = preg_split('/@/', $string, 2); // type is in $arr[0] and value is in $arr[1]
    if (strcmp($arr[0], "string") == 0)
        // replace characters problematic for XML ("<" and "&" are necessary, ">", """ and "'" are not)
        $arr[1] = preg_replace(['/&/', '/</', '/>/', '/"/', '/\'/'], ['&amp;', '&lt;', '&gt;', '&quot;', '&apos;'], $arr[1]);
    echo "<arg$order type=\"$arr[0]\">$arr[1]</arg$order>\n";
    return true;
}

function checksymb_xmlout($string, $order)
// checks if $string can be a symbol (variable or constant), and if so, outputs corresponding XML code and returns true, if it can't be, false is returned
// will exit() if error occurs in preg_match() called in checkvar_xmlout() or checkconst_xmlout()
{
    if (!checkvar_xmlout($string, $order))
        if (!checkconst_xmlout($string, $order))
            return false;

    return true;
}

function checklabel_xmlout($string, $order)
// checks if $string can be a label, and if so, outputs corresponding XML code and returns true, if it can't be, false is returned
// will exit() if error occurs in preg_match()
{
    $match = preg_match('/\A[[:alpha:]_\-\$&%\*!\?][[:alnum:]_\-\$&%\*!\?]*\z/', $string);
    if ($match === false)
        err_regexpr(); // exit(99) with message
    if (!$match)
        return false;

    $string = preg_replace('/&/', '&amp;', $string); // replace & in label name, which is a problematic character for XML
    echo "<arg$order type=\"label\">$string</arg$order>\n";
    return true;
}

function checktype_xmlout($string, $order)
// checks if $string can be a type, and if so, outputs corresponding XML code and returns true, if it can't be, false is returned
{
    if (strcmp($string, "int") != 0 && strcmp($string, "string") != 0 && strcmp($string, "bool") != 0)
        return false;

    echo "<arg$order type=\"type\">$string</arg$order>\n";
    return true;
}

# ---------------- #
# Code processing. #
# ---------------- #

$instructions = array(
    "MOVE", "CREATEFRAME", "PUSHFRAME", "POPFRAME", "DEFVAR", "CALL", "RETURN", // frames and function calls
    "PUSHS", "POPS", // data stack
    "ADD", "SUB", "MUL", "IDIV", "LT", "GT", "EQ", "AND", "OR", "NOT", "INT2CHAR", "STRI2INT", // arithmetic, relation, boolean and conversion instructions
    "READ", "WRITE", // input, output
    "CONCAT", "STRLEN", "GETCHAR", "SETCHAR", // strings
    "TYPE", // types
    "LABEL", "JUMP", "JUMPIFEQ", "JUMPIFNEQ", "EXIT", // program flow
    "DPRINT", "BREAK" // debug
    );
$order = 0;

while (($line = fgets(STDIN)) !== FALSE) // loops until EOF is reached or error occurs in fgets() call (the cause is checked after the cycle body)
{
    $line = preg_replace('/#.*/', '', $line); // comment trimming
    $line = trim($line, " \t\n\r"); // intitial and final whitespace trimming
    if (strlen($line) == 0) // line with no code - skip
        continue;
    $parts = preg_split('/[ \t]+/', $line); // splits the line into array of strings, using spaces and/or tabs as the divider

    $opcode = strtoupper($parts[0]); // needed for XML output, also good for easier search in array
    if (!in_array($opcode, $instructions)) // checks if such instruction exists
    {
        fwrite(STDERR, "Unknown opcode.\n");
        exit(22);
    }
    $order++;
    echo "<instruction order=\"$order\" opcode=\"$opcode\">\n"; // opcode must be in uppercase

    switch ($opcode) // if opcode is a valid instruction name, the count of operands is checked, as well as the types of operands
    {
        // 3 operands, var symb symb
        case "ADD":
        case "SUB":
        case "MUL":
        case "IDIV":
        case "LT":
        case "GT":
        case "EQ":
        case "AND":
        case "OR":
        case "NOT":
        case "STRI2INT":
        case "CONCAT":
        case "GETCHAR":
        case "SETCHAR":

            if (count($parts) != 4)
                err_operandcnt($opcode); // exit(23) with message

            if (!checkvar_xmlout($parts[1], 1) || !checksymb_xmlout($parts[2], 2) || !checksymb_xmlout($parts[3], 3))
                err_wrongoperand($opcode); // exit(23) with message

            break;

        // 3 operands, label symb symb
        case "JUMPIFEQ":
        case "JUMPIFNEQ":

            if (count($parts) != 4)
                err_operandcnt($opcode); // exit(23) with message

            if (!checklabel_xmlout($parts[1], 1) || !checksymb_xmlout($parts[2], 2) || !checksymb_xmlout($parts[3], 3))
                err_wrongoperand($opcode); // exit(23) with message

            break;

        // 2 operands, var symb
        case "MOVE":
        case "INT2CHAR":
        case "STRLEN":
        case "TYPE":

            if (count($parts) != 3)
                err_operandcnt($opcode); // exit(23) with message

            if (!checkvar_xmlout($parts[1], 1) || !checksymb_xmlout($parts[2], 2))
                err_wrongoperand($opcode); // exit(23) with message

            break;

        // 2 operands, var type
        case "READ":

            if (count($parts) != 3)
                err_operandcnt($opcode); // exit(23) with message

            if (!checkvar_xmlout($parts[1], 1) || !checktype_xmlout($parts[2], 2))
                err_wrongoperand($opcode); // exit(23) with message

            break;

        // 1 operand, var
        case "DEFVAR":
        case "POPS":

            if (count($parts) != 2)
                err_operandcnt($opcode); // exit(23) with message

            if (!checkvar_xmlout($parts[1], 1))
                err_wrongoperand($opcode); // exit(23) with message

            break;

        // 1 operand, label
        case "CALL":
        case "LABEL":
        case "JUMP":

            if (count($parts) != 2)
                err_operandcnt($opcode); // exit(23) with message

            if (!checklabel_xmlout($parts[1], 1))
                err_wrongoperand($opcode); // exit(23) with message

            break;

        // 1 operand, symb
        case "PUSHS":
        case "WRITE":
        case "EXIT":
        case "DPRINT":

            if (count($parts) != 2)
                err_operandcnt($opcode); // exit(23) with message

            if (!checksymb_xmlout($parts[1], 1))
                err_wrongoperand($opcode); // exit(23) with message

            break;

        // no operands
        //case "CREATEFRAME":
        //case "PUSHFRAME":
        //case "POPFRAME":
        //case "RETURN":
        //case "BREAK":
        default:

            if (count($parts) != 1)
                err_operandcnt($opcode); // exit(23) with message

            break;
    }

    echo "</instruction>\n";
}
if (!feof(STDIN))
{
    fwrite(STDERR, "Error in fgets() function.\n");
    exit(99);
}

echo "</program>\n";

exit(0);

?>
