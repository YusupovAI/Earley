#include <iostream>
#include "EarleyAlgorithm.h"
#include "Grammar.h"

int main() {
    Grammar grammar;
    try {
        grammar.Read(std::cin);
    } catch(std::exception& e) {
        std::cerr << e.what() << '\n';
        return 0;
    }
    std::string word;
    std::cin >> word;
    EarleyAlgorithm algo;
    if (algo.Check(grammar, word)) {
        std::cout << "Accept\n";
    } else {
        std::cout << "Reject\n";
    }
    return 0;
}
