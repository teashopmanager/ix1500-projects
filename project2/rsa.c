#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// Hjälpfunktion som markerar och returnerar en lista med primtal upp från 2 till sqrt(n) 
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
        for (int i = 2 * p; i <= n; i += p)
        {
            arr[i] = 1;
        }

        int next = p + 1;

        while (next <= n && arr[next] == 1)

        {
            next++;
        }

        if (next > n)
        {
            break;
        }

        p = next;
    }

    //nytt
    *prime_count = 0;

    for (int i = 2; i <= n; i++) {
        if (arr[i] == 0)
        {
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

void find_pq(long long n, long long *p, long long *q) {
    int limit = (int)sqrt((double)n);

    int prime_count;

    int *primes = sieves(limit, &prime_count);

    if (primes == NULL) {
        return;
    }

    for (int i = 0; i < prime_count; i++) {
        if (n % primes[i] == 0) {
            *p = primes[i];
            *q = n / primes[i];

            free(primes);
            return;
        }
    }

    free(primes);
}

void euler_phi(long long *phi, long long *p, long long *q, int index) {

    phi[index] = (p[index]-1) * (q[index]-1);    

}

long long find_d(long long e, long long phi) {
    long long remainder = e;
    long long prev_remainder = phi;
    
    long long e_coefficient = 1;
    long long prev_e_coefficient = 0;

    while(remainder != 0) {
        long long quotient = prev_remainder / remainder;

        long long new_remainder = prev_remainder - quotient * remainder;
        prev_remainder = remainder;
        remainder = new_remainder;

        long long new_e_coefficient = prev_e_coefficient - quotient * e_coefficient;

        prev_e_coefficient = e_coefficient;
        e_coefficient = new_e_coefficient;
    }

    // Kontroll så vi inte får den negativa modulära inversen
    if(prev_e_coefficient < 0) {
        prev_e_coefficient = prev_e_coefficient + phi;
    }

    return prev_e_coefficient;
}

void decrypt() {
    
}

int main() {
    int key = 0; 

    long long p[] = {0, 0, 0, 0, 0, 0};
    long long q[] = {0, 0, 0, 0, 0, 0};

    int e[] = {
        7,
        23,
        7,
        19,
        29,
        29,
    };

    long long n[] = {
        391,
        100289621329340257LL, 
        882238272068111039LL, 
        182469164307407143LL, 
        799710404000289581LL, 
        901082142384103049LL
    };

    long long phi[] = {0, 0, 0, 0, 0, 0};

    find_pq(n[key], &p[key], &q[key]);
    euler_phi(phi, p, q, key); 
    //gcd(e[key], phi[key]);

    printf("p = %lld\n", p[key]);
    printf("q = %lld\n", q[key]);
    printf("phi = %lld\n", phi[key]);
    printf("d = %lld\n", find_d(e[key], phi[key]));
    return 0;
}