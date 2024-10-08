#include "brain.h"

#include <stdio.h>

namespace Komari
{

    template <typename tp>
    void Print(std::vector<tp> v)
    {
        for (auto ele : v)
            std::cout << ele << ' ';
        std::cout << std::endl;
    }

    // 二项分布的逆累积分布函数
    float BinomQuantile(uint32_t k, float p, float percent)
    {
        double pi = std::pow(1.0 - p, k);
        double mul = (1.0 * p) / (1.0 - p);
        double total_p = pi;
        uint32_t i = 0;
        while (total_p < percent)
        {
            pi *= ((k - i) * mul) / (i + 1);
            total_p += pi;
            ++i;
        }
        return i;
    }

    // 截尾正态分布。
    template <typename Trng>
    float TruncatedNorm(float a, Trng &rng)
    {
        if (a <= 0.0f)
        {
            std::normal_distribution<float> norm(0.0f, 1.0f);
            for (;;)
            {
                const float x = norm(rng);
                if (x >= a)
                    return x;
            }
        }
        else
        {
            // Exponential accept-reject algorithm from Robert,
            // https://arxiv.org/pdf/0907.4010.pdf
            const float alpha = (a + std::sqrt(a * a + 4)) * 0.5f;
            std::exponential_distribution<float> d(alpha);
            std::uniform_real_distribution<float> u(0.0f, 1.0f);
            for (;;)
            {
                const float z = a + d(rng);
                const float dz = z - alpha;
                const float rho = std::exp(-0.5f * dz * dz);
                if (u(rng) < rho)
                    return z;
            }
        }
    }

    template <typename tp>
    std::vector<int> findLargestIndices(const std::vector<tp> &vec, int k)
    {
        std::vector<int> indices(vec.size());
        std::iota(indices.begin(), indices.end(), 0);

        std::partial_sort(indices.begin(), indices.begin() + k, indices.end(),
                          [&vec](tp i, tp j)
                          { return vec[i] > vec[j]; });

        indices.resize(k);

        return indices;
    }

    std::vector<int> choice(const std::vector<int> &population, int k, bool replace, std::mt19937 rng)
    {
        std::vector<int> indices(population);

        if (!replace)
        {
            std::shuffle(indices.begin(), indices.end(), rng);
            indices.resize(k);
        }
        else
        {
            std::vector<int> result;
            for (int i = 0; i < k; ++i)
            {
                int randIndex = std::uniform_int_distribution<int>(0, indices.size() - 1)(rng);
                result.push_back(indices[randIndex]);
                if (!replace)
                {
                    indices.erase(indices.begin() + randIndex);
                }
            }
            return result;
        }

        return indices;
    }

    void Area::applyRule(bool inhibit, int population)
    {
        if (inhibit)
            bit_status_ |= (1 << population);
        else
            bit_status_ &= ~(1 << population);
    }

    bool Area::isActive()
    {
        return bit_status_ == 0;
    }

    void Area::info()
    {
        using namespace std;
        printf("area_id: %u\n", area_id_);
        cout << "is_explicit: " << is_explicit << "\t";
        cout << "is_fixed: " << is_fixed_ << endl;
        cout << "num_neurons: " << n_neurons_ << "\tnum_winners: " << k_winners_ << endl;
        cout << "winners: ";
        if (winners_[0] != -1)
        {
            for (auto ele : winners_)
                cout << ele << ' ';
        }
        cout << endl;
    }

    void Fiber::applyRule(bool inhibit, int population)
    {
        if (inhibit)
            bit_status_ |= (1 << population);
        else
            bit_status_ &= ~(1 << population);
    }

    bool Fiber::isActive()
    {
        return bit_status_ == 0;
    }

    void Area::update()
    {
        winners_ = new_winners_;
        if (!is_explicit)
            size_support_ = new_size_support_;
    }

    void Area::fire(int begin, int end)
    {
        for (size_t i = begin; i < end; i++)
            winners_[i - begin] = i;
    }

    Brain::Brain(float p, float beta, uint32_t seed) : p_(p), beta_(beta), rng_(seed) {}

    Area &Brain::getAreaByName(std::string name)
    {
        return areas_[area_by_name_[name]];
    }

    void Brain::AddArea(std::string area_name, int num_neurons, int k_winners, float beta)
    {
        if (num_areas_ == MAX_NUM_AREAS)
        {
            std::cerr << "num_areas of the brain has exceeded the limitation!!!\n Exit right now.\n";
            exit(1);
        }

        Fiber fib_in, fib_out;
        int other_size;

        area_names_.push_back(area_name);
        area_by_name_[area_name] = num_areas_;
        areas_[num_areas_] = Area(num_areas_, num_neurons, k_winners, beta);

        for (size_t i = 0; i < num_areas_ + 1; i++)
        {
            // 建立连接
            fib_in = Fiber(i, num_areas_), fib_out = Fiber(num_areas_, i);
            other_size = areas_[i].is_explicit ? areas_[i].n_neurons_ : 0;
            fib_in.outgoing_synapses_ = std::vector<std::vector<float>>(other_size, std::vector<float>(0));
            fib_in.shape_[0] = other_size, fib_in.shape_[1] = 0;
            fib_in.beta_ = areas_[i].beta_;

            if (i != num_areas_)
            {
                fib_out.outgoing_synapses_ = std::vector<std::vector<float>>(0, std::vector<float>(other_size));
                fib_out.shape_[0] = 0, fib_out.shape_[1] = other_size;
                fib_out.beta_ = beta;
                fibers_[num_areas_][i] = fib_out;
            }
            fibers_[i][num_areas_] = fib_in;
        }
        num_areas_++;
    }

    void Brain::AddExplicitArea(std::string area_name, int num_neurons, int k_winners, float beta)
    {
        if (num_areas_ == MAX_NUM_AREAS)
        {
            std::cerr << "num_areas of the brain has exceeded the limitation!!!\n Exit right now.\n";
            exit(1);
        }

        Fiber fib_in, fib_out;
        int other_size;
        std::binomial_distribution<> binom(1, p_);

        area_names_.push_back(area_name);
        area_by_name_[area_name] = num_areas_;
        areas_[num_areas_] = Area(num_areas_, num_neurons, k_winners, beta);
        areas_[num_areas_].size_support_ = num_neurons;
        areas_[num_areas_].is_explicit = true;

        for (size_t i = 0; i < num_areas_ + 1; i++)
        {
            // 建立连接
            fib_in = Fiber(i, num_areas_), fib_out = Fiber(num_areas_, i);
            other_size = areas_[i].is_explicit ? areas_[i].n_neurons_ : 0;

            fib_in.outgoing_synapses_ = std::vector<std::vector<float>>(other_size, std::vector<float>(num_neurons));
            fib_in.shape_[0] = other_size, fib_in.shape_[1] = num_neurons;
            fib_in.beta_ = areas_[i].beta_;

            if (i != num_areas_) // 尚未实现增加对显式区域的连接，不过也许本实验中根本没必要？
            {
                fib_out.outgoing_synapses_ = std::vector<std::vector<float>>(num_neurons, std::vector<float>(other_size));
                fib_out.shape_[0] = num_neurons, fib_out.shape_[1] = other_size;
                fib_out.beta_ = beta;
                for (size_t i = 0; i < num_neurons; i++)
                {
                    for (size_t j = 0; j < other_size; j++)
                    {
                        fib_out.outgoing_synapses_[i][j] = binom(rng_);
                    }
                }
                fibers_[num_areas_][i] = fib_out;
            }
            else
            {
                for (size_t i = 0; i < num_neurons; i++)
                {
                    for (size_t j = 0; j < num_neurons; j++)
                    {
                        fib_in.outgoing_synapses_[i][j] = binom(rng_);
                    }
                }
            }

            fibers_[i][num_areas_] = fib_in;
        }
        num_areas_++;
    }

    int Brain::Project(std::string target_area, std::vector<std::string> from_areas)
    {
#ifdef _OPENMP
        std::mt19937 rng_(omp_get_thread_num());
#else
        std::mt19937 rng_(rng_);
#endif

        int tag_id = area_by_name_[target_area], total_k = 0, num_inputs = 0;
        int effective_k, effective_n, num_new_winners = 0;
        float quantile, alpha;
        std::vector<int> from_ids;
        Area &tag_area = areas_[tag_id];
        int size_support = tag_area.size_support_;
        std::vector<float> first_winner_inputs, all_potential_winner_inputs;
        std::vector<float> prev_winner_inputs = std::vector<float>(size_support, 0.0);
        std::vector<float> potential_new_winner_inputs = std::vector<float>(tag_area.k_winners_, 0.0);
        std::vector<int> new_winner_indices, input_size_by_from_area_index;
        std::binomial_distribution<> binom(1, p_);

        for (auto ele : from_areas)
        {
            if (areas_[area_by_name_[ele]].winners_[0] < 0)
            {
                std::cerr << "No winners inside of " << ele << std::endl;
                continue;
            }
            from_ids.push_back(area_by_name_[ele]);
        }

        // 计算先前胜者的输入
        for (auto from_id : from_ids)
        {
            if (areas_[from_id].winners_[0] < 0)
                continue;

            auto synapses = fibers_[from_id][tag_id].outgoing_synapses_;

            for (int w : areas_[from_id].winners_)
                for (size_t i = 0; i < size_support; i++)
                    prev_winner_inputs[i] += synapses[w][i];
        }

        // 计算所有输入脑区胜者总数
        if (!tag_area.is_explicit)
        {
            for (auto from_id : from_ids)
            {
                Area &from_area = areas_[from_id];
                effective_k = from_area.winners_.size();
                input_size_by_from_area_index.push_back(effective_k);
                num_inputs++;
                total_k += effective_k;
            }
            effective_n = tag_area.n_neurons_ - size_support;
            quantile = float(effective_n - tag_area.k_winners_) / effective_n;
            alpha = BinomQuantile(total_k, p_, quantile);
            const float mu = total_k * p_;
            const float stddev = std::sqrt(total_k * p_ * (1.0f - p_));
            const float a = (alpha - mu) / stddev;
            for (size_t i = 0; i < tag_area.k_winners_; i++)
            {
                const float x = TruncatedNorm(a, rng_);
                const float d = std::min<float>(total_k, std::round(x * stddev + mu));
                potential_new_winner_inputs[i] = d;
            }
            // 拼接
            all_potential_winner_inputs = std::vector<float>(size_support + tag_area.k_winners_);
            for (size_t i = 0; i < size_support; i++)
                all_potential_winner_inputs[i] = prev_winner_inputs[i];
            for (size_t i = 0; i < tag_area.k_winners_; i++)
                all_potential_winner_inputs[i + size_support] = potential_new_winner_inputs[i];
        }
        else
        {
            all_potential_winner_inputs = prev_winner_inputs;
        }
        new_winner_indices = findLargestIndices(all_potential_winner_inputs, tag_area.k_winners_);

        if (!tag_area.is_explicit)
        {
            for (int &idx : new_winner_indices)
            {
                if (idx >= size_support)
                {
                    first_winner_inputs.push_back(all_potential_winner_inputs[idx]);
                    idx = size_support + num_new_winners;
                    num_new_winners++;
                }
            }
        }
        tag_area.new_winners_ = new_winner_indices;
        tag_area.new_size_support_ = tag_area.size_support_ + num_new_winners;

        if (tag_area.is_fixed_)
        {
            tag_area.new_winners_ = tag_area.winners_;
            tag_area.new_size_support_ = size_support;
            first_winner_inputs.clear();
            num_new_winners = 0;
        }
        // By now, we have no threads conflicts, tabun
        std::vector<std::vector<int>> inputs_by_first_winner_index(num_new_winners, std::vector<int>(num_inputs, 0));
        std::vector<int> range(total_k);
        for (size_t i = 0; i < total_k; i++)
            range[i] = i;
        for (int i = 0; i < num_new_winners; ++i)
        {
            std::vector<int> input_indices = choice(range, first_winner_inputs[i], false, rng_);
            std::vector<int> num_connections_by_input_index(num_inputs, 0);
            int total_so_far = 0;

            for (int j = 0; j < num_inputs; j++)
            {
                for (int w : input_indices)
                    if (total_so_far <= w and w < total_so_far + input_size_by_from_area_index[j])
                        num_connections_by_input_index[j]++;

                total_so_far += input_size_by_from_area_index[j];
            }

            inputs_by_first_winner_index[i] = std::move(num_connections_by_input_index);
        }
        // From now on, we have to consider about safety within threads!!!
        int num_inputs_processed = 0;
#pragma omp critical
        {
            for (const int from_id : from_ids)
            {
                Area &from_area = areas_[from_id];
                std::vector<std::vector<float>> &synapses = fibers_[from_id][tag_id].outgoing_synapses_;

                // 扩展对新优胜者的邻接矩阵
                for (auto &row : synapses)
                {
                    row.resize(synapses[0].size() + num_new_winners, 0.0);
                }
                fibers_[from_id][tag_id].shape_[1] += num_new_winners;

                for (int i = 0; i < num_new_winners; i++)
                {
                    int total_in = inputs_by_first_winner_index[i][num_inputs_processed];

                    std::unordered_set<int> from_area_winners_set(from_area.winners_.begin(), from_area.winners_.end());
                    std::vector<int> sample_indices = choice(from_area.winners_, total_in, false, rng_); // 从输入源中采样

                    for (int j : sample_indices)
                    {
                        synapses[j][size_support + i] = 1.0;
                    }

                    for (int j = 0; j < from_area.size_support_; j++)
                    {
                        if (from_area_winners_set.find(j) == from_area_winners_set.end())
                        {
                            synapses[j][size_support + i] = binom(rng_);
                        }
                    }
                }

                for (int i : tag_area.new_winners_)
                {
                    for (int j : from_area.winners_)
                    {
                        float beta = disable_plasticity ? 0 : fibers_[from_id][tag_id].beta_;
                        synapses[j][i] *= (1.0 + beta); // 权重更新
                    }
                }

                num_inputs_processed++;
            }
            for (size_t i = 0; i < num_areas_; i++)
            {
                if (std::find(from_ids.begin(), from_ids.end(), i) == from_ids.end())
                {
                    auto &synapses = fibers_[i][tag_id].outgoing_synapses_;
                    for (auto &row : synapses)
                    {
                        row.resize(fibers_[i][tag_id].shape_[1] + num_new_winners, 0.0);
                        for (size_t j = size_support; j < row.size(); j++)
                            row[j] = binom(rng_);
                    }
                    fibers_[i][tag_id].shape_[1] += num_new_winners;
                }

                auto &synapses = fibers_[tag_id][i].outgoing_synapses_;
                synapses.resize(fibers_[tag_id][i].shape_[0] + num_new_winners);
                for (size_t j = size_support; j < synapses.size(); j++)
                {
                    for (size_t k = 0; k < fibers_[tag_id][i].shape_[1]; k++)
                        synapses[j].push_back(binom(rng_));
                }
                fibers_[tag_id][i].shape_[0] += num_new_winners;
            }
        }
        return num_new_winners;
    }

    void Brain::info()
    {
        for (size_t i = 0; i < num_areas_; i++)
        {
            areas_[i].info();
            std::cout << "*********************" << std::endl;
        }
        std::cout << "***************************************" << std::endl;
    }

    void Brain::brainReset()
    {
        for (size_t i = 0; i < num_areas_; i++)
        {
            areas_[i].unfixAssembly();
            areas_[i].winners_ = std::vector<int>(areas_[i].k_winners_, -1);
            areas_[i].new_winners_ = std::vector<int>(areas_[i].k_winners_, -1);
            areas_[i].bit_status_ = 0;
            if (!areas_[i].is_explicit)
            {
                areas_[i].size_support_ = 0;
                areas_[i].new_size_support_ = 0;
                for (size_t j = 0; j < num_areas_; j++)
                {
                    Fiber fib_in = Fiber(j, i), fib_out = Fiber(i, j);
                    int other_size = areas_[j].is_explicit ? areas_[j].n_neurons_ : 0;

                    fib_in.outgoing_synapses_ = std::vector<std::vector<float>>(other_size, std::vector<float>(0));
                    fib_in.shape_[0] = other_size, fib_in.shape_[1] = 0;

                    if (j != i)
                    {
                        fib_out.outgoing_synapses_ = std::vector<std::vector<float>>(0, std::vector<float>(other_size));
                        fib_out.shape_[0] = 0, fib_out.shape_[1] = other_size;
                        fibers_[i][j] = fib_out;
                        fibers_[i][j].bit_status_ = 0;
                    }
                    fibers_[j][i] = fib_in;
                    fibers_[j][i].bit_status_ = 0;
                }
            }
            else
            {
                std::binomial_distribution<> binom(1, p_);
                int num_neurons = areas_[i].n_neurons_;

                for (size_t j = 0; j < num_areas_; j++)
                {
                    Fiber fib_in = Fiber(j, i), fib_out = Fiber(i, j);
                    int other_size = areas_[j].is_explicit ? areas_[j].n_neurons_ : 0;

                    fib_in.outgoing_synapses_ = std::vector<std::vector<float>>(other_size, std::vector<float>(num_neurons));
                    fib_in.shape_[0] = other_size, fib_in.shape_[1] = num_neurons;

                    if (j != i)
                    {
                        fib_out.outgoing_synapses_ = std::vector<std::vector<float>>(num_neurons, std::vector<float>(other_size));
                        fib_out.shape_[0] = num_neurons, fib_out.shape_[1] = other_size;
                        for (size_t k = 0; k < num_neurons; k++)
                            for (size_t t = 0; t < other_size; t++)
                                fib_out.outgoing_synapses_[k][t] = binom(rng_);

                        fibers_[i][j] = fib_out;
                        fibers_[i][j].bit_status_ = 0;
                    }
                    else
                    {
                        for (size_t k = 0; k < num_neurons; k++)
                            for (size_t t = 0; t < num_neurons; t++)
                                fib_in.outgoing_synapses_[k][t] = binom(rng_);
                    }

                    fibers_[j][i] = fib_in;
                    fibers_[j][i].bit_status_ = 0;
                }
            }
        }
    }

    void Brain::ProjectStar(std::map<std::string, std::vector<std::string>> graph)
    {
        std::vector<std::string> targets;
        for (auto proj : graph)
        {
            targets.push_back(proj.first);
        }
        // #pragma omp parallel for num_threads(targets.size()) schedule(static)
        for (size_t i = 0; i < targets.size(); i++)
        {
            Project(targets[i], graph[targets[i]]);
        }

        for (auto proj : graph)
            getAreaByName(proj.first).update();
    }

    void Brain::ApplyAreaRule(std::string area, bool inhibit, int population)
    {
        areas_[area_by_name_[area]].applyRule(inhibit, population);
    }

    void Brain::ApplyFiberRule(std::string first_area, std::string second_area, bool inhibit, int population)
    {
        fibers_[area_by_name_[first_area]][area_by_name_[second_area]].applyRule(inhibit, population);
        fibers_[area_by_name_[second_area]][area_by_name_[first_area]].applyRule(inhibit, population);
    }

    void Brain::InhibitAllAreas(bool inhibit, int population)
    {
        for (size_t i = 0; i < num_areas_; i++)
            areas_[i].applyRule(inhibit, population);
    }

    void Brain::InhibitAllFibers(int population)
    {
        for (size_t i = 0; i < num_areas_; i++)
            for (size_t j = 0; j < num_areas_; j++)
                fibers_[i][j].applyRule(true, population);
    }

    std::map<std::string, std::vector<std::string>> Brain::GetProjectMap()
    {
        std::map<std::string, std::vector<std::string>> proj_map;

        for (size_t i = 0; i < num_areas_; i++)
        {
            if (areas_[i].isActive())
            {
                for (size_t j = 0; j < num_areas_; j++)
                {
                    if (areas_[i].is_explicit and areas_[j].is_explicit)
                        continue;
                    if (areas_[j].isActive())
                    {
                        if (fibers_[i][j].isActive() and areas_[i].winners_[0] >= 0)
                        {
                            auto it = proj_map.find(area_names_[j]);
                            if (it != proj_map.end() and
                                std::find(it->second.begin(), it->second.end(), area_names_[i]) == it->second.end())
                                it->second.push_back(area_names_[i]);
                            else if (it == proj_map.end())
                                proj_map[area_names_[j]] = {area_names_[i]};
                        }

                        if (fibers_[j][i].isActive() and areas_[j].winners_[0] >= 0)
                        {
                            auto it = proj_map.find(area_names_[j]);
                            if (it != proj_map.end() and
                                std::find(it->second.begin(), it->second.end(), area_names_[j]) == it->second.end())
                                it->second.push_back(area_names_[j]);
                            else if (it == proj_map.end())
                                proj_map[area_names_[j]] = {area_names_[j]};
                        }
                    }
                }
            }
        }

        return proj_map;
    }

    void Brain::UpdatePlasticities(const std::vector<std::string> recurrent_areas,
                                   std::string strong_area,
                                   float recurrent_beta = 0.05,
                                   float interarea_beta = 0.5,
                                   float strong_beta = 1.0)
    {
        // LEX : all areas->*strong, *->*can be strong
        // non LEX : other areas->*(?), LEX->* strong, *->* weak
        for (std::string area : recurrent_areas)
        {
            fibers_[area_by_name_[area]][area_by_name_[strong_area]].beta_ = strong_beta;
            fibers_[area_by_name_[strong_area]][area_by_name_[area]].beta_ = strong_beta;
            fibers_[area_by_name_[area]][area_by_name_[area]].beta_ = recurrent_beta;
            for (std::string other_area : recurrent_areas)
            {
                if (area == other_area)
                    continue;
                fibers_[area_by_name_[area]][area_by_name_[other_area]].beta_ = interarea_beta;
            }
        }
    }
}