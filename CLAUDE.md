# 图书馆借阅管理系统（C++ 面向对象课程实验）

## 项目概述

命令行界面的图书馆借阅管理系统，作为 C++ 面向对象程序设计课程实验项目。

## 技术栈

- 语言：纯 C++11（Visual Studio / MSVC 编译）
- 界面：命令行交互（`std::cin` / `std::cout` 循环菜单）
- 数据存储：C++ 标准库 `fstream` 读写 CSV 文本文件（不使用数据库）
- 文件结构：多文件（`.h` 头文件与 `.cpp` 源文件分离）

## 核心约束（实验指导书要求）

1. 必须使用 **类与对象**、**vector 容器**、**继承与多态（基类指针管理对象）**。
2. 功能模块：图书管理、读者管理、借阅/归还管理；区分**管理员**与**普通读者**登录权限。
3. 代码需包含**详细中文注释**，便于撰写实验报告。

## 设计要点

- 抽象基类 `User` → 派生类 `Admin`（管理员）、`Reader`（读者）；用 `vector<User*>` 统一管理两类对象，通过虚函数（`getRole()`、`showMenu()`）实现运行时多态。
- 数据文件：`data/users.csv`、`data/books.csv`、`data/records.csv`，UTF-8 编码。

## 中文编码约定（Windows）

- 源码保存为 **UTF-8（带 BOM）**，每个 `.cpp` 文件顶部加 `#pragma execution_character_set("utf-8")`。
- 程序启动调用 `system("chcp 65001 >nul")` 把控制台代码页设为 UTF-8，避免中文乱码。

## 实验进度

- 实验一（已完成）：User 基类 + Admin/Reader 派生类 + 登录菜单框架（`main.cpp`）。
- 实验二（已完成）：图书管理（Book/BookManager）+ CSV 文件读写（FileUtil），图书与用户数据持久化到 `data/*.csv`。
- 实验三（待做）：借阅/归还管理（BorrowRecord/BorrowManager）+ 逾期罚款。

## 默认账号（演示用）

- 管理员：`admin` / `123456`
- 读者：`2023001` / `123456`（张三）、`2023002` / `123456`（李四）
