#ifndef PARSER_H
#define PARSER_H

#include "brain.h"
#include "constants.h"
#include "rules.h"

#include <fstream>
#include <sstream>
#include <iostream>

/// @brief 生物可解释性自然语言解析器
namespace BioParser
{
    class Parser : public Komari::Brain
    {
    public:
        /// @brief 创建一个解析器。
        /// @param p 随机图的连接生成概率
        /// @param lexeme_dict 存储单词及其对应规则的字典
        /// @param all_areas 所有的脑区
        /// @param initial_areas 初始化时不被抑制的脑区
        /// @param recurrent_areas 自循环的脑区
        Parser(float p,
               std::map<std::string, PosAndIndex> lexeme_dict_,
               std::vector<std::string> all_areas_,
               std::vector<std::string> initial_areas_,
               std::vector<std::string> recurrent_areas_,
               const std::map<std::string, std::vector<std::string>> &readout_rules_);

        /// @brief 使用文件路径创建一个解析器。
        /// @param lexeme_dict_path 词典文件路径
        Parser(float p,
               const std::string &lexeme_dict_path,
               std::vector<std::string> all_areas_,
               std::vector<std::string> initial_areas_,
               std::vector<std::string> recurrent_areas_,
               const std::map<std::string, std::vector<std::string>> &readout_rules_);

        /// @brief readout 方法
        /// @param area_name
        /// @param readout_map
        /// @param dependency
        void ReadOut(const std::string &area_name,
                     const std::map<std::string, std::vector<std::string>> &readout_map,
                     std::vector<std::vector<std::string>> &dependency);

        /// @brief 激活指定区域中的一个单词。
        /// @param area_name 脑区名字
        /// @param word 单词
        void ActivateWord(std::string area_name, std::string word);

        /// @brief 试图在指定区域中获取一个单词，指定区域中投影形成的神经集与表征单词的神经集的交集应当达到一个阈值。
        /// @param area_name 脑区
        /// @param threshold 交集阈值
        /// @return 如果成功，返回单词；否则返回 "<Not A Word>"
        std::string GetWord(std::string area_name, float threshold);

        /// @brief 应用脑区规则以及纤维规则
        /// @param rules 待应用的脑区及纤维规则。
        void ApplyRules(AreaAndFiberRules rules);

        /// @brief 解析器执行一次强投影。
        void Step();

        /// @brief 初始化解析器状态。
        void Initiate();

        /// @brief 解除所有脑区中神经集的固定状态（请注意与抑制状态的区别）。
        void UnfixAllAreas();

        /// @brief 获取激活纤维
        std::map<std::string, std::vector<std::string>> getActivatedFibers();

        /// @brief 获取单词的词性和索引
        PosAndIndex GetPosAndIndex(const std::string &word);

        /// @brief 重置解析器
        void Reset();

    private:
        /// @brief 记住曾历经投影的单向神经纤维，用于读出规则。
        /// @param proj_map 投影图
        /// @todo FIBER_READOUT 读出规则的实现应当会用到该函数，请仔细检查。
        void RememberFibers(std::map<std::string, std::vector<std::string>> proj_map);

        /// @brief  @brief 读入词典文件，在初始化时被调用
        /// @param lexeme_dict_path
        void LoadLexemeDict(const std::string &lexeme_dict_path);

        POS String2Pos(const std::string &pos_str); // 将 string类型转换为 POS enum类型

        // 存储解析器支持的单词及其对应规则的字典
        std::map<std::string, PosAndIndex> lexeme_dict_;
        // 所有脑区
        std::vector<std::string> all_areas_;
        // 初始脑区
        std::vector<std::string> initial_areas_;
        // 自循环脑区（存在自循环连接）
        std::vector<std::string> recurrent_areas_;
        // 曾经激活过的纤维，用于 FIBER_READOUT 读出规则
        std::map<std::string, std::unordered_set<std::string>> activated_fibers_;
        // 语言读出规则
        std::map<std::string, std::unordered_set<std::string>> readout_rules_;
    };

} // namespace BioParser

#endif // PARSER_H