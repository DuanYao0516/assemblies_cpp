# Overview

本项目为中山大学编译原理课程设计。我们使用 C++ 重新实现了 Mitropolsky D 等人在 `A biologically plausible parser` 提出的集合演算算法，并利用该算法实现自然语言的依赖解析过程。

# 代码说明

- 项目主要CPP代码位于 `my_brain/` 文件夹下，任何环境依赖与运行指引都写明于 `my_brain/README.md` 中。
- 报告文件位于 `report/` 文件夹下。
- 解析器功能测试代码位于 `my_brain/test` 文件夹下。
- 解析器流水线和性能测试位于 `my_brain/main`文件夹下。
- 如有需要，原作者代码均位于 `original_codes/` 目录下。请注意，原作者的 C++ 代码只实现了简单的 AC 框架，而我们几乎完全重构了原作者的代码，并以 AC 框架为基础实现了自然语言句法依赖解析器。

