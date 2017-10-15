#include <iostream>
#include <iostream>
#include <tuple>
#include <map>
#include <cassert>
#include <sstream>
#include <random>
#include <algorithm>
#include <vector>
#include <set>
#include <bitset>
#include <boost/preprocessor.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/optional.hpp>

#define PRINT_SEQ_detail(r, d, i, e) do{ std::cout << ( i ? ", " : "" ) << BOOST_PP_STRINGIZE(e) << " = " << (e); }while(0);
#define PRINT_SEQ(SEQ) do{ BOOST_PP_SEQ_FOR_EACH_I( PRINT_SEQ_detail, ~, SEQ) std::cout << std::endl; }while(0)
#define PRINT(X) PRINT_SEQ((X))

/*
        Connect Four
        
        | | | | | | | |
        | | | | | | | |
        | | | | | | | |
        | |X| | | | | |
        | |O|X|O|X| | |
        | |O|O|X|O| | |


                We create a graph, with nodes of states of the Game, where a
        state is a 2-tuple of (board,turn). We then construct the directed
        graph, which edges which represent placements of tiles.
                The terminal nodes of the graph are those for which their
        is a winner, loser, or draw.

                The strategyies for the respected players is a subset of the
        graph for which.
        
                
*/

#include "Board.hpp"
#include "Context.hpp"
#include "Decl.hpp"
#include "GameLogic.hpp"
#include "InputOutput.hpp"



// returns the lowest (*this)(x, Level(x)) which is empty











