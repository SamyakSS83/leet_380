#include <vector>
#include <cmath>
#include <omp.h>

std::vector<int> parallel_sieve(int N) {
    if (N < 2) return {};
    std::vector<char> is_prime(N+1, 1);  // char not bool: vector<bool> is not thread-safe
    is_prime[0] = is_prime[1] = 0;
    int sq = (int)std::sqrt((double)N);
    // Sequential phase: sieve small primes
    for (int p = 2; p <= sq; p++) {
        if (is_prime[p]) {
            #pragma omp parallel for schedule(static)
            for (int j = p*p; j <= N; j += p)
                is_prime[j] = false;
        }
    }
    std::vector<int> primes;
    for (int i = 2; i <= N; i++)
        if (is_prime[i]) primes.push_back(i);
    return primes;
}
