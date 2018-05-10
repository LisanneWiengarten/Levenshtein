/*
Lisanne Wiengarten
Matrikelnr. 764870
EAIDCL WiSe13/14
gcc 4.9.1 C++11 Win10

Compact representation of a Levenshtein Automaton
Searches for all valid words in Levenshtein distance k
Or can be used for "Did you mean"-purposes
*/

#include "dfautomaton.hpp"
#include "nfautomaton.hpp"

#ifndef LEVAUTOMATON_H_INCLUDED
#define LEVAUTOMATON_H_INCLUDED

#define ANY       "ANY"
#define EPSILON   "EPSILON"
#define NONE      "\0"
#define NUL       "-"

/**
  @brief LevenshteinAutomaton is a class for representing
         Levenshtein automata
         They depict all words in Levenshtein distance k to a given word
         Can also be used for a "Did you mean" function
*/
class LevenshteinAutomaton
  {
   public: // Types
      typedef std::tuple<int, int>              NState;
      typedef typename std::set<NState>         DState;

   private: // Types
      typedef std::string                       Word;
      typedef std::vector<Word>                 WordVec;


   public: // Functions
    /**
      @brief Constructor from word and maximum edit distance
      @param Word w
      @param maximum edit distance k
      @param database corpus of words
    */
    LevenshteinAutomaton(const Word& input, const unsigned& distance, const WordVec& words)
    {
        lookupword = input;
        k = distance;
        corpus = words;
        NFAutomaton nfa(std::make_tuple(0, 0));

        init();
     }


    /**
      @brief Returns a list of all the words within Levenshtein distance k in the given corpus
      @return A vector containing all the words
    */
    WordVec get_all_matches()
    {
        WordVec matchWords;
        dfa = nfa.to_dfa();

        Word match = dfa.next_valid(NUL);

        while (match != NONE) {
            // Find the first word in the corpus that is lexicographically greater than or equal to the current match
            Word next = next_in_corpus(match);

            if (next == NONE) {
                // If there is no next word in the corpus, all matches have been found
                return matchWords;
            }

            // If the current match is a valid word in the corpus, it is added to the matches
            if (match == next) {
                matchWords.push_back(match);
                next = next + NUL;
            }
            match = dfa.next_valid(next);
        }

        return matchWords;

    } // get_all_matches

    /**
        @brief Prints the whole Lev automaton in a readable way
    */
    void lev_printer() const
    {
        dfa.dfa_printer();

    } // lev_printer


    /**
        @brief Prints a dot representation of the Lev Automaton to stream 'out'
        @param ostream out
    */
    void lev_to_dot(std::ostream& out) const
    {
        dfa.dfa_to_dot(out);

    } // dot_out


   private: // Functions
   /**
      @brief Starts building the complete automaton
   */
      void init()
      {
        for(int i = 0; i < lookupword.size(); ++i) {
            for (unsigned e = 0; e <= k; e++) {

                // Transitions with all the characters from the input word
                std::string s(1, lookupword[i]);
                nfa.add_transition(std::make_tuple(i, e), s, std::make_tuple(i+1, e));

                if (e < k) {

                    // Transitions for deletion in the Levenshtein distance algorithm
                    nfa.add_transition(std::make_tuple(i, e), ANY, std::make_tuple(i, e+1));

                    // Transitions for insertion in the Levenshtein distance algorithm
                    nfa.add_transition(std::make_tuple(i, e), EPSILON, std::make_tuple(i+1, e+1));

                    // Transitions for substitution in the Levenshtein distance algorithm
                    nfa.add_transition(std::make_tuple(i, e), ANY, std::make_tuple(i+1, e+1));
                }
            } // for e
        } // for lookupword

        for (unsigned e = 0; e <= k; e++) {
            if (e < k) {
                nfa.add_transition(std::make_tuple(lookupword.size(), e), ANY, std::make_tuple(lookupword.size(), e+1));
            }
            nfa.add_final_state(std::make_tuple(lookupword.size(), e));
        }

      } // init

     /**
      @brief Returns returns the first word in the corpus that is lexicographically greater than or equal to the input word.
      @param word input
    */
    Word next_in_corpus(const Word& input) const
    {
        auto pos = std::lower_bound(corpus.begin(), corpus.end(), input);
        if (pos != corpus.end()) {
            return *pos;
            }

        return NONE;

    } // next_in_corpus



   private: // variables
      NFAutomaton       nfa;        ///< the actual Levenshtein automaton
      DFAutomaton       dfa;        ///< and its deterministic equivalent
      WordVec           corpus;     ///< the list of all possible words
      unsigned          k;          ///< the max. allowed Lev-distance
      Word              lookupword; ///< all words in Lev-distance k from this word are searched

  }; // LevenshteinAutomaton


#endif // LEVAUTOMATON_H_INCLUDED
