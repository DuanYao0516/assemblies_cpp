2024/06/04

- 实现pipeline处理文件
- 实现测试代码

---

2024/06/03

- 重新设计文件，目前 my_brain/ 中，使用 `make` + `make run` 进行编译运行无 pipeline 版本代码。
- 重新设计 readout ,交给 parser 实现
- 实现 pipeline 处理单个句子


另外注意： 硬编码的 kLEXEME_DICT 因为依赖于 PosAndIndex，暂时放在 rules.h 文件中，如果有需要可以提出来单独一个文件。

---

2024/05/30

实现CPP版本位于 my_brain/ 
