#include "parser.h"

#include <fstream>
#include <sstream>
#include <iostream>

namespace BioParser
{
    Parser::Parser(float p,
                   std::map<std::string, PosAndIndex> lexeme_dict_,
                   std::vector<std::string> all_areas_,
                   std::vector<std::string> initial_areas_,
                   std::vector<std::string> recurrent_areas_,
                   const std::map<std::string, std::vector<std::string>> &readout_rules)
        : Brain(p, 0.5, 6666),
          lexeme_dict_(lexeme_dict_),
          all_areas_(all_areas_),
          initial_areas_(initial_areas_),
          recurrent_areas_(recurrent_areas_)
    {
        // Initialize readout_rules_ from the provided readout_rules_ parameter
        for (const auto &rule : readout_rules)
        {
            const auto &from_area = rule.first;
            const auto &to_areas = rule.second;
            readout_rules_[from_area] = std::unordered_set<std::string>(to_areas.begin(), to_areas.end());
        }
    }

    Parser::Parser(float p,
                   const std::string &lexeme_dict_path,
                   std::vector<std::string> all_areas_,
                   std::vector<std::string> initial_areas_,
                   std::vector<std::string> recurrent_areas_,
                   const std::map<std::string, std::vector<std::string>> &readout_rules)
        : Brain(p, 0.5, 6666),
          all_areas_(all_areas_),
          initial_areas_(initial_areas_),
          recurrent_areas_(recurrent_areas_)
    {

        LoadLexemeDict(lexeme_dict_path);

        for (const auto &rule : readout_rules)
        {
            const auto &from_area = rule.first;
            const auto &to_areas = rule.second;
            readout_rules_[from_area] = std::unordered_set<std::string>(to_areas.begin(), to_areas.end());
        }
    }

    void Parser::ReadOut(const std::string &area_name,
                         const std::map<std::string, std::vector<std::string>> &readout_map,
                         std::vector<std::vector<std::string>> &dependency)
    {
        std::vector<std::string> to_areas = readout_map.at(area_name);
        std::map<std::string, std::vector<std::string>> proj_map;
        std::string this_word, other_word;

        for (std::string area : to_areas)
            proj_map[area] = {area_name};

        ProjectStar(proj_map);
        this_word = GetWord(kLEX, 0.7);

        for (std::string to_area : to_areas)
        {
            if (to_area == kLEX)
                continue;
            ProjectStar({{kLEX, {to_area}}});
            if (to_area == kDET)
                other_word = GetWord(kLEX, 0.5);
            else
                other_word = GetWord(kLEX, 0.7);
            dependency.push_back({this_word, other_word, to_area});
        }

        for (std::string to_area : to_areas)
        {
            if (to_area != kLEX)
                ReadOut(to_area, readout_map, dependency);
        }
    }

    void Parser::LoadLexemeDict(const std::string &lexeme_dict_path)
    {
        std::ifstream infile(lexeme_dict_path);
        if (!infile)
        {
            std::cerr << "Cannot open lexeme dictionary file: " << lexeme_dict_path << std::endl;
            return;
        }

        std::string line;
        int line_num = 0;
        while (std::getline(infile, line))
        {
            std::istringstream iss(line);
            std::string word, pos_str;
            if (std::getline(iss, word, ',') && std::getline(iss, pos_str))
            {
                POS pos = String2Pos(pos_str);
                if (pos != INVALID)
                {
                    lexeme_dict_[word] = {pos, line_num};
                }
                else
                {
                    std::cerr << "Invalid POS for word: " << word << std::endl;
                }
            }
            line_num++;
        }
    }

    POS Parser::String2Pos(const std::string &pos_str)
    {
        if (pos_str == "NOUN")
            return NOUN;
        if (pos_str == "TRANSVERB")
            return TRANSVERB;
        if (pos_str == "INTRANSVERB")
            return INTRANSVERB;
        if (pos_str == "COPULA")
            return COPULA;
        if (pos_str == "ADJ")
            return ADJ;
        if (pos_str == "ADVERB")
            return ADVERB;
        if (pos_str == "DET")
            return DET;
        if (pos_str == "PREP")
            return PREP;
        return INVALID;
    }

    void Parser::ActivateWord(std::string area_name, std::string word)
    {
        int k = getAreaByName(area_name).k_winners_;
        int begin = lexeme_dict_[word].index * k;
        Fire(area_name, begin, begin + k);
        FixAssembly(area_name);
    }

    std::string Parser::GetWord(std::string area_name, float threshold)
    {
        Komari::Area &tag_area = getAreaByName(area_name);
        if (tag_area.winners_[0] < -1)
        {
            std::cerr << "GetWord() failed cause no assembly inside of area: " << area_name << std::endl;
            exit(0);
        }
        auto winners = tag_area.winners_;
        std::sort(winners.begin(), winners.end());

        for (auto ele : lexeme_dict_)
        {
            int word_index = ele.second.index;
            std::vector<int> range, v;
            int assembly_begin = word_index * tag_area.k_winners_;

            for (size_t i = assembly_begin; i < assembly_begin + tag_area.k_winners_; i++)
                range.push_back(i);

            std::set_intersection(winners.begin(), winners.end(), range.begin(), range.end(), std::back_inserter(v));

            if (v.size() >= threshold * tag_area.k_winners_)
                return ele.first;
        }

        return "<Not A Word>";
    }

    void Parser::RememberFibers(std::map<std::string, std::vector<std::string>> proj_map)
    {
        for (auto ele : proj_map)
        {
            auto it = activated_fibers_.find(ele.first); // target
            if (it == activated_fibers_.end())           // 防止没有key，插入key
            {
                activated_fibers_[ele.first] = {};
                it = activated_fibers_.find(ele.first);
            }

            for (auto s : ele.second) // 插入各个source
                it->second.insert(s);
        }
    }

    void Parser::Step()
    {
        auto proj_map = GetProjectMap();
        RememberFibers(proj_map);
        ProjectStar(proj_map);
    }

    void Parser::ApplyRules(AreaAndFiberRules rules)
    {
        for (AreaRule rule : rules.area_rules)
            ApplyAreaRule(rule.name, rule.inhibit, rule.population);
        for (FiberRule rule : rules.fiber_rules)
            ApplyFiberRule(rule.first_name, rule.second_name, rule.inhibit, rule.population);
    }

    void Parser::Initiate()
    {
        InhibitAllAreas(true, 0);
        InhibitAllFibers(0);

        // 除了初始脑区的脑区一开始都应当被抑制。
        for (auto name : initial_areas_)
            getAreaByName(name).applyRule(false, 0);
    }

    void Parser::UnfixAllAreas()
    {
        for (std::string area : all_areas_)
            UnfixAssembly(area);
    }

    std::map<std::string, std::vector<std::string>> Parser::getActivatedFibers()
    {
        std::map<std::string, std::vector<std::string>> pruned_activated_fibers;

        for (const auto &from_area_pair : activated_fibers_)
        {
            const std::string &from_area = from_area_pair.first;
            const std::unordered_set<std::string> &to_areas = from_area_pair.second;

            for (const auto &to_area : to_areas)
            {
                if (readout_rules_.find(from_area) != readout_rules_.end() &&
                    readout_rules_.at(from_area).find(to_area) != readout_rules_.at(from_area).end())
                {
                    pruned_activated_fibers[from_area].push_back(to_area);
                }
            }
        }

        return pruned_activated_fibers;
    }

    PosAndIndex Parser::GetPosAndIndex(const std::string &word)
    {
        return lexeme_dict_.at(word);
    }

    void Parser::Reset()
    {
        activated_fibers_.clear();
        UnfixAllAreas();
        EnablePlasticity();
        brainReset();

        Initiate();
    }

} // namespace BioParser
