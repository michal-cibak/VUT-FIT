/*
    IMP project: Morseovka
    Author: xcibak00 - Michal Cibák
    Author's share: 99%
    Changes: Generated frame changed so it fits my project.
    Last change: 22.12.2019
*/

#include <fitkitlib.h>
#include <keyboard/keyboard.h>
#include <lcd/display.h>
#include <stdbool.h>

/*
    void delay_cycle(unsigned int delay);
    void delay_ms(unsigned long msecs);
*/

void print_user_help(void) { }

unsigned char decode_user_cmd(char *cmd_ucase, char *cmd) { return (CMD_UNKNOWN); }

void fpga_initialized(void) { }

enum states {menu, decoder};
enum unknown_chars {ch_Od, ch_Ou, ch_Ru, ch_Uu, ch_Uud, ch_Rud, ch_Zu, ch_Cu};

enum states state = menu; // also refered to as screen
char key_prev = '\0';
char text[33];
int fpos = 0;
int spos = 0;
char speed = '\0';
bool dot = true;
bool decode = false;
bool space = false;


void clear_frow()
{
    int i = 0;
    while (i < 16)
        text[i++] = ' ';
    fpos = 0;
}

void clear_srow()
{
    int i = 16;
    while (i < 32)
        text[i++] = ' ';
    spos = 0;
}

void clear_rows()
{
    clear_frow();
    clear_srow();
}

void reset_timer_dependend_variables()
{
    dot = true;
    decode = false;
    space = false;
}

void text_init()
{
    clear_rows();
    text[32] = '\0';
}

// as I know the format of the input string and it is a global variable, there is no need for a general function, but I made one so it can be reused
char morse_decode(char *signal)
/*
    takes morse code characters '.' and '_' from string until other character is encountered (marking end of coded character) and decodes them
    returns a decoded character if a valid character (alphanumeric, ',', '.', '?' and '!') could be decoded from input string, '\0' otherwise
*/
{
    char character = '\0';
    enum unknown_chars unknown_char;
    bool decoding = true;

    if (signal)
    {
        int i = 0;
        do
        {
            if (signal[i] == '.')
            {
                switch (character)
                {
                    case 'E': // . . (this comment = present character + incoming dot)
                        character = 'I'; // result of decoding the comment above
                        break;
                    case 'T': // _ .
                        character = 'N';
                        break;
                    case 'I': // . . .
                        character = 'S';
                        break;
                    case 'A': // . _ .
                        character = 'R';
                        break;
                    case 'N': // _ . .
                        character = 'D';
                        break;
                    case 'M': // _ _ .
                        character = 'G';
                        break;
                    case 'S':
                        character = 'H';
                        break;
                    case 'U': // . . _ .
                        character = 'F';
                        break;
                    case 'R': // . _ . .
                        character = 'L';
                        break;
                    case 'W': // . _ _ .
                        character = 'P';
                        break;
                    case 'D': // _ . . .
                        character = 'B';
                        break;
                    case 'K': // _ . _ .
                        character = 'C';
                        break;
                    case 'G': // _ _ . .
                        character = 'Z';
                        break;
                    case 'O': // _ _ _ .
                        character = '\0';
                        unknown_char = ch_Od;
                        break;
                    case 'H': // . . . . .
                        character = '5';
                        break;
                    case 'B': // _ . . . .
                        character = '6';
                        break;
                    case 'Z': // _ _ . . .
                        character = '7';
                        break;
                    case '\0':
                        if (i == 0) // .
                            character = 'E';
                        else
                        {
                            switch (unknown_char)
                            {
                                case ch_Od: // _ _ _ . .
                                    character = '8';
                                    break;
                                case ch_Ou: // _ _ _ _ .
                                    character = '9';
                                    break;
                                case ch_Uu: // . . _ _ .
                                    // character = '\0'; // already set
                                    unknown_char = ch_Uud;
                                    break;
                                case ch_Ru: // . _ . _ .
                                    // character = '\0'; // already set
                                    unknown_char = ch_Rud;
                                    break;
                                case ch_Uud: // . . _ _ . .
                                    character = '?';
                                    break;
                                default:
                                    break;
                            }
                        }
                        break;
                    default: // any character from which you can't get to another one by adding dot and anything after that
                        character = '\0';
                        decoding = false;
                        break;
                }
            }
            else if (signal[i] == '_')
            {
                switch (character)
                {
                    case 'T': // _ _ (this comment = present character + incoming underscore)
                        character = 'M'; // result of decoding the comment above
                        break;
                    case 'E': // . _
                        character = 'A';
                        break;
                    case 'M': // _ _ _
                        character = 'O';
                        break;
                    case 'N': // _ . _
                        character = 'K';
                        break;
                    case 'A': // . _ _
                        character = 'W';
                        break;
                    case 'I': // . . _
                        character = 'U';
                        break;
                    case 'O': // _ _ _ _
                        character = '\0';
                        unknown_char = ch_Ou;
                        break;
                    case 'G': // _ _ . _
                        character = 'Q';
                        break;
                    case 'K': // _ . _ _
                        character = 'Y';
                        break;
                    case 'D': // _ . . _
                        character = 'X';
                        break;
                    case 'W': // . _ _ _
                        character = 'J';
                        break;
                    case 'R': // . _ . _
                        character = '\0';
                        unknown_char = ch_Ru;
                        break;
                    case 'U': // . . _ _
                        character = '\0';
                        unknown_char = ch_Uu;
                        break;
                    case 'S': // . . . _
                        character = 'V';
                        break;
                    case 'Z': // _ _ . . _
                        character = '\0';
                        unknown_char = ch_Zu;
                        break;
                    case 'C': // _ . _ . _
                        character = '\0';
                        unknown_char = ch_Cu;
                        break;
                    case 'J': // . _ _ _ _
                        character = '1';
                        break;
                    case 'V': // . . . _ _
                        character = '3';
                        break;
                    case 'H': // . . . . _
                        character = '4';
                        break;
                    case '\0':
                        if (i == 0) // _
                            character = 'T';
                        else
                        {
                            switch (unknown_char)
                            {
                                case ch_Ou: // _ _ _ _ _
                                    character = '0';
                                    break;
                                case ch_Uu: // . . _ _ _
                                    character = '2';
                                    break;
                                case ch_Zu: // _ _ . . _ _
                                    character = ',';
                                    break;
                                case ch_Cu: // _ . _ . _ _
                                    character = '!';
                                    break;
                                case ch_Rud: // . _ . _ . _
                                    character = '.';
                                    break;
                                default:
                                    break;
                            }
                        }
                        break;
                    default: // any character from which you can't get to another one by adding underscore and anything after that
                        character = '\0';
                        decoding = false;
                        break;
                }
            }
            else
            {
                decoding = false;
            }

            i++;
        }
        while (decoding);
    }

    return character;
}

void keyboard_idle() // reaction to key press
{
    char key;
    key = key_decode(read_word_keyboard_4x4());
    if (key != key_prev) // state of keyboard has chandged since last check
    {
        switch (state) // which screen is the app on, reaction to keys depends on current screen
        {
            case menu:
                switch (key) // key reactions in menu
                {
                    /* just output, for now, the speed doesn't change */
                    case 'A':
                    case 'B':
                    case 'C':
                        speed = key;
                        LCD_write_string("MENU: speed ");
                        LCD_append_char(speed);
                        break;
                    case 'D': // Decoder
                        if (speed)
                        {
                            state = decoder;
                            LCD_write_string(text);
                            reset_timer_dependend_variables();
                            TACTL |= MC_1; // counter enabled
                        }
                        break;
                    default: // includes key up ("unpress" of button)
                        break;
                }
                break;
            case decoder:
                switch (key) // key reactions in decoder
                {
                    case 'A': // Again actual character
                        TACCR0 = 0x0000; // stop counting (if it is)
                        clear_srow();
                        reset_timer_dependend_variables();
                        break;

                    case 'B': // Back to menu
                        TACCR0 = 0x0000; // stop counting (if it is)
                        clear_srow();
                        state = menu;
                        LCD_write_string("MENU: speed ");
                        LCD_append_char(speed);
                        TACTL &= ~MC_1; // counter disabled (instead of "TACL |= MC_0" as or-ing with zeros doesn't set zeros)
                        break;

                    case 'C': // Clear text
                        TACCR0 = 0x0000; // stop counting (if it is)
                        clear_rows();
                        reset_timer_dependend_variables();
                        break;

                    case 'D': // Delete character
                        TACCR0 = 0x0000; // stop counting (if it is)
                        clear_srow();
                        if (fpos > 0)
                            text[--fpos] = ' ';
                        reset_timer_dependend_variables();
                        break;

                    case '*': // morse code character input
                        TACCR0 = 0x0000; // stop counting (if it is from previous press (decode or space counter))
                        reset_timer_dependend_variables();
                        TACCR0 = 0x4000; // start counting for dot / underscore recognition
                        break;

                    case '\0': // key up
                        if (key_prev == '*')
                        {
                            TACCR0 = 0x0000; // stop counting (if it hasn't already - maximum reached)
                            if (spos < 16)
                                text[16 + spos++] = dot ? '.' : '_';
                            reset_timer_dependend_variables();
                            TACCR0 = 0x8000; // start counting for next input / decoding recognition
                        }
                        break;

                    default:
                        break;
                }
                if (key != 'B') // refresh screen after any action other than going back to menu
                    LCD_write_string(text);
                break;

            default:
                break;
        }
        key_prev = key;
    }
}

int main(void)
{
    initialize_hardware();
    keyboard_init();
    LCD_init();

    /* timer A init */
    TACTL = TASSEL_1; // clock sourced from ACKL (32768 Hz) -> 32 counts ~ 1 ms
    TACCTL0 = CCIE; // interrupt enabled, compare mode by default
    // TACCR0 = 0x0000; // deafult, value will be specified when the timer is needed
    TACTL |= MC_0; // default, counter will be enabled outside of menu screen - for energy saving, though time spent in menu is insignificant

    text_init();
    LCD_write_string("MENU: press A/B/C to set speed");

    while (1)
    {
        terminal_idle();
        keyboard_idle();
    }
}

interrupt (TIMERA0_VECTOR) Timer_A (void)
{
    TACCR0 = 0x0000; // stop counting

    dot = false;

    if (key_prev != '*')
    {
        if (decode) // id decode is true, it has already been done and now the interrupt is called to print space
        {
            space = true;
            if (fpos >= 16)
                clear_frow();

            text[fpos++] = '_'; // underscore is used instead of space for better visibility
        }

        decode = true;

        if (!space) // if it isn't time to print space, then the interrupt must mean it is time to decode
        {
            if (fpos >= 16)
                clear_frow();

            char character = morse_decode(&text[16]);
            text[fpos++] = character ? character : '-'; // if the character is unknown ('\0'), write '-' instead
            clear_srow();
            TACCR0 = 0x8000; // start counting for space recognition
        }

        LCD_write_string(text);
    }
}
