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
- [Qt6](https://www.qt.io/product/qt6)

> Visual Studio 最好安装在默认目录。

推荐使用 [scoop](https://scoop.sh) 作为包管理。

```sh
scoop install xmake aqt
```

推荐使用 [aqtinstall](https://github.com/miurahr/aqtinstall) 下载 qt。

```sh
git clone --depth 1 https://github.com/star-hengxing/vn-tool.git
cd vn-tool
mkdir Qt
aqt install-qt windows desktop 6.6.0 win64_msvc2019_64 --outputdir Qt
```

## 构建

如果系统已经有 qt，修改参数`--qt=path/to/qt`。

```sh
xmake f -y --qt=Qt
xmake
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
- [Qt](https://www.qt.io)
