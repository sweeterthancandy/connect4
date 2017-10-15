#include "ConnectFour.hpp"
#include "Graph.hpp"

#include <set>
#include <fstream>
#include <boost/timer/timer.hpp>
#include <boost/range/adaptor/reversed.hpp>

#include <boost/serialization/vector.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

#include <boost/variant.hpp>


void Driver0(){
        //BoardInputOutput io;
        //auto board = io.ParseBoard(6,5,"      "
                                       //"      "
                                       //"      "
                                       //"      "
                                       //"      ");
        //auto root = GenerateGameTree(board.get());
        auto root = GenerateGameTree();
        NodeMarker marker;
        marker.Run(root);

        PRINT( marker.Lookup(root.GetStart() ) );
        Profiler prof;
        prof.Run(root);


}


/*
        This represents the level of the board,
        ie for 
                (width,height,n)
                
                (7,6,0) => P =  {(0,0,0,0,0,0,0)}
                (7,6,1) => P =  {(1,0,0,0,0,0,0),
                                 (0,1,0,0,0,0,0),
                                 (0,1,1,0,0,0,0),
                                 ...
                                 }
                (7,6,2) => P =  {(1,0,0,0,0,0,0),
                                 (0,1,0,0,0,0,0),
                                 (0,1,1,0,0,0,0),
                                 ...
                                 }
 */

template<class T>
std::string tostring(T const& iter){
        std::stringstream sstr;
        sstr << "{";
        bool first = false;
        for( auto const& _ : iter){
                sstr
                        << ( first ? (first=false,"") : ", ")
                        << (int)_;
        }
        sstr << "}";
        return sstr.str();
}

struct LevelCombinationSet{
        using CombinationType = std::vector<char>;

        explicit LevelCombinationSet(char width, char height, char n)
                :width_{width}
                ,height_{height}
        {
                CombinationType iter(width, 0 );
                Recurse_( iter, n );
                #if 0
                char tmp = n; 
                for(auto& _ : iter){
                        _ = std::min(height, tmp);
                        tmp -= _;
                        if( _ == 0 )
                                break;
                }
                #endif

        }
        auto begin()const{ return set_.begin(); }
        auto end()const{ return set_.end(); }
private:
        void Recurse_( std::vector<char> proto, char n ){
                if( n == 0 ){
                        set_.insert(std::move(proto));
                        return;
                }
                for(size_t i=0;i!=proto.size();++i){
                        if( ! (proto[i] < height_ ) )
                                continue;
                        auto next = proto;
                        ++next[i];
                        Recurse_(std::move(next), n-1);
                }
        }
        char width_;
        char height_;
        std::set<CombinationType> set_;
};

std::string Bits(unsigned long long val){
        std::bitset<sizeof(val)*8> aux{val};
        return aux.to_string();
}

void PermuattionTest(){

        #if 0
        for(int k=0;;++k){
                boost::timer::auto_cpu_timer at;
                std::vector<int> proto(7*6, 0);
                for(int i=0;i!=k;++i)
                        proto[proto.size()-1-i] = 1;
                for(;;){
                        if( ! std::next_permutation(proto.begin(), proto.end()))
                                break;
                }
                PRINT(k);
                std::cout << "--------------\n";
        }
        #endif
        unsigned long long upper = 1;
        for(int i=0;i!=6*7;++i)
                upper *= 2;
        for(int k=0;;++k){
                boost::timer::auto_cpu_timer at;
                unsigned long long iter = 0;
                int n = 0;
                PRINT_SEQ((k)(iter)(upper));
                for(;iter < upper;++iter){
                        if( __builtin_popcountll(iter) == k ){
                                PRINT_SEQ( (Bits(iter))(__builtin_popcountll(iter)) );
                                ++n;
                        }
                }
                PRINT_SEQ((upper)(k)(n));
        }

        #if 0
        LevelCombinationSet ps(7,6,10);
        for( auto const& _ : ps )
                PRINT( tostring(_));
                #endif
        /*
                want sequence
                        s1 .. sn
                where there are only K sn for sn = Empty.
                This imples can take sequence
                        t1 ... tm  where m = n -k
                where ti \in {Hero, Villian},
                        and take 
                        q1 .. qk
                are index positions for inset the Empy ones

         */


}

struct SubGraph{
        void Push(Node* ptr){
                start_.push_back(ptr);
        }
        auto begin()const{ return start_.begin(); }
        auto end()const{ return start_.end(); }
        void Debug()const{
                PRINT( start_.size() );
                #if 0
                for( auto ptr : start_){
                        ptr->Ctx().Display();
                }
                #endif
        }
private:
        std::vector<Node*> start_;
};

struct Graph{
        void Push(SubGraph* sub){
                subs_.push_back(sub);
        }
        auto begin()const{ return subs_.begin(); }
        auto end()const{ return subs_.end(); }
        auto begin(){ return subs_.begin(); }
        auto end(){ return subs_.end(); }
        auto Back(){  return subs_.back(); }
        // Assume first one only have one entry
        auto Start(){ return *subs_.front()->begin(); }
        auto At(size_t idx)const{ return subs_[idx]; }
        auto Size()const{ return subs_.size(); }
private:
        // increasing popcount
        std::vector<SubGraph*> subs_;
};

#include <boost/pool/object_pool.hpp>

/*
        The essence of the problem is that I can't easily work backwards
        in this game. This is because I can't git the game tree
        with depth of 42 into memory. I also have to keep results in 
        memory, because otherwise I'll be re-computing the result
        for a certain board for each path from it.
                What I need to do is create auxiallary sets of all the
        board at each depth. From this, I can work backwards.

*/

#if 0
struct BoardAllocator{
        using hash_t = decltype(std::declval<Board>().Hash());
        // (was_allocated, ptr)
        std::pair<bool, Board*> FindOrAllocate(Board const& brd){
                auto h = brd.Hash();
                auto iter = nodes_.find(h);
                if( iter == nodes_.end()){
                        auto tmp = pool_.construct(brd);
                        nodes_.emplace(h, tmp);
                        return std::make_pair(true, tmp);
                }
                return std::make_pair(false, iter->second);
        }
private:
        boost::object_pool<Board> pool_;
        std::map<hash_t, Board*> nodes_;
};

SubGraph* CreateSubGraph(BoardAllocator& alloc, SubGraph const& entry){
        ConnectFourLogic logic;

        SubGraph* result = new SubGraph;

        for( auto node : entry){
                auto const& ctx{node->Ctx()};

                // try to place a tile
                for( unsigned x=0;x!=ctx.BoardWidth();++x){
                        if( ! CanPlace( ctx.GetBoard(), x) ){
                                continue;
                        }
                        GameContext nextCtx{ctx};
                        nextCtx.Place(logic, x);

                        //nextCtx.Display();


                        auto ret = alloc.FindOrAllocate(nextCtx);
                        if( ret.first) {
                                result->Push(ret.second);
                        }

                        node->AddEdge( x, ret.second );
                }
        }
        
        return result;
}
int main(){

        Graph graph;
        SubGraph* start = new SubGraph;
        start->Push( new Node{GameContext{}} );
        //start->Push( alloc.FindOrAllocate(GameContext{}).second );
        graph.Push(start);

        std::vector< std::unique_ptr<BoardAllocator> > alloc;

        for(int i=0;;++i){
                //if( alloc.size())
                        //alloc.back().reset();
                alloc.emplace_back( std::make_unique<BoardAllocator>() );
                boost::timer::auto_cpu_timer at;
                graph.Back()->Debug();

                auto next = CreateSubGraph(*alloc.back(), *graph.Back() );

                graph.Push(next);
                //if( i == 10 )
                        //break;
                PRINT(i);

                if( alloc.size() >= 3 ){
                        alloc[alloc.size()-2].reset();
                        delete graph.At(alloc.size()-2);
                }
                PRINT( sizeof(Node) );
        }


}
#endif

template<class BoardType>
struct LevelGroup{
        using HashType = decltype(std::declval<BoardType>().Hash());
        LevelGroup():player_{Player_NotAPlayer}{}
        template<class IterType>
        LevelGroup(Player p, IterType first, IterType last)
                :player_{p}
                ,boards_{first,last}
        {
                std::sort(boards_.begin(), boards_.end());
        }
        auto GetPlayer()const{ return player_; }
        BoardType const& Find(HashType const& hash)const{
                auto iter = std::lower_bound( boards_.begin(),
                                              boards_.end(),
                                              hash );
                if( iter->Hash() == hash )
                        return *iter;
                throw std::domain_error("hash doesn't exist");
        }
        auto begin()const{ return boards_.begin(); }
        auto end()const{ return boards_.end(); }
        auto Size()const{ return boards_.size(); }
        auto const& operator[](size_t idx)const{
                return boards_.at(idx);
        }
private:
        friend class boost::serialization::access;

        template <typename Archive>
        void serialize(Archive &ar, const unsigned int version){
                ar & player_;
                ar & boards_;
        }
private:
        Player player_;
        std::vector<BoardType> boards_;
};



/*
        G0 -> G1 -> G2
 */
template<class BoardType>
struct Group
{
        using LevelType = LevelGroup<BoardType>;

        struct MemoryManager{
                MemoryManager(std::string filename,
                              std::unique_ptr<LevelType> ptr):
                        filename_{std::move(filename)},
                        ptr_{std::move(ptr)}
                {
                        //PRINT(filename_);
                }
                void Close(){
                        //std::cerr << "Closing " << filename_ << "\n";
                        ptr_.reset();
                }
                bool Load(){
                        if( !! ptr_.get() )
                                return false;
                        //std::cerr << "Loading " << filename_ << "\n";
                        std::ifstream ifs(filename_);
                        if( ! ifs.is_open()){
                                std::cerr << "Unable to open " << filename_ << "\n";
                                return false;
                        }
                        boost::archive::text_iarchive ia{ifs};
                        ptr_.reset(new LevelType{});
                        ia >> *ptr_;
                        return true;
                }
                bool Save()const{
                        //std::cerr << "Saving " << filename_ << "\n";
                        if( ! ptr_.get() ){
                                std::cerr << "Nothing to save\n";
                                return false;
                        }
                        std::ofstream of(filename_);
                        if( ! of.is_open()){
                                std::cerr << "Unable to open " << filename_ << "\n";
                                return false;
                        }
                        boost::archive::text_oarchive oa{of};
                        oa << *ptr_;
                        return true;
                }
                LevelType const* operator->(){
                        // Autoload
                        Load();
                        return ptr_.get();
                }
                LevelType const& operator*(){
                        // Autoload
                        Load();
                        return *ptr_;
                }
        private:
                std::string filename_;
                mutable std::unique_ptr<LevelType> ptr_;
        };

        auto& DeclSubGroup(std::string const& filename, std::unique_ptr<LevelType> ptr){
                groups_.emplace_back(filename, std::move(ptr));
                return groups_.back();
        }
        auto Size()const{ return groups_.size(); }
        auto& operator[](size_t idx){
                return groups_.at(idx);
        }

private:
        mutable std::vector<MemoryManager> groups_;
};

void Driver2(){
        using BoardType = GenericBoard<7,6>;
        //using BoardType = GenericBoard<4,4>;
        ConnectFourLogic logic;
        BoardInputOutput io;

        std::vector<Player> p;
        std::vector< std::set< BoardType > > b;

        p.emplace_back(Player_Hero);
        b.emplace_back();
        b.back().emplace();

        Group<BoardType> group;

        int level=0;

        auto namebroker = [](int lvl){
                std::stringstream sstr;
                sstr << BoardType::Width() << "x" << BoardType::Height() << "Depth" << lvl << ".bin";
                return sstr.str();
        };

        auto& firstSub = group.DeclSubGroup(namebroker(level++), std::make_unique<LevelGroup<BoardType> >(p.back(), b.back().begin(), b.back().end() ));
        firstSub.Save();

        for(;;){
                boost::timer::auto_cpu_timer at;


                p.emplace_back( NextPlayer(p.back()));
                b.emplace_back();

                auto width = b.back().begin()->Width();
                auto height = b.back().begin()->Height();

                auto t = TileForPlayer(NextPlayer(p.back()));


                for( auto const& board : b[b.size()-2]){

                        auto e = logic.Evaluate(board);
                        if( e != Eval_NotFinished )
                                continue;

                        for( unsigned x=0;x!=width;++x){
                                auto level = Level(board, x);
                                if( level == height )
                                        continue;
                                BoardType next(board);
                                next.Set(x, level, t);

                                b.back().emplace(next);

                                //io.Display(next);
                                //std::stringstream sstr;
                                //sstr 
                                        //<< io.ToString(board) << " => "
                                        //<< io.ToString(next);
                                //std::cout << sstr.str() << "\n";
                        }
                }



                if( b.back().size() == 0 ){
                        p.pop_back();
                        b.pop_back();
                        break;
                }
        
                auto& sub = group.DeclSubGroup(namebroker(level++), std::make_unique<LevelGroup<BoardType> >(p.back(), b.back().begin(), b.back().end() ));
                sub.Save();

                if( group.Size() > 2 ){
                        group[group.Size()-2].Close();
                }
                
                
                PRINT(level);
                PRINT(b.back().size());
                io.Display(*b.back().begin());
                PRINT(EvalToString(logic.Evaluate(*b.back().begin())));
        }

        std::vector<
                std::map<
                        decltype( std::declval<BoardType>().Hash() ),
                        int
                >
        > m(b.size());

        auto marking_lt = [](auto left, auto right){
                static std::map<int, int> order = [](){
                        int precedence[] = {
                                Marked_Win,
                                Marked_Win  | Marked_Draw,
                                Marked_Draw,
                                Marked_Win  | Marked_Draw | Marked_Lose,
                                Marked_Win  | Marked_Lose,
                                Marked_Draw | Marked_Lose,
                                Marked_Lose,
                                Marked_Zero, 
                        };
                        std::map<int, int> ret;
                        int index = 0;
                        for( auto mark : precedence ){
                                ret.emplace(mark, index++);
                        }
                        return std::move(ret);
                }();
                return order[left] < order[right];
        };

        for(size_t idx=group.Size();idx!=0;){
                --idx;

                auto& ctrl = group[idx];
                ctrl.Load();
                auto const& sub = *ctrl;

                auto cp = sub.GetPlayer();
                
                PRINT(sub.Size());
                PRINT(cp);

                for(size_t i=0;i!=sub.Size();++i){
                        auto const& board = sub[i];
                        auto h = board.Hash();
                        auto e = logic.Evaluate(board);

                        //std::cout << "processing " << io.ToString(board) << "\n";
                        //PRINT(EvalToString(e));
                        switch(e){
                        case Eval_Hero:
                                m[idx][h] = Marked_Win;
                                break;
                        case Eval_Villian:
                                m[idx][h] = Marked_Lose;
                                break;
                        case Eval_Draw:
                                m[idx][h] = Marked_Draw;
                                break;
                        case Eval_NotFinished:{
                                int aggregate = 0;
                                for( unsigned x=0;x!=board.Width();++x){
                                        auto level = Level(board, x);
                                        if( level == board.Height() )
                                                continue;
                                        BoardType next(board);
                                        next.Set(x, level, TileForPlayer(cp));

                                        auto nh = next.Hash();

                                        if( m[idx+1][nh] == 0 ){
                                                PRINT(x);
                                                std::cout << "####### FROM #######\n";
                                                io.Display(board);
                                                std::cout << io.ToString(board) << "\n";
                                                std::cout << "####### TO   #######\n";
                                                io.Display(next);
                                                std::cout << io.ToString(next) << "\n";
                                                PRINT_SEQ((std::get<0>(nh))(std::get<1>(nh)));
                                                std::cout << "bad marking----------------\n";
                                                std::cout << "####### END  #######\n";
                                        }
                                        if( cp == Player_Hero ){
                                                auto cand = m[idx+1][nh];
                                                if( marking_lt( cand, aggregate) )
                                                        aggregate = cand;
                                        } else {
                                                aggregate |= m[idx+1][nh];
                                        }
                                }
                                m[idx][h] = aggregate;
                        }
                                break;
                        }
                        //io.Display(board);
                        //PRINT_SEQ((std::get<0>(h))(std::get<1>(h))(m[idx][h]));
                }
                std::cout << "\n\n";
                ctrl.Close();

                //if( idx == group.Size()-4)
                        //break;
        }

        PRINT( m[0].size() );
        PRINT( m[0].begin()->second );

        

}

int main(){
        Driver2();
}
