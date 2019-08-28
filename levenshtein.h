#ifndef LEVENSHTEIN_H
#define LEVENSHTEIN_H


#include <string>
#include <vector>
#include<QString>
static int min3(int a, int b, int c) {
    return std::min(std::min(a,b), c);
}

template<typename T>
int distanceLevenshtein( const T& s0, const T& s1) {
    unsigned m = s0.size();
    unsigned n = s1.size();
    // d is a table with m+1 rows and n+1 columns
    int *d = new int[(m+1) * (n + 1)];
    #define d(i,j)  d[(i)*(n+1) + (j)]

    // initialization row0 and colum0
    for( unsigned i = 0; i <= m; ++i) {
        d(i,0) = i;
    }
    for( unsigned i = 0; i <= n; ++i) {
        d(0,i) = i;
    }

    for( unsigned j = 1; j <= n; ++j) {
      for (unsigned i = 1; i <= m; ++i) {
            if (s0[i - 1] == s1[j - 1]) {
                d(i, j) = d(i-1, j-1);
            } else {
                d(i, j) = min3(
                            d(i-1, j),
                            d(i, j-1),
                            d(i-1, j-1)
                          ) + 1;
            }
        }
    }
    int dist = d(m,n);
    delete [] d;
    return dist;
}

// Levenshtein distance between two int vector
int distanceLevenshtein(const std::vector<int>& s0, const std::vector<int>& s1) {
    return distanceLevenshtein< std::vector<int> >(s0,s1);
}

// Levenshtein distance between two string
int distanceLevenshtein(const std::wstring& s0, const std::wstring& s1) {
    return distanceLevenshtein<std::wstring>(s0,s1);
}



#endif // LEVENSHTEIN_H
