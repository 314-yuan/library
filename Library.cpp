// 该指令仅对 MSVC 有效：把窄字符串字面量编码指定为 UTF-8。
#ifdef _MSC_VER
#pragma execution_character_set("utf-8")
#endif

#include "Library.h"
#include "FileUtil.h" // fileExists
#include <direct.h>   // _mkdir()：Windows 下创建目录

using namespace std;

// 构造：把“数据目录 + 文件名”拼成三个 CSV 路径，初始化三个组合成员。
// 每个成员在构造时就拿到自己的文件路径，此后外部无需再传路径。
Library::Library(const string& dir)
    : dataDir(dir),
      users(dir + "/users.csv"),
      books(dir + "/books.csv"),
      records(dir + "/records.csv") {}

// 确保数据目录存在。_mkdir 在目录已存在时返回 -1，忽略即可。
void Library::ensureDataDir() {
    _mkdir(dataDir.c_str());
}

// 首次运行生成默认数据文件。
// 这里用“临时管理器”写文件，而不是直接往三个成员里加数据：因为此时成员
// 还没 load()，若直接加，之后 load() 又会从文件读到一份，造成数据翻倍。
void Library::initDefaultData() {
    ensureDataDir(); // 先保证目录存在，才能写文件

    // 默认用户
    if (!fileExists(dataDir + "/users.csv")) {
        UserManager tmp(dataDir + "/users.csv");
        tmp.addAdmin(Admin("admin", "123456", "系统管理员"));
        tmp.addReader(Reader("2023001", "123456", "张三", "男", "13800000000", 5));
        tmp.addReader(Reader("2023002", "123456", "李四", "女", "13900000000", 5));
        tmp.save();
    }

    // 默认图书资源：一般图书 + 杂志 + 电子资源（体现继承派生与多态）
    if (!fileExists(dataDir + "/books.csv")) {
        BookManager tmp(dataDir + "/books.csv");
        // 一般图书
        tmp.addBook(new GeneralBook(1001, "C++ Primer", "人民邮电出版社", 10, 10,
                                    "Stanley B. Lippman", "9787115333615", "编程"));
        tmp.addBook(new GeneralBook(1002, "算法导论", "机械工业出版社", 8, 8,
                                    "Thomas H. Cormen", "9787111407010", "计算机"));
        tmp.addBook(new GeneralBook(1003, "三体", "重庆出版社", 15, 15,
                                    "刘慈欣", "9787536692930", "科幻"));
        tmp.addBook(new GeneralBook(1004, "红楼梦", "人民文学出版社", 5, 5,
                                    "曹雪芹", "9787020002207", "文学"));
        // 杂志
        tmp.addBook(new Magazine(2001, "程序员", "CSDN", 20, 20, "2026-10", "1672-5945"));
        tmp.addBook(new Magazine(2002, "读者", "读者出版集团", 30, 30, "2026-09", "1005-1803"));
        // 电子资源
        tmp.addBook(new EResource(3001, "C++标准库(电子版)", "电子工业出版社",
                                  "PDF", "http://lib.example.com/3001"));
        tmp.addBook(new EResource(3002, "深度学习综述", "中国知网",
                                  "CAJ", "http://cnki.net/3002"));
        tmp.save();
    }

    // 借阅记录文件首次运行为空，无需预生成（BorrowManager::load 读不到文件返回空）。
}

// 加载全部数据：依次调用三个组合成员的 load()
void Library::load() {
    users.load();
    books.load();
    records.load();
}

// 保存全部数据：依次调用三个组合成员的 save()
void Library::save() const {
    users.save();
    books.save();
    records.save();
}
