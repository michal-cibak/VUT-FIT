/*
Vypracoval Michal Cibak
Projekt 3 - Jednoducha shlukova analyza
*/

/**
 * Kostra programu pro 3. projekt IZP 2017/18
 *
 * Jednoducha shlukova analyza
 * Unweighted pair-group average
 * https://is.muni.cz/th/172767/fi_b/5739129/web/web/usrov.html
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h> // sqrtf
#include <limits.h> // INT_MAX
#include <string.h>

/*****************************************************************
 * Ladici makra. Vypnout jejich efekt lze definici makra
 * NDEBUG, napr.:
 *   a) pri prekladu argumentem prekladaci -DNDEBUG
 *   b) v souboru (na radek pred #include <assert.h>
 *      #define NDEBUG
 */
#ifdef NDEBUG
#define debug(s)
#define dfmt(s, ...)
#define dint(i)
#define dfloat(f)
#else

// vypise ladici retezec
#define debug(s) printf("- %s\n", s)

// vypise formatovany ladici vystup - pouziti podobne jako printf
#define dfmt(s, ...) printf(" - "__FILE__":%u: "s"\n",__LINE__,__VA_ARGS__)

// vypise ladici informaci o promenne - pouziti dint(identifikator_promenne)
#define dint(i) printf(" - " __FILE__ ":%u: " #i " = %d\n", __LINE__, i)

// vypise ladici informaci o promenne typu float - pouziti
// dfloat(identifikator_promenne)
#define dfloat(f) printf(" - " __FILE__ ":%u: " #f " = %g\n", __LINE__, f)

#endif

/*****************************************************************
 * Deklarace potrebnych datovych typu:
 *
 * TYTO DEKLARACE NEMENTE
 *
 *   struct obj_t - struktura objektu: identifikator a souradnice
 *   struct cluster_t - shluk objektu:
 *      pocet objektu ve shluku,
 *      kapacita shluku (pocet objektu, pro ktere je rezervovano
 *          misto v poli),
 *      ukazatel na pole shluku.
 */

struct obj_t {
    int id;
    float x;
    float y;
};

struct cluster_t {
    int size;
    int capacity;
    struct obj_t *obj;
};

int premium_case=0;

/*****************************************************************
 * Deklarace potrebnych funkci.
 *
 * PROTOTYPY FUNKCI NEMENTE
 *
 * IMPLEMENTUJTE POUZE FUNKCE NA MISTECH OZNACENYCH 'TODO'
 *
 */

/*
 Inicializace shluku 'c'. Alokuje pamet pro cap objektu (kapacitu).
 Ukazatel NULL u pole objektu znamena kapacitu 0.
*/
void init_cluster(struct cluster_t *c, int cap)
{
    assert(c != NULL);
    assert(cap >= 0);

    // TODO
    if (cap==0)
        c->obj=NULL;
    else
    {
        c->obj=malloc(cap*sizeof(struct obj_t));
        if (c->obj==NULL)
        {
            fprintf(stderr, "Zlyhanie alokacie!\n");
            c->size=-1; //pre kontrolu chybneho stavu
            return;
        }
    }
    c->size=0;
    c->capacity=cap;
}

/*
 Odstraneni vsech objektu shluku a inicializace na prazdny shluk.
 */
void clear_cluster(struct cluster_t *c)
{
    // TODO
    free(c->obj);
    init_cluster(c, 0);
}

/// Chunk of cluster objects. Value recommended for reallocation.
const int CLUSTER_CHUNK = 10;

/*
 Zmena kapacity shluku 'c' na kapacitu 'new_cap'.
 */
struct cluster_t *resize_cluster(struct cluster_t *c, int new_cap)
{
    // TUTO FUNKCI NEMENTE
    assert(c);
    assert(c->capacity >= 0);
    assert(new_cap >= 0);

    if (c->capacity >= new_cap)
        return c;

    size_t size = sizeof(struct obj_t) * new_cap;

    void *arr = realloc(c->obj, size);
    if (arr == NULL)
        return NULL;

    c->obj = (struct obj_t*)arr;
    c->capacity = new_cap;
    return c;
}

/*
 Prida objekt 'obj' na konec shluku 'c'. Rozsiri shluk, pokud se do nej objekt
 nevejde.
 */
void append_cluster(struct cluster_t *c, struct obj_t obj)
{
    // TODO
    if (c->size == c->capacity)
    {
        c=resize_cluster(c, c->capacity+CLUSTER_CHUNK);
        if (c==NULL)
        {
            fprintf(stderr, "Zlyhanie realokacie!\n");
            c->size=-1; //pre kontrolu chybneho stavu
            return;
        }
    }
    c->obj[c->size]=obj;
    c->size++;
}

/*
 Seradi objekty ve shluku 'c' vzestupne podle jejich identifikacniho cisla.
 */
void sort_cluster(struct cluster_t *c);

/*
 Do shluku 'c1' prida objekty 'c2'. Shluk 'c1' bude v pripade nutnosti rozsiren.
 Objekty ve shluku 'c1' budou serazeny vzestupne podle identifikacniho cisla.
 Shluk 'c2' bude nezmenen.
 */
void merge_clusters(struct cluster_t *c1, struct cluster_t *c2)
{
    assert(c1 != NULL);
    assert(c2 != NULL);

    // TODO
    //pridanie objektov
    for (int i=0; i<(c2->size); i++)
    {
        append_cluster(c1, c2->obj[i]);
        if (c1->size==-1) //chyba pripadnej realokacie miesta
            return;
    }

    //zoradenie objektov
    sort_cluster(c1);
}

/**********************************************************************/
/* Prace s polem shluku */

/*
 Odstrani shluk z pole shluku 'carr'. Pole shluku obsahuje 'narr' polozek
 (shluku). Shluk pro odstraneni se nachazi na indexu 'idx'. Funkce vraci novy
 pocet shluku v poli.
*/
int remove_cluster(struct cluster_t *carr, int narr, int idx)
{
    assert(idx < narr);
    assert(narr > 0);

    // TODO
    clear_cluster(&carr[idx]);
    while (idx<narr-1) //prvky napravo od idx posunie o jedno dolava
    {
        carr[idx]=carr[idx+1];
        idx++;
    }
    return narr-1;
}

/*
 Pocita Euklidovskou vzdalenost mezi dvema objekty.
 */
float obj_distance(struct obj_t *o1, struct obj_t *o2)
{
    assert(o1 != NULL);
    assert(o2 != NULL);

    // TODO
    float dist_x, dist_y;
    dist_x = o1->x - o2->x;
    dist_y = o1->y - o2->y;
    return sqrt(dist_x*dist_x+dist_y*dist_y);
}

/*
 Pocita vzdalenost dvou shluku.
*/
float cluster_distance(struct cluster_t *c1, struct cluster_t *c2)
{
    assert(c1 != NULL);
    assert(c1->size > 0);
    assert(c2 != NULL);
    assert(c2->size > 0);

    // TODO
    float dist;

    if (premium_case==1)
    {
        float dist_max=-INFINITY;
        for (int i=0; i < c1->size; i++)
        {
            for (int j=0; j < c2->size; j++)
            {
                dist=obj_distance(&c1->obj[i], &c2->obj[j]);
                if (dist>dist_max)
                    dist_max=dist;
            }
        }
        return dist_max;
    }
    else if (premium_case==-1)
    {
        float dist_min=INFINITY;
        for (int i=0; i < c1->size; i++)
        {
            for (int j=0; j < c2->size; j++)
            {
                dist=obj_distance(&c1->obj[i], &c2->obj[j]);
                if (dist<dist_min)
                    dist_min=dist;
            }
        }
        return dist_min;
    }
    else
    {
        float dist_sum=0;
        for (int i=0; i < c1->size; i++)
        {
            for (int j=0; j < c2->size; j++)
            {
                dist=obj_distance(&c1->obj[i], &c2->obj[j]);
                dist_sum+=dist;
            }
        }
        return dist_sum/(c1->size * c2->size);
    }
}

/*
 Funkce najde dva nejblizsi shluky. V poli shluku 'carr' o velikosti 'narr'
 hleda dva nejblizsi shluky. Nalezene shluky identifikuje jejich indexy v poli
 'carr'. Funkce nalezene shluky (indexy do pole 'carr') uklada do pameti na
 adresu 'c1' resp. 'c2'.
*/
void find_neighbours(struct cluster_t *carr, int narr, int *c1, int *c2)
{
    assert(narr > 0);

    // TODO
    if (narr==1)
    {
        fprintf(stderr, "V poli je len jeden zhluk, nie je mozne najst suseda!\n");
        *c1=0;
        *c2=-1; //pre kontrolu chyby
        return;
    }

    if (narr==2)
    {
        *c1=0;
        *c2=1;
        return;
    }

    float dist, min_dist;

    min_dist=INFINITY;
    for (int i=0; i<narr-1; i++)
    {
        for (int j=i+1; j<narr; j++)
        {
            dist=cluster_distance(&carr[i], &carr[j]);
            if (dist<min_dist)
            {
                min_dist=dist;
                *c1=i;
                *c2=j;
            }
        }
    }
}

// pomocna funkce pro razeni shluku
static int obj_sort_compar(const void *a, const void *b)
{
    // TUTO FUNKCI NEMENTE
    const struct obj_t *o1 = (const struct obj_t *)a;
    const struct obj_t *o2 = (const struct obj_t *)b;
    if (o1->id < o2->id) return -1;
    if (o1->id > o2->id) return 1;
    return 0;
}

/*
 Razeni objektu ve shluku vzestupne podle jejich identifikatoru.
*/
void sort_cluster(struct cluster_t *c)
{
    // TUTO FUNKCI NEMENTE
    qsort(c->obj, c->size, sizeof(struct obj_t), &obj_sort_compar);
}

/*
 Tisk shluku 'c' na stdout.
*/
void print_cluster(struct cluster_t *c)
{
    // TUTO FUNKCI NEMENTE
    for (int i = 0; i < c->size; i++)
    {
        if (i) putchar(' ');
        printf("%d[%g,%g]", c->obj[i].id, c->obj[i].x, c->obj[i].y);
    }
    putchar('\n');
}

//Dealokuje polia objektov v zhlukoch a pole zhlukov.
void clear_cluster_array(struct cluster_t **arr, int clusters_with_objets)
{
    for (int i=0; i<clusters_with_objets; i++) //dealokacia nacitanych objektov
    {
        free(&(*arr)[i].obj);
    }
    free(*arr); //dealokacia pola zhlukov
    *arr=NULL;
}

/*
 Ze souboru 'filename' nacte objekty. Pro kazdy objekt vytvori shluk a ulozi
 jej do pole shluku. Alokuje prostor pro pole vsech shluku a ukazatel na prvni
 polozku pole (ukalazatel na prvni shluk v alokovanem poli) ulozi do pameti,
 kam se odkazuje parametr 'arr'. Funkce vraci pocet nactenych objektu (shluku).
 V pripade nejake chyby uklada do pameti, kam se odkazuje 'arr', hodnotu NULL.
*/
int load_clusters(char *filename, struct cluster_t **arr)
{
    assert(arr != NULL);

    // TODO
    FILE *subor;

    //otvorenie suboru
    subor=fopen(filename, "r");
    if (subor==NULL)
    {
        fprintf(stderr, "Chyba pri otvarani suboru!\n");
        *arr=NULL;
        return 0;
    }

    const int max_chars_in_line=30; //max-1 ocakavanych znakov v riadku
    char line[max_chars_in_line];

    //nacitanie prveho riadku
    if (fgets(line, max_chars_in_line, subor)==NULL)
    {
        fprintf(stderr, "Chyba pri nacitavani 1. riadku zo suboru!\n");
        *arr=NULL;
        return 0;
    }

    int obj_count;

    //zistenie poctu objektov a kontrola
    if (sscanf(line, "count=%d\n", &obj_count)!=1)
    {
        fprintf(stderr, "Chyba pri citani udajov z 1. riadku v subore!\n");
        *arr=NULL;
        return 0;
    }
    if (obj_count<1)
    {
        fprintf(stderr, "Chybny pocet objektov v subore!\n");
        *arr=NULL;
        return 0;
    }

    //alokacia miesta pre pole zhlukov
    *arr=malloc(obj_count*sizeof(struct cluster_t));
    if (*arr==NULL)
    {
        fprintf(stderr, "Zlyhanie alokacie!\n");
        //netreba nastavit NULL, urobi to malloc pri chybe
        return 0;
    }

    struct obj_t object;

    for (int i=0; i<obj_count; i++) //pre pocet objektov
    {
        //nacitanie riadku
        if (fgets(line, 30, subor)==NULL)
        {
            fprintf(stderr, "Chyba pri nacitavani %d. riadku zo suboru!\n", i+2);
            clear_cluster_array(arr, i); //dealokuje pole zhlukov a jeho objekty, nastavi *arr na NULL
            return i;
        }

        //precitanie riadku a zapis objektu
        if (sscanf(line, "%d %g %g\n", &object.id, &object.x, &object.y)!=3) //mozno by chcelo kontrolu == id
        {
            fprintf(stderr, "Chyba pri citani udajov z %d. riadku v subore!\n", i+2);
            clear_cluster_array(arr, i); //dealokuje pole zhlukov a jeho objekty, nastavi *arr na NULL
            return i;
        }
        if (object.id<0 || object.x<0 || object.y<0 || object.x>1000 || object.y>1000)
        {
            fprintf(stderr, "Udaj z %d. riadku v subore je mimo limitov!\n", i+2);
            clear_cluster_array(arr, i); //dealokuje pole zhlukov a jeho objekty, nastavi *arr na NULL
            return i;
        }

        //inicialiazacia zhluku, priradenie objektu do prislusneho zhluku v poli zhlukov
        init_cluster(&(*arr)[i], 1); //alokacia objektu pre jednotlivy zhluk
        if ((*arr)[i].size==-1) //ak zlyhala
        {
            clear_cluster_array(arr, i); //dealokuje pole zhlukov a jeho objekty, nadstavi *arr na NULL
            return i;
        }
        append_cluster(&(*arr)[i], object);//priradenie objektu na koniec zhluku
        //nedochadza k realokacii, nemoze nastat chyba
    }

    //zatvorenie suboru
    if (fclose(subor)==EOF)
    {
        fprintf(stderr, "Chyba pri zatvarani suboru!\n");
        clear_cluster_array(arr, obj_count); //dealokuje pole zhlukov a jeho objekty, nadstavi *arr na NULL
        return obj_count;
    }

    return obj_count;
}

/*
 Tisk pole shluku. Parametr 'carr' je ukazatel na prvni polozku (shluk).
 Tiskne se prvnich 'narr' shluku.
*/
void print_clusters(struct cluster_t *carr, int narr)
{
    printf("Clusters:\n");
    for (int i = 0; i < narr; i++)
    {
        printf("cluster %d: ", i);
        print_cluster(&carr[i]);
    }
}

int main(int argc, char *argv[])
{
    struct cluster_t *clusters;

    // TODO
    //kontrola argumentov
    if (argc==1)
    {
        fprintf(stderr, "Neboli zadane argumenty!\n");
        return 1;
    }
    if (argc>4)
    {
        fprintf(stderr, "Bolo zadanych prilis vela argumentov!\n");
        return 1;
    }

    int objective_cluster_count;

    //nacitanie nazvu suboru, poctu zhlukov a pripadny typ zhlukovania
    if (argc>2)
    {
        if (sscanf(argv[2], "%d", &objective_cluster_count)!=1)
        {
            fprintf(stderr, "Chyba pri citani argumentu!\n");
            return 1;
        }
        if (objective_cluster_count<1)
        {
            fprintf(stderr, "Chybny argument poctu zhlukov!\n");
            return 1;
        }
        if (argc==4)
        {
            if (strcmp(argv[3], "--min")==0)
                premium_case=-1;
            else if (strcmp(argv[3], "--max")==0)
                premium_case=1;
            else if (strcmp(argv[3], "--avg")!=0)
            {
                fprintf(stderr, "Chybny argument metody zhlukovania!\n");
                return 1;
            }
        }
    }
    else objective_cluster_count=1;

    int loaded_objects;

    //vytvori pole zhlukov a prida don objekty, kontrola
    loaded_objects=load_clusters(argv[1], &clusters);
    if (clusters==NULL)
        return 1;
    if (objective_cluster_count>loaded_objects) //moznost dosiahnutia poctu chcenych zhlukov
    {
        fprintf(stderr, "Zadany pocet zhlukov je viac ako objektov!\n");
        clear_cluster_array(&clusters, loaded_objects);
        return 1;
    }

    //pripadne zhlukovanie
    if (objective_cluster_count<loaded_objects)
    {
        int cluster_count=loaded_objects;

        if (objective_cluster_count==1)
        {
            //zluci vsetky zhluky do jedneho a odstrani zvysne
            for (int i=cluster_count-1; cluster_count>1; i--)
            {
                merge_clusters(&clusters[0], &clusters[i]);
                if (clusters[0].size==-1)
                {
                    clear_cluster_array(&clusters, cluster_count);
                    return 1;
                }
                cluster_count=remove_cluster(clusters, cluster_count, i);
            }
        }
        else
        {
            int index1, index2;

            //najde susedne zhluky, zluci ich, odstrani nepotrebny, az kym nebude pozadovany pocet zhlukov
            do
            {
                find_neighbours(clusters, cluster_count, &index1, &index2);
                if (index2==-1) //nie je mozne najst suseda
                {
                    clear_cluster_array(&clusters, cluster_count);
                    return 1;
                }
                merge_clusters(&clusters[index1], &clusters[index2]);
                if (clusters[index1].size==-1)
                {
                    clear_cluster_array(&clusters, cluster_count);
                    return 1;
                }
                cluster_count=remove_cluster(clusters, cluster_count, index2);
            }
            while (cluster_count>objective_cluster_count);
        }
    }

    //vypis pola zhlukov
    print_clusters(clusters, objective_cluster_count);

    //dealokacia objektov a pola zhlukov
    clear_cluster_array(&clusters, objective_cluster_count);

    return 0;
}
