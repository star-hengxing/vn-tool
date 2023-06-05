# 视觉小说工具

仅供学习

# 功能

- [ ] 提取 BGI 引擎的 arc 图片包
- [ ] 记录游戏时长（放松和专注两种模式）
- [ ] 内置转区
- [ ] 爬取[bangumi](https://bgm.tv)补充游戏信息

# 开发

## 依赖

- 构建系统：[xmake](https://xmake.io/#/zh-cn/guide/installation)
- 编译工具链：[Visual Studio](https://visualstudio.microsoft.com/)（不需要 vs 则下载[Microsoft C++ 生成工具](https://visualstudio.microsoft.com/visual-cpp-build-tools/)）

> 编译工具链最好安装在默认目录

## 构建

```sh
xmake --yes
```

切换构建模式和工具链

```sh
xmake config --mode=debug --toolchain=clang
```

# 依赖的第三方库

- [libpng](https://github.com/glennrp/libpng)
- [fast_io](https://github.com/cppfastio/fast_io)
- [VC-LTL5](https://github.com/Chuyu-Team/VC-LTL5)
