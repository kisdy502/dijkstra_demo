/**
 * C++ 模板学习 Demo —— 由浅入深
 *
 * 编译运行：
 *   g++ -std=c++17 -o template_demo src/template_demo.cpp && ./template_demo
 *
 * 和 Java 泛型的核心区别：
 *   Java：类型擦除，运行时只有一份代码（List<Object>）
 *   C++：编译期展开，每种类型生成一份独立的机器码（零运行时开销）
 */

#include <iostream>
#include <string>
#include <vector>
#include <cmath>

// ============================
// 第 1 关：函数模板（最基础）
// ============================
namespace Level1 {

// 同一个函数，适配多种类型
template<typename T>
T maxValue(T a, T b) {
    return a > b ? a : b;
}

void demo() {
    std::cout << "=== 第 1 关：函数模板 ===\n";
    std::cout << "max(3, 5)       = " << maxValue(3, 5) << "\n";       // T = int
    std::cout << "max(3.14, 2.71) = " << maxValue(3.14, 2.71) << "\n"; // T = double
    std::cout << "max('a', 'z')   = " << maxValue('a', 'z') << "\n";   // T = char
    // 编译器会生成三份独立代码：maxValue<int>、maxValue<double>、maxValue<char>
    std::cout << "\n";
}

} // namespace Level1


// ============================
// 第 2 关：类模板
// ============================
namespace Level2 {

// 相当于 Java 的 class Box<T> { private T value; }
template<typename T>
class Box {
public:
    Box(T value) : value_(value) {}

    T get() const { return value_; }
    void set(T value) { value_ = value; }

    void print() const { std::cout << "Box 里的值是: " << value_ << "\n"; }

private:
    T value_;
};

void demo() {
    std::cout << "=== 第 2 关：类模板 ===\n";

    Box<int> intBox(42);
    intBox.print();

    Box<std::string> strBox("hello world");
    strBox.print();

    Box<double> doubleBox(3.14159);
    doubleBox.print();

    std::cout << "\n";
}

} // namespace Level2


// ============================
// 第 3 关：多个模板参数
// ============================
namespace Level3 {

// 可以同时接受多个类型参数
template<typename Key, typename Value>
class Pair {
public:
    Pair(Key k, Value v) : key_(k), value_(v) {}

    Key first() const { return key_; }
    Value second() const { return value_; }

    void print() const {
        std::cout << "{" << key_ << " : " << value_ << "}\n";
    }

private:
    Key key_;
    Value value_;
};

void demo() {
    std::cout << "=== 第 3 关：多个模板参数 ===\n";

    Pair<std::string, int> age("张三", 25);
    age.print();

    Pair<int, double> coord(10, 3.14);
    coord.print();

    Pair<char, std::string> grade('A', "优秀");
    grade.print();

    std::cout << "\n";
}

} // namespace Level3


// ============================
// 第 4 关：非类型模板参数（编译期常量）
// ============================
namespace Level4 {

// Size 不是类型，而是一个编译期已知的 int 值
// 这 Java 做不到，Java 的泛型参数只能是类型
template<typename T, int Size>
class FixedArray {
public:
    FixedArray() {
        for (int i = 0; i < Size; ++i) {
            data_[i] = T{}; // 默认值初始化
        }
    }

    T& operator[](int i) { return data_[i]; }
    const T& operator[](int i) const { return data_[i]; }

    int size() const { return Size; }

    void print() const {
        std::cout << "[";
        for (int i = 0; i < Size; ++i) {
            if (i > 0) std::cout << ", ";
            std::cout << data_[i];
        }
        std::cout << "]\n";
    }

private:
    T data_[Size];  // 栈上分配，不是堆！
};

void demo() {
    std::cout << "=== 第 4 关：非类型模板参数 ===\n";

    FixedArray<int, 5> arr;     // 编译期生成 int[5] 的类
    arr[0] = 10;
    arr[1] = 20;
    arr[2] = 30;
    std::cout << "数组大小: " << arr.size() << "\n";
    arr.print();

    FixedArray<double, 3> arr2;  // 编译期生成 double[3] 的类
    arr2[0] = 1.1;
    arr2[1] = 2.2;
    arr2[2] = 3.3;
    arr2.print();

    std::cout << "\n";
}

} // namespace Level4


// ============================
// 第 5 关：模板特化（为特定类型提供不同实现）
// ============================
namespace Level5 {

// 通用模板
template<typename T>
class TypeInfo {
public:
    static void print() {
        std::cout << "未知类型\n";
    }
};

// 对 int 的特化
template<>
class TypeInfo<int> {
public:
    static void print() {
        std::cout << "这是 int 类型，占 " << sizeof(int) << " 字节\n";
    }
};

// 对 double 的特化
template<>
class TypeInfo<double> {
public:
    static void print() {
        std::cout << "这是 double 类型，占 " << sizeof(double) << " 字节\n";
    }
};

// 对 std::string 的特化
template<>
class TypeInfo<std::string> {
public:
    static void print() {
        std::cout << "这是 std::string 类型\n";
    }
};

void demo() {
    std::cout << "=== 第 5 关：模板特化 ===\n";

    TypeInfo<int>::print();
    TypeInfo<double>::print();
    TypeInfo<std::string>::print();
    TypeInfo<float>::print();        // 没特化，走通用模板
    TypeInfo<long long>::print();    // 没特化，走通用模板

    std::cout << "\n";
}

} // namespace Level5


// ============================
// 第 6 关：偏特化（部分类型参数特化）
// ============================
namespace Level6 {

// 通用模板
template<typename T, typename U>
class Matcher {
public:
    static void desc() {
        std::cout << "两个不同类型\n";
    }
};

// 偏特化：两个类型相同时
template<typename T>
class Matcher<T, T> {
public:
    static void desc() {
        std::cout << "两个相同类型\n";
    }
};

// 偏特化：第二个类型固定为 int
template<typename T>
class Matcher<T, int> {
public:
    static void desc() {
        std::cout << "第二个类型是 int\n";
    }
};

void demo() {
    std::cout << "=== 第 6 关：偏特化 ===\n";

    Matcher<int, double>::desc();       // 通用：两个不同类型
    Matcher<std::string, std::string>::desc();  // 偏特化：两个相同类型
    Matcher<double, int>::desc();     // 偏特化：第二个是 int
    Matcher<std::string, int>::desc(); // 偏特化：第二个是 int

    std::cout << "\n";
}

} // namespace Level6


// ============================
// 第 7 关：模板 + 策略模式（nav2 常用模式）
// ============================
namespace Level7 {

// 定义不同的"策略"（可以是函数对象或 lambda）
struct ManhattanCost {
    int operator()(int x, int y) const {
        return std::abs(x) + std::abs(y);
    }
};

struct EuclideanCost {
    double operator()(int x, int y) const {
        return std::sqrt(x * x + y * y);
    }
};

struct ChebyshevCost {
    int operator()(int x, int y) const {
        return std::max(std::abs(x), std::abs(y));
    }
};

// 模板类接受"策略"作为类型参数
// CostFn 可以是上面任意一个 struct，编译期多态，零虚函数开销
template<typename CostFn>
class PathPlanner {
public:
    PathPlanner(CostFn costFn) : costFn_(costFn) {}

    // 模拟计算从 (0,0) 到 (dx, dy) 的代价
    void computeCost(int dx, int dy) {
        std::cout << "  从 (0,0) 到 (" << dx << "," << dy << ") 的代价: "
                  << costFn_(dx, dy) << "\n";
    }

private:
    CostFn costFn_;
};

void demo() {
    std::cout << "=== 第 7 关：模板 + 策略模式（nav2 风格） ===\n";

    std::cout << "曼哈顿距离:\n";
    PathPlanner<ManhattanCost> planner1(ManhattanCost{});
    planner1.computeCost(3, 4);

    std::cout << "欧式距离:\n";
    PathPlanner<EuclideanCost> planner2(EuclideanCost{});
    planner2.computeCost(3, 4);

    std::cout << "切比雪夫距离:\n";
    PathPlanner<ChebyshevCost> planner3(ChebyshevCost{});
    planner3.computeCost(3, 4);

    // 也可以用 lambda（C++20 之前需要 decltype 推导）
    auto lambdaCost = [](int x, int y) { return (x + y) * 2; };
    PathPlanner<decltype(lambdaCost)> planner4(lambdaCost);
    std::cout << "自定义 lambda:\n";
    planner4.computeCost(3, 4);

    std::cout << "\n";
}

} // namespace Level7


// ============================
// 第 8 关：可变参数模板（C++11+）
// ============================
namespace Level8 {

// 递归终止：没有参数时什么都不做
void logAll() {
    std::cout << "\n";
}

// 可变参数模板：接受任意数量和类型的参数
template<typename First, typename... Rest>
void logAll(First first, Rest... rest) {
    std::cout << first;
    if (sizeof...(rest) > 0) {
        std::cout << ", ";
    }
    logAll(rest...);  // 递归展开
}

void demo() {
    std::cout << "=== 第 8 关：可变参数模板 ===\n";

    std::cout << "logAll(1, 2, 3):         ";
    logAll(1, 2, 3);

    std::cout << "logAll(\"hello\", 42, 3.14, 'x'): ";
    logAll("hello", 42, 3.14, 'x');

    std::cout << "\n";
}

} // namespace Level8


// ============================
// 第 9 关：SFINAE / enable_if（条件编译）
// ============================
namespace Level9 {

#include <type_traits>

// 仅当 T 是整数类型时，这个函数才存在
template<typename T>
typename std::enable_if<std::is_integral<T>::value, T>::type
half(T value) {
    return value / 2;
}

// 仅当 T 是浮点类型时，这个函数才存在
template<typename T>
typename std::enable_if<std::is_floating_point<T>::value, T>::type
half(T value) {
    return value / 2.0;
}

void demo() {
    std::cout << "=== 第 9 关：SFINAE / enable_if ===\n";

    std::cout << "half(10)   = " << half(10) << "     (整数版本)\n";
    std::cout << "half(10.0) = " << half(10.0) << " (浮点版本)\n";
    std::cout << "half(3.14) = " << half(3.14) << " (浮点版本)\n";

    // half("hello")  // 编译错误！string 既不是整数也不是浮点

    std::cout << "\n";
}

} // namespace Level9


// ============================
// 第 10 关：真实场景 —— 把 dijkstra 模板化
// ============================
namespace Level10 {

// 把 dijkstra 的核心数据结构模板化
// 你可以用 int 作为距离，也可以换成 double 或 int64_t

template<typename DistType>
struct Node {
    int row;
    int col;
    DistType distance;
};

// 优先队列的比较器模板
template<typename DistType>
struct Compare {
    bool operator()(const Node<DistType>& a, const Node<DistType>& b) const {
        return a.distance > b.distance;  // 小顶堆
    }
};

void demo() {
    std::cout << "=== 第 10 关：真实场景 —— dijkstra 模板化 ===\n";

    Node<int> nodeInt{0, 0, 5};
    Node<double> nodeDouble{1, 2, 3.14};
    Node<long long> nodeLL{3, 4, 10000000000LL};

    std::cout << "int 版本:    (" << nodeInt.row << "," << nodeInt.col
              << ") dist=" << nodeInt.distance << "\n";
    std::cout << "double 版本: (" << nodeDouble.row << "," << nodeDouble.col
              << ") dist=" << nodeDouble.distance << "\n";
    std::cout << "long long 版本: (" << nodeLL.row << "," << nodeLL.col
              << ") dist=" << nodeLL.distance << "\n";

    std::cout << "\n说明：你可以将距离类型从 int64_t 换成 double，\n"
              << "只要修改模板参数即可，算法逻辑完全不变。\n";
    std::cout << "\n";
}

} // namespace Level10


// ============================
// main
// ============================
int main() {
    Level1::demo();
    Level2::demo();
    Level3::demo();
    Level4::demo();
    Level5::demo();
    Level6::demo();
    Level7::demo();
    Level8::demo();
    Level9::demo();
    Level10::demo();

    std::cout << "===== 总结 =====\n\n";
    std::cout << "Java 泛型 vs C++ 模板：\n";
    std::cout << "┌──────────────┬──────────────────┬──────────────────┐\n";
    std::cout << "│     特性     │    Java 泛型      │    C++ 模板       │\n";
    std::cout << "├──────────────┼──────────────────┼──────────────────┤\n";
    std::cout << "│  实现方式    │ 类型擦除（运行时）│ 代码生成（编译期） │\n";
    std::cout << "│  基本类型    │ ❌ 需装箱 Integer │ ✅ 原生 int      │\n";
    std::cout << "│  非类型参数  │ ❌ 不支持         │ ✅ template<int N> │\n";
    std::cout << "│  特化/偏特化 │ ❌ 不支持         │ ✅ 完整支持       │\n";
    std::cout << "│  编译期计算  │ ❌ 不支持         │ ✅ 图灵完备       │\n";
    std::cout << "│  运行时开销  │ 有（类型检查等）  │ 零（完全展开）    │\n";
    std::cout << "│  代码体积    │ 一份字节码       │ 每种类型一份      │\n";
    std::cout << "└──────────────┴──────────────────┴──────────────────┘\n";

    return 0;
}
