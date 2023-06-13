# 视觉小说工具

仅供学习

# 功能

- [ ] BGI 引擎解包，目前已复现 arc 格式，图片格式 version 1 的解包
- [ ] 记录游戏时长（放松和专注两种模式）
- [ ] 转区
- [ ] 爬取[bangumi](https://bgm.tv)补充游戏信息

# 开发

## 依赖

- 构建系统：[xmake](https://xmake.io/#/zh-cn/guide/installation)
- 编译工具链：[Visual Studio](https://visualstudio.microsoft.com)（如果只构建项目而不进行开发，请下载 [Microsoft C++ 生成工具](https://visualstudio.microsoft.com/visual-cpp-build-tools)）

> Visual Studio 最好安装在默认目录

## 构建

```sh
xmake -y
```

切换构建模式和工具链

```sh
xmake f -m debug --toolchain=clang
```

# 鸣谢

- [libpng](https://github.com/glennrp/libpng)
- [toml++](https://github.com/marzer/tomlplusplus)
- [fast_io](https://github.com/cppfastio/fast_io)
- [VC-LTL5](https://github.com/Chuyu-Team/VC-LTL5)
