#ifndef CONNECT_FOUR_BOARD_HPP
#define CONNECT_FOUR_BOARD_HPP

#include "Decl.hpp"

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
                //right_.Set(Width_ - x - 1, y, t);
        }
        Tile Get(size_t x, size_t y)const{
                return left_.Get(x,y);
        }
        auto Hash()const{
                Detail::BoardImpl<Width_, Height_> mirror;
                for(size_t x=0;x!=Width_;++x){
                        for(size_t y=0;y!=Height_;++y){
                                mirror.Set(Width_ -1 - x,y, left_.Get(x,y));
                        }
                }
                return std::min(left_.Hash(), mirror.Hash() );
                //return left_.Hash();
        }
        auto Width()const{ return Width_; }
        auto Height()const{ return Height_; }
private:
        Detail::BoardImpl<Width_, Height_> left_;
        //Detail::BoardImpl<Width_, Height_> right_;
};


using Board = GenericBoard<7,6>;


inline
unsigned Level(Board const& board, unsigned idx){
        unsigned y=0;
        for(;y!=board.Height();++y){
                if( board.Get(idx, y) == Tile_Empty)
                        break;
        }
        return y;
}

inline
bool CanPlace(Board const& board, unsigned idx){
        return Level(board, idx) < board.Height();
}

// returns number of tiles different from Empty
inline
unsigned PopCount(Board const& board){
        unsigned ret=0;
        for(size_t x=0;x!=board.Width();++x){
                for(size_t y=0;y!=board.Height();++y){
                        ret += ( board.Get(x,y) == Tile_Empty ? 0 : 1 );
                }
        }
        return ret;
}

#endif // CONNECT_FOUR_BOARD_HPP
