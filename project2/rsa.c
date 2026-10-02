#include <math.h>
#include <stdio.h>
#include <stdlib.h>

long long mod_add(long long x, long long y, long long n) {
    if (x >= n - y) {
        return x - (n - y);
    }

    return x + y;
}

long long mod_mult(long long a, long long b, long long n) {
    long long result = 0;

    a = a % n;

    while (b > 0) {

        if ((b % 2) == 1) {
            result = mod_add(result, a, n);
        }

        // Dubblar a, 2a
        a = mod_add(a, a, n);

        // Halverar b, b / 2
        b = b / 2;
    }

    return result;
}

long long square_and_multiply(long long c, long long d, long long n) {
    long long m = 1;

    c = c % n;

    while (d > 0) {

        if ((d % 2) == 1) {
            m = mod_mult(m, c, n);
        }

        c = mod_mult(c, c, n);

        d = d / 2;
    }

    return m;
}

/**
 * @brief Hittar alla primtal från 2 till n.
 *
 * Funktionen använder Sieve of Eratosthenes algoritm för att markera alla tal
 * som inte är primtal. Därefter räknas primtalen och sparas i en dynamisk
 * allokerad array som returneras. Antalet primtal sparas i prime_count.
 *
 * @param n                 Övre gränsen för vilka primtal som ska hittas.
 * @param prime_count       Pekare där antalet hittade primtal sparas.
 * @return int*             Pekare till en array som innehåller primtalen, eller
 *                          NULL om minnesallokeringen misslyckades
 */
int *sieves(int n, int *prime_count) {
    char *arr = malloc((n + 1) * sizeof(char));

    if (arr == NULL) {
        printf("Memoty error: ");
        return NULL;
    }

    // 0 = omarkerad
    // 1 = markerad
    for (int i = 0; i <= n; i++) {
        arr[i] = 0;
    }

    int p = 2;

    while (p <= n) {
        // Markera multiplar av p
        for (int i = 2 * p; i <= n; i += p) {
            arr[i] = 1;
        }

        int next = p + 1;

        while (next <= n && arr[next] == 1)

        {
            next++;
        }

        if (next > n) {
            break;
        }

        p = next;
    }

    *prime_count = 0;

    for (int i = 2; i <= n; i++) {
        if (arr[i] == 0) {
            (*prime_count)++;
        }
    }

    int *primes = malloc(*prime_count * sizeof(int));

    if (primes == NULL) {
        free(arr);
        return NULL;
    }

    int index = 0;

    for (int i = 2; i <= n; i++) {
        if (arr[i] == 0) {
            primes[index] = i;
            index++;
        }
    }

    free(arr);

    return primes;
}

/**
 * @brief Hittar primtalsfaktorerna p och q till n.
 *
 * Funktionen genererar alla primtal upp till sqrt(n) mha av sieves().
 * Därefter testas primtalen ett i taget för att hitta ett primtal som delar n
 * utan rest (rest == 0). När en faktor hittas sparas den i p och den andra
 * faktorn beräknas som n / p och sparas i q.
 *
 * @param n     Talet som ska faktoriseras.
 * @param p     Pekare där den första primtalsfaktorn sparas.
 * @param q     Pekare där den andra primtalsfaktorn sparas.
 */
void find_pq(long long n, long long *p, long long *q) {
    int limit = (int)sqrt(n);

    int prime_count;

    int *primes = sieves(limit, &prime_count);

    if (primes == NULL) {
        return;
    }

    for (int i = 0; i < prime_count; i++) {
        if ((n % primes[i]) == 0) {
            *p = primes[i];
            *q = n / primes[i];

            free(primes);
            return;
        }
    }

    free(primes);
}

/**
 * @brief Beräknar Eulers phi-funktion för n = p * q.
 *
 * Funktionen beräknar phi(n) med formlen (p - 1) * (q - 1), där p och q är
 * primtalsfaktorerna till n. Resultatet sparas på motsvarande position i
 * phi-arrayen.
 *
 * @param phi       Array där det beräknade phy-värdet sparas.
 * @param p         Array med den första primtalsfaktorn.
 * @param q         Array med den andra primtalsfaktorn.
 * @param index     Index för den RSA-nyckel som ska användas.
 */
void euler_phi(long long *phi, long long *p, long long *q, int index) {
    phi[index] = (p[index] - 1) * (q[index] - 1);
}

/**
 * @brief Beräknar den privata RSA-exponenten d.
 *
 * Funktionen använder den utökade Euklidiska algoritmen för att hitta den
 * modulära inversen till e modulo phi. Resultatet d uppfyller villkoret ed ≡ 1
 * (mod phi). Om inversen blir negativ görs den positiv.
 *
 * @param e             Den publika exponenten.
 * @param phi           Värdet av Eulers phi-funktion.
 * @return long long    Den privata exponenten d.
 */
long long find_d(long long e, long long phi) {
    long long remainder = e;
    long long prev_remainder = phi;

    long long e_coefficient = 1;
    long long prev_e_coefficient = 0;

    while (remainder > 1) {
        long long quotient = prev_remainder / remainder;

        long long new_remainder = prev_remainder % remainder;

        prev_remainder = remainder;
        remainder = new_remainder;

        long long new_e_coefficient =
            prev_e_coefficient - quotient * e_coefficient;

        prev_e_coefficient = e_coefficient;
        e_coefficient = new_e_coefficient;
    }

    // Kontroll så vi inte får den negativa modulära inversen
    if (e_coefficient < 0) {
        e_coefficient = e_coefficient + phi;
    }

    return e_coefficient;
}

/**
 * @brief Dekrypterar ett RSA-krypterat tal.
 *
 * Funktionen dekrypterar ciphertext c genom att beräkna c upphöjt till den
 * privata exponenten d modulo n.
 *
 * @param c             Det krypterade talet.
 * @param d             Den privata RSA-exponenten.
 * @param n             RSA-modulus.
 * @return long long    Det dekrypterade talet.
 */
long long decrypt(long long c, long long d, long long n) {
    return square_and_multiply(c, d, n);
}

/**
 * @brief Omvandlar dekrypterade tal till en textsträng.
 *
 * Funktionen delar upp varje tal i message i fyra bytes. Varje byte motsvarar
 * ett tecken och sparas i text-arrayen. När alla tal har omvandlas avslutas
 * textsträngen med null-tecknet '\0'.
 *
 * @param message   Arrayen med de dekrypterade talen.
 * @param size      Antalet tal i message-arrayen.
 * @param text      Array där den färdiga textsträngen sparas.
 */
void message_to_char(long long message[], int size, char text[]) {
    int index = 0;

    for (int i = 0; i < size; i++) {
        text[index++] = (message[i] >> 24) & 0xFF;
        text[index++] = (message[i] >> 16) & 0xFF;
        text[index++] = (message[i] >> 8) & 0xFF;
        text[index++] = message[i] & 0xFF;
    }

    text[index] = '\0';
}

int main() {
    // Message 1
    long long c1[] = {
        149649839764178808LL, 252878538982056735LL, 497934955293207178LL,
        172648785482619635LL, 656607680354881715LL, 638801707986766030LL,
        515247709215559273LL, 710918966244821582LL, 192468062756311027LL,
        9857247048729515LL,   375608426694197877LL, 683311645781965742LL,
        224149347182834745LL, 731151269698728041LL, 659228660695071059LL,
        47624357183919359LL,  540870172293647063LL, 614705883029905109LL,
        553937957263353797LL, 775165508327422892LL, 197386967122131333LL,
        323082446252707101LL, 687778510075363903LL, 731661625102635212LL,
        192702468501772915LL, 359097889006849696LL, 749565003456467142LL,
        26174935616554115LL,  306236794508340164LL, 113079711660153090LL,
        251358301954265662LL, 676614554993934600LL, 57617725644332551LL,
        46135350444887208LL,  254302295789074656LL, 178155434647538125LL,
        427901145481157506LL, 461530037442965084LL, 612649437365205454LL,
        359650987237347253LL, 426434063409358389LL, 434191866497008369LL,
        384886512332338493LL, 183502994621770100LL, 292541991811658094LL,
        157628905030165664LL, 230979715213592711LL, 533118943241338625LL,
        267219666219457345LL, 281833040987207736LL, 430645665122274838LL,
        746276946213955295LL, 282996664733493235LL, 638801707986766030LL,
        515247709215559273LL, 451789698862589153LL, 783213498743374062LL,
        339357196107245275LL, 694835634213445646LL, 644843105223251373LL,
        515247709215559273LL, 687815738898642098LL, 692011310721020586LL,
        386448796194002451LL, 734751054295505502LL, 431084152163717848LL,
        549229503942638497LL, 60576729926926220LL,  575859564737248714LL,
        182204173668978979LL, 97145339708494214LL,  76940569558610991LL,
        52710021803156139LL,  649370366276498734LL, 275210229684020263LL,
        244730593444280918LL, 101375331927309202LL, 536082723542177060LL,
        4025394718179726LL,   152033735304391266LL, 521197021991369LL,
        779184264168806915LL, 98485624713583567LL,  304030094821681015LL,
        516426167448936609LL, 88292810820565161LL,  261175243466460612LL,
        449218780126100008LL, 230979715213592711LL, 252878538982056735LL,
        497934955293207178LL, 728579167394600308LL};

    // Message 2
    long long c2[] = {
        641687406837544932LL, 538059401358101721LL, 22860130891370151LL,
        891895469775487398LL, 28858935852448055LL,  393144359821148017LL,
        691848956514697563LL, 792170944230739092LL, 175793919989175719LL,
        511894394233323177LL, 373932905875529730LL, 322467976207696187LL,
        55285804026575045LL,  275353880747066735LL, 388858618025135139LL,
        326584868220856398LL, 389929170738906748LL, 589323799392346933LL,
        354674042601910578LL, 32642706434258298LL,  151931731952525683LL,
        756891698848309350LL, 493959320336134251LL, 423415648952955689LL,
        875469044972077224LL, 860430305875992906LL, 618241459003519983LL,
        816912511667380656LL, 159957709352850432LL, 209229058673745783LL,
        859314772184645047LL, 238559654616755901LL, 678671613721836374LL,
        754053418753724319LL, 263029683400666671LL, 189027441288584305LL,
        823455478724860213LL, 173837753669822946LL, 733116310975764869LL,
        608525911792145862LL, 290459247836336314LL, 534633900673962386LL,
        490455400164159081LL, 755104272126608181LL, 97893291255322399LL,
        86157489453384282LL,  45016147752515981LL,  339727671566674086LL,
        676355595693669976LL, 844689279880831675LL, 415397633707664025LL,
        762611399080006323LL, 447258377633082098LL, 28858935852448055LL,
        393144359821148017LL, 236935079904908502LL, 26577381028091707LL,
        48869286267506361LL,  611621885056301791LL, 374411854661032617LL,
        680759203720648750LL, 480513269176534263LL, 652289536116888681LL,
        22131430410379122LL,  798323349302966869LL, 603164452936206251LL,
        738325996215091129LL, 725423458123899138LL, 435984937664752937LL,
        801526073331305541LL, 21225118317526002LL,  394611939242304595LL,
        863818940779196870LL, 28858935852448055LL,  179574067175888644LL,
        397035631558513749LL, 840528405199812023LL, 342807990378507765LL,
        858594930476779769LL, 487154777426112658LL, 250534309987430579LL,
        456679589228011192LL, 180510112944673479LL, 299450608777177368LL,
        608525911792145862LL, 351429717078587976LL, 855670199427431726LL,
        343357513816365861LL, 591309747677877761LL, 352919160011014006LL,
        31398075847806594LL,  379157127259015998LL, 97341194601454864LL,
        827221120418699817LL, 537021451556077643LL};

    // Message 3
    long long c3[] = {
        343022136742801393LL, 543852162920763953LL, 187662346600663868LL,
        64363741519788173LL,  530303735679904871LL, 665748278601844403LL,
        251521554552616820LL, 772265008608952343LL, 612649437365205454LL,
        52702162195369680LL,  594693925336595248LL, 49390200678220701LL,
        366625234217684477LL, 350378143640882684LL, 782047298346299518LL,
        191265307147848763LL, 125000439880533632LL, 542469522607943309LL,
        688564881563644950LL, 121135777831732587LL, 325032373376049681LL,
        439638695873655713LL, 790219865083922791LL, 674348469799700949LL,
        368802023118333328LL, 478816896516334310LL, 687778510075363903LL,
        113203431019095987LL, 437725636262778729LL, 755544148240688053LL,
        732100597910242041LL, 458362016448179465LL, 361708786135651399LL,
        766206662153808508LL, 102763653042723240LL, 475198569334934081LL,
        656703212739094544LL, 738297221935904896LL, 590881859058962299LL,
        35459181501091455LL,  701978423236256338LL, 1294046624381011LL,
        676089688458350978LL, 688564881563644950LL, 506963823608262404LL,
        99989241250781301LL,  729139540893776168LL, 170527995115218386LL,
        23616823557532025LL,  88292810820565161LL,  422788196153082785LL,
        245127398743516477LL, 522242924778961661LL, 536993513112248503LL,
        469945482566314862LL, 186863629046296186LL, 535274533673347536LL,
        122165884230241857LL, 243854567102407016LL, 193846426978713870LL,
        558576193224827693LL, 4781392657808285LL,   303271003704302882LL,
        766988753915136948LL, 190784188215985015LL, 635725552430915544LL,
        480171557257896357LL, 691703433180958986LL, 444657327837069481LL,
        411681019428870145LL, 379987262796286170LL, 33223172431212507LL,
        473890692205586133LL, 90601069869485799LL,  459480340654067846LL,
        557932582091151668LL, 29797590491418802LL,  4237984428275562LL,
        3119342738192712LL,   410077437256529329LL, 207360838019755933LL,
        490260594761408591LL, 40740318826488055LL,  603226382109056190LL,
        224244051091863530LL, 613354849827747550LL, 535908433555613956LL,
        7453979465096143LL,   503842863128411748LL};

    // Storleken på arrayerna c1, c2 och c3
    int c1_size = sizeof(c1) / sizeof(c1[0]);
    int c2_size = sizeof(c2) / sizeof(c1[0]);
    int c3_size = sizeof(c3) / sizeof(c3[0]);

    long long message1[c1_size];
    long long message2[c2_size];
    long long message3[c3_size];

    int key = 4;

    long long c = 149649839764178808;
    long long p[] = {0, 0, 0, 0, 0, 0};
    long long q[] = {0, 0, 0, 0, 0, 0};

    int e[] = {
        7, 23, 7, 19, 29, 29,
    };

    long long n[] = {391,
                     100289621329340257LL,
                     882238272068111039LL,
                     182469164307407143LL,
                     799710404000289581LL,
                     901082142384103049LL};

    long long phi[] = {0, 0, 0, 0, 0, 0};

    find_pq(n[key], &p[key], &q[key]);
    euler_phi(phi, p, q, key);

    long long d = find_d(e[key], phi[key]);

    for (int i = 0; i < c1_size; i++) {
        message1[i] = decrypt(c1[i], d, n[key]);
    }

    for (int i = 0; i < c2_size; i++) {
        message2[i] = decrypt(c2[i], d, n[key]);
    }

    for (int i = 0; i < c3_size; i++) {
        message3[i] = decrypt(c3[i], d, n[key]);
    }

    printf("\nMessage 1:\n");
    for (int i = 0; i < c1_size; i++) {
        printf("%08llX ", message1[i]);
    }

    printf("\n\nMessage 2:\n");
    for (int i = 0; i < c2_size; i++) {
        printf("%08llX ", message2[i]);
    }

    printf("\n\nMessage 3:\n");
    for (int i = 0; i < c3_size; i++) {
        printf("%08llX ", message3[i]);
    }

    char text1[c1_size * 4 + 1];
    char text2[c2_size * 4 + 1];
    char text3[c3_size * 4 + 1];

    message_to_char(message1, c1_size, text1);
    message_to_char(message2, c2_size, text2);
    message_to_char(message3, c3_size, text3);

    printf("\nMessage 1:\n%s\n", text1);
    printf("Message 2:\n%s\n", text2);
    printf("Message 3:\n%s\n", text3);

    printf("\n");

    return 0;
}