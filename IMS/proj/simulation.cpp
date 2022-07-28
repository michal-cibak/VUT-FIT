// Project: IMS - Epidemiologické modely - mikro-úroveň
// Authors: Michal Cibák - xcibak00
//          Mirka Kolaříková - xkolar76

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>
#include <simlib.h>
#include <math.h>
#include <getopt.h>

// parameters which can be changed to show the importance of cleaning hands
double prob_diddis = 0;
double prob_willdis = 0;
// can be changed, but it isn't needed (semi-constants)
int people_board_at_stop = 15;
int person_travels_for_stops = 5;
double prob_sick = 0.1;
double prob_holder = 0.9222;
double prob_toucher_in = 0.3814;
double prob_toucher_out = 0.5488;
double prob_sitter = 0.786;
double infection_produces = 30;
double infection_gives = 0.14;
double infection_takes = 0.21;
int disafter = 20;

// constants
const double trips = 20;
const double stops = 22;
const int seats_cnt = 44;
const int sspots_cnt = 156;
const int handles_cnt = 82;
const int poles_cnt = 4;
// probability of getting sick certain in TCID range
const double pgs_1_2 = 0.1378;
const double pgs_2_9 = 0.1929;
const double pgs_9_23 = 0.3439;
const double pgs_23_53 = 0.4113;
const double pgs_53_163 = 0.4967;
const double pgs_163_375 = 0.5961;

// for simulation
double handles_infection_lvl[handles_cnt] = {0};
double poles_infection_lvl[poles_cnt] = {0};

// for results of simulation
int traveled = 0;
int infectuous = 0;
int infected = 0;

Store seats("Seats", seats_cnt);
Store standing_spots("Spots to stand on", sspots_cnt);
Facility handles[handles_cnt];

class Person : public Process
{
    bool was_sick;
    bool did_disinfect;
    bool got_sick = false;
    bool will_disinfect;
    bool holder;
    bool toucher_in;
    bool toucher_out;
    bool sitter;
    int travel;
    bool on_board = false;
    bool sitting = false;
    int holding = -1;
    double infected_hands = 0;

    public:
        Person(int travel)
        {
            if (travel < person_travels_for_stops)
                this->travel = travel;
            else
                this->travel = person_travels_for_stops;

            was_sick = Random() < prob_sick;
            holder = Random() < prob_holder;
            toucher_in = Random() < prob_toucher_in;
            toucher_out = Random() < prob_toucher_out;
            sitter = Random() < prob_sitter;
            did_disinfect = Random() < prob_diddis;
            will_disinfect = Random() < prob_willdis;
            if (was_sick && !did_disinfect)
                infected_hands = infection_produces;
        }

    void touch()
    {
        int pick;
        while ((pick = floor(Uniform(0, poles_cnt))) == poles_cnt)
            ;
        double took = poles_infection_lvl[pick] * infection_takes;
        double gave = infected_hands * infection_gives;
        poles_infection_lvl[pick] -= took;
        poles_infection_lvl[pick] += gave;
        infected_hands -= gave;
        infected_hands += took;
    }

    void try_hold()
    {
        int free = 0;
        for (int i = 0; i < handles_cnt; i++)
            free++;
        if (free)
        {
            int pick;
            if (free == 1)
                pick = 1;
            else
                while ((pick = floor(Uniform(1, free + 1))) == free + 1)
                    ;
            free = 0;
            for (int i = 0; i < handles_cnt; i++)
            {
                if (!handles[i].Busy())
                {
                    free++;
                    if (free == pick)
                    {
                        Seize(handles[i]);
                        holding = i;
                        double took = handles_infection_lvl[holding] * infection_takes;
                        double gave = infected_hands * infection_gives;
                        handles_infection_lvl[holding] -= took;
                        handles_infection_lvl[holding] += gave;
                        infected_hands -= gave;
                        infected_hands += took;
                        break;
                    }
                }
            }
        }
    }

    void stand()
    {
        on_board = true;
        Enter(standing_spots, 1);
        sitting = false;
        if (holder)
        {
            try_hold();
        }
    }

    void sit()
    {
        on_board = true;
        Enter(seats, 1);
        sitting = true;
    }

    bool gets_sick()
    {
        if (!was_sick)
        {
            double prob_gets_sick;
            if (infected_hands < 1) prob_gets_sick = 0;
            else if (infected_hands >= 1 && infected_hands < 2) prob_gets_sick = pgs_1_2;
            else if (infected_hands >= 2 && infected_hands < 9) prob_gets_sick = pgs_2_9;
            else if (infected_hands >= 9 && infected_hands < 23) prob_gets_sick = pgs_9_23;
            else if (infected_hands >= 23 && infected_hands < 53) prob_gets_sick = pgs_23_53;
            else if (infected_hands >= 53 && infected_hands < 163) prob_gets_sick = pgs_53_163;
            else prob_gets_sick = pgs_163_375;
            return Random() < prob_gets_sick;
        }
        else
            return false;
    }

    void Behavior()
    {
        while (true)
        {
            if (on_board) // already traveling
            {
                travel--;
                if (travel) // not in destination
                {
                    Wait(0.0625); // wait for people to leave tram, but not for new people to board
                    if (sitting)
                    {
                        if (!sitter && !standing_spots.Full())
                        {
                            Leave(seats, 1);
                            stand();
                        }
                    }
                    else // standing
                    {
                        if (sitter && !seats.Full())
                        {
                            if (holding >= 0)
                            {
                                Release(handles[holding]);
                                holding = -1;
                            }
                            Leave(standing_spots, 1);
                            sit();
                        }
                        else // didn't sit
                            if (holder && holding == -1)
                                try_hold();
                    }
                    Wait(1 - 0.0625);
                    continue;
                }
                else // leave the tram
                {
                    if (sitting)
                    {
                        Leave(seats, 1);
                    }
                    else
                    {
                        if (holding >= 0)
                            Release(handles[holding]);
                        Leave(standing_spots, 1);
                    }
                    if (toucher_out)
                        touch();
                    if (!will_disinfect)
                        got_sick = gets_sick();
                    if (was_sick)
                        infectuous++;
                    if (got_sick)
                        infected++;
                    break;
                }
            }
            else // about to board
            {
                Wait(0.125); // wait for people to leave tram before entering
                if (!seats.Full() || !standing_spots.Full())
                {
                    if (toucher_in)
                        touch();
                    if (sitter)
                    {
                        if (!seats.Full())
                            sit();
                        else
                            stand();
                    }
                    else
                    {
                        if (!standing_spots.Full())
                            stand();
                        else
                            sit();
                    }
                    traveled++;
                    Wait(0.875);
                    continue;
                }
            }
        }
    }
};

class Tram : public Event
{
    int stops_left = stops;

    void Behavior()
    {
        stops_left--;
        if (stops_left)
        {
            for (int i = 0; i < people_board_at_stop; i++)
            {
                (new Person(stops_left))->Activate();
            }
            Activate(Time + 1);
        }
    }
};

class TramGen : public Event
{
    int trips_left = trips;
    int disinfect_in = disafter;

    void Behavior()
    {
        if (--disinfect_in == 0)
        {
            disinfect_in = disafter;
            for (int i = 0; i < poles_cnt; i++)
                poles_infection_lvl[i] = 0;
            for (int i = 0; i < handles_cnt; i++)
                handles_infection_lvl[i] = 0;
        }
        if (--trips_left)
        {
            (new Tram())->Activate();
            Activate(Time + stops);
        }
    }
};

int main(int argc, char const *argv[])
{
    int opt = 0;
    static struct option long_options[] = {
        {"people", 1, 0, 0}, // 0
        {"travel", 1, 0, 0}, // 1
        {"psick", 1, 0, 0}, // 2
        {"pholder", 1, 0, 0}, // 3
        {"ptouchin", 1, 0, 0}, // 4
        {"ptouchout", 1, 0, 0}, // 5
        {"psitter", 1, 0, 0}, // 6
        {"pdiddis", 1, 0, 0}, // 7
        {"pwilldis", 1, 0, 0}, // 8
        {"disafter", 1, 0, 0}, // 9
        {"addinf", 1, 0, 0}, // 10
        {"takeinf", 1, 0, 0}, // 11
        {"produce", 1, 0, 0}, // 12
        {0, 0, 0, 0}
    };
    int long_index = 0;
    while ((opt = getopt_long(argc, (char **)argv, "", long_options, &long_index )) != -1)
    {
        if (opt == 0)
            switch (long_index)
            {
                case 0 : people_board_at_stop = atoi(optarg);
                    break;
                case 1 : person_travels_for_stops = atoi(optarg);
                    break;
                case 2 : prob_sick = atof(optarg);
                    break;
                case 3 : prob_holder = atof(optarg);
                    break;
                case 4 : prob_toucher_in = atof(optarg);
                    break;
                case 5 : prob_toucher_out = atof(optarg);
                    break;
                case 6 : prob_sitter = atof(optarg);
                    break;
                case 7 : prob_diddis = atof(optarg);
                    break;
                case 8 : prob_willdis = atof(optarg);
                    break;
                case 9 : disafter = atoi(optarg);
                    break;
                case 10 : infection_gives = atof(optarg);
                    break;
                case 11 : infection_takes = atof(optarg);
                    break;
                case 12 : infection_produces = atoi(optarg);
                    break;
                default: fprintf(stderr, "Wrong program argument\n");
                    return 1;
            }
        else
        {
            fprintf(stderr, "Wrong program argument\n");
                return 1;
        }
    }

    double simtime = trips * stops;
    Init(0, simtime);
    (new TramGen)->Activate();
    Run();

    printf("Total people traveled: %d\nInfected prior to travel: %d\nNewly infected because of travel: %d\n", traveled, infectuous, infected);

    return 0;
}
