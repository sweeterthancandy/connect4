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
        Tile_NotATile = 3,
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
                void Set(size_t x, size_t y, Tile t){
                        auto m = Map_(x,y);
                        switch(t){
                        case Tile_Hero:
                                mask_.set(m);
                                value_.set(m);
                                break;
                        case Tile_Villian:
                                mask_.set(m);
                                value_.reset(m);
                                break;
                        case Tile_Empty:
                                mask_.reset(m);
                                value_.reset(m);
                                break;
                        default:
                                break;
                        }
                        //Debug();

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
                void Debug()const{
                        PRINT( mask_.to_string() );
                        PRINT( value_.to_string() );
                }
        private:
                static constexpr size_t BitSize(){
                        return Width * Height;
                }
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


// returns the lowest (*this)(x, Level(x)) which is empty
unsigned Level(Board const& board, unsigned idx){
        unsigned y=0;
        for(;y!=board.Height();++y){
                if( board.Get(idx, y) == Tile_Empty)
                        break;
        }
        return y;
}
bool CanPlace(Board const& board, unsigned idx){
        return Level(board, idx) < board.Height();
}

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
std::string EvalToString(Eval e) {
        switch (e) {
        case Eval_NotFinished:
                return "Eval_NotFinished";
        case Eval_Draw:
                return "Eval_Draw";
        case Eval_Hero:
                return "Eval_Hero";
        case Eval_Villian:
                return "Eval_Villian";
        default:
                return "";
        }
}

struct ConnectFourLogic{
        struct SequenceConsumer{
                Eval Consume(Tile t){
                        if( t == last_ ){
                                // Case we have 2 consective Hero or Villian
                                ++count_;
                                if( count_ == 4 && eval_ == Eval_NotFinished ){
                                        switch(last_){
                                        case Tile_Hero:
                                                eval_ = Eval_Hero;
                                                break;
                                        case Tile_Villian:
                                                eval_ = Eval_Villian;
                                                break;
                                        }
                                }
                        } else if( t == Tile_Empty){
                                // Case we run into a blank
                                count_ = 0;
                                last_ = Tile_NotATile;
                        } else{
                                // Case we start a new run
                                count_ = 1;
                                last_ = t;
                        }
                        return Get();
                }
                Eval Get()const{
                        return eval_;
                }
        private:
                Tile last_{Tile_NotATile};
                size_t count_{0};
                Eval eval_{Eval_NotFinished};
        };

        Eval Evaluate(Board const& board)const{
                enum Group{
                        /*
                               /\
                                |
                                |
                                +--------->
                        */
                        Left,
                        Bottom,
                        /*
                                <---------+
                                          |
                                          |
                                         \/
                        */

                        Right,
                        Top,
                };
                /* for dx, dy, theres a difference 
                   between (1,-1) and (-1,1), because
                   we have the sequence.
                        (a,b), (a+dx, b+dy), (a+2*dx,b+2*dy)...,
                   where we start at either the left, the bottom
                        (0,0),...(a,b),...
                  
                       /\
                        |
                        |
                        +--------->

                */
                  
                  
                     
                struct Ticker{
                        Group group;
                        int dx;
                        int dy;
                };
                Ticker ticker[] = {
                        /*
                                | | | | | | | |
                                | | | | | | | |
                                | | | | | | | |
                                | | | | | | | |
                                | | | | | | | |
                                | |X|X|X|X| | |
                        */
                        { Left,    1,  0 },
                        /*
                                | | | | | | | |
                                | | |X| | | | |
                                | | |X| | | | |
                                | | |X| | | | |
                                | | |X| | | | |
                                | | | | | | | |
                        */
                        { Bottom,  0,  1 },
                        /*
                                | | | | | | |X|
                                | | | | | |X| |
                                | | | | |X| | |
                                | | | |X| | | |
                                | | | | | | | |
                                | | | | | | | |
                        */
                        { Left,    1,  1 },
                        { Bottom,  1,  1 },
                        /*
                                | | | | | | | |
                                | |X| | | | | |
                                | | |X| | | | |
                                | | | |X| | | |
                                | | | | |X| | |
                                | | | | | | | |
                        */
                        { Left,   1, -1 },
                        { Top  ,  1, -1 },
                }; 

                for( auto const& decl : ticker ){
                        int start_x = 0;
                        int start_y = 0;

                        switch(decl.group){
                        case Top:
                                start_y = board.Height() -1; 
                                break;
                        case Right:
                                start_x = board.Width() -1; 
                                break;
                        }

                        for(;;){
                                SequenceConsumer seq;
                                for(int i=0;;++i){
                                        auto x =  start_x + i * decl.dx;
                                        auto y =  start_y + i * decl.dy;

                                        if( ! ( 0 <= x && x < board.Width() &&
                                                0 <= y && y < board.Height() ) ){
                                                break;
                                        }
                                        seq.Consume( board.Get(x,y ) );
                                }
                                if( seq.Get() != Eval_NotFinished )
                                        return seq.Get();

                                switch(decl.group){
                                case Left:
                                case Right:
                                        ++start_y;
                                        if( start_y == board.Height() )
                                                goto __next__;
                                        break;
                                case Bottom:
                                case Top:
                                        ++start_x;
                                        if( start_x == board.Width() )
                                                goto __next__;
                                        break;
                                }
                        }
                        __next__:;
                }
                        
                return Eval_NotFinished;
        }
};

struct BoardInputOutput{
        void Display(Board const& board, std::ostream& ostr = std::cout)const{
                for(unsigned y=board.Height();y!=0;){
                        --y;
                        for(unsigned x=0;x!=board.Width();++x){
                                ostr << '|' << TileToString(board.Get(x,y));
                        }
                        ostr << "|\n";
                }
                ostr << "\n";
        }
        boost::optional<Board> ParseBoard(size_t width, size_t height, std::string str)const{
                boost::erase_all(str, "|");
                boost::erase_all(str, "\n");
                if( str.size() != width*height){
                        return boost::none;
                }
                Board board;
                auto iter = str.begin();
                for(size_t y=height;y!=0;){
                        --y;
                        for(size_t x=0;x!=width;++x){
                                auto t = ParseTile(*iter);
                                if( t == Tile_NotATile){
                                        std::cerr << "Tile " << *iter << " is not a tile\n";
                                        return boost::none;
                                }
                                board.Set(x,y,t);
                                ++iter;
                        }
                }
                return std::move(board);
        }
        Tile ParseTile(char c)const{
                switch(c){
                case 'X':
                case 'x':
                        return Tile_Hero;
                case 'O':
                case 'o':
                case '0':
                        return Tile_Villian;
                case ' ':
                        return Tile_Empty;
                default:
                        return Tile_NotATile;
                }
        }
        std::string TileToString(Tile t)const{
                switch (t) {
                case Tile_Empty:
                        return " ";
                case Tile_Hero:
                        return "X";
                case Tile_Villian:
                        return "O";
                default:
                        return "N";
                }
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
        
        void Place(ConnectFourLogic const& logic, unsigned idx){
                board_.Set(idx, Level(board_, idx), GetCurrentTile() );
                NextPlayer();
                auto e = logic.Evaluate(board_);
                if( e != Eval_NotFinished )
                        ctrl_ = Ctrl_Finish;
        }
        void Display()const{
                static BoardInputOutput io;
                io.Display(board_);
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
                        std::cout << "Finished\n";
                        ptr->Ctx().Display();
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

#include <gtest/gtest.h>

TEST(EvalTest,Eval){
        BoardInputOutput io;
        ConnectFourLogic logic;

        std::vector<std::tuple<std::string, Eval> > ticker = {
                {
                        "|     |"
                        "|     |"
                        "|     |"
                        "|     |", Eval_NotFinished},
                {
                        "| XXXX|"
                        "|     |"
                        "|     |"
                        "|     |", Eval_Hero},
                {
                        "|XXXX |"
                        "|     |"
                        "|     |"
                        "|     |", Eval_Hero},
                {
                        "|     |"
                        "|     |"
                        "|     |"
                        "|OXXXX|", Eval_Hero},
                {
                        "|     |"
                        "|     |"
                        "|     |"
                        "|OXOXX|", Eval_NotFinished},
                {
                        "|  X  |"
                        "|  X  |"
                        "|  X  |"
                        "|  X  |", Eval_Hero},
                {
                        "|X    |"
                        "| X   |"
                        "|  X  |"
                        "|   X |", Eval_Hero},
                {
                        "|    X|"
                        "|   X |"
                        "|  X  |"
                        "| X   |", Eval_Hero},
                {
                        "|   X |"
                        "|  X  |"
                        "| X   |"
                        "|X    |", Eval_Hero},
                {
                        "|   X |"
                        "|     |"
                        "| X   |"
                        "|X    |", Eval_NotFinished},
        };
        for( auto const& item : ticker){
                auto board = io.ParseBoard(5,4, std::get<0>(item)).get();
                auto e = logic.Evaluate(board);
                #if 0
                io.Display(board);
                PRINT(EvalToString(e));
                #endif
                EXPECT_EQ(std::get<1>(item), e) << std::get<0>(item);
        }

#if 0

        };

        std::string input = 
                            
                            
                            ;
        auto board = io.ParseBoard(5,4, input).get();
        io.Display(board);

        PRINT( Level(board, 0 ));
        PRINT( Level(board, 1 ));
        PRINT( Level(board, 2 ));
        PRINT( Level(board, 3 ));
        PRINT( Level(board, 4 ));
        #endif




}

//int main(){
        //Driver0();
//}

int main(int argc, char **argv) {
          ::testing::InitGoogleTest(&argc, argv);
            return RUN_ALL_TESTS();
}

