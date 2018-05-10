/*
Lisanne Wiengarten
Matrikelnr. 764870
EAIDCL WiSe13/14
gcc 4.9.1 C++11 Win10
*/

/**
  @brief Demonstrates some possible uses of Levensthein automata
*/

#include <iostream>
#include <set>
#include <map>
#include <vector>
#include <tuple>
#include <fstream>
#include <algorithm>
#include <typeinfo>
#include <iterator>
#include <deque>
#include <sstream>
#include <cctype>
#include <utility>
#include <typeinfo>

#include "levautomaton.hpp"
#include "nfautomaton.hpp"


int main(int argc, char** argv)
{
    if (argc != 2) {
        std::cerr << "Missing corpus file!\n";
        return -1;
    }

    std::ifstream filey(argv[1]);
    if (!filey) {
        std::cerr << "Could not open '" << argv[1] << "'\n";
        return -2;
    }

    // Read the given corpus file into a vector
    std::vector<std::string> corpus;
    std::string line;
    while(filey >> line) {
        // Tranform each word to lowercase
        std::transform(line.begin(), line.end(), line.begin(), ::tolower);
        corpus.push_back(line);
    }

    // Sort the corpus alphabetically
    std::sort(corpus.begin(), corpus.end());

    // Run a test with 'badger' and k = 1
    std::cout << "\nAll matches for word 'badger' in Levensthein distance 1:\n";
    LevenshteinAutomaton lev_b("badger", 1, corpus);
    std::vector<std::string> matches = lev_b.get_all_matches();
    for (auto m: matches) {
        std::cout << m << "\t";
    }
    std::cout << "\n";

    std::cout << "\nWriting Badger Lev automaton to dot file.." << std::endl;
    std::ofstream dot_out("Lev_badger_1.dot");
    lev_b.lev_to_dot(dot_out);


    // Run a test with 'duckling' and k = 2
    std::cout << "\nAll matches for word 'duckling' in Levensthein distance 2:\n";
    LevenshteinAutomaton lev_d("duckling", 2, corpus);
    matches = lev_d.get_all_matches();
    for (auto m: matches) {
        std::cout << m << "\t";
    }
    std::cout << "\n";

    std::cout << "\nWriting Duckling Lev automaton to dot file.." << std::endl;
    std::ofstream dot_out2("Lev_duckling_2.dot");
    lev_d.lev_to_dot(dot_out2);


    // Run a test with 'crocodile' and k = 3
    std::cout << "\nAll matches for word 'crocodile' in Levensthein distance 3:\n";
    LevenshteinAutomaton lev_c("crocodile", 3, corpus);
    matches = lev_c.get_all_matches();
    for (auto m: matches) {
        std::cout << m << "\t";
    }
    std::cout << "\n";
}
