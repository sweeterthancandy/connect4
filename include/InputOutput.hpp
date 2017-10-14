#ifndef CONNECT_FOUR_INPUT_OUTPUT_HPP
#define CONNECT_FOUR_INPUT_OUTPUT_HPP

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
#endif // CONNECT_FOUR_INPUT_OUTPUT_HPP
