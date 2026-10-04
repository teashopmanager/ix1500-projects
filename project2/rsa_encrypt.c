#include <bits/time.h>
#include <complex.h>
#include <inttypes.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
/**
 * @brief
 *
 * @param start
 * @param stop
 * @return long long
 */
long long nano_seconds(struct timespec *start, struct timespec *stop) {
    return (stop->tv_sec - start->tv_sec) * 1000000000LL +
           (stop->tv_nsec - start->tv_nsec);
}

/**
 * @brief Alias för ett 128-bitars unsigned heltal
 *
 * Kan lagra heltal från 0 till 2^127 -1
 */
typedef unsigned __int128 u128;

/**
 * @brief Alias för ett 128-bitars heltal.
 *
 * Kan lagra heltal från -2^127 till 2^127 -1.
 */
typedef __int128 i128;

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
 * @brief Beräknar gcd(a, b) med Euklides algoritm
 *
 * @param a
 * @param b
 * @return
 */
u128 gcd(u128 a, u128 b) {
    while (b != 0) {
        u128 remainder = a % b;
        a = b;
        b = remainder;
    }

    return a;
}

/**
 * @brief Genererar ett slumpässigt tal med angivet antal bitar.
 *
 * Högsta biten sätts till 1 så att talet får den önskade bit storleken.
 *
 * Lägsta biten sätts till 1 så att talet blir udda.
 *
 * @param bits      Hur många bitar talet som ska genereras är 1
 * @return
 */
u128 random_bits(int bits) {
    u128 number = 0;

    for (int i = 0; i < bits; i++) {
        number = (number << 1) | (rand() & 1);
    }

    number = number | ((u128)1 << (bits - 1));

    number = number | 1;

    return number;
}

/**
 * @brief Kontrollerar om n är ett primtal.
 *
 * @param n
 * @return int
 */
int is_prime(u64 n) {
    if (n < 2) {
        return 0;
    }

    if (n == 2) {
        return 1;
    }

    if (n % 2 == 0) {
        return 0;
    }

    for (u64 divisor = 3; divisor <= n / divisor; divisor += 2) {
        if ((n % divisor) == 0) {
            return 0;
        }
    }

    return 1;
}

/**
 * @brief Genererar ett primtal med ungefär bits bitar.
 *
 * @param bits
 * @return
 */
u64 generate_prime(int bits) {
    while (1) {
        u64 candidate = random_bits(bits);
        if (is_prime(candidate)) {
            return candidate;
        }
    }
}

/**
 * @brief
 *
 * @param phi
 * @return
 */
u128 generate_e(u128 phi) {
    u128 e;

    do {
        e = 2 + (u128)rand() % (phi - 2);
    } while (gcd(e, phi) != 1);

    return e;
}

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
u128 mod_add(u128 x, u128 y, u128 n) {
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
u128 mod_mult(u128 a, u128 b, u128 n) {

    // Resultatet byggs upp stegvis
    u128 result = 0;

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
u128 square_and_multiply(u128 c, u128 d, u128 n) {

    /**
     * m innehåller det resultat som byggs upp under algoritmens gång.
     *
     * Vi börjar med 1 eftersom 1 är ett neutralt element vid multi.
     *
     */
    u128 m = 1;

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
void find_pq(u128 n, u128 *p, u128 *q) {
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
u128 euler_phi(u64 p, u64 q) {
    return (u128)(p - 1) * (u128)(q - 1);
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
u128 find_d(u128 e, u128 phi) {
    i128 remainder = e;
    i128 prev_remainder = phi;

    i128 e_coefficient = 1;
    i128 prev_e_coefficient = 0;

    while (remainder > 1) {
        i128 quotient = prev_remainder / remainder;

        i128 new_remainder = prev_remainder % remainder;

        prev_remainder = remainder;
        remainder = new_remainder;

        i128 new_e_coefficient = prev_e_coefficient - quotient * e_coefficient;

        prev_e_coefficient = e_coefficient;
        e_coefficient = new_e_coefficient;
    }

    // Kontroll så vi inte får den negativa modulära inversen
    if (e_coefficient < 0) {
        e_coefficient = e_coefficient + (i128)phi;
    }

    return (u128)e_coefficient;
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
u128 decrypt(u128 c, u128 d, u128 n) {
    return square_and_multiply(c, d, n);
}

u128 encrypt(u128 m, u128 e, u128 n) {
    return square_and_multiply(m, e, n);
}
/**
 * @brief Funktion för att ersätta SCNu64 eftersom det inte finns för 128
 *
 * https://stackoverflow.com/questions/11656241/how-can-i-print-uint128-t-number-using-gcc
 * inspirerad från denna, men modfierad så den gör samma som SC<Nu64.>
 * @param file
 * @param value
 */
void fprint_u128(FILE *file, u128 value) {
    char buffer[40];
    int index = 0;

    if (value == 0) {
        fputc('0', file);
        return;
    }

    while (value > 0) {
        buffer[index++] = '0' + (value % 10);
        value /= 10;
    }

    while (index > 0) {
        fputc(buffer[--index], file);
    }
}

/**
 * @brief
 *
 * @param file
 * @param value
 * @return int
 */
int read_u128(FILE *file, u128 *value) {
    int ch;

    do {
        ch = fgetc(file);

        if (ch == EOF) {
            return 0;
        }
    } while (ch == ' ' || ch == '\n' || ch == '\t' || ch == '\r');

    u128 result = 0;

    while (ch >= '0' && ch <= '9') {
        result = result * 10 + (ch - '0');
        ch = fgetc(file);
    }

    *value = result;

    return 1;
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
void decrypt_file(const char *filename, u128 d, u128 n) {

    FILE *file = fopen(filename, "r");

    if (file == NULL) {
        printf("Could not open file: %s\n", filename);
        return;
    }

    u128 c;

    int read_result = read_u128(file, &c);

    while (read_result == 1) {

        u64 m = decrypt(c, d, n);

        unsigned char byte1 = (m >> 24) & 0xFF;
        unsigned char byte2 = (m >> 16) & 0xFF;
        unsigned char byte3 = (m >> 8) & 0xFF;
        unsigned char byte4 = m & 0xFF;

        read_result = read_u128(file, &c);
    }

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
                  u128 e, u128 n) {

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
        u128 c = encrypt(m, e, n);

        // Skriver det krypterade blocket c till outputfilen som ett decimalt
        // heltal
        fprint_u128(output_file, c);
        fputc(' ', output_file);
    }

    // Stänger filerna när hela meddelandet har behandlats
    fclose(input_file);
    fclose(output_file);
}

int main() {

    srand(time(NULL));

    int bits = 128;
    int tests = 10;

    long long total_encryption_time = 0;
    long long total_decryption_time = 0;

    for (int i = 0; i < tests; i++) {

        u64 p = generate_prime(bits / 2);
        u64 q;

        do {
            q = generate_prime(bits / 2);
        } while (p == q);

        u128 n = (u128)p * (u128)q;

        u128 phi = euler_phi(p, q);

        u64 e = 65537;

        do {
            p = generate_prime(bits / 2);

            do {
                q = generate_prime(bits / 2);
            } while (p == q);

            n = (u128)p * (u128)q;

            phi = euler_phi(p, q);

        } while (gcd(e, phi) != 1);

        u128 d = find_d(e, phi);

        // printf("\n--- Test %d ---\n", i + 1);
        //  printf("p   = %" PRIu64 "\n", p);
        //  printf("q   = %" PRIu64 "\n", q);
        //  printf("n   = %" PRIu64 "\n", n);
        //  printf("phi = %" PRIu64 "\n", phi);
        //  printf("e   = %" PRIu64 "\n", e);
        //  printf("d   = %" PRIu64 "\n", d);

        struct timespec start;
        struct timespec stop;

        // Kryptera hela meddelandet
        clock_gettime(CLOCK_MONOTONIC, &start);
        encrypt_file("encrypt.txt", "decrypt.txt", e, n);
        clock_gettime(CLOCK_MONOTONIC, &stop);

        long long encryption_time = nano_seconds(&start, &stop);

        total_encryption_time += encryption_time;

        // Dekryptera hela meddelandet
        clock_gettime(CLOCK_MONOTONIC, &start);
        decrypt_file("decrypt.txt", d, n);
        clock_gettime(CLOCK_MONOTONIC, &stop);

        long long decryption_time = nano_seconds(&start, &stop);

        total_decryption_time += decryption_time;

        // printf("Encryption: %lld ns\n", encryption_time);
        // printf("Decryption: %lld ns\n", decryption_time);
    }

    double average_encryption = (double)total_encryption_time / tests;
    double average_decryption = (double)total_decryption_time / tests;

    printf("\n%d-bit RSA average\n", bits);
    printf("Encryption: %.2f ns\n", average_encryption);
    printf("Decryption: %.2f ns\n", average_decryption);

    return 0;
}