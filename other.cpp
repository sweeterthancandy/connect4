namespace Detail{

        struct Tag_None{};
        struct Tag_Terminal{
                using Without_Terminal = Tag_None{};
        };
        struct Tag_NonTerminal{
                using Without_NonTerminal = Tag_None{};
        };
        struct Tag_Any{} : Tag_Terminal , Tag_NonTerminal{
                using Without_Terminal    = Tag_NonTerminal{};
                using Without_NonTerminal = Tag_Terminal{};
        };

        template<class Tag>
        struct StateVisitorBuilder{
                using fun_t = std::function<void(State*)>;

                template<class Tag = Tag>
                std::enable_if_t< std::is_base_of_v<Tag_Terminal, Tag> > CaseTerminal(fun_t f){
                        return StateVisitorBuilder< typename Tag::Without_Terminal>{ std::move(f), std::move(non_term_});
                }
                template<class Tag = Tag>
                std::enable_if_t< std::is_base_of_v<Tag_NonTerminal, Tag> > CaseNonTerminal(fun_t f){
                        return StateVisitorBuilder< typename Tag::Without_NonTerminal>{ std::move(term_), std::move(f)};
                }
                void Accept(State* state){
                        switch(state->Type()){
                        case  State::Type_NonTerminal:
                                Apply( term_, state);
                                break;
                        case  State::Type_Terminal:
                                Apply( non_term_, state);
                                break;
                        }
                }
        private:
                fun_t term_;
                fun_t non_term_;
        };
} // Detail
