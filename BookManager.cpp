// 该指令仅对 MSVC 有效：把窄字符串字面量编码指定为 UTF-8。
#ifdef _MSC_VER
#pragma execution_character_set("utf-8")
#endif

#include "BookManager.h"
#include "FileUtil.h"
#include "TemplateUtil.h" // 实验五：通用模板函数 findPtr / forEach
#include <iostream>
#include <iomanip>

using namespace std;

BookManager::BookManager(const string& filePath) : path(filePath) {}

BookManager::~BookManager() {
    // 逐个释放 new 出来的资源对象；虚析构保证派生类被正确销毁
    for (auto b : books) {
        delete b;
    }
}

// 从 CSV 加载。每行第一列是类型，后面字段按类型不同，工厂创建对应派生类。
void BookManager::load() {
    for (const auto& row : readCSV(path)) {
        BookResource* r = BookResource::createFromCSV(row);
        if (r) books.push_back(r);
    }
}

// 写回 CSV：多态调用 toCSV，不同类型生成不同列
void BookManager::save() const {
    vector<vector<string>> rows;
    for (const auto& b : books) {
        rows.push_back(b->toCSV());
    }
    writeCSV(path, rows);
}

// 列出全部资源：表头带“类型”列，showInfo 是多态调用，不同类型显示不同字段
void BookManager::listBooks() const {
    cout << "\n---------------- 图书资源列表 ----------------\n";
    cout << left
         << setw(6)  << "编号"
         << setw(10) << "类型"
         << setw(20) << "题名"
         << setw(14) << "作者/期号/格式"
         << setw(12) << "分类/ISSN/来源"
         << setw(8)  << "可借/总\n";
    // forEach 模板遍历：对每个资源指针调用 showInfo（虚函数，多态显示）
    forEach(books, [](BookResource* b) { b->showInfo(); });
    cout << "共 " << books.size() << " 条资源。\n"; // size()：容器元素个数
    cout << "----------------------------------------------\n";
}

// 按关键字搜索：在编号/题名/出版社/类型名里做“包含”匹配
void BookManager::searchBooks(const string& keyword) const {
    cout << "\n---------------- 搜索结果 ----------------\n";
    bool found = false;
    for (const auto& b : books) {
        if (to_string(b->getId()).find(keyword) != string::npos ||
            b->getTitle().find(keyword) != string::npos ||
            b->getPublisher().find(keyword) != string::npos ||
            b->getType().find(keyword) != string::npos) { // getType 也是虚函数
            b->showInfo();
            found = true;
        }
    }
    if (!found) cout << "没有找到匹配的资源。\n";
    cout << "------------------------------------------\n";
}

// 按编号查找，返回基类指针；找不到返回 nullptr。
// 用模板函数 findPtr 替换手写循环：lambda 作为谓词判断编号是否匹配。
BookResource* BookManager::findBook(int id) {
    return findPtr(books, [id](BookResource* b) { return b->getId() == id; });
}

// 下一个可用编号 = 当前最大编号 + 1（初始从 1001 开始）
int BookManager::getNextId() const {
    int maxId = 1000;
    for (const auto& b : books) {
        if (b->getId() > maxId) maxId = b->getId();
    }
    return maxId + 1;
}

// 交互式新增：先选资源类型，再输入该类型的专属字段
void BookManager::addBook() {
    cout << "\n---------- 新增图书资源 ----------\n";
    cout << "  1. 一般图书   2. 杂志   3. 电子资源\n";
    int type = readInt("请选择资源类型: ");
    if (type < 1 || type > 3) {
        cout << "无效的类型选择。\n";
        return;
    }

    int id = getNextId();
    string title = readLine("题名: ");
    string publisher = readLine("出版社/来源: ");

    BookResource* r = nullptr;
    if (type == 1) {
        int total = readInt("总数量: "); // 纸质资源才需要副本数量
        string author = readLine("作者: ");
        string isbn = readLine("ISBN: ");
        string category = readLine("分类: ");
        r = new GeneralBook(id, title, publisher, total, total, author, isbn, category);
    } else if (type == 2) {
        int total = readInt("总数量: ");
        string issue = readLine("期号: ");
        string issn = readLine("ISSN: ");
        r = new Magazine(id, title, publisher, total, total, issue, issn);
    } else {
        // 电子资源无副本数量，不询问总数量，直接询问专属字段
        string format = readLine("文件格式(PDF/EPUB等): ");
        string url = readLine("下载地址/数据库: ");
        r = new EResource(id, title, publisher, format, url);
    }
    // 新增时纸质资源都可借，故可借数量 = 总数量（电子资源无此概念）
    books.push_back(r);
    save();
    cout << "新增成功，资源编号为 " << id << "。\n";
}

// 直接添加（默认数据初始化用），接管传入指针的所有权
void BookManager::addBook(BookResource* b) {
    books.push_back(b);
}

// 删除指定编号的资源
void BookManager::deleteBook(int id) {
    for (auto it = books.begin(); it != books.end(); ++it) {
        if ((*it)->getId() == id) {
            delete *it;       // 先释放堆上的对象
            books.erase(it);  // 再从容器移除
            save();
            cout << "删除成功。\n";
            return;
        }
    }
    cout << "未找到编号为 " << id << " 的资源。\n";
}

// 修改：公共字段 + 按类型修改专属字段（用 dynamic_cast 判断实际类型）
void BookManager::updateBook(int id) {
    BookResource* b = findBook(id);
    if (!b) {
        cout << "未找到编号为 " << id << " 的资源。\n";
        return;
    }

    cout << "\n---------- 修改资源（直接回车表示不修改）----------\n";
    string t = readLine("新题名(" + b->getTitle() + "): ");
    if (!t.empty()) b->setTitle(t);

    string p = readLine("新出版社/来源(" + b->getPublisher() + "): ");
    if (!p.empty()) b->setPublisher(p);

    // 电子资源无副本数量，跳过“总数量”修改；只有纸质资源才处理
    if (dynamic_cast<EResource*>(b) == nullptr) {
        string totalStr = readLine("新总数量(" + to_string(b->getTotal()) + "): ");
        if (!totalStr.empty()) {
            int newTotal = toInt(totalStr, b->getTotal());
            int diff = newTotal - b->getTotal();
            b->setTotal(newTotal);
            b->setAvailable(b->getAvailable() + diff); // 可借数量随总数量同步调整
            if (b->getAvailable() < 0) b->setAvailable(0);
        }
    }

    // 不同类型有不同的专属字段，用 dynamic_cast 判断实际类型后分别处理
    if (GeneralBook* g = dynamic_cast<GeneralBook*>(b)) {
        string a = readLine("新作者(" + g->getAuthor() + "): ");
        if (!a.empty()) g->setAuthor(a);
        string i = readLine("新ISBN(" + g->getIsbn() + "): ");
        if (!i.empty()) g->setIsbn(i);
        string c = readLine("新分类(" + g->getCategory() + "): ");
        if (!c.empty()) g->setCategory(c);
    } else if (Magazine* m = dynamic_cast<Magazine*>(b)) {
        string issue = readLine("新期号(" + m->getIssue() + "): ");
        if (!issue.empty()) m->setIssue(issue);
        string issn = readLine("新ISSN(" + m->getIssn() + "): ");
        if (!issn.empty()) m->setIssn(issn);
    } else if (EResource* e = dynamic_cast<EResource*>(b)) {
        string format = readLine("新格式(" + e->getFormat() + "): ");
        if (!format.empty()) e->setFormat(format);
        string url = readLine("新下载地址(" + e->getUrl() + "): ");
        if (!url.empty()) e->setUrl(url);
    }
    save();
    cout << "修改成功。\n";
}
