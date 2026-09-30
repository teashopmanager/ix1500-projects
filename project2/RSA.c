#include <stdio.h>
#include <stdlib.h>
#include <math.h>

long long *sieves(long long n, int *prime_count)
{
    char *arr = malloc((n + 1) * sizeof(char));

    if (arr == NULL)
    {
        printf("Memoty error: ");
        return;
    }

    // 0 = omarkerad
    // 1 = markerad
    for (int i = 0; i <= n; i++)
    {
        arr[i] = 0;
    }

    int p = 2;

    while (p <= n)
    {
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

    *prime_count = 0;

    for (long i = 2; i <= n; i++)
    {
        if (arr[i] == 0)
        {
            (*prime_count)++;
        }
    }

    long long *primes = malloc(*prime_count * sizeof(long long));

    if (primes == NULL)
    {
        free(arr);
        return NULL;
    }

    int index = 0;

    for (int i = 2; i <= n; i++)
    {
        if (arr[i] == 0)
        {
            primes[index] = i;
            index++;
        }
    }
    free(arr);

    return primes;
}

void find_pq(long long n, long *p, long *q)
{
    int limit = (int)sqrt((double)n);

    int prime_count;

    long long *primes = sieves(limit, &prime_count);

    if (primes == NULL)
    {
        return;
    }

    for (int i = 0; i < prime_count; i++)
    {
        if (n % primes[i] == 0)
        {
            *p = primes[i];
            *q = n / primes[i];

            free(primes);
            return;
        }
    }
    free(primes);
}

int main()
{
    long long p = 0;
    long long q = 0;

    // n från 1
    long long n = 100289621329340257;

    find_pq(n, &p, &q);

    printf("p = #lld\n", p);
    printf("q = #lld\n", q);

    return 0;
}