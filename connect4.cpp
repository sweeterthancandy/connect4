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

#define PRINT_SEQ_detail(r, d, i, e) do{ std::cout << ( i ? ", " : "" ) << BOOST_PP_STRINGIZE(e) << " = " << (e); }while(0);
#define PRINT_SEQ(SEQ) do{ BOOST_PP_SEQ_FOR_EACH_I( PRINT_SEQ_detail, ~, SEQ) std::cout << "\n"; }while(0)
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

enum Tile{
        Tile_Empty   = 0,
        Tile_Hero    = 1,
        Tile_Villian = 2,
};
std::string TileToString(Tile e) {
        switch (e) {
        case Tile_Empty:
                return " ";
        case Tile_Hero:
                return "X";
        case Tile_Villian:
                return "O";
        default:
                return "";
        }
}
#if 0
struct Board{
        explicit Board(unsigned width, unsigned height)
                :width_{width}, height_{height}, rep_(Width() * Height(), ' ')
        {}
        char& operator()(unsigned x, unsigned y){
                return rep_[map_(x,y)];
        }
        char operator()(unsigned x, unsigned y)const{
                return rep_[map_(x,y)];
        }
        unsigned Width()const{ return width_; }
        unsigned Height()const{ return height_; }
        void Display()const{
                for(unsigned y=Height();y!=0;){
                        --y;
                        for(unsigned x=0;x!=Width();++x){
                                std::cout << '|' << (*this)(x,y);
                        }
                        std::cout << "|\n";
                }
                std::cout << "\n";
        }
        std::string Hash()const{
                #if 1
                std::string alt;
                for(unsigned y=0;y!=Height();++y){
                        for(unsigned x=Width();x!=0;){
                                --x;
                                alt += (*this)(x,y);
                        }
                }
                if( alt < rep_)
                        return std::move(alt);
                return rep_;
                #endif
                return rep_;
        }
        char Full(unsigned idx)const{
                return (*this)(idx, Height()-1) != ' ';
        }
private:
        unsigned map_(unsigned x, unsigned y)const{
                return y * width_ + x;
        }
        unsigned width_;
        unsigned height_;
        std::string rep_;
};
#endif

namespace Detail{
        template<size_t Width, size_t Height>
        struct BoardImpl{
                static constexpr size_t BitSize(){ return Width * Height; }
                void Set(size_t x, size_t y, Tile t){
                        auto m = Map_(x,y);
                        mask_.set(m);
                        if( t == Tile_Hero )
                                value_.set(m);
                }
                Tile Get(size_t x, size_t y)const{
                        auto m = Map_(x,y);
                        if( ! mask_.test(m) )
                                return Tile_Empty;
                        if( value_.test(m) )
                                return Tile_Hero;
                        return Tile_Villian;
                }
                auto Hash()const{
                        return std::make_tuple( mask_.to_ullong(), value_.to_ullong() );
                }
        private:
                static constexpr size_t Map_(size_t x, size_t y){
                        return y * Width + x;
                }
                std::bitset< BitSize() > mask_;
                std::bitset< BitSize() > value_;
        };
}

template<size_t Width_, size_t Height_>
struct GenericBoard{
        void Set(size_t x, size_t y, Tile t){
                left_.Set(x,y,t);
                right_.Set(Width_ - x - 1, y, t);
        }
        Tile Get(size_t x, size_t y)const{
                return left_.Get(x,y);
        }
        auto Hash()const{
                return std::min(left_.Hash(), right_.Hash() );
                //return left_.Hash();
        }
        auto Width()const{ return Width_; }
        auto Height()const{ return Height_; }
private:
        Detail::BoardImpl<Width_, Height_> left_;
        Detail::BoardImpl<Width_, Height_> right_;
};


using Board = GenericBoard<5,4>;



enum Ctrl{
        Ctrl_PlayerMove,
        Ctrl_Finish,
};

enum Player{
        Player_Hero,
        Player_Villian,
        Player_NotAPlayer,
};

enum Eval{
        Eval_NotFinished,
        Eval_Draw,
        Eval_Hero,
        Eval_Villian,
};

struct ConnectFourLogic{
        Eval Evaluate(Board const& board)const{

                auto match = [](char a, char b, char c, char d){
                        return
                                ( a == 'X' || a == 'O' ) &&
                                a == b &&
                                c == d &&
                                a == c;
                };
                auto eval = [](char x){
                        switch(x){
                        case 'X':
                                return Eval_Hero;
                        case 'O':
                                return Eval_Villian;
                        default:
                                return Eval_NotFinished;
                        }
                };


                // long ways
                for(unsigned x=0;x < board.Width(); ++x){
                        for(unsigned y=0;y < board.Height(); ++y){
                                bool cond_hori = x + 3 < board.Width();
                                bool cond_vert = y + 3 < board.Height();

                                if( cond_hori ){
                                        if( match(board.Get(x+0,y+0),
                                                  board.Get(x+1,y+0),
                                                  board.Get(x+2,y+0),
                                                  board.Get(x+3,y+0))){
                                                return eval(board.Get(x,y));
                                        }

                                }
                                if( cond_vert ){
                                        if( match(board.Get(x+0,y+0),
                                                  board.Get(x+0,y+1),
                                                  board.Get(x+0,y+2),
                                                  board.Get(x+0,y+3))){
                                                return eval(board.Get(x,y));
                                        }

                                }
                                if( cond_hori && cond_vert ){
                                        if( match(board.Get(x+0,y+0),
                                                  board.Get(x+1,y+1),
                                                  board.Get(x+2,y+2),
                                                  board.Get(x+3,y+3))){
                                                return eval(board.Get(x,y));
                                        }

                                }
                        }
                }
                return Eval_NotFinished;
        }
};

struct GameContext{
        auto BoardHeight()const{
                return board_.Height();
        }
        auto BoardWidth()const{
                return board_.Width();
        }
        void NextPlayer(){
                switch(turn_){
                case Player_Hero:
                        turn_ = Player_Villian;
                        break;
                case Player_Villian:
                        turn_ = Player_Hero;
                        break;
                default:
                        break;
                }
        }
        Tile GetCurrentTile(){
                switch(turn_){
                case Player_Hero:
                        return Tile_Hero;
                case Player_Villian:
                        return Tile_Villian;
                default:
                        return Tile_Empty;
                }
        }
        //std::tuple<decltype(board_.Hash()), Player, Ctrl> 
        auto Hash()const{
                return std::make_tuple( board_.Hash(), turn_, ctrl_);

        }
        auto const& GetBoard()const{ return board_; }
        
        // returns the lowest (*this)(x, Level(x)) which is empty
        unsigned Level(unsigned idx)const{
                unsigned y=0;
                for(;y!=board_.Height();++y){
                        if( board_.Get(idx, y) == Tile_Empty)
                                break;
                }
                //PRINT_SEQ(("Level")(y));
                return y;
        }
        unsigned Full(unsigned idx)const{
                auto ret =  Level(idx) == board_.Height();
                PRINT_SEQ(("Full")(idx)(ret));
                return ret;
        }
        bool CanPlace(unsigned idx){
                return Level(idx) == BoardHeight();
        }
        void Place(ConnectFourLogic const& logic, unsigned idx){
                board_.Set(idx, Level(idx), GetCurrentTile() );
                NextPlayer();
                auto e = logic.Evaluate(board_);
                if( e != Eval_NotFinished )
                        ctrl_ = Ctrl_Finish;
        }
        void Display()const{
                for(unsigned y=board_.Height();y!=0;){
                        --y;
                        for(unsigned x=0;x!=board_.Width();++x){
                                std::cout << '|' << TileToString(board_.Get(x,y));
                        }
                        std::cout << "|\n";
                }
                std::cout << "\n";
        }
        auto GetCtrl()const{ return ctrl_; }
private:
        Board board_;
        Player turn_{Player_Hero};
        Ctrl ctrl_{Ctrl_PlayerMove};
};


struct Node{
        explicit Node(GameContext ctx):ctx_{std::move(ctx)}{}
        auto const& Ctx()const{ return ctx_; }
        void AddEdge(unsigned x, Node* n){
                edges_.emplace_back(x,n);
        }
private:

        GameContext ctx_;
        std::vector< std::tuple<unsigned, Node*> > edges_;
};




struct GameTree{
private:
        Node* start_;
};

struct GameTreeGenerator{
        template<class F>
        void Generate(Board const& initial, F gen){
        }
};


struct ConnectFourEval{
        Eval Evaluate(Board const& board)const;
};




int main(){
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
                        std::cout << "Finished\n";
                        continue;
                }

                //ptr->Ctx().Display();

                // try to place a tile
                for( unsigned x=0;x!=ctx.BoardWidth();++x){
                        if( ctx.Full(x) ){
                                std::cout << "full\n";
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
                                tmp->Ctx().Display();
                        }

                        ptr->AddEdge( x, iter->second);
                }
                if( nodes.size() % 100 == 0 )
                        PRINT( nodes.size() );
        }


        PRINT( nodes.size() );
        PRINT( stack.size() );

}
