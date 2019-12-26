#include "Grammar.h"
#include <stdexcept>
#include <algorithm>
#include <sstream>
#include <utility>

bool Grammar::IsNotTerminal(char symbol) const {
    return std::find(not_termainal_symbols_.begin(), not_termainal_symbols_.end(), symbol) !=
           not_termainal_symbols_.end();
}

char Grammar::GetStartingSymbol() const {
    return starting_symbol_;
}

const std::vector<uint32_t> &Grammar::GetRulesFrom(char symbol) const {
    if (!IsNotTerminal(symbol)) {
        throw std::logic_error("Couldn't find rules from terminal symbol");
    }
    for (size_t i = 0; i < not_termainal_symbols_.size(); ++i) {
        if (not_termainal_symbols_[i] == symbol) {
            return graph_[i];
        }
    }
    throw std::logic_error("Strange error: structure state changed in const method");
}

void Grammar::Read(std::istream &in) {
    ReadNotTerminalSymbols(in);
    ReadRules(in);
}

void Grammar::ReadNotTerminalSymbols(std::istream &in) {
    starting_symbol_ = {};
    not_termainal_symbols_ = {};
    std::string current;
    std::getline(in, current);
    if (current != "Not terminal:") {
        throw std::logic_error("Bad input pattern in not terminal input");
    }
    std::getline(in, current);
    for (auto symbol:current) {
        if (IsNotTerminal(symbol)) {
            throw std::logic_error("Duplicated not terminals");
        }
        if (symbol == ':' || symbol == '|') {
            throw std::logic_error("Used special symbols as not terminal");
        }
        not_termainal_symbols_.emplace_back(symbol);
    }
    std::getline(in, current);
    if (current != "Starting symbol:") {
        throw std::logic_error("Bad input pattern in start symbol input");
    }
    std::getline(in, current);
    if (current.size() != 1) {
        throw std::logic_error("More than one starting symbol");
    }
    starting_symbol_ = current[0];
}

void Grammar::ReadRules(std::istream &in) {
    rules_ = {};
    graph_ = {};
    graph_.resize(not_termainal_symbols_.size());
    std::string current;
    std::getline(in, current);
    if (current != "Rules:") {
        throw std::logic_error("Bad input pattern in rules input");
    }
    do {
        std::getline(in, current);
        if (current == "End") {
            break;
        }
        std::istringstream rule(current);
        std::string cur_token;
        std::getline(rule, cur_token, ':');
        if (cur_token.size() != 1 || !IsNotTerminal(cur_token[0])) {
            throw std::logic_error("Error in reading left part");
        }
        char symbol = cur_token[0];
        uint32_t idx = std::find(not_termainal_symbols_.begin(), not_termainal_symbols_.end(), symbol) -
                       not_termainal_symbols_.begin();
        do {
            std::getline(rule, cur_token, '|');
            graph_[idx].emplace_back(rules_.size());
            rules_.emplace_back(symbol, cur_token);
        } while (!rule.eof());
    } while (!in.eof());
}

Grammar::Rule Grammar::GetRule(uint32_t idx) const {
    return rules_[idx];
}

uint32_t Grammar::GetTerminalIndex(char symbol) const {
    return std::find(not_termainal_symbols_.begin(), not_termainal_symbols_.end(), symbol) - not_termainal_symbols_.begin();
}

size_t Grammar::RulesNumber() const {
    return rules_.size();
}

char Grammar::Rule::LeftPart() const {
    return left_part_;
}

const std::string &Grammar::Rule::RightPart() const {
    return right_part_;
}

Grammar::Rule::Rule(char left, std::string right) : left_part_(left), right_part_(std::move(right)) {}

bool Grammar::Rule::operator<(const Grammar::Rule &rhs) const {
    if (left_part_ == rhs.left_part_) {
        return right_part_ < rhs.right_part_;
    }
    return left_part_ < rhs.left_part_;
}

bool Grammar::Rule::operator==(const Grammar::Rule &rhs) const {
    return !(*this < rhs || rhs < *this);
}