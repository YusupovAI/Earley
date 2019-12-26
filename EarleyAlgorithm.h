#ifndef EARLEY_EARLEYALGORITHM_H
#define EARLEY_EARLEYALGORITHM_H

#include "Grammar.h"

class EarleyAlgorithm {
public:
    bool Check(Grammar grammar, const std::string& input);
private:
    class Situation {
        friend class EarleyAlgorithm;
        uint32_t rule_idx_{}, position_in_rule_{}, begin_position_{};
    public:
        Situation& operator=(const Situation&) = default;
    };

    void Predict(uint32_t idx, std::vector<std::vector<std::vector<char> > >& used);
    void PredictSituation(uint32_t idx, Situation situation, std::vector<std::vector<std::vector<char> > >& used);
    void Complete(uint32_t idx, std::vector<std::vector<std::vector<char> > >& used);
    void CompleteSituation(uint32_t idx, Situation situation, std::vector<std::vector<std::vector<char> > >& used);
    void Scan(uint32_t idx);
    void ScanSituation(uint32_t idx, Situation situation, char symbol);
    std::vector<std::vector<std::vector<Situation> > > table_;
    std::string input_{};
    Grammar grammar_;
    bool changed_{};
};


#endif //EARLEY_EARLEYALGORITHM_H
