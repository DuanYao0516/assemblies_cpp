// rules.h
#ifndef RULES_H
#define RULES_H

#include "constants.h"

#include <iostream>
#include <sstream>

/// @brief 存储词表单词对应的词性以及索引
/// @brief 为了更灵活地存储单词表中单词对应的规则，我们需要这个结构体。
typedef struct PosAndIndex
{
    // 词性（part of speech）
    int pos;
    // 词索引
    int index;
} PosAndIndex;

/// @brief 脑区规则
typedef struct AreaRule
{
    std::string name;
    // 是否抑制？真则抑制，否则解除抑制
    bool inhibit;
    // 抑制性种群识别号
    int population;
} AreaRule;

/// @brief 纤维规则
typedef struct FiberRule
{
    std::string first_name;
    std::string second_name;
    // 是否抑制？真则抑制，否则解除抑制
    bool inhibit;
    // 抑制性种群识别号
    int population;
} FiberRule;

/// @brief 脑区和纤维规则。
typedef struct AreaAndFiberRules
{
    std::vector<AreaRule> area_rules;
    std::vector<FiberRule> fiber_rules;
} AreaAndFiberRules;

/// @brief 通用规则。
typedef struct GenericRules
{
    // 词索引
    int index;
    // 预命令
    AreaAndFiberRules pre_rules;
    // 后命令
    AreaAndFiberRules post_rules;
} GenericRules;

// 声明规则函数
GenericRules genericNoun(int index);
GenericRules genericTransVerb(int index);
GenericRules genericIntransVerb(int index);
GenericRules genericCopula(int index);
GenericRules genericAdjective(int index, int order);
GenericRules genericAdverb(int index);
GenericRules genericDeterminant(int index);
GenericRules genericPreposition(int index);

GenericRules getRules(PosAndIndex arg, int order);

const std::map<std::string, PosAndIndex> kLEXEME_DICT = {
    {"the", {DET, 0}},
    {"a", {DET, 1}},
    {"dogs", {NOUN, 2}},
    {"cats", {NOUN, 3}},
    {"mice", {NOUN, 4}},
    {"people", {NOUN, 5}},
    {"chase", {TRANSVERB, 6}},
    {"love", {TRANSVERB, 7}},
    {"bite", {TRANSVERB, 8}},
    {"of", {PREP, 9}},
    {"bad", {ADJ, 10}},
    {"big", {ADJ, 11}},
    {"run", {INTRANSVERB, 12}},
    {"fly", {INTRANSVERB, 13}},
    {"quickly", {ADVERB, 14}},
    {"in", {PREP, 15}},
    {"are", {COPULA, 16}},
    {"man", {NOUN, 17}},
    {"woman", {NOUN, 18}},
    {"saw", {TRANSVERB, 19}},
    {"fat", {ADJ, 20}},
    {"go", {INTRANSVERB, 21}},
    {"to", {PREP, 22}},
    {"school", {NOUN, 23}},
    {"problems", {NOUN, 24}},
    {"scary", {ADJ, 25}},
};

#endif // RULES_H