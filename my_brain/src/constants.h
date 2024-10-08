#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <string>
#include <vector>
#include <map>

#define INHIBIT true
#define DISINHIBIT false
// 大脑的最大容纳脑区数，如有必要请自行修改。
#define MAX_NUM_AREAS 14

const int kLEX_SIZE = 30; // 词表的最大容纳单词数，如有必要请自行修改。

// 各个脑区的相关配置
const int LEX_k = 20, non_LEX_n = 10000, non_LEX_k = 100; // LEX 中的帽子(cap)大小，非 LEX 脑区神经元数，非 LEX 脑区的帽子(cap)大小
const float kDEFAULT_BETA = 0.2;                          // 您几乎可以忽略该超参数
const float kINTERAREA_BETA = 0.5;                        // 隐式脑区间可塑性超参数
const float kRECURRENT_BETA = 0.05;                       // 脑区自循环可塑性超参数
const float kLEX_BETA = 1.0;                              // 显式脑区 LEX 可塑性超参数

// 修改以下常量时，请务必清楚自己在干啥!
const int kNUM_ADJ = 2;

const std::string kLEX = "LEX";
const std::string kDET = "DET";
const std::string kSUBJ = "SUBJ";
const std::string kVERB = "VERB";
const std::string kOBJ = "OBJ";
const std::string kADJ = "ADJ";
const std::string kADJ1 = "ADJ1";
const std::string kADJ2 = "ADJ2";
const std::string kADVERB = "ADVERB";
const std::string kPREP = "PREP";
const std::string kPREP_P = "PREP_P";

const std::vector<std::string> kAREAS = {kLEX, kDET, kSUBJ, kVERB, kOBJ, kADJ, kADJ1, kADJ2, kADVERB, kPREP, kPREP_P};
const std::vector<std::string> kEXPLICIT_AREAS = {kLEX};
const std::vector<std::string> kRECURRENT_AREAS = {kSUBJ, kVERB, kOBJ, kADJ, kADJ1, kADJ2, kADVERB, kPREP, kPREP_P};

enum POS
{
    NOUN,
    TRANSVERB,
    INTRANSVERB,
    COPULA,
    ADJ,
    ADVERB,
    DET,
    PREP,
    INVALID
};

// Readout methods
enum ReadoutMethod
{
    FIXED_MAP_READOUT,
    FIBER_READOUT
};

const std::map<std::string, std::vector<std::string>> kENGLISH_READOUT_RULES = {
    {kVERB, {kLEX, kSUBJ, kOBJ, kPREP_P, kADVERB, kADJ}},
    {kSUBJ, {kLEX, kDET, kADJ, kADJ1, kADJ2, kPREP_P}},
    {kOBJ, {kLEX, kDET, kADJ, kADJ1, kADJ2, kPREP_P}},
    {kPREP_P, {kLEX, kPREP, kADJ, kADJ1, kADJ2, kDET}},
    {kPREP, {kLEX}},
    {kADJ, {kLEX}},
    {kADJ1, {kLEX}},
    {kADJ2, {kLEX}},
    {kADVERB, {kLEX}},
    {kDET, {kLEX}},
    {kLEX, {}}};

#endif // CONSTANTS_H
