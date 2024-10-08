// rules.cc
#include <map>
#include "rules.h"
#include "constants.h"

GenericRules genericNoun(int index)
{
    return {
        index,
        // AreaAndFiberRules pre_rules
        {
            // std::vector<AreaRule> area_rules
            {

            },
            // std::vector<FiberRule> fiber_rules
            {
                {kLEX, kSUBJ, DISINHIBIT, 0},
                {kLEX, kOBJ, DISINHIBIT, 0},
                {kLEX, kPREP_P, DISINHIBIT, 0},
                {kDET, kSUBJ, DISINHIBIT, 0},
                {kDET, kOBJ, DISINHIBIT, 0},
                {kDET, kPREP_P, DISINHIBIT, 0},
                {kADJ, kSUBJ, DISINHIBIT, 0},
                {kADJ, kOBJ, DISINHIBIT, 0},
                {kADJ, kPREP_P, DISINHIBIT, 0},
                {kADJ1, kSUBJ, DISINHIBIT, 0},
                {kADJ1, kOBJ, DISINHIBIT, 0},
                {kADJ1, kPREP_P, DISINHIBIT, 0},
                {kADJ2, kSUBJ, DISINHIBIT, 0},
                {kADJ2, kOBJ, DISINHIBIT, 0},
                {kADJ2, kPREP_P, DISINHIBIT, 0},
                {kVERB, kOBJ, DISINHIBIT, 0},
                {kPREP_P, kPREP, DISINHIBIT, 0},
                {kPREP_P, kSUBJ, DISINHIBIT, 0},
                {kPREP_P, kOBJ, DISINHIBIT, 0}}},

        // AreaAndFiberRules post_rules
        {
            // std::vector<AreaRule> area_rules
            {
                {kDET, INHIBIT, 0},
                {kADJ, INHIBIT, 0},
                {kADJ1, INHIBIT, 0},
                {kADJ2, INHIBIT, 0},
                {kPREP, INHIBIT, 0},
                {kPREP_P, INHIBIT, 0}},
            // std::vector<FiberRule> fiber_rules
            {
                {kLEX, kSUBJ, INHIBIT, 0},
                {kLEX, kOBJ, INHIBIT, 0},
                {kLEX, kPREP_P, INHIBIT, 0},
                {kADJ, kSUBJ, INHIBIT, 0},
                {kADJ, kOBJ, INHIBIT, 0},
                {kADJ, kPREP_P, INHIBIT, 0},
                {kADJ1, kSUBJ, INHIBIT, 0},
                {kADJ1, kOBJ, INHIBIT, 0},
                {kADJ1, kPREP_P, INHIBIT, 0},
                {kADJ2, kSUBJ, INHIBIT, 0},
                {kADJ2, kOBJ, INHIBIT, 0},
                {kADJ2, kPREP_P, INHIBIT, 0},
                {kDET, kSUBJ, INHIBIT, 0},
                {kDET, kOBJ, INHIBIT, 0},
                {kDET, kPREP_P, INHIBIT, 0},
                {kVERB, kOBJ, INHIBIT, 0},
                {kPREP_P, kPREP, INHIBIT, 0},
                {kPREP_P, kVERB, INHIBIT, 0},
                {kLEX, kSUBJ, DISINHIBIT, 1},
                {kLEX, kOBJ, DISINHIBIT, 1},
                {kDET, kSUBJ, DISINHIBIT, 1},
                {kDET, kOBJ, DISINHIBIT, 1},
                {kADJ, kSUBJ, DISINHIBIT, 1},
                {kADJ, kOBJ, DISINHIBIT, 1},
                {kADJ1, kSUBJ, DISINHIBIT, 1},
                {kADJ1, kOBJ, DISINHIBIT, 1},
                {kADJ2, kSUBJ, DISINHIBIT, 1},
                {kADJ2, kOBJ, DISINHIBIT, 1},
                {kPREP_P, kSUBJ, INHIBIT, 0},
                {kPREP_P, kOBJ, INHIBIT, 0},
                {kVERB, kADJ, INHIBIT, 0},
                {kVERB, kADJ1, INHIBIT, 0},
                {kVERB, kADJ2, INHIBIT, 0},
            }}};
}

GenericRules genericTransVerb(int index)
{
    return {
        index,
        // AreaAndFiberRules pre_rules
        {
            // std::vector<AreaRule> area_rules
            {
                {kADVERB, DISINHIBIT, 1}},
            // std::vector<FiberRule> fiber_rules
            {
                {kLEX, kVERB, DISINHIBIT, 0},
                {kVERB, kSUBJ, DISINHIBIT, 0},
                {kVERB, kADVERB, DISINHIBIT, 0}}},
        // AreaAndFiberRules post_rules
        {
            // std::vector<AreaRule> area_rules
            {
                {kOBJ, DISINHIBIT, 0},
                {kSUBJ, INHIBIT, 0},
                {kADVERB, INHIBIT, 0},
            },
            // std::vector<FiberRule> fiber_rules
            {
                {kLEX, kVERB, INHIBIT, 0},
                {kPREP_P, kVERB, DISINHIBIT, 0}}}};
}

GenericRules genericIntransVerb(int index)
{
    return {
        index,
        // AreaAndFiberRules pre_rules
        {
            // std::vector<AreaRule> area_rules
            {
                {kADVERB, DISINHIBIT, 1}},
            // std::vector<FiberRule> fiber_rules
            {
                {kLEX, kVERB, DISINHIBIT, 0},
                {kVERB, kSUBJ, DISINHIBIT, 0},
                {kVERB, kADVERB, DISINHIBIT, 0}}},
        // AreaAndFiberRules post_rules
        {
            // std::vector<AreaRule> area_rules
            {
                {kSUBJ, INHIBIT, 0},
                {kADVERB, INHIBIT, 0}},
            // std::vector<FiberRule> fiber_rules
            {
                {kLEX, kVERB, INHIBIT, 0},
                {kPREP_P, kVERB, DISINHIBIT, 0}}}};
}

GenericRules genericCopula(int index)
{
    return {
        index,
        // AreaAndFiberRules pre_rules
        {
            // std::vector<AreaRule> area_rules
            {

            },
            // std::vector<FiberRule> fiber_rules
            {
                {kLEX, kVERB, DISINHIBIT, 0},
                {kVERB, kSUBJ, DISINHIBIT, 0}}},
        // AreaAndFiberRules post_rules
        {
            // std::vector<AreaRule> area_rules
            {
                {kSUBJ, INHIBIT, 0},
                {kOBJ, DISINHIBIT, 0}},
            // std::vector<FiberRule> fiber_rules
            {
                {kLEX, kVERB, INHIBIT, 0},
                {kADJ, kVERB, DISINHIBIT, 0},
                {kADJ1, kVERB, DISINHIBIT, 0},
                {kADJ2, kVERB, DISINHIBIT, 0},
            }}};
}

GenericRules genericAdverb(int index)
{
    return {
        index,
        // AreaAndFiberRules pre_rules
        {
            // std::vector<AreaRule> area_rules
            {
                {kADVERB, DISINHIBIT, 0}},
            // std::vector<FiberRule> fiber_rules
            {
                {kLEX, kADVERB, DISINHIBIT, 0}}},
        // AreaAndFiberRules post_rules
        {
            // std::vector<AreaRule> area_rules
            {
                {kADVERB, INHIBIT, 1}},
            // std::vector<FiberRule> fiber_rules
            {
                {kLEX, kADVERB, INHIBIT, 0}}}};
}

GenericRules genericAdjective(int index, int order)
{
    std::string adj = order > 0 ? kADJ + char('0' + order) : kADJ;
    return {index,
            // AreaAndFiberRules pre_rules
            {// std::vector<AreaRule> area_rules
             {{adj, DISINHIBIT, 0}},
             // std::vector<FiberRule> fiber_rules
             {{kLEX, adj, DISINHIBIT, 0}}},
            // AreaAndFiberRules post_rules
            {// std::vector<AreaRule> area_rules
             {

             },
             // std::vector<FiberRule> fiber_rules
             {{kLEX, adj, INHIBIT, 0}, {kVERB, adj, INHIBIT, 0}}}};
}

GenericRules genericDeterminant(int index)
{
    return {
        index,
        // AreaAndFiberRules pre_rules
        {
            // std::vector<AreaRule> area_rules
            {
                {kDET, DISINHIBIT, 0}},
            // std::vector<FiberRule> fiber_rules
            {
                {kLEX, kDET, DISINHIBIT, 0}}},
        // AreaAndFiberRules post_rules
        {
            // std::vector<AreaRule> area_rules
            {

            },
            // std::vector<FiberRule> fiber_rules
            {
                {kLEX, kDET, INHIBIT, 0},
                {kVERB, kADJ, INHIBIT, 0},
                {kVERB, kADJ1, INHIBIT, 0},
                {kVERB, kADJ2, INHIBIT, 0},
            }}};
}

GenericRules genericPreposition(int index)
{
    return {
        index,
        // AreaAndFiberRules pre_rules
        {
            // std::vector<AreaRule> area_rules
            {
                {kPREP, DISINHIBIT, 0}},
            // std::vector<FiberRule> fiber_rules
            {
                {kLEX, kPREP, DISINHIBIT, 0}}},
        // AreaAndFiberRules post_rules
        {
            // std::vector<AreaRule> area_rules
            {
                {kPREP_P, DISINHIBIT, 0}},
            // std::vector<FiberRule> fiber_rules
            {
                {kLEX, kPREP, INHIBIT, 0},
                {kLEX, kSUBJ, INHIBIT, 1},
                {kLEX, kOBJ, INHIBIT, 1},
                {kDET, kSUBJ, INHIBIT, 1},
                {kDET, kOBJ, INHIBIT, 1},
                {kADJ, kSUBJ, INHIBIT, 1},
                {kADJ, kOBJ, INHIBIT, 1},
                {kADJ1, kSUBJ, INHIBIT, 1},
                {kADJ1, kOBJ, INHIBIT, 1},
                {kADJ2, kSUBJ, INHIBIT, 1},
                {kADJ2, kOBJ, INHIBIT, 1},
            }}};
}

GenericRules getRules(PosAndIndex arg, int order)
{
    switch (arg.pos)
    {
    case NOUN:
        return genericNoun(arg.index);
    case TRANSVERB:
        return genericTransVerb(arg.index);
    case INTRANSVERB:
        return genericIntransVerb(arg.index);
    case COPULA:
        return genericCopula(arg.index);
    case ADJ:
        return genericAdjective(arg.index, order);
    case ADVERB:
        return genericAdverb(arg.index);
    case DET:
        return genericDeterminant(arg.index);
    case PREP:
        return genericPreposition(arg.index);
    default:
        std::cerr << "Undefined part of speech\n";
        exit(0);
    }
}