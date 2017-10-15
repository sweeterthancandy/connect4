#ifndef CONNECT_FOUR_INPUT_OUTPUT_HPP
#define CONNECT_FOUR_INPUT_OUTPUT_HPP

struct BoardInputOutput{
        template<class BoardType>
        void Display(BoardType const& board, std::ostream& ostr = std::cout)const{
                for(unsigned y=board.Height();y!=0;){
                        --y;
                        for(unsigned x=0;x!=board.Width();++x){
                                ostr << '|' << TileToString(board.Get(x,y));
                        }
                        ostr << "|\n";
                }
                ostr << "\n";
        }
        template<class BoardType>
        std::string ToString(BoardType const& board)const{
                std::stringstream sstr;
                for(unsigned y=board.Height();y!=0;){
                        --y;
                        for(unsigned x=0;x!=board.Width();++x){
                                switch (board.Get(x,y)) {
                                default:
                                case Tile_Empty:
                                        sstr << "_";
                                        break;
                                case Tile_Hero:
                                        sstr << "X";
                                        break;
                                case Tile_Villian:
                                        sstr << "O";
                                        break;
                                }
                        }
                }
                return sstr.str();
        }
        template<class BoardType>
        boost::optional<BoardType> ParseBoard(std::string str)const{
                boost::erase_all(str, "|");
                boost::erase_all(str, "\n");
                if( str.size() != BoardType::Width() * BoardType::Height()){
                        return boost::none;
                }
                BoardType board;
                auto iter = str.begin();
                for(size_t y= BoardType::Height();y!=0;){
                        --y;
                        for(size_t x=0;x!=BoardType::Width() ;++x){
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
                return "Tile{" + boost::lexical_cast<std::string>(e) + "}";
        }
}
std::string TileToMetaString(Tile e) {
        switch (e) {
        case Tile_Empty:
                return "Empty";
        case Tile_Hero:
                return "Hero";
        case Tile_Villian:
                return "Villian";
        default:
                return "Tile{" + boost::lexical_cast<std::string>(e) + "}";
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
