#include "ConnectFour.hpp"

void Driver0(){
        ConnectFourLogic logic;
        std::map<decltype(std::declval<GameContext>().Hash()), Node*> nodes;

        auto start = new Node{GameContext{}};
        std::vector<Node*> stack{start};

        nodes[start->Ctx().Hash()] = start;

        for(;stack.size();){
                auto ptr = stack.back();
                stack.pop_back();
                auto const& ctx{ptr->Ctx()};

                if( ptr->Ctx().GetCtrl() == Ctrl_Finish ){
                        //std::cout << "Finished\n";
                        //ptr->Ctx().Display();
                        continue;
                }

                //ptr->Ctx().Display();

                // try to place a tile
                for( unsigned x=0;x!=ctx.BoardWidth();++x){
                        if( ! CanPlace( ctx.GetBoard(), x) ){
                                continue;
                        }
                        //ctx.Display();
                        GameContext nextCtx{ctx};
                        nextCtx.Place(logic, x);


                        auto h = nextCtx.Hash();
                        auto iter = nodes.find(h);
                        if( iter == nodes.end()){
                                auto tmp = new Node{std::move(nextCtx)};
                                nodes.emplace(h, tmp);
                                stack.push_back(tmp);
                                iter = nodes.find(h);
                                //tmp->Ctx().Display();
                        }

                        ptr->AddEdge( x, iter->second);
                }
                if( nodes.size() % 100 == 0 )
                        PRINT( nodes.size() );
        }


        PRINT( nodes.size() );
        PRINT( stack.size() );

}


int main(){
        Driver0();
}
