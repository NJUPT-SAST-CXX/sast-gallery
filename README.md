<div align=center>
  <img width=64 src="res/icon/app_icon.svg">
</div>

<h1 align="center">
  SAST Gallery
</h1>
<p align="center">
  NJUPT SAST C++ 2024 WoC
</p>

<p align="center">
  <img src="https://img.shields.io/badge/frame-Qt6.8.0-blue">
  <img src="https://img.shields.io/badge/lang-C%2B%2B20-yellow.svg">
  <img src="https://img.shields.io/badge/platform-Windows%20%7C%20macOS%20%7C%20Linux-lightgreen.svg">
  <img src="https://img.shields.io/badge/license-MIT-blue.svg">
</p>

## 简介

桌面端电子相册应用，使用Qt6编写

## Build

注意：此项目使用子模块，因此请确保使用 `--recursive` 标志克隆存储库：

```bash
git clone --recursive https://github.com/NJUPT-SAST-CXX/sast-gallery.git 
```

或者在克隆后执行以下命令：


```bash
git submodule update --init --recursive
```

使用你的 IDE (QtCreator等) 来打开项目. 只支持 `CMake` 构建.

### 提交检查 Hook

此项目使用 [pre-commit](https://pre-commit.com/) 进行提交检查，以确保代码风格一致性。请先安装 pre-commit 工具：

```bash
# 对于 Arch Linux
sudo pacman -S pre-commit
# 对于 Pipx 用户（跨平台）
pipx install pre-commit
```

然后，在克隆项目后，执行以下命令安装 pre-commit 钩子：

```bash
pre-commit install
```

> [!TIP]  
> 如果您发现工具提供的结果不可靠，可以使用 `git commit --no-verify` 暂时跳过提交检查。

### 已知问题及解决方法

- **Linux平台CMake生成报错**
    
    保证目录下存在build文件夹后再执行CMake，或执行第二遍CMake

- **CMake `find_package` 报错**
    
    下载安装缺少的组件，例如 `Qt6::MultiMedia`

## 组件库

- [ElaWidgetTools](https://github.com/Liniyous/ElaWidgetTools)

## 任务和其他说明

[SAST2024.C++组 Winter of Code 企划](https://njupt-sast.feishu.cn/docx/PCundVXlYoFOeixGjDlcciRwnsh)
