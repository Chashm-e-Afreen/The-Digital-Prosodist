#ifndef EDIT_DIST_H
#define EDIT_DIST_H

#include <iostream>
#include <string.h>
#include <cmath>
#include <algorithm>



// Returns the Levenshtein distance between word1 and word2.
unsigned int levenshteinDist(std::wstring word1, std::wstring word2) {
    unsigned int size1 = word1.size();
    unsigned int size2 = word2.size();
    unsigned int** verif = new unsigned int*[size1 + 1]; // Verification matrix i.e. 2D array which will store the calculated distance.

    for (unsigned int i = 0; i < size1 + 1; i++)
      {
        verif[i] = new unsigned int [size2 + 1];
      }

    // If one of the words has zero length, the distance is equal to the size of the other word.
    if (size1 == 0)
        return size2;
    if (size2 == 0)
        return size1;

    // Sets the first row and the first column of the verification matrix with the numerical order from 0 to the length of each word.
    for (unsigned int i = 0; i <= size1; i++)
        verif[i][0] = i;
    for (unsigned int j = 0; j <= size2; j++)
        verif[0][j] = j;

    // Verification step / matrix filling.
    for (unsigned int i = 1; i <= size1; i++) {
        for (unsigned int j = 1; j <= size2; j++) {
            // Sets the modification cost.
            // 0 means no modification (i.e. equal letters) and 1 means that a modification is needed (i.e. unequal letters).
            unsigned int cost = (word2[j - 1] == word1[i - 1]) ? 0 : 1;

            // Sets the current position of the matrix as the minimum value between a (deletion), b (insertion) and c (substitution).
            // a = the upper adjacent value plus 1: verif[i - 1][j] + 1
            // b = the left adjacent value plus 1: verif[i][j - 1] + 1
            // c = the upper left adjacent value plus the modification cost: verif[i - 1][j - 1] + cost
            verif[i][j] = std::min(
                std::min(verif[i - 1][j] + 1, verif[i][j - 1] + 1),
                verif[i - 1][j - 1] + cost
            );
        }
    }

    unsigned int last_position = verif[size1][size2];

    for (unsigned int i = 0; i < size1 + 1; i++)
      {
        delete[] verif[i];
      }

    delete[] verif;

    // The last position of the matrix will contain the Levenshtein distance.
    return last_position;
}

#endif // EDIT_DIST_H
