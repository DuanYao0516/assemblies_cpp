#include "pipeline.h"
#include "constants.h"

#include <sstream>

int main()
{
    const float p = 0.1;
    const std::string input_file_path = "../../config/input.txt";
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

    // 用上述解析器实例化一个 pipeline，及 pipeline 使用这个 parser 进行解析
    Pipeline pipeline(parser_brain);

    // 依赖解析，读出曾激活过的纤维
    pipeline.FileDepParse(input_file_path, p, LEX_k, 20, non_LEX_n, non_LEX_k, FIBER_READOUT);

    return 0;
}
