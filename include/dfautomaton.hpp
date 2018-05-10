/*
Lisanne Wiengarten
Matrikelnr. 764870
EAIDCL WiSe13/14
gcc 4.9.1 C++11 Win10

Compact representation of a deterministic final automaton
*/

#ifndef DFAUTOMATON_HPP_INCLUDED
#define DFAUTOMATON_HPP_INCLUDED

#define ANY       "ANY"
#define EPSILON   "EPSILON"
#define NONE      "\0"
#define NUL       "-"

#include <iostream>

/**
  @brief DFAutomaton is a class for representing
         standard deterministic final automata
         Includes functions used to find words in Levenshtein distance
*/
class DFAutomaton
  {
   public: // Types
      typedef std::tuple<int, int>              NState;
      typedef typename std::set<NState>         DState;

   private: // Types
      typedef std::string                       Word;
      typedef std::vector<Word>                 WordVec;
      typedef std::map<Word, DState>            WordStateMap;


   public: // Functions
    /**
      @brief Default constructor
    */
    DFAutomaton() {
        startState.insert(std::make_tuple(0, 0));
    }

    /**
      @brief Constructor from deterministic start state
      @param inputState, the incoming start state
    */
    DFAutomaton(const DState& inputState)
    {
        startState = inputState;
    }

     /**
      @brief Constructor from nondeterministic start state
      @param inputState, the incoming start state
    */
    DFAutomaton(const NState& inputState)
    {
        startState.insert(inputState);
    }

    /**
      @brief Adds a transition from one state to another with a certain input
      @param src, the first DState
      @param input, the Word label
      @param dest, the reached DState
    */
    void add_transition(const DState& src, const Word& input, const DState& dest)
    {
        for (auto s: dest) {
            transitions[src][input].insert(s);
        }

    } // add_transition

    /**
      @brief Establishes a default transition without input data
      @param DState src
      @param DState dest
    */
    void set_default_transition(const DState& src, const DState& dest)
    {
        for (auto s: dest) {
            defaults[src].insert(s);
        }

    } // set_default_transition

    /**
      @brief Adds a final state to the automaton
      @param state, a DState which will be added
    */
    void add_final_state(const DState& state)
    {
        final_states.insert(state);

    } // add_final_state

    /**
      @brief Tests whether a given state is among the final states
      @param DState state, the state to be tested
      @return true iff the state is final
    */
    const bool is_final(const DState& state) const
    {
       return final_states.find(state) != final_states.end();

    } // is_final

    /**
      @brief Looks for the next reachable state from a given state and an input word
      @param src, a DState
      @param input, a Word
      @return The state reachable from this state and input
    */
    DState next_state(const DState& src, const Word& input)
    {
        // If the given input is valid for this state, all reachable states from the map of transitions are returned
        if (transitions[src].count(input) == 1) {  return transitions[src][input];   }

        // Else if this state can be found in the default transitions, all reachable states from the map of defaults are returned
        else {
            if (defaults.count(src) == 1) { return defaults[src];  }
        }

        return NOSTATE;

    } // next_state

    /**
      @brief Searches the automaton for the next valid Word given an input Word
      @param input, a Word
      @return The next valid Word from this one
    */
    Word next_valid(const Word& input)
    {
        DState state = startState;
        std::deque<std::tuple<Word, DState, Word>> current_tuples;
        bool looper = true;

        int i = 0;
        for (; i < input.size(); i++) {
            // Word x is the letter at the current position in the input
            Word x(1, input[i]);
            // Create and add a new tuple with the first part of the input, the current state and x
            current_tuples.push_back(std::make_tuple(input.substr(0,i), state, x));

            state = next_state(state, x);

            // If there is no state reachable from the current state with input x, the loop is stopped
            if (state == NOSTATE) {
                looper = false;
                break;
            }

        } // for
        // If the loop was not stopped by the NOSTATE condition, another tuple is created and added
        if (looper == true) {
            current_tuples.push_back(std::make_tuple(input.substr(0,i+1), state, NONE));
        }

        // If the recently found state is final, the given input was already valid and can be returned
        if (is_final(state) == true) {
            return input;
        }

        while (current_tuples.size() > 0) {
            // Retrieve the elements from the last tuple in the queue and pop this tuple
            Word path = std::get<0>(current_tuples.back());
            state = std::get<1>(current_tuples.back());
            Word x = std::get<2>(current_tuples.back());
            current_tuples.pop_back();

            x = find_next_edge(state, x);

            // If there is a valid next edge from the current state with Word x,
            // then the current path is extended by x
            if (x != NONE) {
                path += x;
                state = next_state(state, x);

                // If the next reachable state with the current state and Word x is final, the path is valid and can be returned
                if (is_final(state) == true) {
                    return path;
                }

                current_tuples.push_back(std::make_tuple(path, state, NONE));
            } // if x

        } // while

        return NONE;

    } // next_valid

    /**
      @brief Retrieves the next valid edge given a DState and an input Word
      @param state, a DState
      @param Word x
      @return The next valid edge (Word)
    */
    Word find_next_edge(DState state, Word x)
    {
        if (x == NONE) {
            x = NUL;
       }
        else {
            // x will be one single character; now retrieve the next ascii char and transform x back
            int i = *(x.c_str());
            Word next(1, i+1);
            x = next;
       }

       if (transitions.find(state) != transitions.end()) {
           // If x is a valid input for the given state or if this state is among the default transitions, then x is valid and can be returned
           if (transitions[state].find(x) != transitions[state].end() || defaults.find(state) != defaults.end()) {
                return x;
            }

            // Get all valid inputs for the current state and sort them alphabetically
            WordVec inputs;
            for (auto iter = transitions[state].begin(); iter != transitions[state].end(); iter++) {
                inputs.push_back(iter->first);
            }
            std::sort(inputs.begin(), inputs.end());

            // Now find the leftmost position where x fits between these inputs and return the Word at this position
            auto pos = std::lower_bound(inputs.begin(), inputs.end(), x);
            if (pos != inputs.end()) {
                return *pos;
            }

       }

        return NONE;

    } // find_next_edge


    /**
        @brief Prints the whole automaton in a readable way
    */
    void dfa_printer() const
    {
        std::cout << "My DFA contains these transitions:\n";
        for (auto i = transitions.begin(); i != transitions.end(); i++) {
            for (auto j = i->second.begin(); j != i->second.end(); j++) {
                std::cout << dstate_to_string(i->first) << " -> ";
                std::cout << j->first << " -> " << dstate_to_string(j->second) << "\n";
            }
            std::cout << "\n";
        }

    } // dfa_printer


    /**
        @brief Prints a dot representation of the DFA to stream 'out'
        @param ostream out
    */
    void dfa_to_dot(std::ostream& out) const
    {

        out << "digraph FSM {" << std::endl;
        out << "graph [rankdir=LR, fontsize=14, center=1, orientation=Portrait];" << std::endl;
        out << "node  [font = \"Arial\", shape = circle, style=filled, fontcolor=black, color=lightgray]" << std::endl;
        out << "edge  [fontname = \"Arial\"]" << std::endl << std::endl;

        for (auto i = transitions.begin(); i != transitions.end(); i++) {
            for (auto state_iter = i->first.begin(); state_iter != i->first.end(); state_iter++) {
                out << std::get<0>(*state_iter) << std::get<1>(*state_iter);
            }
            out << " [label = \"" << dstate_to_string(i->first) << "\"";

            if (is_final(i->first) == true) { out << ", shape = doublecircle]" << std::endl; }
            else { out << "]" << std::endl; }


            for (auto j = i->second.begin(); j != i->second.end(); j++) {
                for (auto state_iter = i->first.begin(); state_iter != i->first.end(); state_iter++) {
                    out << std::get<0>(*state_iter) << std::get<1>(*state_iter);
                }

                out << " -> ";

                for (auto state_iter = j->second.begin(); state_iter != j->second.end(); state_iter++) {
                    out << std::get<0>(*state_iter) << std::get<1>(*state_iter);
                }
                out << " [label = \"" << j->first << "\"]\n";
            }
        }

        out << "}" << std::endl;

    } // dfa_to_dot



    /**
        @brief Converts a DState into a readable string representation
        @param state, a DState
        @return The DState represented as a string for easier printing and reading
    */
    std::string dstate_to_string(const DState& state) const
    {
        Word d = "[";
        for (auto i = state.begin(); i != state.end(); i++) {
            std::ostringstream zero;    zero << std::get<0>(*i);
            std::ostringstream one;     one << std::get<1>(*i);

            d += "(" + zero.str() + "," + one.str() + ")";
        }
        d += "]";

        return d;

    } // dstate_to_string


   private: // variables
      DState                            startState;     ///< the automaton's start state
      std::map<DState, WordStateMap>    transitions;    ///< the map containing all transitions of the automaton
      std::set<DState>                  final_states;   ///< the set of final states
      std::map<DState, DState>          defaults;       ///< the map for storing all default transitions
      DState                            NOSTATE;        ///< default for invalid or nonexisting states

  }; // DFAutomaton

#endif // NFAUTOMATON_HPP_INCLUDED
