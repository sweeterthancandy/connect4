#include "ConnectFour.hpp"
#include <gtest/gtest.h>

TEST(EvalTest,Eval){
        BoardInputOutput io;
        ConnectFourLogic logic;

        std::vector<std::tuple<std::string, Eval> > ticker = {
                {
                        "|       |"
                        "|       |"
                        "|       |"
                        "|       |"
                        "|       |"
                        "|       |", Eval_NotFinished},
                {
                        "| XXXX  |"
                        "|       |"
                        "|       |"
                        "|       |"
                        "|       |"
                        "|       |", Eval_Hero},
                {
                        "|XXXX   |"
                        "|       |"
                        "|       |"
                        "|       |"
                        "|       |"
                        "|       |", Eval_Hero},
                {
                        "|       |"
                        "|       |"
                        "|       |"
                        "|       |"
                        "|       |"
                        "|OXXXX  |", Eval_Hero},
                {
                        "|       |"
                        "|       |"
                        "|       |"
                        "|       |"
                        "|       |"
                        "|OXOXX  |", Eval_NotFinished},
                {
                        "|       |"
                        "|       |"
                        "|  X    |"
                        "|  X    |"
                        "|  X    |"
                        "|  X    |", Eval_Hero},
                {
                        "|       |"
                        "|       |"
                        "|X      |"
                        "| X     |"
                        "|  X    |"
                        "|   X   |", Eval_Hero},
                {
                        "|       |"
                        "|       |"
                        "|    X  |"
                        "|   X   |"
                        "|  X    |"
                        "| X     |", Eval_Hero},
                {
                        "|       |"
                        "|       |"
                        "|   X   |"
                        "|  X    |"
                        "| X     |"
                        "|X      |", Eval_Hero},
                {
                        "|       |"
                        "|       |"
                        "|   X   |"
                        "|       |"
                        "| X     |"
                        "|X      |", Eval_NotFinished},
                {
                        "|       |"
                        "|       |"
                        "|       |"
                        "|       |"
                        "|OOOO   |"
                        "|XXOOXXX|", Eval_Villian},
                {
                        "|       |"
                        "|       |"
                        "|       |"
                        "|       |"
                        "| OOOO  |"
                        "|XXOOXXX|", Eval_Villian},
                {
                        "|       |"
                        "|       |"
                        "|       |"
                        "|       |"
                        "|  OOOO |"
                        "|XXOOXXX|", Eval_Villian},
                {
                        "|       |"
                        "|       |"
                        "|       |"
                        "|       |"
                        "|   OOOO|"
                        "|XXOOXXX|", Eval_Villian},
                {
                        "|OOOO   |"
                        "|       |"
                        "|       |"
                        "|       |"
                        "|       |"
                        "|XXOOXXX|", Eval_Villian},
                {
                        "| OOOO  |"
                        "|       |"
                        "|       |"
                        "|       |"
                        "|       |"
                        "|XXOOXXX|", Eval_Villian},
                {
                        "|       |"
                        "|  OOOO |"
                        "|       |"
                        "|       |"
                        "|       |"
                        "|XXOOXXX|", Eval_Villian},
                {
                        "|   OOOO|"
                        "|       |"
                        "|       |"
                        "|       |"
                        "|       |"
                        "|XXOOXXX|", Eval_Villian},
        };
        for( auto const& item : ticker){
                auto board = io.ParseBoard<GenericBoard<7,6> >(std::get<0>(item)).get();
                auto e = logic.Evaluate(board);
                #if 0
                io.Display(board);
                PRINT(EvalToString(e));
                #endif
                EXPECT_EQ(std::get<1>(item), e) << std::get<0>(item);
        }
}

#if 0
TEST(Context, FinishConditions){
        BoardInputOutput io;
        ConnectFourLogic logic;
        auto board = io.ParseBoard(5,4,"     "
                                       "x0x0x"
                                       "x0x0x"
                                       "x0x0x").get();
        GameContext proto{board};

        auto _0 = proto;
        _0.Place(logic, 0);
        EXPECT_EQ( Ctrl_Finish, _0.Ctx().GetCtrl());
        
        auto _1 = proto;
        _1.Place(logic, 1);
        EXPECT_EQ( Ctrl_PlayerMove, _1.Ctx().GetCtrl());

}
#endif


int main(int argc, char **argv) {
          ::testing::InitGoogleTest(&argc, argv);
            return RUN_ALL_TESTS();
}

