#include <random>
#include <chrono>

int random_number(int min,int max) {
    // 1. 种子 (Seeding)
    // 使用当前时间作为种子，以确保每次程序运行时得到不同的随机数序列。
    // std::chrono::system_clock::now().time_since_epoch().count() 是一个常用的高质量种子。
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();

    // 2. 随机数引擎 (Engine)
    // 这是一个实际产生伪随机比特流的工具。std::mt19937 是一个高质量的梅森旋转算法引擎。
    std::mt19937 generator(seed);

    // 3. 分布 (Distribution)
    // 告诉引擎如何将产生的比特流映射到我们需要的范围内。
    // 这里的范围是 [1, 10]，包含 1 和 10。
    std::uniform_int_distribution<int> distribution(min, max);

    // 4. 生成随机数
    int random_number = distribution(generator);

    return random_number;
}