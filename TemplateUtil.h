#ifndef TEMPLATEUTIL_H
#define TEMPLATEUTIL_H

#include <vector>

// ============================================================
// 实验五：模板与 vector —— 通用模板函数（查找 / 遍历）
//
// 下面三个“函数模板”只写一次，就能被不同类型复用：
//   findPtr   : 在 vector<T*>（基类指针容器）中查找，返回 T*
//   findValue : 在 vector<T> （值容器）中查找，返回指向元素的指针
//   forEach   : 遍历 vector<T>，对每个元素执行指定操作
//
// 标准库其实提供了等价的 std::find_if / std::for_each，这里自己实现
// 一遍，是为了演示“模板”语法以及“一次编写、多处复用”的思想。
// ============================================================

// 在 vector<T*> 中查找第一个满足条件 pred 的元素，返回该指针；找不到返回 nullptr。
// 用于 vector<BookResource*>、vector<User*> 这类基类指针容器。
// pred 是“谓词”：函数指针 / lambda / 函数对象，形如 (T*) -> bool。
template <typename T, typename Predicate>
T* findPtr(const std::vector<T*>& v, Predicate pred) {
    for (T* p : v) {           // 范围 for 遍历指针容器
        if (pred(p)) return p; // 命中即返回
    }
    return nullptr;
}

// 在 vector<T>（值容器）中查找第一个满足条件 pred 的元素，返回指向它的指针；
// 找不到返回 nullptr。用于 vector<BorrowRecord> 这类值容器。
template <typename T, typename Predicate>
T* findValue(std::vector<T>& v, Predicate pred) {
    for (T& x : v) {           // 引用遍历，才能返回容器内元素的地址
        if (pred(x)) return &x;
    }
    return nullptr;
}

// 遍历 vector<T>，对每个元素执行 func（函数指针 / lambda / 函数对象）。
template <typename T, typename Func>
void forEach(const std::vector<T>& v, Func func) {
    for (const T& x : v) {
        func(x);
    }
}

#endif
