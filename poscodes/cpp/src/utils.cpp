#include "utils.hpp"

#include <algorithm>
#include <cctype>
#include <fstream>
#include <iostream>
#include <vector>

Poscode *readCodes(const std::string &strfile, std::size_t n){
    auto *codes = new Poscode[n];

    std::ifstream input(strfile);
    if (!input.is_open()){
        std::cerr << "Error: cannot open file: " << strfile << std::endl;
        delete[] codes;
        return nullptr;
    }

    std::string line;
    std::size_t i = 0;
    while (i < n && std::getline(input, line)){
        if (!line.empty()) codes[i++] = Poscode(line);
    }
    input.close();

    if (i < n){
        std::cerr << "Warning: file had only " << i << " lines; "
                  << "padding with dummy codes.\n";
        while (i < n) codes[i++] = Poscode("0000AA");
    }
    return codes;
}

void deleteCodes(Poscode *codes){
    if (codes != nullptr) delete[] codes; // importante: delete[]
}

static inline bool less_code(const Poscode &a, const Poscode &b){
    return a.getData() < b.getData(); // compara string completo ddddLL
}

static inline int map_digit(char c){ return static_cast<int>(c - '0'); }         
static inline int map_letter(char c){
    unsigned char uc = static_cast<unsigned char>(c);
    char u = static_cast<char>(std::toupper(uc));
    return static_cast<int>(u - 'A'); 
}

static void counting_by_digit(const Poscode *in, Poscode *out,
                              std::size_t n, std::size_t pos){
    const int K = 10;
    std::size_t count[K + 1] = {0};

    for (std::size_t i = 0; i < n; ++i) count[ map_digit(in[i].getValue(pos)) + 1 ]++;
    for (int r = 0; r < K; ++r) count[r + 1] += count[r];
    for (std::size_t i = 0; i < n; ++i){
        int b = map_digit(in[i].getValue(pos));
        out[count[b]++] = in[i];
    }
}

static void counting_by_letter(const Poscode *in, Poscode *out,
                               std::size_t n, std::size_t pos){
    const int K = 26;
    std::size_t count[K + 1] = {0};

    for (std::size_t i = 0; i < n; ++i) count[ map_letter(in[i].getValue(pos)) + 1 ]++;
    for (int r = 0; r < K; ++r) count[r + 1] += count[r];
    for (std::size_t i = 0; i < n; ++i){
        int b = map_letter(in[i].getValue(pos));
        out[count[b]++] = in[i];
    }
}

void radix_sort(Poscode *A, std::size_t n){
    if (n <= 1) return;
    Poscode *B = new Poscode[n];

    counting_by_letter(A, B, n, 5); 
    counting_by_letter(B, A, n, 4); 
    counting_by_digit  (A, B, n, 3); 
    counting_by_digit  (B, A, n, 2); 
    counting_by_digit  (A, B, n, 1); 
    counting_by_digit  (B, A, n, 0); 

    delete[] B;
}

static void merge_rec(Poscode *A, Poscode *aux, std::size_t l, std::size_t r){
    if (r - l <= 1) return;
    std::size_t m = l + (r - l) / 2;
    merge_rec(A, aux, l, m);
    merge_rec(A, aux, m, r);

    std::size_t i = l, j = m, k = l;
    while (i < m && j < r){
        if (less_code(A[i], A[j])) aux[k++] = A[i++];
        else                        aux[k++] = A[j++];
    }
    while (i < m) aux[k++] = A[i++];
    while (j < r) aux[k++] = A[j++];
    for (std::size_t t = l; t < r; ++t) A[t] = aux[t];
}

void merge_sort(Poscode *A, std::size_t n){
    if (n <= 1) return;
    std::vector<Poscode> aux(n);
    merge_rec(A, aux.data(), 0, n);
}

static inline void iswap(Poscode &a, Poscode &b){ std::swap(a, b); }

static void quick_3way(Poscode *A, long l, long r){
    if (r <= l) return;

    long m = l + (r - l) / 2;
    if (less_code(A[m], A[l])) iswap(A[m], A[l]);
    if (less_code(A[r], A[m])) iswap(A[r], A[m]);
    if (less_code(A[m], A[l])) iswap(A[m], A[l]);
    Poscode pivot = A[m];

    long i = l, lt = l, gt = r;
    while (i <= gt){
        if (less_code(A[i], pivot))      iswap(A[i++], A[lt++]);
        else if (less_code(pivot, A[i])) iswap(A[i], A[gt--]);
        else                             i++;
    }
    quick_3way(A, l, lt - 1);
    quick_3way(A, gt + 1, r);
}

void quick_sort(Poscode *A, std::size_t n){
    if (n <= 1) return;
    quick_3way(A, 0, static_cast<long>(n) - 1);
}
