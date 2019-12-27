//
// Created by iusupov on 25.12.2019.
//

#include "EarleyAlgorithm.h"

void EarleyAlgorithm::Scan(uint32_t idx) {
    if (idx == input_.size()) {
        return;
    }
    for (size_t i = 0; i < table_[idx].size(); ++i) {
        auto situation = table_[idx][i];
        ScanSituation(idx, situation, input_[idx]);
    }
}

void EarleyAlgorithm::ScanSituation(uint32_t idx, EarleyAlgorithm::Situation situation, char symbol) {
    auto& right = grammar_.GetRule(situation.rule_idx_).RightPart();
    if (right.size() > situation.position_in_rule_ && right[situation.position_in_rule_] == symbol) {
        ++situation.position_in_rule_;
        table_[idx + 1].emplace_back(situation);
        complete_position_[idx + 1].emplace_back(0);
    }
}

void EarleyAlgorithm::Predict(uint32_t idx, std::vector<std::vector<std::vector<char> > >& used) {
    for (;predict_begin_index_ < table_[idx].size(); ++predict_begin_index_) {
        auto situation = table_[idx][predict_begin_index_];
        PredictSituation(idx, situation, used);
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
            used[rule_idx][0][idx] = changed_ = true;
            Situation sit;
            sit.position_in_rule_ = 0;
            sit.rule_idx_ = rule_idx;
            sit.begin_position_ = idx;
            table_[idx].emplace_back(sit);
            complete_position_[idx].emplace_back(0);
        }
    }
}

void EarleyAlgorithm::Complete(uint32_t idx, std::vector<std::vector<std::vector<char> > >& used) {
    for (size_t i = 0; i < table_[idx].size(); ++i) {
        CompleteSituation(idx, i, used);
    }
}

void EarleyAlgorithm::CompleteSituation(uint32_t idx, uint32_t sit_idx,
                                        std::vector<std::vector<std::vector<char> > > &used) {
    Situation situation = table_[idx][sit_idx];
    if (situation.position_in_rule_ != grammar_.GetRule(situation.rule_idx_).RightPart().size()) {
        return;
    }
    char left = grammar_.GetRule(situation.rule_idx_).LeftPart();
    for (; complete_position_[idx][sit_idx] < table_[situation.begin_position_].size(); ++complete_position_[idx][sit_idx]) {
        Situation sit = table_[situation.begin_position_][complete_position_[idx][sit_idx]];
        if (sit.position_in_rule_ == grammar_.GetRule(sit.rule_idx_).RightPart().size()) {
            continue;
        }
        if (grammar_.GetRule(sit.rule_idx_).RightPart()[sit.position_in_rule_] != left) {
            continue;
        }
        ++sit.position_in_rule_;
        if (!used[sit.rule_idx_][sit.position_in_rule_][sit.begin_position_]) {
            used[sit.rule_idx_][sit.position_in_rule_][sit.begin_position_] = changed_ = true;
            table_[idx].emplace_back(sit);
            complete_position_[idx].emplace_back(0);
        }
    }
}

bool EarleyAlgorithm::Check(Grammar grammar, const std::string &input) {
    grammar_ = std::move(grammar);
    input_ = input;
    table_.clear();
    table_.resize(input_.size() + 1);
    complete_position_.clear();
    complete_position_.resize(input_.size() + 1);
    std::vector<std::vector<std::vector<char> > > used(grammar_.RulesNumber(), std::vector<std::vector<char> >(grammar_.MaxRuleSize() + 1, std::vector<char> (input_.size() + 1)));
    Situation sit;
    sit.begin_position_ = 0;
    sit.position_in_rule_ = 0;
    for (size_t i = 0; i < grammar_.RulesNumber(); ++i) {
        if (grammar_.GetRule(i).LeftPart() == grammar_.GetStartingSymbol()) {
            sit.rule_idx_ = i;
        }
    }
    uint32_t rule_idx = grammar_.GetRulesFrom(grammar_.GetStartingSymbol())[0];
    table_[0].emplace_back(sit);
    used[sit.rule_idx_][0][sit.begin_position_] = true;
    predict_begin_index_ = 0;
    do {
        changed_ = false;
        Predict(0, used);
        Complete(0, used);
    } while (changed_);
    for (size_t i = 1; i <= input_.size(); ++i) {
        used.assign(grammar_.RulesNumber(), std::vector<std::vector<char> >(100, std::vector<char>(input_.size() + 1)));
        Scan(i - 1);
        predict_begin_index_ = 0;
        do {
            changed_ = false;
            Predict(i, used);
            Complete(i, used);
        } while (changed_);

    }
    for (auto sit : table_[input_.size()]) {
        uint32_t idx = grammar_.GetRulesFrom(grammar_.GetStartingSymbol())[0];
        if (sit.rule_idx_ == idx && sit.position_in_rule_ == grammar_.GetRule(idx).RightPart().size()) {
            return true;
        }
    }
    return false;
}
