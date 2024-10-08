#ifndef BRAIN_H
#define BRAIN_H

#include "constants.h"

#include <stdint.h>
#ifdef _OPENMP
#include <omp.h>
#endif

#include <map>
#include <random>
#include <string>
#include <vector>
#include <algorithm>
#include <unordered_set>
#include <iostream>

namespace Komari
{
    // 该死的。我想我们需要去 Github 找一个打印库。
    template <typename tp>
    void Print(std::vector<tp> v);

    // <脑区>
    class Area
    {
    public:
        Area() {}
        Area(uint8_t area_id, int n_neurons, int k_winners, float beta)
            : area_id_(area_id), n_neurons_(n_neurons), k_winners_(k_winners), beta_(beta)
        {
            winners_ = std::vector<int>(k_winners, -1);
            new_winners_ = std::vector<int>(k_winners, -1);
            size_support_ = 0, new_size_support_ = 0;
        }

        /// @brief 高级系统控制，抑制或解除抑制。
        /// @param inhibit 为真则抑制，否则解除抑制
        /// @param population 抑制性种群识别号
        void applyRule(bool inhibit, int population);

        /// @brief 更新脑区中胜者相关信息（胜者索引，支持集大小）。
        void update();

        /// @brief 激活 [begin, end) 区间的神经元作为胜者。
        /// @param begin 胜者开头索引
        /// @param end 胜者结尾索引
        void fire(int begin, int end);

        /// @brief 判断脑区是否处于非抑制状态。
        /// @return 非抑制返回真，否则假
        bool isActive();

        /// @brief 固定脑区中的神经集。
        void fixAssembly() { is_fixed_ = true; }

        /// @brief 解除脑区中神经集的固定状态（请注意区分固定状态和抑制状态）。
        void unfixAssembly() { is_fixed_ = false; }

        // 输出脑区相关信息。
        void info();

        // 脑区索引。
        uint8_t area_id_;
        // 脑区中神经元总数。
        int n_neurons_;
        // 一个时刻中脑区中有多少胜者。
        int k_winners_;
        // 默认可塑性超参数。
        float beta_;
        // 支持集（脑区中曾放过电的神经元集）大小
        int size_support_;
        // 同 size_support_，缓存用。
        int new_size_support_;
        // 脑区的神经集(assembly)是否被固定。
        bool is_fixed_ = false;
        // 脑区是否以显式方式模拟。
        bool is_explicit = false;
        // 脑区中胜者索引列表。
        std::vector<int> winners_;
        // 同 winners_，缓存用。
        std::vector<int> new_winners_;

        // 抑制状态码。
        uint8_t bit_status_ = 0;
    };

    // <连接脑区的神经纤维>
    // 论文中，神经纤维是双向（或者理解为无向）的，这里我们采用单向设计。
    // 也就是说，论文中的一条纤维需要我们建立俩 Fiber 类。
    // 这是出于脑区间突触（有序对）是单向的考虑——我把突触的邻接矩阵也囊括在 Fiber 类当中了。
    // 也许有更好的设计？
    class Fiber
    {
    public:
        Fiber() {}
        Fiber(uint8_t from_area, uint8_t to_area)
        {
            shape_[0] = 0;
            shape_[1] = 0;
        }
        // 高级系统控制，抑制或解除抑制，inhibit 为 0 则抑制，为 1 则解除抑制。
        void applyRule(bool inhibit, int population);
        // 判断纤维是否处于非抑制状态。
        bool isActive();

        // 可塑性超参数，初始化为源脑区的可塑性超参数，后续可更新。
        float beta_;

        // 纤维两端神经元的邻接矩阵形状，类似 numpy.ShapeLike。
        // 也许并不一定需要？不过为了调试以及防止莫名的错误，还是加上吧。
        int shape_[2];
        // 纤维两端的神经元的单向邻接矩阵
        std::vector<std::vector<float>> outgoing_synapses_;

        // 抑制状态码。
        uint8_t bit_status_ = 0;
    };

    class Brain
    {
    public:
        // Brain() {}
        Brain(float p, float beta, uint32_t seed);
        Area &getAreaByName(std::string name);
        // 添加隐式脑区。
        void AddArea(std::string area_name, int num_neurons, int k_winners, float beta);
        // 添加显式脑区。
        void AddExplicitArea(std::string area_name, int num_neurons, int k_winners, float beta);
        // 多个脑区投影到一个脑区。
        int Project(std::string target_area, std::vector<std::string> from_areas);

        /// @brief 强投影。
        /// @param proj_map 投影映射，形如 {目标脑区1：<源脑区序列1>, 目标脑区2：<源脑区序列2>}
        void ProjectStar(std::map<std::string, std::vector<std::string>> proj_map);
        void ApplyAreaRule(std::string area, bool inhibit, int population);
        void ApplyFiberRule(std::string first_area, std::string second_area, bool inhibit, int population);

        bool IsActive(std::string area) { return areas_[area_by_name_[area]].isActive(); }
        bool IsActive(std::string first_area, std::string second_area)
        {
            return fibers_[area_by_name_[first_area]][area_by_name_[second_area]].isActive();
        }

        std::map<std::string, std::vector<std::string>> GetProjectMap();
        void FixAssembly(std::string area_name) { areas_[area_by_name_[area_name]].fixAssembly(); }
        void UnfixAssembly(std::string area_name) { areas_[area_by_name_[area_name]].unfixAssembly(); }
        void Fire(std::string area_name, int begin, int end) { areas_[area_by_name_[area_name]].fire(begin, end); }
        void InhibitAllAreas(bool inhibit, int population);
        void InhibitAllFibers(int population);
        void UpdatePlasticities(const std::vector<std::string> recurrent_areas, std::string strong_area,
                                float recurrent_beta, float interarea_beta, float strong_beta);
        void DisablePlasticity() { disable_plasticity = true; }
        void EnablePlasticity() { disable_plasticity = false; }
        void info();
        void brainReset();

    private:
        // 连接生成概率。
        float p_;
        // 可塑性超参数。
        float beta_;
        // 随机数生成器。
        std::mt19937 rng_;
        // 脑区名 -> 脑区索引 的字典
        std::map<std::string, uint32_t> area_by_name_;
        // 所有脑区名
        std::vector<std::string> area_names_;

        // 脑区，这里用数组先顶着。
        Area areas_[MAX_NUM_AREAS];

        // 纤维，视作属于脑区的临界矩阵。
        Fiber fibers_[MAX_NUM_AREAS][MAX_NUM_AREAS];

        // 脑区数。
        int num_areas_ = 0;

        // 是否禁止可塑性。
        bool disable_plasticity = false;
    };

}

#endif // BRAIN_H