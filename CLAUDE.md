# 图书馆借阅管理系统（C++ 面向对象课程实验）

## 项目概述

命令行界面的图书馆借阅管理系统，作为 C++ 面向对象程序设计课程实验项目。

## 技术栈

- 语言：纯 C++11（Dev-C++ 5.11 / MinGW GCC 4.9.2 编译）
- 界面：命令行交互（`std::cin` / `std::cout` 循环菜单）
- 数据存储：C++ 标准库 `fstream` 读写 CSV 文本文件（不使用数据库）
- 文件结构：多文件（`.h` 头文件与 `.cpp` 源文件分离）

## 核心约束（实验指导书要求）

1. 必须使用 **类与对象**、**vector 容器**、**继承与多态（基类指针管理对象）**。
2. 功能模块：图书管理、读者管理、借阅/归还管理；区分**管理员**与**普通读者**登录权限。
3. 代码需包含**详细中文注释**，便于撰写实验报告。

## 设计要点

- 用户维度：抽象基类 `User` → 派生类 `Admin`（管理员）、`Reader`（读者）；用 `vector<User*>` 统一管理两类对象，通过虚函数（`getRole()`、`showMenu()`）实现运行时多态。
- 图书资源维度（实验三起）：抽象基类 `BookResource` → 派生 `GeneralBook`（一般图书）、`Magazine`（杂志）、`EResource`（电子资源）；不同资源有不同属性和 `showInfo()` 显示。
- 图书馆组合类 `Library`（由 BookManager 演进）用 `vector<BookResource*>` 统一管理资源，基类指针调用虚函数实现多态（实验四）。
- 数据文件：`data/users.csv`、`data/books.csv`、`data/records.csv`，UTF-8 编码。

## 中文编码约定（Windows）

- 源码保存为 **UTF-8（带 BOM）**，每个 `.cpp` 文件顶部加 `#pragma execution_character_set("utf-8")`。
- 程序启动调用 `system("chcp 65001 >nul")` 把控制台代码页设为 UTF-8，避免中文乱码。

## 实验进度（对齐实验指导书：①类与对象 ②组合/依赖 ③继承与派生 ④多态与虚函数 ⑤模板与vector ⑥输入输出流）

- 实验一（已完成）：类与对象 → User 基类 + Admin/Reader 派生类 + 登录菜单框架（`main.cpp`）。
- 实验二（已完成）：组合/依赖 → 图书管理（Book/BookManager 组合）+ CSV 文件读写（FileUtil），图书与用户数据持久化到 `data/*.csv`。
- 实验三（已完成）：继承与派生 → 图书资源继承层级（`BookResource` → `GeneralBook`/`Magazine`/`EResource`）+ 借阅/归还/续借/逾期罚款（`BorrowManager`）。
- 实验四（待做）：多态与虚函数 → `BookResource` 声明虚函数（`showInfo()`/`toCSV()`），`Library` 组合类用 `vector<BookResource*>` 统一管理，基类指针调用虚函数 + 虚析构函数。
- 实验五（待做）：模板与 vector → `vector<BookResource*>` 增删改查（push_back/erase/size/遍历），借阅记录 `vector<BorrowRecord>`，通用模板函数（查找/遍历）。
- 实验六（待做）：输入输出流 → fstream 对象级持久化（多态资源存盘，不同类型字段不同），关闭时保存、重启时加载。
- 项目答辩（设计报告2）：整合完整系统 + 用户管理（登录/注册）+ 学生/教师信息管理。

## 默认账号（演示用）

- 管理员：`admin` / `123456`
- 读者：`2023001` / `123456`（张三）、`2023002` / `123456`（李四）
