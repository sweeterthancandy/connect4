#include "ConnectFour.hpp"
#include "Graph.hpp"

#include <set>
#include <boost/timer/timer.hpp>

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

struct NodeAllocator{
        using hash_t = decltype(std::declval<GameContext>().Hash());
        // (was_allocated, ptr)
        std::pair<bool, Node*> FindOrAllocate(GameContext const& ctx){
                auto h = ctx.Hash();
                auto iter = nodes_.find(h);
                if( iter == nodes_.end()){
                        //auto tmp = new Node{ctx};
                        auto tmp = pool_.construct(ctx);
                        nodes_.emplace(h, tmp);
                        return std::make_pair(true, tmp);
                }
                return std::make_pair(false, iter->second);
        }
private:
        boost::object_pool<Node> pool_;
        std::map<hash_t, Node*> nodes_;
};

SubGraph* CreateSubGraph(NodeAllocator& alloc, SubGraph const& entry){
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

        std::vector< std::unique_ptr<NodeAllocator> > alloc;

        for(int i=0;;++i){
                //if( alloc.size())
                        //alloc.back().reset();
                alloc.emplace_back( std::make_unique<NodeAllocator>() );
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
