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

u64 encrypt(u64 m, u64 e, u64 n) {
    return square_and_multiply(m, e, n);
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

    int read_result = fscanf(file, "%" SCNu64, &c);
    while (read_result == 1) {
        u64 m = decrypt(c, d, n);

        unsigned char byte1 = (m >> 24) & 0xFF;
        unsigned char byte2 = (m >> 16) & 0xFF;
        unsigned char byte3 = (m >> 8) & 0xFF;
        unsigned char byte4 = m & 0xFF;

        putchar(byte1);
        putchar(byte2);
        putchar(byte3);
        putchar(byte4);
    }

    putchar('\n');

    fclose(file);
}

/**
 * @brief Krypterar innehåller i en textfil med RSA.
 *
 * Funktionen läser upp till fyra bytes åt gången från inputfilen och
 * kombinerar dessa till ett heltal m. Därefter krypteras m med den
 * publika nyckeln (e, n) genom att beräkna c ≡ m^e mod n. Det krypterade talet
 * c skrivs sedan till outputfilen.
 *
 * Processen upprepas tills hela inputfilen har lästs
 *
 * @param input_filename        Fil med den text som sak krypteras
 * @param output_filename       Fil med den krypterade texten
 * @param e                     Den publika exponenten
 * @param n                     RSA-modulus
 */
void encrypt_file(const char *input_filename, const char *output_filename,
                  u64 e, u64 n) {

    // Öpnnar inputfilen för läsning, "r" = read mode
    FILE *input_file = fopen(input_filename, "r");

    // Om filen inte kunde öppnas returnerar fopen() NULL
    if (input_file == NULL) {
        printf("Could not open file: %s\n", input_filename);
        return;
    }

    // Öppnar outputfilen för skrivning, "w" = write mode
    FILE *output_file = fopen(output_filename, "w");

    // Om filen inte kunde öppnas
    if (output_file == NULL) {
        printf("Could not open file: %s\n", output_filename);
        fclose(input_file);
        return;
    }

    // Arrayen som lagrar upp till fyra bytes från inputfilen.
    unsigned char bytes[4];

    // Läser och kryperar ett block i taget tills filen är slut.
    while (1) {

        // Antal bytes som lästs till det aktuella blocket
        int byte_count = 0;

        // Läs maximalt fyra bytes till det aktuella blocket
        while (byte_count < 4) {

            // Läser ett tecken i taget från inputfilen
            int ch = fgetc(input_file);

            // Avsluta läsningen av blocket om filen är slut, EOF = End of File.
            if (ch == EOF) {
                break;
            }

            // Omvandlar det lästa tecknet till en unsigned byte och sparar det
            // på nästa lediga plats
            bytes[byte_count] = (unsigned char)ch;

            // Ökar antalet bytes som finns i det aktuella blocket.
            byte_count++;
        }

        // Om inga bytes kunde läsas har vi nått slutet av filen och den yttre
        // loopen avslutas.
        if (byte_count == 0) {
            break;
        }

        // Plaintextblocket m börjar på 0 och byggs sedan upp från de bytes som
        // lästs från filen
        u64 m = 0;

        // Packar blockets bytes till ett enda heltal m
        for (int i = 0; i < byte_count; i++) {

            // Flyttar m 8 bitar åt vänster för att skapa plats för nästa byte
            // och lägger sedan in bytes[i].
            m = (m << 8) | bytes[i];
        }

        // Krypterar m med den publika nyckeln (e, n)
        // c = m^e mod n
        u64 c = encrypt(m, e, n);

        // Skriver det krypterade blocket c till outputfilen som ett decimalt
        // heltal
        fprintf(output_file, "%" PRIu64 " ", c);

        // Skriver även det krypterade blocket till terminalen
        printf("%" PRIu64 " ", c);
    }

    printf(" \n \n");

    // Stänger filerna när hela meddelandet har behandlats
    fclose(input_file);
    fclose(output_file);
}

int main() {

    u64 p = 65537;
    u64 q = 65539;
    u64 e = 17;

    u64 n = p * q;
    u64 phi = euler_phi(p, q);
    u64 d = find_d(e, phi);

    printf("Public key:  e = %" PRIu64 ", n = %" PRIu64 "\n", e, n);
    printf("Private key: d = %" PRIu64 ", n = %" PRIu64 "\n", d, n);

    encrypt_file("encrypt.txt", "decrypt.txt", e, n);

    decrypt_file("decrypt.txt", d, n);

    return 0;
}