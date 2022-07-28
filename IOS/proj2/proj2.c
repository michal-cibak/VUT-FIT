/*
Vypracoval Michal Cibak
IOS - Projekt 2: The Senate Bus Problem
*/

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <time.h>
#include <sys/wait.h>
#include <sys/file.h>
#include <stdbool.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <semaphore.h>
#include <sys/stat.h>

int string_to_long(char *string, long *number)
//converts string to long, returns 0 if OK, 1 if not a number, 2 if out of range
{
    char *p_after_number=NULL;

    errno=0;
    *number=strtol(string, &p_after_number, 0);
    if (!p_after_number || p_after_number==string || *p_after_number) // only one of !p_after_number / p_after_number=string may be needed
        return 1;
    else if (errno)
        return 2;

    return 0;
}

int arg_load(char *argv[], long *R, long *C, unsigned int *ART, unsigned int *ABT)
//converts string arguments to numbers and stores them, returns 0 if OK, 1 if error occurred
{
    long number=0;

    for (int i=1; i<5; i++) //pre argumenty 1 - 4
    {
        switch (string_to_long(argv[i], &number)) //prevod argumentu na cislo long
        {
        case 0: //prevod bez chyby
            if (i<3 && number<=0) //R alebo C je zle
            {
                fprintf(stderr, "Zadane %i. cislo nie je vacsie ako 0!\n", i);
                return 1;
            }
            else if (i>2 && (number<0 || number>1000)) //ART alebo ABT je zle
            {
                fprintf(stderr, "Zadane %i. cislo nie je z intervalu <0 ; 1000>!\n", i);
                return 1;
            }
            switch (i)
            {
            case 1:
                *R=number;
                break;
            case 2:
                *C=number;
                break;
            case 3:
                *ART=(unsigned int)number;
                break;
            case 4:
                *ABT=(unsigned int)number;
                break;
            }
            break;
        case 1:
            fprintf(stderr, "Zadany %i. argument nie je cislo!\n", i);
            return 1;
        case 2:
            fprintf(stderr, "Zadane %i. cislo je mimo rozsahu long int!\n", i);
            return 1;
        default:
            fprintf(stderr, "ERROR: Toto sa nema vypisat!\n");
            return 1;
        }
    }
    return 0;
}

int process_bus(long R, long C, unsigned int ABT, FILE *txt_output, unsigned long long *A, long *CR, sem_t *sem_zapis, sem_t *sem_vstup, sem_t *sem_nastup, sem_t *sem_odchod, sem_t *sem_vystup);

int process_generator(long R, unsigned int ART, FILE *txt_output, unsigned long long *A, long *CR, sem_t *sem_zapis, sem_t *sem_vstup, sem_t *sem_nastup, sem_t *sem_odchod, sem_t *sem_vystup);

int process_rider(char *NAME, long I, FILE *txt_output, unsigned long long *A, long *CR, sem_t *sem_zapis, sem_t *sem_vstup, sem_t *sem_nastup, sem_t *sem_odchod, sem_t *sem_vystup);

int rand_divisor(int range)
//auxiliary function returning divisor to be used in a formula for getting a random number from a range
//needed in case RAND_MAX+1 would cause an overflow
{
    if ((RAND_MAX%range)==(range-1))
        return (RAND_MAX/range+1);
    else
        return (RAND_MAX/range);
}

int cleanup(bool always_fail, FILE *txt_output, int *A_ID, int *CR_ID, sem_t *sem_zapis, sem_t *sem_vstup, sem_t *sem_nastup, sem_t *sem_odchod, sem_t *sem_vystup)
//cleans resources; if always_fail is set to false, returns 0 on success, 1 on failure; else if true always returns 1
{
    if (A_ID)
        shmctl(*A_ID, IPC_RMID, NULL);
    if (CR_ID)
        shmctl(*CR_ID, IPC_RMID, NULL);

    if (sem_zapis)
    {
        sem_close(sem_zapis);
        sem_unlink("xcibak00_sem_zapis");
    }
    if (sem_vstup)
    {
        sem_close(sem_vstup);
        sem_unlink("xcibak00_sem_vstup");
    }
    if (sem_nastup)
    {
        sem_close(sem_nastup);
        sem_unlink("xcibak00_sem_nastup");
    }
    if (sem_odchod)
    {
        sem_close(sem_odchod);
        sem_unlink("xcibak00_sem_odchod");
    }
    if (sem_vystup)
    {
        sem_close(sem_vystup);
        sem_unlink("xcibak00_sem_vystup");
    }

    if (txt_output && fclose(txt_output)==EOF)
    {
        fprintf(stderr, "Nepodarilo sa zatvorit subor!\n");
        fflush(stderr);
        return 1;
    }

    if (always_fail)
        return 1;
    else
        return 0;
}

int main(int argc, char *argv[])
{
    //kontrola poctu argumentov
	if (argc!=5)
	{
		fprintf(stderr, "Bol zadany zly pocet argumentov!\n");
		return 1;
	}

    long R, C;
    unsigned int ART, ABT;

    //nacitanie do premennych + kontrola spravneho nacitania argumentov
    if (arg_load(argv, &R, &C, &ART, &ABT))
        return 1;

    FILE *txt_output=fopen("proj2.out", "w");
    if (txt_output==NULL)
    {
        fprintf(stderr, "Chyba pri vytvarani/otvarani suboru!\n");
		return 1;
    }

    //zdielane premenne
    //unsigned long long A=0 shared
    errno=0;
    int A_ID=shmget(IPC_PRIVATE, sizeof(long long), IPC_CREAT | 0666); //same size as unsigned long long
    if (A_ID==-1)
    {
        if (errno==EINVAL)
            fprintf(stderr, "EINVAL!\n");
        else if (errno==ENOENT)
            fprintf(stderr, "ENOENT!\n");
        else if (errno==ENOSPC)
            fprintf(stderr, "ENOSPC!\n");
        else if (errno==EEXIST)
            fprintf(stderr, "EEXIST!\n");
        else
            fprintf(stderr, "Iny error kod!\n");

        fprintf(stderr, "Chyba pri ziskavani ID zdielanej pamate pre A!\n");
		return cleanup(true, txt_output, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
    }

    unsigned long long *A=shmat(A_ID, NULL, 0);
    if (A==NULL)
    {
        fprintf(stderr, "Chyba pri ziskavani odkazu na zdielanu pamat A!\n");
		return cleanup(true, txt_output, &A_ID, NULL, NULL, NULL, NULL, NULL, NULL);
    }
    *A=0;

    //long CR=1 shared
    errno=0;
    int CR_ID=shmget(IPC_PRIVATE, sizeof(unsigned long long), IPC_CREAT | 0666);
    if (CR_ID==-1)
    {
        if (errno==EINVAL)
            fprintf(stderr, "EINVAL!\n");
        else if (errno==ENOENT)
            fprintf(stderr, "ENOENT!\n");
        else if (errno==ENOSPC)
            fprintf(stderr, "ENOSPC!\n");
        else if (errno==EEXIST)
            fprintf(stderr, "EEXIST!\n");
        else
            fprintf(stderr, "Iny error kod!\n");

        fprintf(stderr, "Chyba pri ziskavani ID zdielanej pamate pre CR!\n");
		return cleanup(true, txt_output, &A_ID, NULL, NULL, NULL, NULL, NULL, NULL);
    }

    long *CR=shmat(CR_ID, NULL, 0);
    if (CR==NULL)
    {
        fprintf(stderr, "Chyba pri ziskavani odkazu na zdielanu pamat CR!\n");
		return cleanup(true, txt_output, &A_ID, &CR_ID, NULL, NULL, NULL, NULL, NULL);
    }
    *CR=0;

    /// TO BE DELETED - in case testing causes semaphores to stay
    sem_unlink("xcibak00_sem_zapis");
    sem_unlink("xcibak00_sem_vstup");
    sem_unlink("xcibak00_sem_nastup");
    sem_unlink("xcibak00_sem_odchod");
    sem_unlink("xcibak00_sem_vystup");
    ///

    //semafory
    sem_t *sem_zapis=sem_open("xcibak00_sem_zapis", O_CREAT | O_EXCL, 0666, 1);
    if (sem_zapis==SEM_FAILED)
    {
        fprintf(stderr, "Nepodarilo sa vytvorit semafor sem_zapis!\n");
		return cleanup(true, txt_output, &A_ID, &CR_ID, NULL, NULL, NULL, NULL, NULL);
    }
    sem_t *sem_vstup=sem_open("xcibak00_sem_vstup", O_CREAT | O_EXCL, 0666, 1);
    if (sem_vstup==SEM_FAILED)
    {
        fprintf(stderr, "Nepodarilo sa vytvorit semafor sem_vstup!\n");
		return cleanup(true, txt_output, &A_ID, &CR_ID, sem_zapis, NULL, NULL, NULL, NULL);
    }
    sem_t *sem_nastup=sem_open("xcibak00_sem_nastup", O_CREAT | O_EXCL, 0666, 0);
    if (sem_nastup==SEM_FAILED)
    {
        fprintf(stderr, "Nepodarilo sa vytvorit semafor sem_nastup!\n");
		return cleanup(true, txt_output, &A_ID, &CR_ID, sem_zapis, sem_vstup, NULL, NULL, NULL);
    }
    sem_t *sem_odchod=sem_open("xcibak00_sem_odchod", O_CREAT | O_EXCL, 0666, 0);
    if (sem_odchod==SEM_FAILED)
    {
        fprintf(stderr, "Nepodarilo sa vytvorit semafor sem_odchod!\n");
		return cleanup(true, txt_output, &A_ID, &CR_ID, sem_zapis, sem_vstup, sem_nastup, NULL, NULL);
    }
    sem_t *sem_vystup=sem_open("xcibak00_sem_vystup", O_CREAT | O_EXCL, 0666, 0);
    if (sem_vystup==SEM_FAILED)
    {
        fprintf(stderr, "Nepodarilo sa vytvorit semafor sem_vystup!\n");
		return cleanup(true, txt_output, &A_ID, &CR_ID, sem_zapis, sem_vstup, sem_nastup, sem_odchod, NULL);
    }

    //vytvorenie BUS
    pid_t pid_bus;

    if ((pid_bus=fork()) < 0)
    {
        fprintf(stderr, "Nepodarilo sa vytvorit proces \"BUS\"!\n");
		return cleanup(true, txt_output, &A_ID, &CR_ID, sem_zapis, sem_vstup, sem_nastup, sem_odchod, sem_vystup);
    }
    else if (pid_bus==0) //child
    {
    //child code (in function)
        if (process_bus(R, C, ABT, txt_output, A, CR, sem_zapis, sem_vstup, sem_nastup, sem_odchod, sem_vystup)) //child vratil chybu
            return 1;
        else //prebehol uspesne
            return 0;
    }

    //parent code
    //vytvorenie GEN
    pid_t pid_gen;

    if ((pid_gen=fork()) < 0)
    {
        fprintf(stderr, "Nepodarilo sa vytvorit proces \"GEN\"!\n");
        fflush(stderr);
		return 1;
    }
    else if (pid_gen==0) //child
    {
    //child code (in function)
        if (process_generator(R, ART, txt_output, A, CR, sem_zapis, sem_vstup, sem_nastup, sem_odchod, sem_vystup)) //child vratil chybu
            return 1;
        else //prebehol uspesne
            return 0;
    }

    //parent code
    int status;

    if ((pid_bus=wait(&status))==-1) //cakanie na ukoncenie prveho child procesu (pravdepodobne autobus)
    {
        fprintf(stderr, "Prvy proces, na ktoreho ukoncenie sa cakalo, bol preruseny!\n"); //proces existuje, teda jedine prerusenie signalom
        fflush(stderr);
        return cleanup(true, txt_output, &A_ID, &CR_ID, sem_zapis, sem_vstup, sem_nastup, sem_odchod, sem_vystup);
    }
    else if (status) //child process error
    {
        fprintf(stderr, "Prvy proces, na ktory sa cakalo, bol ukonceny s chybou!\n");
        fflush(stderr);
        return cleanup(true, txt_output, &A_ID, &CR_ID, sem_zapis, sem_vstup, sem_nastup, sem_odchod, sem_vystup);
    }

    if ((pid_gen=wait(&status))==-1) //cakanie na ukoncenie druheho child procesu (pravdepodobne generator)
    {
        fprintf(stderr, "Druhy proces, na ktoreho ukoncenie sa cakalo, bol preruseny!\n"); //proces existuje, teda jedine prerusenie signalom
        fflush(stderr);
        return cleanup(true, txt_output, &A_ID, &CR_ID, sem_zapis, sem_vstup, sem_nastup, sem_odchod, sem_vystup);
    }
    else if (status) //child process error
    {
        fprintf(stderr, "Druhy proces, na ktory sa cakalo, bol ukonceny s chybou!\n");
        fflush(stderr);
        return cleanup(true, txt_output, &A_ID, &CR_ID, sem_zapis, sem_vstup, sem_nastup, sem_odchod, sem_vystup);
    }

	return cleanup(false, txt_output, &A_ID, &CR_ID, sem_zapis, sem_vstup, sem_nastup, sem_odchod, sem_vystup);
}

int process_bus(long R, long C, unsigned int ABT, FILE *txt_output, unsigned long long *A, long *CR, sem_t *sem_zapis, sem_t *sem_vstup, sem_t *sem_nastup, sem_t *sem_odchod, sem_t *sem_vystup)
//code for process "BUS", returns 0 if OK, 1 if error occurred
{
    srand(time(NULL)-(unsigned)getpid()); //seed pre rand(), rozdielny pre rozne procesy (bus/gen) v roznom case (samozrejme, vzdy sa najde vynimka)

    char *NAME="BUS";

    sem_wait(sem_zapis);
        fprintf(txt_output, "%llu: %s: start\n", ++*A, NAME);
        fflush(txt_output);
    sem_post(sem_zapis);

    unsigned int simdrive=0;
    int rand_div; //aby sa v cykle nevolalo delenie x krat, nemusi byt pouzite

    if (ABT)
        rand_div=rand_divisor(ABT); //rand_div=(RAND_MAX+1)/ABT; --> rand_div=rand_divisor(ABT); instead if RAND_MAX+1 could cause an overflow

    do
    {
        sem_wait(sem_vstup); //vstup na zastavku - kym je na zastavke autobus, ostatne procesy prichadzajuce sa davaju do fronty
            sem_wait(sem_zapis);
                fprintf(txt_output, "%llu: %s: arrival\n", ++*A, NAME);
                fflush(txt_output);
            sem_post(sem_zapis);

            int nastupujuci=0;

            sem_wait(sem_zapis);
                if (*CR)
                {
                        fprintf(txt_output, "%llu: %s: start boarding: %li\n", ++*A, NAME, *CR);
                        fflush(txt_output);
                    sem_post(sem_zapis);

                    if (C>*CR) //v tomto case by nik nemal menit CR, je to bezpecne citanie
                    {
                        R-=*CR;
                        nastupujuci=*CR;
                    }
                    else
                    {
                        R-=C;
                        nastupujuci=C;
                    }
                    for (int i=0; i<nastupujuci; i++)
                        sem_post(sem_nastup); //C krat pusti do autobusu

                    for (int i=0; i<nastupujuci; i++)
                        sem_wait(sem_odchod);

                    sem_wait(sem_zapis);
                        fprintf(txt_output, "%llu: %s: end boarding: %li\n", ++*A, NAME, *CR);
                        fflush(txt_output);
                }
            sem_post(sem_zapis);

            sem_wait(sem_zapis);
                fprintf(txt_output, "%llu: %s: depart\n", ++*A, NAME);
                fflush(txt_output);
            sem_post(sem_zapis);
        sem_post(sem_vstup); //uvolnenie vstupu na zastavku pre prichadzajuce procesy

        if (ABT)
        {
            do
                simdrive=rand()/rand_div+1; //non biased rand()
            while (simdrive==ABT+1);

            if (usleep(simdrive*1000)) //uspatie na nahodnu dobu <1 - 1000>
            {
                fprintf(stderr, "Simulacia jazdy autobusu bola prerusena!\n");
                fflush(stderr);
                return 1;
            }
        }

        sem_wait(sem_zapis);
            fprintf(txt_output, "%llu: %s: end\n", ++*A, NAME);
            fflush(txt_output);
        sem_post(sem_zapis);

        for (int i=0; i<nastupujuci; i++) //pusti nastupenych ludi k vystupeniu
            sem_post(sem_vystup);
    }
    while (R>0);


    /// CHANGE
    int chyba=0;
    if (chyba)
    {
        fprintf(stderr, "Doslo k chybe v procese BUS!\n");
        fflush(stderr);
        return 1;
    }
    /// !

    sem_wait(sem_zapis);
        fprintf(txt_output, "%llu: %s: finish\n", ++*A, NAME);
        fflush(txt_output);
    sem_post(sem_zapis);

    return 0;
}

int process_generator(long R, unsigned int ART, FILE *txt_output, unsigned long long *A, long *CR, sem_t *sem_zapis, sem_t *sem_vstup, sem_t *sem_nastup, sem_t *sem_odchod, sem_t *sem_vystup)
//code for process "GEN" generating processes "RID", returns 0 if OK, 1 if error occurred
{
    srand(time(NULL)+(unsigned)getpid()); //seed pre rand(), rozdielny pre rozne procesy (bus/gen) v roznom case (samozrejme, vzdy sa najde vynimka)

    pid_t pid_rid;
    char *NAME="RID";

    unsigned int genafter=0;
    int rand_div; //aby sa v cykle nevolalo delenie x krat, nemusi byt pouzite

    if (ART)
        rand_div=rand_divisor(ART); //rand_div=(RAND_MAX+1)/ART; --> rand_div=rand_divisor(ART); if RAND_MAX+1 could cause an overflow

    //vytvorenie RID R krat
    for (long I=1; I<=R; I++)
    {
        if (ART)
        {
            do
                genafter=rand()/rand_div+1; //non biased rand()
            while (genafter==ART+1);

            if (usleep(genafter*1000)) //uspatie na nahodnu dobu <1 - 1000>
            {
                fprintf(stderr, "Cakanie na generovanie procesu rider bolo prerusene!\n");
                fflush(stderr);
                return 1;
            }
        }

        if ((pid_rid=fork()) < 0)
        {
            fprintf(stderr, "Nepodarilo sa vytvorit proces \"RID\" %li!\n", I);
            fflush(stderr);
            return 1; /// TO BE CHANGED - asi by bolo vhodne ukoncit vytvorene procesy?...
        }
        else if (pid_rid==0) //child
        {
        //child code (in function)
            if (process_rider(NAME, I, txt_output, A, CR, sem_zapis, sem_vstup, sem_nastup, sem_odchod, sem_vystup)) //child vratil chybu
                return 1;
            else //prebehol uspesne
                return 0;
        }

        //parent code
        //NONE
    }

    int status;
    bool error=false;

    //cakanie na ukoncenie vsetkych riderov
    for (long I=1; I<=R; I++)
    {
        if ((pid_rid=wait(&status))==-1)
        {
            fprintf(stderr, "%li. proces rider na ktoreho ukoncenie sa cakalo bol preruseny!\n", I); //proces existuje, mozne jedine prerusenie signalom
            fflush(stderr);
            error=true;
        }
        else if (status) //child process error
        {
            fprintf(stderr, "%li. proces rider na ktoreho ukoncenie sa cakalo skoncil s chybou!\n", I);
            fflush(stderr);
            error=true;
        }
    }
    if (error)
        return 1;

    /// CHANGE
    int chyba=0;
    if (chyba)
    {
        fprintf(stderr, "Doslo k chybe v procese GEN!\n");
        fflush(stderr);
        return 1;
    }
    /// !

    return 0;
}

int process_rider(char *NAME, long I, FILE *txt_output, unsigned long long *A, long *CR, sem_t *sem_zapis, sem_t *sem_vstup, sem_t *sem_nastup, sem_t *sem_odchod, sem_t *sem_vystup)
//code for processes "RID", returns 0 if OK, 1 if error occurred
{
    sem_wait(sem_zapis);
        fprintf(txt_output, "%llu: %s %li: start\n", ++*A, NAME, I);
        fflush(txt_output);
    sem_post(sem_zapis);

    sem_wait(sem_vstup); //vstup na zastavku, caka ak uz je tam autobus, postavi sa do fronty na vstup (mozno by bolo dobre riesit nejako bez fronty, napr. pouzit zdielanu premennu, ktora hovori, ci tam je autobus)
        sem_wait(sem_zapis);
            fprintf(txt_output, "%llu: %s %li: enter: %li\n", ++*A, NAME, I, ++*CR);
            fflush(txt_output);
        sem_post(sem_zapis);
    sem_post(sem_vstup);

    sem_wait(sem_nastup); //cakaju na autobus
    sem_wait(sem_zapis);
        fprintf(txt_output, "%llu: %s %li: boarding\n", ++*A, NAME, I);
        fflush(txt_output);
        --*CR;
    sem_post(sem_zapis);
    sem_post(sem_odchod); //pripraveny na odchod

    sem_wait(sem_vystup);
    sem_wait(sem_zapis);
        fprintf(txt_output, "%llu: %s %li: finish\n", ++*A, NAME, I);
        fflush(txt_output);
    sem_post(sem_zapis);

    /// CHANGE
    int chyba=0;
    if (chyba)
    {
        fprintf(stderr, "Doslo k chybe v procese RID %li!\n", I);
        fflush(stderr);
        return 1;
    }
    /// !

    return 0;
}
