#ifndef LR_1_GRAMMAR_H
#define LR_1_GRAMMAR_H

#include <string>
#include <utility>
#include <vector>
#include <istream>

class Grammar {
public:
    Grammar() = default;

    struct Rule {
    private:
        char left_part_{};
        std::string right_part_{};

    public:
        [[nodiscard]] bool operator<(const Rule& rhs) const;

        [[nodiscard]] bool operator==(const Rule& rhs) const;

        friend class Grammar;

        Rule() = default;

        Rule(char left, std::string right);

        [[nodiscard]] char LeftPart() const;

        [[nodiscard]] const std::string &RightPart() const;
    };

    Grammar(const Grammar &) = default;

    Grammar(Grammar &&) = default;

    [[nodiscard]] bool IsNotTerminal(char symbol) const;

    [[nodiscard]] const std::vector<uint32_t> &GetRulesFrom(char symbol) const;

    [[nodiscard]] char GetStartingSymbol() const;

    [[nodiscard]] Rule GetRule(uint32_t idx) const;

    [[nodiscard]] uint32_t GetTerminalIndex(char symbol) const;

    [[nodiscard]] size_t RulesNumber() const;

    Grammar& operator=(Grammar&&) = default;

    void Read(std::istream &in);

private:
    char starting_symbol_{};
    std::vector<char> not_termainal_symbols_{};
    std::vector<Rule> rules_{};
    std::vector<std::vector<uint32_t> > graph_{};

    void ReadNotTerminalSymbols(std::istream &in);

    void ReadRules(std::istream &in);
};

#endif
