#ifndef CONNECT_FOUR_TTT_LOGIC_HPP
#define CONNECT_FOUR_TTT_LOGIC_HPP

struct TickTackToeLogic{

        template<class BoardType, class V>
        void ForEachMove(BoardType const& board, Player p, V v)const{
                static_assert( BoardType::Width() == 3 ,"");
                static_assert( BoardType::Height() == 3 ,"");

                for( unsigned x=0;x!=BoardType::Width();++x){
                        for( unsigned y=0;y!=BoardType::Width();++x){
                        }
                }
        }
        template<class BoardType>
        Eval Evaluate(BoardType const& board)const{
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
                for(int x=0;x!=board.Width();++x){
                        for(int y=0;y!=board.Height();++y){
                                if( board.Get(x,y) == Tile_Empty )
                                        return Eval_NotFinished;
                        }
                }
                return Eval_Draw;
        }
};
#endif // CONNECT_FOUR_TTT_LOGIC_HPP
