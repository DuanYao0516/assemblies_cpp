#ifndef PIPELINE_H
#define PIPELINE_H

#include "parser.h"

#include <map>
#include <vector>
#include <string>
#include <iostream>

class Pipeline
{
public:
    // 构造函数
    Pipeline(BioParser::Parser &parser);

    // 将句子分词
    std::vector<std::string> Tokenize(const std::string &sentence);

    // 依赖解析
    std::vector<std::vector<std::string>> DepParse(
        const std::vector<std::string> &word_vector, float p,
        int LEX_k = 20, int proj_rounds = 20, int non_LEX_n = 10000, int non_LEX_k = 100,
        ReadoutMethod method = FIBER_READOUT, bool verbose = false);

    // 文件依赖解析
    void FileDepParse(const std::string &file_path, float p,
                      int LEX_k = 20, int proj_rounds = 20, int non_LEX_n = 10000, int non_LEX_k = 100,
                      ReadoutMethod method = FIBER_READOUT);

private:
    double total_time_;
    int total_num_words;
    BioParser::Parser parser_brain_;
    BioParser::Parser origin_parser_brain_;
};

#endif // PIPELINE_H
