#include "parser.h"
#include "pipeline.h"
#include "constants.h"
#include "rules.h"

#include <sstream>

#include <gtest/gtest.h>
#include <vector>
#include <string>
#include <unordered_set>


// 结果预处理：
// 预处理1：依赖三元组拼接成字符串
void dependencies_tostring(const std::vector<std::vector<std::string>>& dependencies,
    std::vector<std::string>& concatenatedStrings) 
{
    for (const auto& subvector : dependencies) {
        std::ostringstream oss;
        for (size_t i = 0; i < subvector.size(); ++i) {
            oss << subvector[i];
            if (i < subvector.size() - 1) { // 避免在最后一个元素后添加空格
                oss << ' ';
            }
        }
        concatenatedStrings.push_back(oss.str());
    }
}

// 预处理2：将含有<not a word>的三元组去掉
void removeNotAWord(std::vector<std::vector<std::string>>& vecs) {
    std::string target = "<Not A Word>";
    vecs.erase(
        std::remove_if(vecs.begin(), vecs.end(),
                       [&target](const std::vector<std::string>& vec) {
                           return std::find(vec.begin(), vec.end(), target) != vec.end();
                       }),
        vecs.end()
    );
}


// gtest测试
// 一条样例类
class TestCase {
public:
    std::string sentence;
    std::vector<std::string> expectedDependencies;

    TestCase(const std::string& sent, const std::vector<std::string>& deps)
        : sentence(sent), expectedDependencies(deps) {}

    // 重载 operator<< 以便于输出
    friend std::ostream& operator<<(std::ostream& os, const TestCase& testCase) {
        os << "\nSentence: \"" << testCase.sentence << "\"\n";
        return os;
    }
};

// 读取文件中的测试用例
std::vector<TestCase> readTestCases(const std::string& filename) {
    std::vector<TestCase> testCases;
    std::ifstream file(filename);

    if (!file.is_open()) {
        std::cerr << "can not open file." << std::endl;
        return testCases;
    }

    std::string sentence, expected;
    while (getline(file, sentence) && getline(file, expected)) {
        std::vector<std::string> dependencies;
        std::stringstream ss(expected);
        std::string dependency;

        while (getline(ss, dependency, ',')) {
            dependencies.push_back(dependency);
        }

        testCases.emplace_back(sentence, dependencies);
    }

    file.close();
    return testCases;
}

// 参数化测试类
class DependencyGenerationParamTest : public testing::TestWithParam<TestCase> {
protected:
    static Pipeline* pipeline;

    static void SetUpTestSuite() {
        const float p = 0.1;
        const int LEX_k = 20, non_LEX_n = 10000, non_LEX_k = 100;
        const std::string kLEXEME_DICT_PATH = "../../config/dict.csv";

        // 实例化一个解析器，采用读文件的方式初始化内部词表
        BioParser::Parser parser_brain = BioParser::Parser(p, kLEXEME_DICT_PATH, kAREAS, {kLEX, kSUBJ, kVERB}, kRECURRENT_AREAS, kENGLISH_READOUT_RULES);

        parser_brain.AddExplicitArea(kLEX, LEX_k * kLEX_SIZE, LEX_k, kDEFAULT_BETA);

        parser_brain.AddArea(kSUBJ, non_LEX_n, non_LEX_k, kDEFAULT_BETA);
        parser_brain.AddArea(kVERB, non_LEX_n, non_LEX_k, kDEFAULT_BETA);
        parser_brain.AddArea(kOBJ, non_LEX_n, non_LEX_k, kDEFAULT_BETA);
        parser_brain.AddArea(kADJ, non_LEX_n, non_LEX_k, kDEFAULT_BETA);
        parser_brain.AddArea(kADJ1, non_LEX_n, non_LEX_k, kDEFAULT_BETA);
        parser_brain.AddArea(kADJ2, non_LEX_n, non_LEX_k, kDEFAULT_BETA);
        parser_brain.AddArea(kADVERB, non_LEX_n, non_LEX_k, kDEFAULT_BETA);
        parser_brain.AddArea(kPREP, non_LEX_n, non_LEX_k, kDEFAULT_BETA);
        parser_brain.AddArea(kPREP_P, non_LEX_n, non_LEX_k, kDEFAULT_BETA);
        parser_brain.AddArea(kDET, non_LEX_n, LEX_k, kDEFAULT_BETA);

        parser_brain.Initiate();
        parser_brain.UpdatePlasticities(kRECURRENT_AREAS, kLEX, kRECURRENT_BETA, kINTERAREA_BETA, kLEX_BETA);

        // 初始化 pipeline
        pipeline = new Pipeline(parser_brain);
    }

    static void TearDownTestSuite() {
        delete pipeline;
        pipeline = nullptr;
    }
};

// 静态成员变量的定义
Pipeline* DependencyGenerationParamTest::pipeline = nullptr;

// 比较生成的依赖关系和期望的依赖关系
bool compareDependencies(const std::vector<std::string>& generated, const std::vector<std::string>& expected) {
    std::unordered_set<std::string> generatedSet(generated.begin(), generated.end());
    std::unordered_set<std::string> expectedSet(expected.begin(), expected.end());

    return generatedSet == expectedSet;
}


// 参数化测试的实现
TEST_P(DependencyGenerationParamTest, DependencyGeneration) {
    const auto& testCase = GetParam();
    const float p = 0.1;
    const int LEX_k = 20, non_LEX_n = 10000, non_LEX_k = 100;
    std::vector<std::string> words = pipeline->Tokenize(testCase.sentence);
    std::vector<std::vector<std::string>> result = pipeline->DepParse(words, p, LEX_k, 20, non_LEX_n, non_LEX_k, FIBER_READOUT, false);
    removeNotAWord(result);
    std::vector<std::string> generatedDeps;
    dependencies_tostring(result, generatedDeps);
    EXPECT_TRUE(compareDependencies(generatedDeps, testCase.expectedDependencies));
}

// 注册参数化测试案例
INSTANTIATE_TEST_SUITE_P(
    DependencyTests,
    DependencyGenerationParamTest,
    testing::ValuesIn(readTestCases("../../test/test_cases.txt"))
);

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    if(RUN_ALL_TESTS()){}
    return 0;
}
