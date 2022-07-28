/*
Vypracoval Michal Cibak
Projekt 1 - Prace s textem
*/

#include <stdio.h>
#include <stdbool.h>

#define USER_INPUT_SIZE 101 //max. pocet znakov na vstupe +1
#define ALLOWED_ENABLE_CHARS 129 //max. pocet roznych znakov na vypis +1

char lowercase_to_uppercase(char character) //zmena maleho pismena na velke
{
    if (character>='a' && character<='z')
        character=character+('A'-'a');
    return character;
}

//prepis argumentu do retazca
void arg_to_str(char *argv[], char *user_input)
{
    int i;
    for (i=0; argv[1][i] && i<100; i++)
    {
        user_input[i]=lowercase_to_uppercase(argv[1][i]);
    }
    user_input[i]='\0'; //ukoncenie retazca
}

//kontrola, ze znak este nie je zapisany v poli
bool no_duplicate(char file_char, char enable[], int enable_count)
{
    if (enable_count==0)
        return true;
    else
        for (int i=0; i<enable_count; i++)
            if (enable[i]==file_char)
                return false;
    return true;
}

//bublinkive utriedenie pola znakov
void sort(char *enable, int enable_count)
{
    int a, b, temp;
    for (a=0; a<(enable_count-1); a++)
    {
        for (b=0; b<(enable_count-a-1); b++)
        {
            if (enable[b]>enable[b+1])
            {
                temp=enable[b];
                enable[b]=enable[b+1];
                enable[b+1]=temp;
            }
        }
    }
}

//zisti, ci sa zadany retazec nachadza na zaciatku niektoreho nazvu mesta a podla toho priradi nasledovne znaky do enable a found
void find_enable_or_found(char *user_input, char *enable, char *found, int *found_count)
{
    char file_char;
    bool enabled_writing=true;
    int user_input_id=0;
    int enable_id=0;
    int found_id=0;
    bool enabled_found=false;
    while ((file_char=lowercase_to_uppercase(getchar()))!=EOF) //prechadza znakmi z textovveho suboru
    {
        /*
        vyhodnocovacia podmienka zhodnosti vstupov pre pripadny zapis do prislusnych retazcov
        akekolvek zapisovanie sa deje iba ak sa zhoduje cely vstup so zaciatkom riadku v .txt
        do enable sa zapise iba jeden nasledujuci znak, ak este neskoncil riadok v .txt
        do found sa zapisu vsetky znaky az po koniec riadku iba ak je to prva najdena zhoda
        */
        if (enabled_writing)
        {
            if (file_char!=user_input[user_input_id])
            {
                if (user_input[user_input_id]=='\0') //cely zadany retazec sa zhoduje so zaciatkom niektoreho riadku z .txt suboru
                {
                    ++*found_count; //+1 najdena zhoda
                    if (file_char!='\n') //riadok v .txt subore pokracuje
                    {
                        enabled_writing=false; //zakaze pristup do hlavnej vetvy
                        if (no_duplicate(file_char, enable, enable_id))
                            enable[enable_id++]=file_char;
                        if (*found_count==1)
                        {
                            found[found_id++]=file_char;
                            enabled_found=true; //povoli zapis zvysku riadku do found ak sa jedna o prvy zapis
                        }
                    }
                    else
                    {
                        user_input_id=0; //vrati na zaciatok retazca pre dalsie porovnanie
                    }
                }
                else if (file_char!='\n')
                {
                    enabled_writing=false;
                    user_input_id++;
                }
                else user_input_id=0;
            }
            else user_input_id++;
        }
        else if (file_char!='\n') //vetva sa vykonava, kym sa nedostane na koniec riadku v subore
        {
            if (enabled_found==true)
                found[found_id++]=file_char;
            user_input_id++;
        }
        else
        {
            enabled_found=false;
            enabled_writing=true;
            user_input_id=0;
        }
    }
    enable[enable_id]='\0';
    found[found_id]='\0';
    if (*found_count>1)
        sort(enable, enable_id);
}

//zapise prve pismena nazvu mesta do pola
void find_only_enable(char *enable)
{
    char file_char;
    int enable_id=0;
    bool enable_enable=true; //povolene alebo zakazane zapisovanie do enable
    while ((file_char=lowercase_to_uppercase(getchar()))!=EOF) //prechadza znakmi z textoveho suboru
    {
        if (enable_enable) //zapisanie pismena zakaze vetvu
        {
            if (file_char!='\n' && no_duplicate(file_char, enable, enable_id))
            {
                enable[enable_id++]=file_char;
                enable_enable=false;
            }
        }
        else if (file_char=='\n') //najdenie konca riadku opat povoli zapisovanie
            enable_enable=true;
    }
    enable[enable_id]='\0';
    sort(enable, enable_id);
}

//skontroluje co ma vypisat a vypise
void write_results(int found_count, char *user_input, char *found, char *enable)
{
    if (found_count>1)
    {
        if (enable[0]=='\0')
            printf("Found: %s", user_input);
        else
            printf("Enable: %s", enable);
    }
    if (found_count==0) printf("Not found");
    if (found_count==1) printf("Found: %s%s", user_input, found);
}

/*
program nacita znak po znaku z textoveho suboru a porovna so standardnym vstupom
podla potreby si zapamata znaky pre enable alebo found vypis
vyhodnoti co je potrebne vypisat a vypise
*/
int main(int argc, char *argv[])
{
    char user_input[USER_INPUT_SIZE]={'\0'};
    char enable[ALLOWED_ENABLE_CHARS]={'\0'};
    if (argc>1)
    {
        arg_to_str(argv, user_input);
        char found[USER_INPUT_SIZE-1]={'\0'}; //zapisuju sa don len znaky nasledujuce za zadanym retazcom, staci o jedno mensie
        int found_count=0; //pocet nazvov miest so zaciatkom zhodnym so zadanym retazcom
        find_enable_or_found(user_input, enable, found, &found_count);
        write_results(found_count, user_input, found, enable);
    }
    else
    {
        find_only_enable(enable);
        printf("Enable: %s", enable);
    }
    return 0;
}
