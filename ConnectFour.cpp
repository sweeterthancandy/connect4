#include "ConnectFour.hpp"
#include "Graph.hpp"

int main(){
        //BoardInputOutput io;
        //auto board = io.ParseBoard(6,5,"      "
                                       //"      "
                                       //"      "
                                       //"      "
                                       //"      ");
        //auto root = GenerateGameTree(board.get());
        auto root = GenerateGameTree();
        NodeMarker marker;
        marker.Run(root);

        PRINT( marker.Lookup(root.GetStart() ) );
        Profiler prof;
        prof.Run(root);


}
