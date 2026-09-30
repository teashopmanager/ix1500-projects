#include <stdio.h>
#include <stdlib.h>
#include <math.h>

void sieves(int n)
{

    char *arr = malloc((n + 1) * sizeof(char));
    // char arr[n + 1];

    if (arr == NULL)
    {
        printf("Memoty error: ");
        return;
    }

    // Initiera arrayen. Vi måste ha n + 1 platser eftersom vi vill kunna använda indexet direkt efter talet
    for (int i = 0; i <= n; i++) // Sätter alla positioner till 0 (omarkerade)
    {
        arr[i] = 0;
    }

    // Börja med p = 2 (minsta primtalet)

    int p = 2;

    // Markera multiplar av p
    while (p <= n) // Sålänge vi har ett giltigt p fortsätter vi
    {
        for (int i = 2 * p; i <= n; i += p)
        {
            arr[i] = 1;
        }

        int next = p + 1;

        while (next <= n && arr[next] == 1) // Letar efter nästa tal som inte är markerat

        {
            next++;
        }

        if (next > n)
        {
            break;
        }

        p = next;
    }
    free(arr);
}

void find_pq(long long n, long *p, long *q)
{
}

int main()
{
    long long p = 0;
    long long q = 0;

    // n från 1
    long long n = 100289621329340257;

    find_pq(n, &p, &q);
}