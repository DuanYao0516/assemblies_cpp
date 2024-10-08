#include "pipeline.h"

#include <fstream>
#include <sstream>
#include <chrono>

Pipeline::Pipeline(BioParser::Parser &parser) : total_time_(0.0), total_num_words(0),
                                                origin_parser_brain_(parser), parser_brain_(parser) {}

std::vector<std::string> Pipeline::Tokenize(const std::string &sentence)
{
    std::vector<std::string> words;
    std::stringstream ss(sentence);
    std::string word;
    while (ss >> word)
    {
        words.push_back(word);
    }
    return words;
}

std::vector<std::vector<std::string>> Pipeline::DepParse(
    const std::vector<std::string> &word_vector, float p,
    int LEX_k, int proj_rounds, int non_LEX_n, int non_LEX_k,
    ReadoutMethod method, bool verbose)
{
    using namespace BioParser;

    // double time_begin = omp_get_wtime();
    auto time_begin = std::chrono::steady_clock::now();
    int order = 0;
    int last_pos = INVALID;

    parser_brain_ = origin_parser_brain_;

    for (const auto &word : word_vector)
    {
        PosAndIndex pos_and_index;
        try
        {
            pos_and_index = parser_brain_.GetPosAndIndex(word);
        }
        catch (std::out_of_range)
        {
            std::cerr << "Word undefined in lexeme dict. Plz check ur inputs.\n Exit right now." << std::endl;
            exit(1);
        }

        if (last_pos == pos_and_index.pos)
            order++;
        else
            order = 0;

        GenericRules lexeme = getRules(pos_and_index, order);
        parser_brain_.ActivateWord(kLEX, word);

        if (verbose)
            std::cout << "Activated word: " << word << std::endl;

        parser_brain_.ApplyRules(lexeme.pre_rules);

        std::map<std::string, std::vector<std::string>> proj_map = parser_brain_.GetProjectMap();
        std::unordered_set<std::string> from_areas;
        for (const auto &ele : proj_map)
        {
            for (const auto &v : ele.second)
            {
                from_areas.insert(v);
            }
        }

        for (const auto &ele : from_areas)
        {
            if (std::find(proj_map[ele].begin(), proj_map[ele].end(), kLEX) == proj_map[ele].end())
            {
                parser_brain_.FixAssembly(ele);
                if (verbose)
                    std::cout << "Fixed assembly because no projection from LEX into " << ele << std::endl;
                continue;
            }
            else
            {
                if (ele == kLEX)
                    continue;
                for (const auto &e : proj_map)
                {
                    if (std::find(e.second.begin(), e.second.end(), ele) != e.second.end())
                    {
                        parser_brain_.UnfixAssembly(ele);
                        parser_brain_.getAreaByName(ele).winners_[0] = -1;
                        if (verbose)
                            std::cout << "Erased assembly because of projection from LEX into " << ele << std::endl;
                        break;
                    }
                }
            }
        }

        for (size_t i = 0; i < proj_rounds; i++)
        {
            parser_brain_.Step();
        }

        parser_brain_.ApplyRules(lexeme.post_rules);
        last_pos = pos_and_index.pos;
    }

    parser_brain_.UnfixAllAreas();
    parser_brain_.DisablePlasticity();

    // double time_end = omp_get_wtime();
    auto time_end = std::chrono::steady_clock::now();

    double duration = std::chrono::duration<double>(time_end - time_begin).count();
    if (verbose)
        std::cout << "Time elapsed: " << duration << " seconds!!!\n";

    total_time_ += duration;

    if (verbose)
        std::cout
            << "Parse end, Readout start..." << std::endl;

    // 根据 method 设置 map
    std::map<std::string, std::vector<std::string>> readout_map;
    if (method == FIXED_MAP_READOUT)
    {
        readout_map = kENGLISH_READOUT_RULES;
    }
    else if (method == FIBER_READOUT)
    {
        readout_map = parser_brain_.getActivatedFibers();
    }
    else
    {
        readout_map = kENGLISH_READOUT_RULES;
    }

    std::vector<std::vector<std::string>> dependencies;
    parser_brain_.ReadOut(kVERB, readout_map, dependencies);

    for (const auto &dependency : dependencies)
    {
        for (const auto &s : dependency)
        {
            std::cout << s << ' ';
        }
        std::cout << "| ";
    }
    std::cout << std::endl;

    return dependencies;
}

void Pipeline::FileDepParse(const std::string &file_path, float p, int LEX_k,
                            int proj_rounds, int non_LEX_n, int non_LEX_k, ReadoutMethod method)
{
    std::ifstream infile(file_path);
    std::string line;
    std::vector<std::string> sentences;
    while (std::getline(infile, line))
        sentences.push_back(line);

    for (std::string line : sentences)
    {
        if (line.size() == 0)
            continue;

        std::cout << line << std::endl;
        std::vector<std::string> words = Tokenize(line);
        total_num_words += words.size();
        DepParse(words, p, LEX_k, proj_rounds, non_LEX_n, non_LEX_k, method, false);
    }
    std::cout << "Finished parsing!!!" << std::endl;
    std::cout << "Total time elapsed: " << total_time_ << " seconds!!!\n";
    std::cout << "Processing frequency: " << total_time_ / total_num_words << " s/word !!!\n";
    std::cout << "Processing speed: " << total_num_words / total_time_ << " words/s !!!\n";
}
