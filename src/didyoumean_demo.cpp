/*
Lisanne Wiengarten
Matrikelnr. 764870
EAIDCL WiSe13/14
gcc 4.9.1 C++11 Win10
*/

/**
  @brief Demonstrates how Levensthein automata
         can be used for corrective "Did you mean?" purposes
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
        std::size_t pos = line.find(" ");
        if (pos != std::string::npos) {
            std::cerr << " Line " << line << " seems to contain more than one word and is ignored.\n";
        }
        else {
            // Tranform each valid word to lowercase
            std::transform(line.begin(), line.end(), line.begin(), ::tolower);
            corpus.push_back(line);
        }
    }

    // Sort the corpus alphabetically
    std::sort(corpus.begin(), corpus.end());

    std::string input;
    std::cout << "\nPlease type a (misspelled) word: ";
    std::cin >> input;

    if (std::find(corpus.begin(), corpus.end(), input) != corpus.end()) { std::cout << "(y) This is a valid word.\n"; return 0; }

    else {
        for (int i = 1; i <= 5; i++) {
            LevenshteinAutomaton lev(input, i, corpus);
            std::vector<std::string> matches = lev.get_all_matches();
            if (matches.size() > 0) {
                std::cout << "Did you mean to write any of these words?\n";
                for (auto m: matches) {
                    std::cout << m << "\t";
                }
                std::cout << "\n";
                return 0;
            }
            else { std::cout << "Could not find any words in Levensthein distance " << i << ".\n"; }
        }
    }
}
