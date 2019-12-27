#include "gtest/gtest.h"
#include "EarleyAlgorithm.h"
#include "Grammar.h"
#include <sstream>
#include <string>

TEST(CBCGrammar, First) {
    std::string input = "Not terminal:\n"
                        "AS\n"
                        "Starting symbol:\n"
                        "A\n"
                        "Rules:\n"
                        "A:S\n"
                        "S:SS|(S)|\n"
                        "End\n";
    std::istringstream in(input);
    Grammar grammar;
    grammar.Read(in);
    EarleyAlgorithm algo;
    EXPECT_TRUE(algo.Check(grammar, "()"));
    EXPECT_TRUE(algo.Check(grammar, "(()())(())"));
    EXPECT_TRUE(algo.Check(grammar, "((())(()))"));
    EXPECT_FALSE(algo.Check(grammar, "("));
    EXPECT_FALSE(algo.Check(grammar, "(()))("));
    EXPECT_FALSE(algo.Check(grammar, "()()()()x()"));
}

TEST(CBCGrammar, Second) {
    std::string input = "Not terminal:\n"
                        "AS\n"
                        "Starting symbol:\n"
                        "A\n"
                        "Rules:\n"
                        "A:S\n"
                        "S:(S)S|[S]S|<S>S|\n"
                        "End\n";
    Grammar grammar;
    std::istringstream in(input);
    grammar.Read(in);
    EarleyAlgorithm algo;
    EXPECT_TRUE(algo.Check(grammar, "[]()<>"));
    EXPECT_TRUE(algo.Check(grammar, "[(([])<>)]<>"));
    EXPECT_TRUE(algo.Check(grammar, "[]<([<>])>"));
    EXPECT_FALSE(algo.Check(grammar, "[(])"));
    EXPECT_FALSE(algo.Check(grammar, "<[]>([)"));
    EXPECT_FALSE(algo.Check(grammar, "()()()()()()()[(])"));
}

TEST(ArithmeticExpressionGrammar, First) {
    std::string input = "Not terminal:\n"
                        "ASXY\n"
                        "Starting symbol:\n"
                        "A\n"
                        "Rules:\n"
                        "A:S\n"
                        "S:S+X|S-X|X\n"
                        "X:X*Y|X/Y|Y\n"
                        "Y:(S)|1|2|3|4|5|6|7|8|9\n"
                        "End";
    std::istringstream in(input);
    Grammar grammar;
    grammar.Read(in);
    EarleyAlgorithm algo;
    EXPECT_TRUE(algo.Check(grammar, "(1+2)/3*4+5*(9-6)"));
    EXPECT_TRUE(algo.Check(grammar, "1-(2+3*4)/5-6/9/8/7"));
    EXPECT_TRUE(algo.Check(grammar, "((1+2))*(3/((2-1)))"));
    EXPECT_FALSE(algo.Check(grammar, "(1+2)-3-4-4--5--5-5"));
    EXPECT_FALSE(algo.Check(grammar, "(1*2*3)/4/4/4/4/4/4//4/4/"));
    EXPECT_FALSE(algo.Check(grammar, "(1+2)+3+4)"));
}

TEST(Sample, First) {
    std::string input = "Not terminal:\n"
                        "ACDS\n"
                        "Starting symbol:\n"
                        "A\n"
                        "Rules:\n"
                        "A:S\n"
                        "S:C|SC\n"
                        "C:cD\n"
                        "D:aDb|\n"
                        "End";
    std::istringstream in(input);
    Grammar grammar;
    grammar.Read(in);
    EarleyAlgorithm algo;
    EXPECT_TRUE(algo.Check(grammar, "caabbcaaabbbcab"));
    EXPECT_TRUE(algo.Check(grammar, "cabcc"));
    EXPECT_TRUE(algo.Check(grammar, "cccaabb"));
    EXPECT_FALSE(algo.Check(grammar, "caabbcabcaabbcabab"));
    EXPECT_FALSE(algo.Check(grammar, "ccccccccccccaab"));
    EXPECT_FALSE(algo.Check(grammar, "cabcabcabcabcabb"));
}

TEST(Sample, Second) {
    std::string input = "Not terminal:\n"
                        "ACDS\n"
                        "Starting symbol:\n"
                        "A\n"
                        "Rules:\n"
                        "A:S\n"
                        "S:Sa|C|SSb\n"
                        "C:Dd\n"
                        "D:cD|\n"
                        "End";
    std::istringstream in(input);
    Grammar grammar;
    grammar.Read(in);
    EarleyAlgorithm algo;
    EXPECT_TRUE(algo.Check(grammar, "ccdacdb"));
    EXPECT_TRUE(algo.Check(grammar, "cdaaaccdbcdab"));
    EXPECT_TRUE(algo.Check(grammar, "cccda"));
    EXPECT_FALSE(algo.Check(grammar, "ccdacd"));
    EXPECT_FALSE(algo.Check(grammar, "ccccdaaccccdad"));
    EXPECT_FALSE(algo.Check(grammar, "ccccdad"));
}