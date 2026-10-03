#include <inttypes.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

/** @brief Alias för ett 64-bitars unsigned heltal.
 *
 * Kan lagra positiva heltal från 0 till 2^64 - 1.
 */
typedef uint64_t u64;

/**
 * @brief Alias för ett 64-bitars signed heltal.
 *
 * Kan lagra heltal från -2^63 till 2^63 - 1.
 *
 */
typedef int64_t i64;

/**
 * @brief Beräknar summan av två tal mod n utan overflow
 *
 * Funktionen beräknar ((x+y) mod n) utan att direkt utföra
 * additionen x + y. Detta förhindrar overflow när talen är
 * för stora
 *
 * @param x Det första talet
 * @param y Det andra talet
 * @param n Modulus som x + y utförs mod
 * @return Summan (x+y) mod n
 */
u64 mod_add(u64 x, u64 y, u64 n) {
    if (x >= n - y) {
        return x - (n - y);
    }

    return x + y;
}

/**
 * @brief Beräknar produkten av två tal mod n
 *
 * Funktionen beräknar: (a * b) mod n, utan att direkt
 * multiplicera a och b. Detta görs för att undvika overflow.
 *
 * Algoritmen använder double-and-add. Vid varje iteration
 * kontrolleras om b är udda. Om b är udda läggs den aktuella
 * versionen av a till resultatet. Därefter dubblas a och b halveras.
 *
 * @param a Det första talet
 * @param b Det andra talet
 * @param n Modulus
 * @return (a * b) mod n
 */
u64 mod_mult(u64 a, u64 b, u64 n) {

    // Resultatet byggs upp stegvis
    u64 result = 0;

    // Minska a mod n innan multi. börjar
    a = a % n;

    while (b > 0) {
        /**
         * Om b är udda ska den aktuella versionen av a
         * multipliceras in i resultatet.
         */

        if ((b % 2) == 1) {
            result = mod_add(result, a, n);
        }

        /**
         * Dubblar a mod n, detta motsvarar (a = 2a mod n)
         * mod_add används för att undvika overflow
         */
        a = mod_add(a, a, n);

        // Halverar b för nästa iteration
        b = b / 2;
    }

    return result;
}

/**
 * @brief Beräknar modulär exponentiering med square-and-multiply
 *
 * Funktionen beräknar: c^d mod n, utan att först beräkna potensen c^d.
 *
 * Algoritmen använder square-and-multiply. Exponenten d behandlas bit
 * för bit genom att kontrollera om den är udda, därefter kvadreras
 * basen och exponenten halveras.
 *
 * @param c Basen som ska exponentieras
 * @param d Exponenten
 * @param n Modulus
 * @return c^d mod n
 */
u64 square_and_multiply(u64 c, u64 d, u64 n) {

    /**
     * m innehåller det resultat som byggs upp under algoritmens gång.
     *
     * Vi börjar med 1 eftersom 1 är ett neutralt element vid multi.
     *
     */
    u64 m = 1;

    // Minskar basen med mod n
    c = c % n;

    while (d > 0) {
        /**
         * Om d är udda ska den aktuella potensen av c
         * multipliceras in i resultatet.
         */
        if ((d % 2) == 1) {
            m = mod_mult(m, c, n);
        }

        // c² mod n
        c = mod_mult(c, c, n);

        // d halveras
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
void find_pq(u64 n, u64 *p, u64 *q) {
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
 * @param p         Array med den första primtalsfaktorn.
 * @param q         Array med den andra primtalsfaktorn.
 */
u64 euler_phi(u64 p, u64 q) {
    return (p - 1) * (q - 1);
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
 * @return              Den privata exponenten d.
 */
u64 find_d(u64 e, u64 phi) {
    i64 remainder = e;
    i64 prev_remainder = phi;

    i64 e_coefficient = 1;
    i64 prev_e_coefficient = 0;

    while (remainder > 1) {
        i64 quotient = prev_remainder / remainder;

        i64 new_remainder = prev_remainder % remainder;

        prev_remainder = remainder;
        remainder = new_remainder;

        i64 new_e_coefficient = prev_e_coefficient - quotient * e_coefficient;

        prev_e_coefficient = e_coefficient;
        e_coefficient = new_e_coefficient;
    }

    // Kontroll så vi inte får den negativa modulära inversen
    if (e_coefficient < 0) {
        e_coefficient = e_coefficient + (i64)phi;
    }

    return (u64)e_coefficient;
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
u64 decrypt(u64 c, u64 d, u64 n) {
    return square_and_multiply(c, d, n);
}

/**
 * @brief Läser och dekrypterar ett RSA-krypterat meddelande från en fil.
 *
 * Funktionen läser ett krypterat tal i taget från filen, dekrypterar talet och
 * delar sedan upp det dekrypterade talet i fyra bytes.
 * Varje byte skrivs direkt till terminalen som ett tecken
 *
 * @param filename  Namnet på filen som innehåller det kryterade meddelandet.
 * @param d         Den privata RSA-exponenten.
 * @param n         RSA-modulus
 */
void decrypt_file(const char *filename, u64 d, u64 n) {
    FILE *file = fopen(filename, "r");

    if (file == NULL) {
        printf("Could not open file: %s\n", filename);
        return;
    }

    u64 c;

    while (fscanf(file, "%" SCNu64, &c) == 1) {
        u64 m = decrypt(c, d, n);

        putchar((m >> 24) & 0xFF);
        putchar((m >> 16) & 0xFF);
        putchar((m >> 8) & 0xFF);
        putchar(m & 0xFF);
    }

    putchar('\n');

    fclose(file);
}

int main() {
    /**
     * 0 = beräkna endast vald nyckel
     * 1 = beräkna alla nycklar
     */
    int calculate_all = 0;

    // Antal RSA-nycklar inklusive Solve by hand
    const int key_count = 6;

    /**
     * Välj vilken nyckel som ska användas för dekrypteringen.
     *
     * 0 = Key 0 (Solve by hand)
     * 1 = Key 1
     * 2 = Key 2
     * 3 = Key 3
     * 4 = Key 4
     * 5 = Key 5
     */
    int key = 4;

    // Publika exponenter
    u64 e[] = {
        7, 23, 7, 19, 29, 29,
    };

    // RSA-modulus
    u64 n[] = {391,
               100289621329340257LL,
               882238272068111039LL,
               182469164307407143LL,
               799710404000289581LL,
               901082142384103049LL};

    // Arrayer för de värden som beräknas för varje nyckel
    u64 p[key_count];
    u64 q[key_count];
    u64 phi[key_count];
    u64 d[key_count];

    // Beräkna p, q, phi(n) och d för alla nycklar

    if (calculate_all) {
        for (int i = 0; i < key_count; i++) {
            p[i] = 0;
            q[i] = 0;

            find_pq(n[i], &p[i], &q[i]);

            phi[i] = euler_phi(p[i], q[i]);

            d[i] = find_d(e[i], phi[i]);
        }

        // Skriv ut alla RSA-nycklar i en tabell
        printf("\nRSA keys\n");

        printf("---------------------------------------------------------------"
               "----"
               "--------------------------------------------\n");

        printf("%-5s %-4s %-20s %-12s %-12s %-20s %-20s\n", "Key", "e", "n",
               "p", "q", "phi(n)", "d");

        printf("---------------------------------------------------------------"
               "----"
               "--------------------------------------------\n");

        for (int i = 0; i < key_count; i++) {
            printf("%-5d %-4" PRIu64 " %-20" PRIu64 " %-12" PRIu64
                   " %-12" PRIu64 " %-20" PRIu64 " %-20" PRIu64 "\n",
                   i, e[i], n[i], p[i], q[i], phi[i], d[i]);
        }

        printf("---------------------------------------------------------------"
               "----"
               "--------------------------------------------\n");
    } else {
        p[key] = 0;
        q[key] = 0;

        find_pq(n[key], &p[key], &q[key]);

        phi[key] = euler_phi(p[key], q[key]);
        d[key] = find_d(e[key], phi[key]);

        // Skriv ut den valda nyckeln
        printf("\nKey %d\n", key);
        printf("e      = %" PRIu64 "\n", e[key]);
        printf("n      = %" PRIu64 "\n", n[key]);
        printf("p      = %" PRIu64 "\n", p[key]);
        printf("q      = %" PRIu64 "\n", q[key]);
        printf("phi(n) = %" PRIu64 "\n", phi[key]);
        printf("d      = %" PRIu64 "\n", d[key]);

        printf("\n");

        // Läs, dekryptera och skriv ut meddelandena
        printf("\nMessage 1:\n");
        decrypt_file("message1.txt", d[key], n[key]);

        printf("\nMessage 2:\n");
        decrypt_file("message2.txt", d[key], n[key]);

        printf("\nMessage 3:\n");
        decrypt_file("message3.txt", d[key], n[key]);
    }

    return 0;
}