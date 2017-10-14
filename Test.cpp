#include "ConnectFour.hpp"
#include <gtest/gtest.h>

TEST(EvalTest,Eval){
        BoardInputOutput io;
        ConnectFourLogic logic;

        std::vector<std::tuple<std::string, Eval> > ticker = {
                {
                        "|     |"
                        "|     |"
                        "|     |"
                        "|     |", Eval_NotFinished},
                {
                        "| XXXX|"
                        "|     |"
                        "|     |"
                        "|     |", Eval_Hero},
                {
                        "|XXXX |"
                        "|     |"
                        "|     |"
                        "|     |", Eval_Hero},
                {
                        "|     |"
                        "|     |"
                        "|     |"
                        "|OXXXX|", Eval_Hero},
                {
                        "|     |"
                        "|     |"
                        "|     |"
                        "|OXOXX|", Eval_NotFinished},
                {
                        "|  X  |"
                        "|  X  |"
                        "|  X  |"
                        "|  X  |", Eval_Hero},
                {
                        "|X    |"
                        "| X   |"
                        "|  X  |"
                        "|   X |", Eval_Hero},
                {
                        "|    X|"
                        "|   X |"
                        "|  X  |"
                        "| X   |", Eval_Hero},
                {
                        "|   X |"
                        "|  X  |"
                        "| X   |"
                        "|X    |", Eval_Hero},
                {
                        "|   X |"
                        "|     |"
                        "| X   |"
                        "|X    |", Eval_NotFinished},
        };
        for( auto const& item : ticker){
                auto board = io.ParseBoard(5,4, std::get<0>(item)).get();
                auto e = logic.Evaluate(board);
                #if 0
                io.Display(board);
                PRINT(EvalToString(e));
                #endif
                EXPECT_EQ(std::get<1>(item), e) << std::get<0>(item);
        }


}

int main(int argc, char **argv) {
          ::testing::InitGoogleTest(&argc, argv);
            return RUN_ALL_TESTS();
}

