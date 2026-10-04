#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/**
 * @brief Alias för ett 128-bitars unsigned heltal
 *
 * Kan lagra heltal från 0 till 2^128 -1
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
 * @brief Beräknar tiden mellan två tidpunkter i nanosekunder.
 *
 * @param start     Tidpunkten då mätningen startarde.
 * @param stop      Tidpunkten då mätningen avslutades.
 * @return          Tidsskillnaden i nanosekunder.
 */
long long nano_seconds(struct timespec *start, struct timespec *stop) {
    return (stop->tv_sec - start->tv_sec) * 1000000000LL +
           (stop->tv_nsec - start->tv_nsec);
}

/**
 * @brief Beräknar största gemensamma delaren för två heltal.
 *
 * Funktionen använder Euklides algoritm för att beräkna gcd(a, b).
 *
 * @param a     Det första heltalet
 * @param b     Det andra heltalet
 * @return      Största gemensamma delaren till a och b
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
 * Funktionen genererar ett slumpmässigt tal bir för bit.
 * Den högsta biten stills till 1 för att säkerställa den önskade bitlängden och
 * den lägsta biten sätts till 1 för att talet ska vara udda.
 *
 * @param bits      Antalet bitar som talet ska innehålla.
 * @return          Det genererade slumpmässiga talet.
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
 * @brief Kontrollerar om ett tal är ett primtal.
 *
 * Funktionen testar om n är delbart med något udda heltal från 3 upp till roten
 * ur n. Jämna tal större än 2 kan direkt uteslutas.
 *
 * @param n     Talet som ska kontrolleras
 * @return      1 om n är ett primtal, annars 0
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
 * @brief Genererar ett slumpmässigt primtal med angivet antal bitar.
 *
 * Funktionen genererar slumpmässiga udda tal tills ett tal som är ett primtal
 * hittas.
 *
 * @param bits      Antalet bitar som primtalet ska innehålla.
 * @return          Det genererade primtalet.
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
 * @brief Beräknar summan av två tal mod n utan overflow
 *
 * Funktionen beräknar ((i128x+y) mod n) utan att direkt utföra
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
 * @brief Beräknar Eulers phi-funktion för n = p * q.
 *
 * För två primtal p och q beräknas phi(n) enligt
 * phi(n) = (p - 1) * (q - 1)
 *
 * @param p         Den första primtalsfaktorn.
 * @param q         Den andra primtalsfaktorn.
 * @return          Värdet av phi(n)
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

/**
 * @brief Krypterar ett plaintextblock med RSA.
 *
 * Funktionen krypterar plaintext m genom att berälna m^e mod n
 *
 * @param m     Plaintextblocket som ska krypteras.
 * @param e     Den publika RSA-exponenten.
 * @param n     RSA-modulus
 * @return      Det krypterade talet c
 */
u128 encrypt(u128 m, u128 e, u128 n) {
    return square_and_multiply(m, e, n);
}

/**
 * @brief Skriver ett 128-bitars unsigned heltal till en fil.
 *
 * Funktionen omvandlar ett u128-värde till dess decimala representation och
 * skriver siffrorna till den angivna filen.
 *
 * (Fungerar som fprintf() för u128)
 *
 * @param file      Filen som talet ska skrivas till.
 * @param value     Talet som ska skrivas.
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
 * @brief Läser ett 128-bitars heltal från en fil.
 *
 * Funktionen hoppar över whitespace och läser därefter ett decimalt heltal
 * tecken för tecken.
 *
 * (Fungerar som en fscanf() för u128)
 *
 * @param file      Filen som talet ska läsas från.
 * @param value     Pekare där det inlästa talet sparas.
 * @return          1 om ett tal lästes, annars 0 om filens slut nåddes
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
 * Funktionen läser ett krypterat tal i taget från filen och dekrypterar varje
 * tal med den privata exponenten d och modulus n.
 *
 * @param filename      Namnet på filen som innehåller det kryterade
 * meddelandet.
 * @param d             Den privata RSA-exponenten.
 * @param n             RSA-modulus
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

        decrypt(c, d, n);
        read_result = read_u128(file, &c);
    }

    fclose(file);
    return;
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

    int bits = 64;
    int tests = 10;

    long long total_encryption_time = 0;
    long long total_decryption_time = 0;

    for (int i = 0; i < tests; i++) {

        u64 p;
        u64 q;
        u128 n;
        u128 phi;
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