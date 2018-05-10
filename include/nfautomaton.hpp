/*
Lisanne Wiengarten
Matrikelnr. 764870
EAIDCL WiSe13/14
gcc 4.9.1 C++11 Win10

Compact representation of a nondeterministic final automaton
*/

#include "dfautomaton.hpp"

#ifndef NFAUTOMATON_HPP_INCLUDED
#define NFAUTOMATON_HPP_INCLUDED

#define ANY       "ANY"
#define EPSILON   "EPSILON"
#define NONE      "\0"
#define NUL       "-"

/**
  @brief NFAutomaton is a class for representing
         standard nondeterministic final automata
         Includes a function to transform it into its deterministic version (DFAutomaton)
*/
class NFAutomaton
{
   public: // Types
      typedef DFAutomaton                       DFA;
      typedef std::tuple<int, int>              NState;
      typedef typename std::set<NState>         Stateset;

   private: // Types
      typedef std::string                       Word;
      typedef std::vector<Word>                 WordVec;
      typedef std::map<Word, Stateset>          WordStatesetMap;


   public: // Functions
    /**
      @brief Default constructor
    */
    NFAutomaton() {
        startStates.insert(std::make_tuple(0, 0));
        expand(startStates);
    }


    /**
      @brief Constructor from start state
      @param inputstate, the incoming state
    */
    NFAutomaton(const NState& inputstate)
    {
        startStates.insert(inputstate);
        expand(startStates);
    }

    /**
      @brief Adds a transition from one state to another with a certain input
      @param NState src
      @param Word input
      @param NState dest
    */
    void add_transition(const NState& src, const Word& input, const NState& dest)
    {
        transitions[src][input].insert(dest);

    } // add_transition

    /**
      @brief Adds a final state to the automaton
      @param NState state which will be added
    */
    void add_final_state(const NState& state)
    {
        final_states.insert(state);

    } // add_final_state

    /**
      @brief Tests whether a given state is among the final states
      @param NState state, the state to be tested
      @return true iff the state is final
    */
    const bool is_final_state(const NState& state) const
    {
        return final_states.find(state) != final_states.end();

    } // is_final

    /**
      @brief Tests whether a given set of states is among the final states
      @param Stateset states, the states to be tested
      @return true iff the stateset contains one or more final states
    */
    const bool contains_final_states(const Stateset& states) const
    {
        // True if the intersection of the incoming stateset with the set of final states contains at least one element
        std::vector<NState> intersection;
        std::set_intersection(states.begin(), states.end(), final_states.begin(), final_states.end(), std::back_inserter(intersection));

        return (intersection.size() > 0);

    } // contains_final_states

    /**
      @brief Expands a set of states
      @param states, a set of NStates
      @return the expanded set of NStates
    */
    Stateset expand(Stateset states)
    {
        std::deque<NState> state_queue(states.begin(), states.end());

        // for every state in the queue, look up which states can be reached from it by using EPSILON
        // these reachable states are added to the input set of states and is returned
        while (state_queue.size() > 0) {
            NState current = state_queue[0];
            state_queue.pop_front();

            WordStatesetMap current_map;
            if (transitions.count(current) == 1) {
                current_map = transitions.at(current);
            }

            Stateset newstates;
            if (current_map.count(EPSILON) == 1) {
                Stateset current_states(current_map.at(EPSILON));
                std::set_difference(current_states.begin(), current_states.end(), states.begin(), states.end(), std::inserter(newstates, newstates.end()));

                for (auto newstate: newstates) {
                    state_queue.push_back(newstate);
                    states.insert(newstate);
                }
            }
        }

        return states;

    } // expand

    /**
      @brief Looks for the next reachable state from a given set of states and an input word
      @param states, a set of NStates
      @param input, a Word
      @return The set of states reachable from this set of states with this input
    */
    Stateset next_states(Stateset states, Word input)
    {
        // The returned set of states contains all the states that can be reached
        // with the given input, the ANY symbol or EPSILON (by expanding the set in the last step)
        Stateset destinations;
        for (auto state: states) {
            if (transitions.count(state) == 1) {

                if (transitions[state].count(input) == 1) {
                    destinations.insert(transitions[state][input].begin(), transitions[state][input].end());
                }

                if (transitions[state].count(ANY) == 1) {
                    destinations.insert(transitions[state][ANY].begin(), transitions[state][ANY].end());
                }
             }
          }

        return expand(destinations);

    } // next_states

    /**
      @brief Retrieves all possible inputs for a given set of states
      @param states, a set of NStates
      @return The set of inputs valid for these states
    */
    WordVec get_inputs(Stateset states)
    {
        WordVec inputs;
        // Looks up every Word stored together with the given states in the map of transitions
        for (auto i = states.begin(); i != states.end(); i++) {
            if (transitions.count(*i) == 1) {

                for (auto j = transitions[*i].begin(); j != transitions[*i].end(); j++) {
                    inputs.push_back(j->first);
                }
            }
        }

        return inputs;

    } // get_inputs


    /**
      @brief Converts the whole NFA into its deterministic version
      @return An equivalent DFA
    */
    DFA to_dfa()
    {
        DFAutomaton dfa(expand(startStates));

        std::deque<Stateset> current_states;
        current_states.push_back(expand(startStates));
        std::set<Stateset> seen_states;

        while (current_states.size() > 0) {
            Stateset current = current_states[0];
            current_states.pop_front();

            // Get every valid Word for the current state
            WordVec inputs = get_inputs(current);

            for (auto input: inputs) {
                if (input == EPSILON) { continue; }

                // Get every state that is reachable from the current state with the current input
                // A set of NStates represents one single state in the DFA
                Stateset new_state = next_states(current, input);

                // If we did not already see these new states, they are added to the queue
                if (seen_states.find(new_state) == seen_states.end()) {
                    current_states.push_back(new_state);
                    seen_states.insert(new_state);

                    // new_state represents one single state from the new DFA
                    // if it contains at least one final state from the NFA, it becomes final in the DFA
                    if (contains_final_states(new_state)) {
                        dfa.add_final_state(new_state);
                    }
                }

                // If the current input is the nondeterministic ANY symbol *, a default transition is added to the DFA
                if (input.compare(ANY) == 0) {
                    dfa.set_default_transition(current, new_state);
                }

                else {
                    dfa.add_transition(current, input, new_state);
                }

            } // for inputs
        } // while

        return dfa;

    } // to_dfa

    /**
        @brief Prints the whole automaton in a readable way
    */
    void nfa_printer() const
    {
        for (auto i = transitions.begin(); i != transitions.end(); i++) {
            for (auto j = i->second.begin(); j != i->second.end(); j++) {
                std::cout << "(" << std::get<0>(i->first) << "," << std::get<1>(i->first) << ") -> ";
                std::cout << j->first << " -> [";
                for (auto k = j->second.begin(); k != j->second.end(); k++) {
                    std::cout << "(" << std::get<0>(*k) << "," << std::get<1>(*k) << ") ";
                }
                std::cout << "]\n";
            }
            std::cout << "\n";
        }
    } // nfa_printer

    /**
        @brief Prints a dot representation of the NFA to stream 'out'
        @param ostream out
    */
    void nfa_to_dot(std::ostream& out) const
    {

        out << "digraph FSM {" << std::endl;
        out << "graph [rankdir=LR, fontsize=14, center=1, orientation=Portrait];" << std::endl;
        out << "node  [font = \"Arial\", shape = circle, style=filled, fontcolor=black, color=lightgray]" << std::endl;
        out << "edge  [fontname = \"Arial\"]" << std::endl << std::endl;

        for (auto i = transitions.begin(); i != transitions.end(); i++) {
            out << std::get<0>(i->first) << std::get<1>(i->first) << " [label = \"" << nstate_to_string(i->first) << "\"";
            if (is_final_state(i->first) == true) { out << ", shape=doublecircle]" << std::endl; }
            else { out << "]" << std::endl; }

            for (auto j = i->second.begin(); j != i->second.end(); j++) {
                out << std::get<0>(i->first) << std::get<1>(i->first) << " -> ";

                for (auto k = j->second.begin(); k != j->second.end(); k++) {
                    out << std::get<0>(*k) << std::get<1>(*k) << " [label = \"" << j->first << "\"]\n";
                }
            }
        }
        out << "}" << std::endl;

    } // nfa_to_dot


    /**
        @brief Converts an NState into a readable string representation
        @param state, an NState
        @return The NState represented as a string for easier printing and reading
    */
    std::string nstate_to_string(const NState& state) const
    {
        std::string n;

        std::ostringstream zero;    zero << std::get<0>(state);
        std::ostringstream one;     one << std::get<1>(state);

        n += "(" + zero.str() + "," + one.str() + ")";

        return n;

    } // nstate_to_string


   private: // variables
      Stateset                          startStates;     ///< the automaton's start state
      std::map<NState, WordStatesetMap> transitions;     ///< the map containing all transitions of the automaton
      Stateset                          final_states;    ///< the set of final states

  }; // NFAutomaton

#endif // NFAUTOMATON_HPP_INCLUDED
