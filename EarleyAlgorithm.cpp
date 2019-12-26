//
// Created by iusupov on 25.12.2019.
//

#include "EarleyAlgorithm.h"

void EarleyAlgorithm::Scan(uint32_t idx) {
    if (idx == input_.size()) {
        return;
    }
    for (size_t i = 0; i < table_[idx].size(); ++i) {
        for (size_t j = 0; j < table_[idx][i].size(); ++j) {
            auto situation = table_[idx][i][j];
            ScanSituation(idx, situation, input_[idx]);
        }
    }
}

void EarleyAlgorithm::ScanSituation(uint32_t idx, EarleyAlgorithm::Situation situation, char symbol) {
    auto& right = grammar_.GetRule(situation.rule_idx_).RightPart();
    if (right.size() > situation.position_in_rule_ && right[situation.position_in_rule_] == symbol) {
        ++situation.position_in_rule_;
        if (situation.position_in_rule_ == right.size()) {
            table_[idx + 1][128].emplace_back(situation);
        } else {
            table_[idx + 1][right[situation.position_in_rule_] + 128].emplace_back(situation);
        }
    }
}

void EarleyAlgorithm::Predict(uint32_t idx, std::vector<std::vector<std::vector<char> > >& used) {
    for (size_t i = 0; i < table_[idx].size(); ++i) {
        for (size_t j = 0; j < table_[idx][i].size(); ++j) {
            auto situation = table_[idx][i][j];
            PredictSituation(idx, situation, used);
        }
    }
}

void EarleyAlgorithm::PredictSituation(uint32_t idx, EarleyAlgorithm::Situation situation, std::vector<std::vector<std::vector<char> > > &used) {
    auto right = grammar_.GetRule(situation.rule_idx_).RightPart();
    if (right.size() <= situation.position_in_rule_) {
        return;
    }
    char symbol = right[situation.position_in_rule_];
    if (!grammar_.IsNotTerminal(symbol)) {
        return;
    }
    for (auto rule_idx:grammar_.GetRulesFrom(symbol)) {
        auto left = grammar_.GetRule(rule_idx).LeftPart();
        if (!used[rule_idx][0][idx] && left == symbol) {
            auto& right = grammar_.GetRule(rule_idx).RightPart();
            used[rule_idx][0][idx] = changed_ = 1;
            if (right.size() == 0) {
                Situation sit;
                sit.begin_position_ = idx;
                sit.position_in_rule_ = 0;
                sit.rule_idx_ = rule_idx;
                table_[idx][128].emplace_back(sit);
            } else {
                Situation sit;
                sit.position_in_rule_ = 0;
                sit.rule_idx_ = rule_idx;
                sit.begin_position_ = idx;
                table_[idx][right[0] + 128].emplace_back(sit);
            }
        }
    }
}

void EarleyAlgorithm::Complete(uint32_t idx, std::vector<std::vector<std::vector<char> > >& used) {
    for (size_t i = 0; i < table_[idx].size(); ++i) {
        for (size_t j = 0; j < table_[idx][i].size(); ++j) {
            CompleteSituation(idx, table_[idx][i][j], used);
        }
    }
}

void EarleyAlgorithm::CompleteSituation(uint32_t idx, EarleyAlgorithm::Situation situation,
                                        std::vector<std::vector<std::vector<char> > > &used) {
    if (situation.position_in_rule_ != grammar_.GetRule(situation.rule_idx_).RightPart().size()) {
        return;
    }
    char left = grammar_.GetRule(situation.rule_idx_).LeftPart();
    for (size_t i = 0; i < table_[situation.begin_position_][left + 128].size(); ++i) {
        Situation sit = table_[situation.begin_position_][left + 128][i];
        ++sit.position_in_rule_;
        if (!used[sit.rule_idx_][sit.position_in_rule_][sit.begin_position_]) {
            used[sit.rule_idx_][sit.position_in_rule_][sit.begin_position_] = changed_ = 1;
            auto& right = grammar_.GetRule(sit.rule_idx_).RightPart();
            if (right.size() == sit.position_in_rule_) {
                table_[idx][128].emplace_back(sit);
            } else {
                table_[idx][128 + right[sit.position_in_rule_]].emplace_back(sit);
            }
        }
    }
}

bool EarleyAlgorithm::Check(Grammar grammar, const std::string &input) {
    grammar_ = std::move(grammar);
    input_ = std::move(input);
    table_.resize(input_.size() + 1);
    for (size_t i = 0; i <= input_.size(); ++i) {
        table_[i] = std::vector<std::vector<Situation> >(256);
    }
    std::vector<std::vector<std::vector<char> > > used(grammar_.RulesNumber(), std::vector<std::vector<char> >(100, std::vector<char> (input_.size() + 1)));
    Situation sit;
    sit.begin_position_ = 0;
    sit.position_in_rule_ = 0;
    for (size_t i = 0; i < grammar_.RulesNumber(); ++i) {
        if (grammar_.GetRule(i).LeftPart() == grammar_.GetStartingSymbol()) {
            sit.rule_idx_ = i;
        }
    }
    uint32_t rule_idx = grammar_.GetRulesFrom(grammar_.GetStartingSymbol())[0];
    table_[0][grammar_.GetRule(rule_idx).RightPart()[0] + 128].emplace_back(sit);
    used[sit.rule_idx_][0][sit.begin_position_] = 1;
    do {
        changed_ = false;
        Predict(0, used);
        Complete(0, used);
    } while (changed_);
    for (size_t i = 1; i <= input_.size(); ++i) {
        used.assign(grammar_.RulesNumber(), std::vector<std::vector<char> >(100, std::vector<char>(input_.size() + 1)));
        Scan(i - 1);

        do {
            changed_ = false;
            Predict(i, used);
            Complete(i, used);
        } while (changed_);

    }
    for (auto sit : table_[input_.size()][128]) {
        uint32_t idx = grammar_.GetRulesFrom(grammar_.GetStartingSymbol())[0];
        if (sit.position_in_rule_ == grammar_.GetRule(idx).RightPart().size() && sit.begin_position_ == 0) {
            return true;
        }
    }
    return false;
}
