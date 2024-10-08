# 基于集合演算的生物可解释性句法依赖解析器的 C++ 实现
本项目为中山大学编译原理课程设计。我们使用 C++ 重新实现了 Mitropolsky D 等人在 `A biologically plausible parser` 提出的集合演算算法，并利用该算法实现自然语言的依赖解析过程。

## 依赖
我们列出在不同操作系统下已被验证可以正常运行代码的依赖环境：
### Linux 操作系统（Ubuntu 18.04）
```
g++ - 7.5.0
cmake - 3.17.0
```

### Windows 操作系统
```
g++ (x86_64-posix-sjlj-rev0, Built by MinGW-W64 project) 8.1.0
cmake - 3.27.6
```
### 其他
1. 编译器使用的是mingw32，兼容gtest需下载使用x86_64-posix-sjlj版本(下载链接 https://sourceforge.net/projects/mingw-w64/files/Toolchains%20targetting%20Win64/Personal%20Builds/mingw-builds/8.1.0/threads-posix/sjlj/x86_64-8.1.0-release-posix-sjlj-rt_v6-rev0.7z/download)，否则会报多线程错误
2. openmp库无论是linux系统还是windows系统g++都自带了
3. gtest库源代码已经拷贝到项目中，cmake生成并链接库，可直接使用
---

## 项目构建
推荐使用 Linux 操作系统构建本项目，比较省事。
### Linux 操作系统（Ubuntu 18.04）
#### 构建解析器流水线主程序
在 `my_brain` 目录下运行以下命令：
```bash
cd main
mkdir build
cd build && cmake ..
cmake --build .
```
#### 构建解析器测试主程序
在 `my_brain` 目录下运行以下命令：
```bash
cd test
mkdir build
cd build && cmake ..
cmake --build .
```

#### 其它
当您只对项目源文件（不包括CMakeLists.txt）进行了修改并试图重新生成可执行文件时，可在 `build` 目录下执行以下命令：
```bash
make clean    # 清理中间文件
cmake --build . # 重新构建
```

### Windows 操作系统
#### 构建解析器流水线主程序
```bash
# 在my_brain目录下，提前手动创建一个文件夹build_main
cd build_main
rm -r *
cmake ../main -G "MinGW Makefiles"
# 如上述指令出现编译错误，请重新执行 rm -r *
mingw32-make
# 可执行文件会生成在build_test/bin目录下
```
#### 构建解析器测试主程序
```bash
# 在my_brain目录下
cd build_test
rm -r *
cmake ../test -G "MinGW Makefiles"
# 如上述指令出现编译错误，请重新执行 rm -r *
mingw32-make
# 可执行文件会生成在build_test/bin目录下
```
#### 其他

如果执行`cmake ../XXX -G "MinGW Makefiles"`命令时出现无法找到XXX，可以使用如下命令替换cmake命令 ：

`cmake -G "MinGW Makefiles" -DCMAKE_MAKE_PROGRAM="your/path/to/mingw32-make.exe" -DCMAKE_C_COMPILER="your/path/to/gcc.exe" -DCMAKE_CXX_COMPILER="your/path/to/g++.exe" ../XXX`

---

## 运行
Linux系统构建完成后，可以在构建目录 `build` 下运行可执行文件：
```bash
./bin/MyBrain		# 流水线主程序
./bin/MyBrainTest	# 测试主程序
```
Windows系统构建完成后，可以在构建目录 `build_main` 和`build_test`下分别运行可执行文件：

```bash
# build_main目录下，流水线主程序
cd bin
./MyBrain

# build_test目录下，测试主程序
cd bin
./MyBrainTest	
```

### 运行配置
#### 脑区配置
脑区的各项配置位于 `src` 目录下的 `constants.h` 文件中，如有必要可以按照文件中指引修改。

但我们的建议是不要修改。

#### 词典配置
**！！！务必确保词典中的单词不要超过脑区配置中的常量`kLEX_SIZE`！！！**

词典文件硬编码为 `my_brain/config/dist.csv`，如有需要请按照文件格式自行更改内容。

词典文件应紧凑编码（不允许空格或空行），每一行格式如下：
```
<单词>,<词性>
```

可选的词性（大小写敏感）：
+ DET			- 冠词
+ NOUN			- 名词
+ TRANSVERB		- 及物动词
+ INTRANSVERB	- 不及物动词
+ COPULA		- 系动词
+ ADJ			- 形容词
+ ADVERB		- 副词
+ PREP			- 介词

#### 输入配置
输入文件硬编码为 `my_brain/config/input.txt`，如有需要请按照文件格式自行更改内容。

文件的每一行应当是一个待解析的句子，允许空行。请检查句子中单词的大小写是否与词表一致。

#### 测试样例配置
词典样例文件硬编码为 `my_brain/test/testcases.txt`，如有需要请按照文件格式自行更改内容。

每行的末尾不要留有空格、句号等字符。
逗号两边不要留有空格等字符。

---

## 待办
同原论文中所述，最基础的解析器无法处理克林星闭包和迪克语言。虽然根据现有的一些研究确实有办法让它能够处理正则语言乃至上下文无关语言，但我们目前实现的却只是非常基础的依赖解析器。针对克林星闭包的情况，我们使用论文中最简单的办法实现了连续最多三个形容词的解析，而对其它句法成分的闭包解析并未实现（但我们至少已经证明了它们是完全可实现的）。如果时间和精力允许，我们会继续完善该解析器（画饼），使其在功能方面尽可能完美。

代码结构方面，我们很难称当前的设计是完善的，因为代码中的冗余和混乱是显而易见的。此外，我们没能提供用户良好的配置体验。当然，对于学术研究来说，有时代码“能跑就行”也就足够了，但我们还是希望有机会能够完善代码结构（继续画饼）。

---

### 参考文献
[1] Mitropolsky, D., Collins, M., & Papadimitriou, C.H. (2021). A Biologically Plausible Parser. Transactions of the Association for Computational Linguistics, 9, 1374-1388.