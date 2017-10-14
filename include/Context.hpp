#ifndef CONNECT_FOUR_CONTEXT_HPP
#define CONNECT_FOUR_CONTEXT_HPP

#include "GameLogic.hpp"
#include "InputOutput.hpp"

struct GameContext{
        explicit GameContext(Board const& board):
                board_{board}
        {}
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
        auto Evaluate(ConnectFourLogic const& logic)const{
                return logic.Evaluate(board_);
        }
private:
        Board board_;
        Player turn_{Player_Hero};
        Ctrl ctrl_{Ctrl_PlayerMove};
};

#endif // CONNECT_FOUR_CONTEXT_HPP
