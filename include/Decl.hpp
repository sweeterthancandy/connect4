#ifndef CONNECT_FOUR_DECL_HPP
#define CONNECT_FOUR_DECL_HPP

enum Tile{
        Tile_Empty   = 0,
        Tile_Hero    = 1,
        Tile_Villian = 2,
        Tile_NotATile = 3,
};

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

#endif // CONNECT_FOUR_DECL_HPP
