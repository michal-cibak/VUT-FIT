# Project: IPP - Interpret XML reprezentace kódu
# Author: Michal Cibák - xcibak00

import sys
import xml.etree.ElementTree as ET
import re

# ------------------------------ #
# Handling of scripts arguments. #
# ------------------------------ #

code_source = 0
code_input = 0

for i in range(1, len(sys.argv)): # // for each program argument, chech if it is a possible argument and whether it doesn't cause a forbidden combination
    if sys.argv[i] == '--help':
        if len(sys.argv) == 2:
            print(
'''Program v jazyku Python 3.8.
Načíta XML reprezentáciu programu písaného v jazyku IPPcode20 a tento program s využitím vstupu podľa parametrov príkazového riadku interpretuje a generuje výstup.

Podporované argumenty skriptu:
  --help - Vypíše túto nápovedu. Nemožno ho kombinovať s inými argumentami.

  "<file>" pre nasledujúce argumenty predstavuje súbor či cestu. Môže byť zadaný/á relatívnou cestou bez zástupného symbolu vlnka (~) alebo absolútnou cestou, obe bez znaku úvodzoviek (") a rovnosti (=), úvodzovkami však môže byť ohraničený/á.
  --source=<file> - Vstupný súbor s XML reprezentáciou zdrojového kódu. Argument je povinný, ak nie je zadaný "--input=<file>", pri jeho absencii sa číta zo štandardného vstupu.
  --input=<file> - Súbor so vstupmi pre samotnú interpretáciu zadaného zdrojového kódu. Argument je povinný, ak nie je zadaný "--source=<file>", pri jeho absencii sa číta zo štandardného vstupu.
''')
            sys.exit(0)
        else:
            sys.stderr.write('# Argument "--help" combined with another argument. Use "--help" as the only argument to show help.\n')
            sys.exit(10)
    elif sys.argv[i].startswith('--source='):
        if code_source == 0:
            code_source = i
        else:
            sys.stderr.write('# Argument "--source" used more than once.\n')
            sys.exit(10)
    elif sys.argv[i].startswith('--input='):
        if code_input == 0:
            code_input = i
        else:
            sys.stderr.write('# Argument "--input" used more than once.\n')
            sys.exit(10)
    else:
        sys.stderr.write('# Unknown or wrong program argument: "' + sys.argv[i] + '"\n')
        sys.exit(10)
else:
    if code_source == 0 and code_input == 0:
        sys.stderr.write('# Missing one or more program arguments. Use "--help" for help.\n')
        sys.exit(10)

if code_source != 0:
    code_source = sys.argv[code_source][9:len(sys.argv[code_source])].replace('"', '') # // single quotation mark (only before/after file) won't cause an error
    if len(code_source) == 0:
        sys.stderr.write('# No value specified for "--source" argument.\n')
        sys.exit(10)

if code_input != 0:
    code_input = sys.argv[code_input][8:len(sys.argv[code_input])].replace('"', '') # // single quotation mark (only before/after file) won't cause an error
    if len(code_input) == 0:
        sys.stderr.write('# No value specified for "--input" argument.\n')
        sys.exit(10)

# ------------------------- #
# Definitions of functions. #
# ------------------------- #

def err_wrclex(message, file, code):
# // WRites "message" to stderr, CLoses "file" if necessary and EXists the program with value of "code"
# // message is string, file is file descriptor, code is integer
    sys.stderr.write(message)
    if file is not sys.stdin:
        file.close()
    sys.exit(code)

def typecheck(type, value):
# // exits the program with error message if "value" is not of type "type"
# // type and value are strings gotten from XML arg element, type is value of argument type (checked if it is known type), values is text from the element
    if type == 'nil':
        if value == 'nil':
            return
    elif type == 'bool':
        if value == 'true' or value == 'false':
            return
    elif type == 'int':
        try:
            int(value)
        except ValueError:
            pass
        else:
            return
    elif type == 'string':
        if re.search("\A(?:[^\s#\\\\]|(?:\\\\[0-9]{3}))*\Z", value) != None:
            return
    elif type == 'var':
        if re.search("\A[GLT]F@[a-zA-Z_\-\$&%\*!\?][a-zA-Z0-9_\-\$&%\*!\?]*\Z", value) != None:
            return
    elif type == 'label':
        if re.search("\A[a-zA-Z_\-\$&%\*!\?][a-zA-Z0-9_\-\$&%\*!\?]*\Z", value) != None:
            return
    elif type == 'type':
        if value == 'int' or value == 'string' or value == 'bool':
            return
    else: # // this really shouldn't happen, as only valid types should be passed to this function - it is checked before first call of this function
        err_wrclex('! Error: Unexpected value of argument "type" in "typecheck()" function!', code_source, 32)

    # // if none of the type tests passed, no return was called and thus this code will be executed
    err_wrclex('# XML - value of instruction operand is not of specified type.', code_source, 32) # // exit program with proper message

# ---------- #
# Main code. #
# ---------- #

# // open file with source code if there is one
if code_source == 0:
    code_source = sys.stdin
else: # // file with source code was specified
    try:
        code_source = open(code_source, encoding="utf-8") # // encoding specified in case autodetection returns something different (decoding exceptions may be raised accordingly when reading from the file)
    except OSError:
        sys.stderr.write('# Failed to open file with source code.\n')
        sys.exit(11)

# // open file with input for source code if there is one
if code_input == 0:
    code_input = sys.stdin
else: # // file with input for source code was specified
    try:
        code_input = open(code_input, encoding="utf-8") # // encoding specified in case autodetection returns something different (decoding exceptions may be raised accordingly when reading from the file)
    except OSError:
        err_wrclex('# Failed to open file with input for source code.\n', code_source, 11)

# // read source code from file and close it afterward
try:
    code_source_string = code_source.read()
except OSError:
    err_wrclex('# Failed to read from file with source code.\n', code_input, 11)
except ValueError:
    err_wrclex('# Text in file with source code could not be decoded as UTF-8.\n', code_input, 11)
finally:
    if code_source is not sys.stdin:
        code_source.close()

# // convert read source code to XML tree
try:
    root = ET.fromstring(code_source_string)
except ET.ParseError: # // not only "well-formed" errors (explicitly stated by parser in its error message), but also any other error in XML file
    err_wrclex('# Wrong format of XML file with source code.\n', code_input, 31)

# // XML tree check (whether or not valid tags are used in correct places with correct and proper values (lexical and syntactic verification))
if root.tag != 'program': # // check name of root element
    err_wrclex('# XML - unknown root element.\n', code_input, 32)
else:
    for attkey in root.attrib: # // for each attribute in root element "program" check name and value
        if attkey == 'language':
            if root.attrib[attkey] != 'IPPcode20':
                err_wrclex('# XML - unknown language of program.\n', code_input, 32)
        elif attkey == 'name' or attkey == 'description':
            if type(root.attrib[attkey] != str): # it is probably always string, but the assignment said it must be text, so... just making sure
                err_wrclex('# XML - value of attribute in program element is not a string.\n', code_input, 32)
        else: # // other attribute
            err_wrclex('# XML - unknown attribute of program element.\n', code_input, 32)
    if 'language' not in root.attrib: # // mandatory attribute
        err_wrclex('# XML - missing language attribute in program element.\n', code_input, 32)
instructions = (
    'MOVE', 'CREATEFRAME', 'PUSHFRAME', 'POPFRAME', 'DEFVAR', 'CALL', 'RETURN', # // frames and function calls
    'PUSHS', 'POPS', # // data stack
    'ADD', 'SUB', 'MUL', 'IDIV', 'LT', 'GT', 'EQ', 'AND', 'OR', 'NOT', 'INT2CHAR', 'STRI2INT', # // arithmetic, relation, boolean and conversion instructions
    'READ', 'WRITE', # // input, output
    'CONCAT', 'STRLEN', 'GETCHAR', 'SETCHAR', # // strings
    'TYPE', # // types
    'LABEL', 'JUMP', 'JUMPIFEQ', 'JUMPIFNEQ', 'EXIT', # // program flow
    'DPRINT', 'BREAK' # // debug
    )
order_set = set(())
argtypes = ('nil', 'bool', 'int', 'string', 'var', 'label', 'type') # // nil, bool, int, string -> const; const, var -> symb
instr_w_vss = ('ADD', 'SUB', 'MUL', 'IDIV', 'LT', 'GT', 'EQ', 'AND', 'OR', 'STRI2INT', 'CONCAT', 'GETCHAR', 'SETCHAR') # // 3 operands, var symb symb
instr_w_lss = ('JUMPIFEQ', 'JUMPIFNEQ') # // 3 operands, label symb symb
instr_w_vs = ('MOVE', 'NOT', 'INT2CHAR', 'STRLEN', 'TYPE') # // 2 operands, var symb
instr_w_vt = ('READ',) # // 2 operands, var type # comma needed so python knows it is a tuple
instr_w_v = ('DEFVAR', 'POPS') # // 1 operand, var
instr_w_l = ('CALL', 'LABEL', 'JUMP') # // 1 operand, label
instr_w_s = ('PUSHS', 'WRITE', 'EXIT', 'DPRINT') # // 1 operand, symb
# instr_wo = ('CREATEFRAME', 'PUSHFRAME', 'POPFRAME', 'RETURN', 'BREAK'] # // no operands - no need for them as they are the only instructions left
maxorder = 0
for child in root: # // for each first level child element check name and attributes (name and value), do the same with its subelements
    if child.tag == 'instruction': # // the only valid element name
        for chattkey in child.attrib: # // for each instruction attribute check name and value
            if chattkey == 'order':
                try:
                    order = int(child.attrib['order'])
                except ValueError:
                    err_wrclex('# XML - value of instruction order is not a whole number.\n', code_input, 32)
                if order > 0:
                    if order not in order_set: # // first use of the order number
                        order_set.add(order)
                        if order > maxorder:
                            maxorder = order
                    else: # // order number already used
                        err_wrclex('# XML - duplicate value ' + str(order) + ' of instruction order.\n', code_input, 32)
                else: # // non-positive value of order
                    err_wrclex('# XML - value of instruction order is not a positive number.\n', code_input, 32)
            elif chattkey == 'opcode':
                if child.attrib['opcode'] in instructions: # // valid instruction name
                    pass # // jumps behind the outmost if/else statement where subelements (operands of instruction) are checked
                else: # // invalid instruction
                    err_wrclex('# XML - unknown opcode of instruction.\n', code_input, 32)
            else: # // other attribute
                err_wrclex('# XML - unknown attribute of instruction element.\n', code_input, 32)
        if 'order' not in child.attrib: # // mandatory attribute
            err_wrclex('# XML - missing order attribute in instruction element.\n', code_input, 32)
        if 'opcode' not in child.attrib: # // mandatory attribute
            err_wrclex('# XML - missing opcode attribute in instruction element.\n', code_input, 32)
    else: # // element isn't an instruction
        err_wrclex('# XML - unknown 1-st level child element.\n', code_input, 32)
    args = [False, False, False]
    arguments = [None, None, None]
    for baby in child: # // for each second level child element (subelement of instruction element) check name and attributes (name and value)
        if baby.tag == 'arg1':
            if not args[0]: # // first occurrence of arg1
                args[0] = True
            else:
                err_wrclex('# XML - duplicate element arg1.\n', code_input, 32)
        elif baby.tag == 'arg2':
            if not args[1]: # // first occurrence of arg2
                args[1] = True
            else:
                err_wrclex('# XML - duplicate element arg2.\n', code_input, 32)
        elif baby.tag == 'arg3':
            if not args[2]: # // first occurrence of arg3
                args[2] = True
            else:
                err_wrclex('# XML - duplicate element arg3.\n', code_input, 32)
        else: # // other attribute
            err_wrclex('# XML - unknown 2-nd level child element.\n', code_input, 32)
        if len(baby) != 0:
            err_wrclex('# XML - unexpected 3-rd level child element.\n', code_input, 32)
        for battkey in baby.attrib: # // for each attribute of arg element check name and value
            if battkey == 'type':
                if baby.attrib['type'] in argtypes:
                    pass
                else:
                    err_wrclex('# XML - unknown type of instrucion operand.\n', code_input, 32)
            else:
                err_wrclex('# XML - unknown attribute in arg element.\n', code_input, 32)
        if 'type' not in baby.attrib: # // mandatory attribute
            err_wrclex('# XML - missing type attribute in arg element.\n', code_input, 32)
    for i in range(0, len(child)): # // check if there isn't missing arg element (in sequence), load the elements into an array
        if args[i]:
            arguments[i] = child.find('arg' + str(i+1))
        else:
            for j in range(i+1, 3):
                if args[j]:
                    err_wrclex('# XML - arg' + str(j+1) + ' is present, but preceding is missing.\n', code_input, 32)
    # // check if the instruction has the right number of operands and if the operands are of proper type
    if child.attrib['opcode'] in instr_w_vss:
        if len(child) == 3:
            if arguments[0].attrib['type'] == 'var' and arguments[1].attrib['type'] in argtypes[:5] and arguments[2].attrib['type'] in argtypes[:5]:
                for i in range(3):
                    typecheck(arguments[i].attrib['type'], str(arguments[i].text or '')) # // if the text attribute is None, str(text or '') will return empty string rather than convert it to text
            else:
                err_wrclex('# XML - unexpected type of instruction operand.\n', code_input, 32)
        else:
            err_wrclex('# XML - instruction element does not have the right number of subelements.\n', code_input, 32)
    elif child.attrib['opcode'] in instr_w_lss:
        if len(child) == 3:
            if arguments[0].attrib['type'] == 'label' and arguments[1].attrib['type'] in argtypes[:5] and arguments[2].attrib['type'] in argtypes[:5]:
                for i in range(3):
                    typecheck(arguments[i].attrib['type'], str(arguments[i].text or '')) # // if the text attribute is None, str(text or '') will return empty string rather than convert it to text
            else:
                err_wrclex('# XML - unexpected type of instruction operand.\n', code_input, 32)
        else:
            err_wrclex('# XML - instruction element does not have the right number of subelements.\n', code_input, 32)
    elif child.attrib['opcode'] in instr_w_vs:
        if len(child) == 2:
            if arguments[0].attrib['type'] == 'var' and arguments[1].attrib['type'] in argtypes[:5]:
                for i in range(2):
                    typecheck(arguments[i].attrib['type'], str(arguments[i].text or '')) # // if the text attribute is None, str(text or '') will return empty string rather than convert it to text
            else:
                err_wrclex('# XML - unexpected type of instruction operand.\n', code_input, 32)
        else:
            err_wrclex('# XML - instruction element does not have the right number of subelements.\n', code_input, 32)
    elif child.attrib['opcode'] in instr_w_vt:
        if len(child) == 2:
            if arguments[0].attrib['type'] == 'var' and arguments[1].attrib['type'] == 'type':
                for i in range(2):
                    typecheck(arguments[i].attrib['type'], str(arguments[i].text or '')) # // if the text attribute is None, str(text or '') will return empty string rather than convert it to text
            else:
                err_wrclex('# XML - unexpected type of instruction operand.\n', code_input, 32)
        else:
            err_wrclex('# XML - instruction element does not have the right number of subelements.\n', code_input, 32)
    elif child.attrib['opcode'] in instr_w_v:
        if len(child) == 1:
            if arguments[0].attrib['type'] == 'var':
                typecheck('var', str(arguments[0].text or '')) # // if the text attribute is None, str(text or '') will return empty string rather than convert it to text
            else:
                err_wrclex('# XML - unexpected type of instruction operand.\n', code_input, 32)
        else:
            err_wrclex('# XML - instruction element does not have the right number of subelements.\n', code_input, 32)
    elif child.attrib['opcode'] in instr_w_l:
        if len(child) == 1:
            if arguments[0].attrib['type'] == 'label':
                typecheck('label', str(arguments[0].text or '')) # // if the text attribute is None, str(text or '') will return empty string rather than convert it to text
            else:
                err_wrclex('# XML - unexpected type of instruction operand.\n', code_input, 32)
        else:
            err_wrclex('# XML - instruction element does not have the right number of subelements.\n', code_input, 32)
    elif child.attrib['opcode'] in instr_w_s:
        if len(child) == 1:
            if arguments[0].attrib['type'] in argtypes[:5]:
                typecheck(arguments[i].attrib['type'], str(arguments[0].text or '')) # // if the text attribute is None, str(text or '') will return empty string rather than convert it to text
            else:
                err_wrclex('# XML - unexpected type of instruction operand.\n', code_input, 32)
        else:
            err_wrclex('# XML - instruction element does not have the right number of subelements.\n', code_input, 32)
    else: # elif child.attrib['opcode'] in instr_wo: # // no need for if as this is the only category left (and it must be instruction at this point)
        if len(child) == 0:
            pass
        else:
            err_wrclex('# XML - instruction element does not have the right number of subelements.\n', code_input, 32)
    arguments.clear() # // so there are no references left, just cleaning up

# // semantic verification of labels
labels = dict()
order = 1
while order <= maxorder:
    if order in order_set:
        # // for each instruction do
        instruction = root.find("./instruction[@order='" + str(order) + "']")
        if instruction.attrib['opcode'] == 'LABEL':
            arg1 = instruction.find('arg1')
            if arg1.text not in labels:
                labels[arg1.text] = order
            else:
                err_wrclex('# Semantics - label redefinition.', code_input, 52)
    order += 1 # MUST BE HERE
order = 1
while order <= maxorder:
    if order in order_set:
        # // for each instruction do
        instruction = root.find("./instruction[@order='" + str(order) + "']")
        if instruction.attrib['opcode'] == 'JUMP' or instruction.attrib['opcode'] == 'JUMPIFEQ' or instruction.attrib['opcode'] == 'JUMPIFNEQ':
            arg1 = instruction.find('arg1')
            if arg1.text in labels:
                pass
            else:
                err_wrclex('# Semantics - label for jump destination does not exist.', code_input, 52)
    order += 1 # MUST BE HERE

# // interpretation; at this point, lexical and syntactic verification is done (type check of constants isn't - it is done as semantic verification here)
global_frame = dict()
order = 1
while order <= maxorder:
    if order in order_set:
        # // for each instruction do

        instruction = root.find("./instruction[@order='" + str(order) + "']") # // only one instruction with given order should exist at this point (check has been done)
        # sys.stderr.write('? Interpreting instruction ' + str(instruction.attrib) + '.\n') # DEBUG
        # sys.stderr.write('? Global frame - ' + str(global_frame) + '.\n') # DEBUG

        if instruction.attrib['opcode'] == 'MOVE':
            arg1 = instruction.find('arg1')
            arg2 = instruction.find('arg2')
            if arg1.text[0] == 'G':
                if arg1.text[3:len(arg1.text)] in global_frame:
                    if arg2.attrib['type'] == 'var':
                        if arg2.text[0] == 'G':
                            if arg2.text[3:len(arg2.text)] in global_frame:
                                if global_frame[arg2.text[3:len(arg2.text)]]['type'] != None:
                                    global_frame[arg1.text[3:len(arg1.text)]]['type'] = global_frame[arg2.text[3:len(arg2.text)]]['type']
                                    global_frame[arg1.text[3:len(arg1.text)]]['value'] = global_frame[arg2.text[3:len(arg2.text)]]['value']
                                else:
                                    err_wrclex('# Interpretation - MOVE - source variable is not initialized.', code_input, 56)
                            else:
                                err_wrclex('# Interpretation - MOVE - source variable does not exist.', code_input, 54)
                        elif arg2.text[0] == 'L':
                            pass # TODO - local frame
                        else: # // arg2.text[0] == 'T'
                            pass # TODO - temporary frame
                    else:
                        global_frame[arg1.text[3:len(arg1.text)]]['type'] = arg2.attrib['type']
                        if arg2.attrib['type'] == 'int':
                            global_frame[arg1.text[3:len(arg1.text)]]['value'] = int(arg2.text)
                        else: # // nil, bool, string
                            global_frame[arg1.text[3:len(arg1.text)]]['value'] = str(arg2.text or '')
                else:
                    err_wrclex('# Interpretation - MOVE - destination variable does not exist.', code_input, 54)
            elif arg1.text[0] == 'L':
                pass # TODO - local frame
            else: # // arg1.text[0] == 'T'
                pass # TODO - temporary frame

        elif instruction.attrib['opcode'] == 'CREATEFRAME':
            pass
        elif instruction.attrib['opcode'] == 'PUSHFRAME':
            pass
        elif instruction.attrib['opcode'] == 'POPFRAME':
            pass

        elif instruction.attrib['opcode'] == 'DEFVAR':
            arg1 = instruction.find('arg1')
            if arg1.text[0] == 'G':
                if arg1.text[3:len(arg1.text)] not in global_frame:
                    global_frame[arg1.text[3:len(arg1.text)]] = {'type' : None, 'value' : None}
                else:
                    err_wrclex('# Interpretation - DEFVAR - global variable redefinition.', code_input, 52)
            elif arg1.text[0] == 'L':
                pass # TODO - local frame
            else: # // arg1.text[0] == 'T'
                pass # TODO - temporary frame

        elif instruction.attrib['opcode'] == 'CALL':
            pass
        elif instruction.attrib['opcode'] == 'RETURN':
            pass
        elif instruction.attrib['opcode'] == 'PUSHS':
            pass
        elif instruction.attrib['opcode'] == 'POPS':
            pass

        elif instruction.attrib['opcode'] == 'ADD' or instruction.attrib['opcode'] == 'SUB' or instruction.attrib['opcode'] == 'MUL' or instruction.attrib['opcode'] == 'IDIV':
            arg1 = instruction.find('arg1')
            arg2 = instruction.find('arg2')
            arg3 = instruction.find('arg3')
            if arg1.text[0] == 'G':
                if arg1.text[3:len(arg1.text)] in global_frame:
                    pass
                else:
                    err_wrclex('# Interpretation - ' + instruction.attrib['opcode'] + ' - destination variable does not exist.', code_input, 54)
            elif arg1.text[0] == 'L':
                pass # TODO - local frame
            else: # // arg1.text[0] == 'T'
                pass # TODO - temporary frame
            if arg2.attrib['type'] == 'var':
                if arg2.text[0] == 'G':
                    if arg2.text[3:len(arg2.text)] in global_frame:
                        if global_frame[arg2.text[3:len(arg2.text)]]['type'] != None:
                            if global_frame[arg2.text[3:len(arg2.text)]]['type'] == 'int':
                                value1 = global_frame[arg2.text[3:len(arg2.text)]]['value']
                            else:
                                err_wrclex('# Interpretation - ' + instruction.attrib['opcode'] + ' - wrong type of source variable.', code_input, 53)
                        else:
                            err_wrclex('# Interpretation - ' + instruction.attrib['opcode'] + ' - source variable is not initialized.', code_input, 56)
                    else:
                        err_wrclex('# Interpretation - ' + instruction.attrib['opcode'] + ' - source variable does not exist.', code_input, 54)
                elif arg3.text[0] == 'L':
                    pass # TODO - local frame
                else: # // arg3.text[0] == 'T'
                    pass # TODO - temporary frame
            elif arg2.attrib['type'] == 'int':
                value1 = int(arg2.text)
            else:
                err_wrclex('# Interpretation - ' + instruction.attrib['opcode'] + ' - wrong operand types.', code_input, 53)
            if arg3.attrib['type'] == 'var':
                if arg3.text[0] == 'G':
                    if arg3.text[3:len(arg3.text)] in global_frame:
                        if global_frame[arg3.text[3:len(arg3.text)]]['type'] != None:
                            if global_frame[arg3.text[3:len(arg3.text)]]['type'] == 'int':
                                value2 = global_frame[arg3.text[3:len(arg3.text)]]['value']
                            else:
                                err_wrclex('# Interpretation - ' + instruction.attrib['opcode'] + ' - wrong type of source variable.', code_input, 53)
                        else:
                            err_wrclex('# Interpretation - ' + instruction.attrib['opcode'] + ' - source variable is not initialized.', code_input, 56)
                    else:
                        err_wrclex('# Interpretation - ' + instruction.attrib['opcode'] + ' - source variable does not exist.', code_input, 54)
                elif arg3.text[0] == 'L':
                    pass # TODO - local frame
                else: # // arg3.text[0] == 'T'
                    pass # TODO - temporary frame
            elif arg3.attrib['type'] == 'int':
                value2 = int(arg3.text)
            else:
                err_wrclex('# Interpretation - ' + instruction.attrib['opcode'] + ' - wrong operand types.', code_input, 53)
            global_frame[arg1.text[3:len(arg1.text)]]['type'] = 'int'
            if instruction.attrib['opcode'] == 'ADD':
                global_frame[arg1.text[3:len(arg1.text)]]['value'] = value1 + value2
            elif instruction.attrib['opcode'] == 'SUB':
                global_frame[arg1.text[3:len(arg1.text)]]['value'] = value1 - value2
            elif instruction.attrib['opcode'] == 'MUL':
                global_frame[arg1.text[3:len(arg1.text)]]['value'] = value1 * value2
            elif instruction.attrib['opcode'] == 'IDIV':
                try:
                    global_frame[arg1.text[3:len(arg1.text)]]['value'] = value1 // value2
                except ZeroDivisionError:
                    err_wrclex('# Interpretation - IDIV - zero division.', code_input, 57)

        elif instruction.attrib['opcode'] == 'SUB':
            pass
        elif instruction.attrib['opcode'] == 'MUL':
            pass
        elif instruction.attrib['opcode'] == 'IDIV':
            pass
        elif instruction.attrib['opcode'] == 'LT':
            pass
        elif instruction.attrib['opcode'] == 'GT':
            pass
        elif instruction.attrib['opcode'] == 'EQ':
            pass
        elif instruction.attrib['opcode'] == 'AND':
            pass
        elif instruction.attrib['opcode'] == 'OR':
            pass
        elif instruction.attrib['opcode'] == 'NOT':
            pass
        elif instruction.attrib['opcode'] == 'INT2CHAR':
            pass
        elif instruction.attrib['opcode'] == 'STRI2INT':
            pass
        elif instruction.attrib['opcode'] == 'READ':
            pass

        elif instruction.attrib['opcode'] == 'WRITE':
            arg1 = instruction.find('arg1')
            if arg1.attrib['type'] == 'var':
                if arg1.text[0] == 'G':
                    if arg1.text[3:len(arg1.text)] in global_frame:
                        if global_frame[arg1.text[3:len(arg1.text)]]['type'] != None:
                            if global_frame[arg1.text[3:len(arg1.text)]]['type'] == 'nil':
                                print('', end='', flush=True)
                            else: # // bool, int, string
                                print(global_frame[arg1.text[3:len(arg1.text)]]['value'], end='', flush=True)
                        else:
                            err_wrclex('# Interpretation - WRITE - source variable is not initialized.', code_input, 56)
                    else:
                        err_wrclex('# Interpretation - WRITE - source variable does not exist.', code_input, 52)
                elif arg1.text[0] == 'L':
                    pass # TODO - local frame
                else: # // arg1.text[0] == 'T'
                    pass # TODO - temporary frame
            elif arg1.attrib['type'] == 'nil':
                print('', end='', flush=True)
            else: # // bool, int, string
                print(str(arg1.text or ''), end='', flush=True)

        elif instruction.attrib['opcode'] == 'CONCAT':
            arg1 = instruction.find('arg1')
            arg2 = instruction.find('arg2')
            arg3 = instruction.find('arg3')
            if arg1.text[0] == 'G':
                if arg1.text[3:len(arg1.text)] in global_frame:
                    pass
                else:
                    err_wrclex('# Interpretation - CONCAT - destination variable does not exist.', code_input, 54)
            elif arg1.text[0] == 'L':
                pass # TODO - local frame
            else: # // arg1.text[0] == 'T'
                pass # TODO - temporary frame
            if arg2.attrib['type'] == 'var':
                if arg2.text[0] == 'G':
                    if arg2.text[3:len(arg2.text)] in global_frame:
                        if global_frame[arg2.text[3:len(arg2.text)]]['type'] != None:
                            if global_frame[arg2.text[3:len(arg2.text)]]['type'] == 'string':
                                value1 = global_frame[arg2.text[3:len(arg2.text)]]['value']
                            else:
                                err_wrclex('# Interpretation - CONCAT - wrong type of source variable.', code_input, 53)
                        else:
                            err_wrclex('# Interpretation - CONCAT - source variable is not initialized.', code_input, 56)
                    else:
                        err_wrclex('# Interpretation - CONCAT - source variable does not exist.', code_input, 54)
                elif arg2.text[0] == 'L':
                    pass # TODO - local frame
                else: # // arg2.text[0] == 'T'
                    pass # TODO - temporary frame
            elif arg2.attrib['type'] == 'string':
                value1 = str(arg2.text or '')
            else:
                err_wrclex('# Interpretation - CONCAT - wrong operand types.', code_input, 53)
            if arg3.attrib['type'] == 'var':
                if arg3.text[0] == 'G':
                    if arg3.text[3:len(arg3.text)] in global_frame:
                        if global_frame[arg3.text[3:len(arg3.text)]]['type'] != None:
                            if global_frame[arg3.text[3:len(arg3.text)]]['type'] == 'string':
                                value2 = global_frame[arg3.text[3:len(arg3.text)]]['value']
                            else:
                                err_wrclex('# Interpretation - CONCAT - wrong type of source variable.', code_input, 53)
                        else:
                            err_wrclex('# Interpretation - CONCAT - source variable is not initialized.', code_input, 56)
                    else:
                        err_wrclex('# Interpretation - CONCAT - source variable does not exist.', code_input, 54)
                elif arg3.text[0] == 'L':
                    pass # TODO - local frame
                else: # // arg3.text[0] == 'T'
                    pass # TODO - temporary frame
            elif arg3.attrib['type'] == 'string':
                value2 = str(arg3.text or '')
            else:
                err_wrclex('# Interpretation - CONCAT - wrong operand types.', code_input, 53)
            global_frame[arg1.text[3:len(arg1.text)]]['type'] = 'string'
            global_frame[arg1.text[3:len(arg1.text)]]['value'] = value1 + value2

        elif instruction.attrib['opcode'] == 'STRLEN':
            pass
        elif instruction.attrib['opcode'] == 'GETCHAR':
            pass
        elif instruction.attrib['opcode'] == 'SETCHAR':
            pass

        elif instruction.attrib['opcode'] == 'TYPE':
            arg1 = instruction.find('arg1')
            arg2 = instruction.find('arg2')
            if arg1.text[0] == 'G':
                if arg1.text[3:len(arg1.text)] in global_frame:
                    pass
                else:
                    err_wrclex('# Interpretation - TYPE - destination variable does not exist.', code_input, 54)
            elif arg1.text[0] == 'L':
                pass # TODO - local frame
            else: # // arg1.text[0] == 'T'
                pass # TODO - temporary frame
            if arg2.attrib['type'] == 'var':
                if arg2.text[0] == 'G':
                    if arg2.text[3:len(arg2.text)] in global_frame:
                        if global_frame[arg2.text[3:len(arg2.text)]]['type'] != None:
                            type1 = global_frame[arg2.text[3:len(arg2.text)]]['type']
                        else:
                            type1 = ''
                    else:
                        err_wrclex('# Interpretation - TYPE - source variable does not exist.', code_input, 54)
                elif arg2.text[0] == 'L':
                    pass # TODO - local frame
                else: # // arg2.text[0] == 'T'
                    pass # TODO - temporary frame
            else:
                type1 = arg2.attrib['type']
            global_frame[arg1.text[3:len(arg1.text)]]['type'] = 'string'
            global_frame[arg1.text[3:len(arg1.text)]]['value'] = type1

        elif instruction.attrib['opcode'] == 'LABEL':
            pass # // doesn't do anything, it just marks a spot in code

        elif instruction.attrib['opcode'] == 'JUMP':
            arg1 = instruction.find('arg1')
            order = labels[arg1.text]
            continue

        elif instruction.attrib['opcode'] == 'JUMPIFEQ':
            arg1 = instruction.find('arg1')
            arg2 = instruction.find('arg2')
            arg3 = instruction.find('arg3')
            if arg2.attrib['type'] == 'var':
                if arg2.text[0] == 'G':
                    if arg2.text[3:len(arg2.text)] in global_frame:
                        if global_frame[arg2.text[3:len(arg2.text)]]['type'] != None:
                            type1 = global_frame[arg2.text[3:len(arg2.text)]]['type']
                            value1 = global_frame[arg2.text[3:len(arg2.text)]]['value']
                        else:
                            err_wrclex('# Interpretation - JUMPIFEQ - variable is not initialized.', code_input, 56)
                    else:
                        err_wrclex('# Interpretation - JUMPIFEQ - variable does not exist.', code_input, 54)
                elif arg2.text[0] == 'L':
                    pass # TODO - local frame
                else: # // arg2.text[0] == 'T'
                    pass # TODO - temporary frame
            else:
                if arg2.attrib['type'] == 'int':
                    value1 = int(arg2.text)
                else:
                    value1 = str(arg2.text or '')
                type1 = arg2.attrib['type']
            if arg3.attrib['type'] == 'var':
                if arg3.text[0] == 'G':
                    if arg3.text[3:len(arg3.text)] in global_frame:
                        if global_frame[arg3.text[3:len(arg3.text)]]['type'] != None:
                            type2 = global_frame[arg3.text[3:len(arg3.text)]]['type']
                            value2 = global_frame[arg3.text[3:len(arg3.text)]]['value']
                        else:
                            err_wrclex('# Interpretation - JUMPIFEQ - variable is not initialized.', code_input, 56)
                    else:
                        err_wrclex('# Interpretation - JUMPIFEQ - variable does not exist.', code_input, 54)
                elif arg3.text[0] == 'L':
                    pass # TODO - local frame
                else: # // arg3.text[0] == 'T'
                    pass # TODO - temporary frame
            else:
                if arg3.attrib['type'] == 'int':
                    value2 = int(arg3.text)
                else:
                    value2 = str(arg3.text or '')
                type2 = arg3.attrib['type']
            if type1 == type2:
                if value1 == value2: # // equal types and values - jump
                    order = labels[arg1.text]
                    continue
            elif type1 == 'nil' or type2 == 'nil':
                pass # // nothing to do, operands are not equal
            else:
                err_wrclex('# Interpretation - JUMPIFEQ - wrong operand types.', code_input, 53)

        elif instruction.attrib['opcode'] == 'JUMPIFNEQ':
            arg1 = instruction.find('arg1')
            arg2 = instruction.find('arg2')
            arg3 = instruction.find('arg3')
            if arg2.attrib['type'] == 'var':
                if arg2.text[0] == 'G':
                    if arg2.text[3:len(arg2.text)] in global_frame:
                        if global_frame[arg2.text[3:len(arg2.text)]]['type'] != None:
                            type1 = global_frame[arg2.text[3:len(arg2.text)]]['type']
                            value1 = global_frame[arg2.text[3:len(arg2.text)]]['value']
                        else:
                            err_wrclex('# Interpretation - JUMPIFNEQ - variable is not initialized.', code_input, 56)
                    else:
                        err_wrclex('# Interpretation - JUMPIFNEQ - variable does not exist.', code_input, 54)
                elif arg2.text[0] == 'L':
                    pass # TODO - local frame
                else: # // arg2.text[0] == 'T'
                    pass # TODO - temporary frame
            else:
                if arg2.attrib['type'] == 'int':
                    value1 = int(arg2.text)
                else:
                    value1 = str(arg2.text or '')
                type1 = arg2.attrib['type']
            if arg3.attrib['type'] == 'var':
                if arg3.text[0] == 'G':
                    if arg3.text[3:len(arg3.text)] in global_frame:
                        if global_frame[arg3.text[3:len(arg3.text)]]['type'] != None:
                            type2 = global_frame[arg3.text[3:len(arg3.text)]]['type']
                            value2 = global_frame[arg3.text[3:len(arg3.text)]]['value']
                        else:
                            err_wrclex('# Interpretation - JUMPIFNEQ - variable is not initialized.', code_input, 56)
                    else:
                        err_wrclex('# Interpretation - JUMPIFNEQ - variable does not exist.', code_input, 54)
                elif arg3.text[0] == 'L':
                    pass # TODO - local frame
                else: # // arg3.text[0] == 'T'
                    pass # TODO - temporary frame
            else:
                if arg3.attrib['type'] == 'int':
                    value2 = int(arg3.text)
                else:
                    value2 = str(arg3.text or '')
                type2 = arg3.attrib['type']
            if type1 == type2:
                if value1 != value2: # // equal types and different values - jump
                    order = labels[arg1.text]
                    continue
            elif type1 == 'nil' or type2 == 'nil': # // different types, but one is nil - jump
                order = labels[arg1.text]
                continue
            else:
                err_wrclex('# Interpretation - JUMPIFNEQ - wrong operand types.', code_input, 53)

        elif instruction.attrib['opcode'] == 'EXIT':
            arg1 = instruction.find('arg1')
            if arg1.attrib['type'] == 'var':
                pass # TODO - variables
            elif arg1.attrib['type'] == 'int':
                value1 = int(arg1.text)
                if value1 >= 0 or value1 <= 49:
                    err_wrclex('', code_input, value1)
                else:
                    err_wrclex('# Interpretation - EXIT - exit value is out of range.', code_input, 57)
            else: # // nil, bool, string
                err_wrclex('# Interpretation - EXIT - wrong operand type.', code_input, 53)

        elif instruction.attrib['opcode'] == 'DPRINT':
            pass
        elif instruction.attrib['opcode'] == 'BREAK':
            pass

        else: # // this really shouldn't happen, as there should only be valid instructions at this point - it is checked before
            err_wrclex('! Error: Unexpected opcode of instruction!', code_input, 32)

    order += 1 # DON'T FORGET THIS!!!

# try:
#     code_input.read()
# except OSError:
#     err_wrclex('# Failed to read from file with input for source code.\n', code_input, 11)
# except ValueError:
#     err_wrclex('# Text in file with input for source code could not be decoded as UTF-8.\n', code_input, 11)

# // close file with input for source code
if code_input is not sys.stdin:
    code_input.close()
