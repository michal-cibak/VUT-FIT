/*
Vypracoval Michal Cibak
Projekt 2 - Iteracni vypocty
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

//zisti, ci je retazec prirodzene cislo
int is_number_N(char *retazec)
{
    if (retazec[0]!='\0' && retazec[0]!=0) //neprazdny retazec a nula nie prva
    {
        int i=0;
        if (retazec[0]=='+')
            i++;
        while (retazec[i]!='\0') //do konca retazca kontrola cisla
        {
            if (retazec[i]>='0' && retazec[i]<='9')
                i++;
            else
                return 0;
        }
        return 1;
    }
    return 0;
}

//zisti, ci je retazec realne cislo
int is_number_float(char *retazec)
{
    char *afternumber=NULL;
    strtod(retazec, &afternumber);
    if (!afternumber || *afternumber)
        return 0;
    else
        return 1;
}

//prezrie akrgumenty a zisti ci su spravne zadane a o ake sa jedna
int argcheck(int argc, char *argv[])
{
    //nezadane argumenty
    if (argc==1)
    {
        fprintf(stderr, "Neboli zadane argumenty programu!");
        return 0;
    }
    //dostatocny pocet argumentov
    else if (argc<7)
    {
        //spravne zadany --help
        if (strcmp(argv[1], "--help")==0)
        {
            if (argc==2)
            {
                return 1;
            }
        }
        //spravne zadany --tan
        else if (strcmp(argv[1], "--tan")==0)
        {
            if (argc==5)
            {
                if (is_number_float(argv[2]) && is_number_N(argv[3]) && is_number_N(argv[4])) //je argument A N M cislo daneho typu
                {
                    int N, M;
                    N=atoi(argv[3]);
                    M=atoi(argv[4]);
                    if (N>0 && M>0 && N<=M && N<14 && M<14) //su v rozmedzi spravnych hodnot
                        return 2;
                }
            }
        }
        //spravne zadany ten henten ony posledny
        else if (strcmp(argv[1], "-c")==0 && argc>4)
        {
            if (strcmp(argv[3], "-m")==0)
            {
                if (argc==6)
                {
                    if (is_number_float(argv[2]) && is_number_float(argv[4]) && is_number_float(argv[5])) //je argument X A B cislo daneho typu
                    {
                        double X, A, B;
                        X=atof(argv[2]);
                        A=atof(argv[4]);
                        B=atof(argv[5]);
                        if (X>0 && X<=100 && A>0 && B>0 && A<=1.4 && B<=1.4) //su v rozmedzi spravnych hodnot
                            return 3;
                    }
                }
                else if (argc==5)
                {
                    if (is_number_float(argv[2]) && is_number_float(argv[4])) //je argument X A cislo daneho typu
                    {
                        double X, A;
                        X=atof(argv[2]);
                        A=atof(argv[4]);
                        if (X>0 && X<=100 && A>0 && A<=1.4) //su v rozmedzi spravnych hodnot
                            return 4;
                    }
                }
            }
        }
        else if (strcmp(argv[1], "-m")==0)
        {
            if (argc==4)
            {
                if (is_number_float(argv[2]) && is_number_float(argv[3])) //je argument A B cislo daneho typu
                {
                    double A, B;
                    A=atof(argv[2]);
                    B=atof(argv[3]);
                    if (A>0 && B>0 && A<=1.4 && B<=1.4) //su v rozmedzi spravnych hodnot
                        return 5;
                }
            }
            else if (argc==3)
            {
                if (is_number_float(argv[2])) //je argument A cislo daneho typu
                {
                    double A;
                    A=atof(argv[2]);
                    if (A>0 && A<=1.4) //je v rozmedzi spravnych hodnot
                        return 6;
                }
            }
        }
    }
    fprintf(stderr, "Boli zle zadane argumenty programu!");
    return 0;
}

//vypocet tangensu uhla pomocou taylorovho polynomu
double taylor_tan(double x, unsigned int n) //x = uhol v radianoch; n = pocet iteracii
{
    long long int rozvoj_tay_pol_cit[]={1, 1, 2, 17, 62, 1382, 21844, 929569, 6404582, 443861162, 18888466084, 113927491862, 58870668456604};
    long long int rozvoj_tay_pol_men[]={1, 3, 15, 315, 2835, 155925, 6081075, 638512875, 10854718875, 1856156927625, 194896477400625, 49308808782358125, 3698160658676859375};
    double sucet=0;
    double y=x;
    for (unsigned int i=0; i<n; i++)
    {
        sucet+=(y*rozvoj_tay_pol_cit[i])/rozvoj_tay_pol_men[i]; //y je x na n-tu (v citateli)
        y*=x*x;
    }
    return sucet;
}

//vypocet tangensu uhla pomocou zretazeneho zlomku
double cfrac_tan(double x, unsigned int n) //x = uhol v radianoch; n = pocet iteracii
{
    double y=0;
    double x_na_2=x*x;
    for (unsigned int i=n; i>1; i--) //od posledneho zlomku postupne hore
    {
        y=x_na_2/(i*2-1-y);
    }
    return x/(1-y);
}

//vypocet vzdialenosti
double f_vzdialenost(unsigned int pocet_iteracii, double alpha, double vyska_meraca)
{
    return vyska_meraca/cfrac_tan(alpha, pocet_iteracii);
}

//vypocet vysky
double f_vyska(unsigned int pocet_iteracii, double beta, double vzdialenost, double vyska_meraca)
{
    return cfrac_tan(beta, pocet_iteracii)*vzdialenost+vyska_meraca;
}

//vypis napovedy
void vypis_help()
{
    printf("--tan A N M\n"
           "A = uhol v radianoch; N a M = prva a posledna vypisana iteracia; 0 < N <= M < 14\n"
           "A je realne cislo, N a M su cele cisla.\n"
           "Porovna presnosti vypoctu tangensu uhlu medzi volanim funkcie tan z matematickej kniznice\n"
           "a vypoctom pomocou Taylorovho polynomu a zretazeneho zlomku. Format vypisu:\n"
           "I M T TE C CE\n"
           "I = Cislo iteracie\n"
           "M = Tan z matematickej kniznice\n"
           "T = Tan pomocou Taylorovho polynomu\n"
           "TE = Absolutna odchylka T voci M\n"
           "C = Tan pomocou zretazeneho zlomku\n"
           "CE = Absolutna odchylka C voci M\n"
           "\n"
           "[-c X] -m A [B]\n"
           "X = vyska v metroch; A a B = uhol v radianoch; 0 < A <= 1.4; 0 < B <= 1.4\n"
           "X, A a B  su realne cisla, [] oznacuju nepovinny argument, pre X je vychodzia hodnota 1.5 metra.\n"
           "Podla uhlu naklonu z urcitej vysky voci spodku objektu vypocita jeho vzdialenost\n"
           "a ak je zadany, podla druheho uhlu voci vrcholu objektu vypocita jeho vysku. Format vypisu:\n"
           "Vzdialenost objektu v metroch\n"
           "Vyska objektu v metroch\n");
}

double absolutna_odchylka(double cislo1, double cislo2)
{
    double rozdiel;
    rozdiel=cislo1-cislo2;
    if (rozdiel<0)
        return -rozdiel;
    else
        return rozdiel;
}

//vypis porovnania tangensov
void vypis_tang(char *argv[])
{
    double alpha=atof(argv[2]);
    unsigned int pocet_iteracii_min=atoi(argv[3]);
    unsigned int pocet_iteracii_max=atoi(argv[4]);
    double tan_a, taylor_tan_a, cfrac_tan_a;

    //pre interval iteracii vypisuje tangens pomocou matematickej kniznice, taylorovho polynomu,
    //ich absolutnu odchylku, zretazeneho zlomku, absolutnu odchylku
    for (; pocet_iteracii_min<=pocet_iteracii_max; pocet_iteracii_min++)
    {
        tan_a=tan(alpha);
        taylor_tan_a=taylor_tan(alpha, pocet_iteracii_min);
        cfrac_tan_a=cfrac_tan(alpha, pocet_iteracii_min);
        printf("%d %e %e %e %e %e\n", pocet_iteracii_min, tan_a, taylor_tan_a,
               absolutna_odchylka(tan_a, taylor_tan_a), cfrac_tan_a, absolutna_odchylka(tan_a, cfrac_tan_a));
    }
}

//vypis vzdialenosti /a vysky predmetu
void vypis_velkost(int argcheck_res, char *argv[])
{
    double vyska_meraca=1.5;
    double alpha, beta;
    double vzdialenost, vyska;
    unsigned int pocet_iteracii=13;

    switch (argcheck_res)
    {
    case 3:
        beta=atof(argv[5]);
    case 4:
        alpha=atof(argv[4]);
        vyska_meraca=atof(argv[2]);
        break;
    case 5:
        beta=atof(argv[3]);
    case 6:
        alpha=atof(argv[2]);
        break;
    }
    vzdialenost=f_vzdialenost(pocet_iteracii, alpha, vyska_meraca);
    printf("%.10e\n", vzdialenost);
    if (argcheck_res==3 || argcheck_res==5)
    {
        vyska=f_vyska(pocet_iteracii, beta, vzdialenost, vyska_meraca);
        printf("%.10e\n", vyska);
    }
}

/*
porovna tangensy uhla vypocitane roznymi sposobmi
pocita vzdialenost a vysku nejakeho objektu podla uhlu naklonu z urcitej vysky voci spodku a vrchu objektu
*/
int main(int argc, char *argv[])
{
    int argcheck_res=argcheck(argc, argv);

    if (argcheck_res==0)
        return 1;
    if (argcheck_res==1)
        vypis_help();
    else if (argcheck_res==2)
        vypis_tang(argv);
    else
        vypis_velkost(argcheck_res, argv);

    return 0;
}
