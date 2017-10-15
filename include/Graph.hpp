#ifndef CONNECT_FOUR_GRAPH_HPP
#define CONNECT_FOUR_GRAPH_HPP

#include <boost/format.hpp>

struct Node;

// This is for sugar
// TODO, we using invariants (expliting symetric, inverse mappings,
// can add flags for inverses etc here, reduce node set by ~4

struct Edge{
        explicit Edge(char idx, Node* node)
                :idx_{idx}, node_{node}
        {}
        auto Idx()const { return idx_; }
        auto GetNode()const{ return node_; }
private:
        char idx_;
        Node* node_;
};

struct Node{
        explicit Node(GameContext ctx):ctx_{std::move(ctx)}{}
        auto const& Ctx()const{ return ctx_; }
        void AddEdge(char x, Node* n){
                edges_.emplace_back(x,n);
        }
        auto begin()const{ return edges_.begin(); }
        auto end()const{ return edges_.end(); }
        bool IsTerminal()const{ return edges_.size()==0; }
private:

        GameContext ctx_;
        std::vector< Edge > edges_;
};

struct GameTree{
        explicit GameTree(Node* start):start_{start}{}
        Node* GetStart(){ return start_; }
private:
        Node* start_;
};

GameTree GenerateGameTree(Board const& board = Board{}){
        ConnectFourLogic logic;
        std::map<decltype(std::declval<GameContext>().Hash()), Node*> nodes;

        auto start = new Node{GameContext{board}};
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
                if( nodes.size() % 10000 == 0 )
                        PRINT( nodes.size() );
        }
        
        PRINT( nodes.size() );
        PRINT( stack.size() );

        return GameTree{start};
}


struct NodeMarker{
        enum Marking{
                Marked_Zero = 0,
                Marked_Win  = 1,
                Marked_Lose = 2,
                Marked_Draw = 4,
        };

        void Run(GameTree& root){
                ConnectFourLogic logic;
                std::vector< Node const* > ticker = { root.GetStart() };
                for( ; ticker.size(); ){
                        auto ptr = ticker.back();
                        ticker.pop_back();

                        if( ptr->IsTerminal()){
                                switch( ptr->Ctx().Evaluate(logic) ){
                                case Eval_NotFinished:
                                        //ptr->Ctx().Display();
                                        throw std::domain_error("bad game tree");
                                case Eval_Hero:
                                        marks_.emplace(ptr, Marked_Win);
                                        break;
                                case Eval_Villian:
                                        marks_.emplace(ptr, Marked_Lose);
                                        break;
                                case Eval_Draw:
                                        marks_.emplace(ptr, Marked_Draw);
                                        break;
                                }
                                continue;
                        }


                        // Go thought all edges, if there are any unmarked, 
                        // enqueue marking, and enqueue node for later processing
                        bool process = true;
                        for( auto const& edge : *ptr ){
                                if( marks_.count(edge.GetNode()) == 0 ){
                                        if( process == true ){
                                                // reque myself
                                                ticker.emplace_back( ptr );
                                                process = false;
                                        }
                                        ticker.emplace_back( edge.GetNode() );
                                }
                        }
                        if( ! process ){
                                continue;
                        }
                        if( ptr->Ctx().ActivePlayer() == Player_Villian ){
                                int mark = 0;
                                for( auto const& edge : *ptr ){
                                        mark |= marks_[edge.GetNode()];
                                }
                                marks_[ptr] = mark;
                                continue;
                        } 

                        std::vector<int> aux;
                        for( auto const& edge : *ptr ){
                                aux.emplace_back( marks_[edge.GetNode()] );
                        }
                        std::sort( aux.begin(), aux.end(), [&](auto left, auto right){
                                static std::map<int, int> order = [](){
                                        int precedence[] = {
                                                Marked_Win,
                                                Marked_Win  | Marked_Draw,
                                                Marked_Draw,
                                                Marked_Win  | Marked_Draw | Marked_Lose,
                                                Marked_Win  | Marked_Lose,
                                                Marked_Draw | Marked_Lose,
                                                Marked_Lose,
                                        };
                                        std::map<int, int> ret;
                                        int index = 0;
                                        for( auto mark : precedence ){
                                                ret.emplace(mark, index++);
                                        }
                                        return std::move(ret);
                                }();
                                return order[left] < order[right];
                        });
                        marks_[ptr] = aux.front();

                }


                PRINT(marks_.size());
        }
        auto Lookup(Node const* ptr)const{
                return marks_.find(ptr)->second;
        }
private:
        std::map<Node const*, int > marks_;
};

struct Profiler{
        void Run(GameTree& root){
                std::map<Node const*, int > marked;
                std::vector< Node const* > ticker = { root.GetStart() };
                std::map<std::string, unsigned> profile_;
                for( ; ticker.size(); ){
                        auto ptr = ticker.back();
                        ticker.pop_back();
                        for( auto const& edge : *ptr ){
                                if( marked.count(edge.GetNode()) == 0 ){
                                        ticker.emplace_back( edge.GetNode() );
                                }
                        }

                        if( marked.count(ptr) != 0 )
                                continue;
                        marked.emplace(ptr, 1);

                        std::map<Tile, int> hist;
                        hist[Tile_Empty];
                        hist[Tile_Hero];
                        hist[Tile_Villian];
                        auto const& board = ptr->Ctx().GetBoard();
                        for(size_t x=0;x!=ptr->Ctx().BoardWidth();++x){

                                for(size_t y=0;y!=ptr->Ctx().BoardHeight();++y){
                                        ++hist[board.Get(x,y)];
                                }
                        }
                        std::stringstream sstr;
                        sstr << "{";
                        bool first = true;
                        for(auto const& p : hist ){
                                sstr << ( first ? (first=false, "") : ", ");
                                sstr << TileToMetaString(p.first) << ":" << p.second;
                        }
                        sstr << "}";
                        ++profile_[sstr.str()];
                        if( marked.size() % 10000 == 0 )
                                PRINT(marked.size());
                }
                for( auto const& p : profile_ ){
                        std::cout << boost::format("%-8s => %s") 
                                % p.second % p.first << "\n";
                }
        }
};
#endif // CONNECT_FOUR_GRAPH_HPP
