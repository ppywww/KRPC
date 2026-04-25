// // #include "Krpcapplication.h"
// // #include "../user.pb.h"
// // #include "Krpccontroller.h"
// // #include <iostream>
// // #include <atomic>
// // #include <thread>
// // #include <chrono>
// // #include "KrpcLogger.h"

// // // 发送 RPC 请求的函数，模拟客户端调用远程服务
// // void send_request(int thread_id, std::atomic<int> &success_count, std::atomic<int> &fail_count,int requests_per_thread) {
// //     // 创建一个 UserServiceRpc_Stub 对象，用于调用远程的 RPC 方法
// //     Kuser::UserServiceRpc_Stub stub(new KrpcChannel(false));

// //     // 设置 RPC 方法的请求参数
// //     Kuser::LoginRequest request;
// //     request.set_name("zhangsan");  // 设置用户名
// //     request.set_pwd("123456");    // 设置密码

// //     // 定义 RPC 方法的响应参数
// //     Kuser::LoginResponse response;
// //     Krpccontroller controller;  // 创建控制器对象，用于处理 RPC 调用过程中的错误
// //     for (int i = 0; i < requests_per_thread; ++i) {
// //         // 调用远程的 Login 方法
// //         stub.Login(&controller, &request, &response, nullptr);

// //         // 检查 RPC 调用是否成功
// //         if (controller.Failed()) {  // 如果调用失败
// //             std::cout << controller.ErrorText() << std::endl;  // 打印错误信息
// //             fail_count++;  // 失败计数加 1
// //         } else {  // 如果调用成功
// //             if (int{} == response.result().errcode()) {  // 检查响应中的错误码
// //                 std::cout << "rpc login response success:" << response.success() << std::endl;  // 打印成功信息
// //                 success_count++;  // 成功计数加 1
// //             } else {  // 如果响应中有错误
// //                 std::cout << "rpc login response error : " << response.result().errmsg() << std::endl;  // 打印错误信息
// //                 fail_count++;  // 失败计数加 1
// //             }
// //         }
// //     }
// // }

// // int main(int argc, char **argv) {
// //     // 初始化 RPC 框架，解析命令行参数并加载配置文件
// //     KrpcApplication::Init(argc, argv);

// //     // 创建日志对象
// //     KrpcLogger logger("MyRPC");

// // const int thread_count = 1;      // 线程数改为 100
// // const int requests_per_thread = 1; // 每个线程发 5000 次请求

// //     std::vector<std::thread> threads;  // 存储线程对象的容器
// //     std::atomic<int> success_count(0); // 成功请求的计数器
// //     std::atomic<int> fail_count(0);    // 失败请求的计数器

// //     auto start_time = std::chrono::high_resolution_clock::now();  // 记录测试开始时间

// //     // 启动多线程进行并发测试
// //     for (int i = 0; i < thread_count; i++) {
// //         threads.emplace_back([argc, argv, i, &success_count, &fail_count, requests_per_thread]() {  
// //                 send_request(i, success_count, fail_count,requests_per_thread);  // 每个线程发送指定数量的请求
// //         });
// //     }

// //     // 等待所有线程执行完毕
// //     for (auto &t : threads) {
// //         t.join();
// //     }

// //     auto end_time = std::chrono::high_resolution_clock::now();  // 记录测试结束时间
// //     std::chrono::duration<double> elapsed = end_time - start_time;  // 计算测试耗时

// //     // 输出统计结果
// //     LOG(INFO) << "Total requests: " << thread_count * requests_per_thread;  // 总请求数
// //     LOG(INFO) << "Success count: " << success_count;  // 成功请求数
// //     LOG(INFO) << "Fail count: " << fail_count;  // 失败请求数
// //     LOG(INFO) << "Elapsed time: " << elapsed.count() << " seconds";  // 测试耗时
// //     LOG(INFO) << "QPS: " << (thread_count * requests_per_thread) / elapsed.count();  // 计算 QPS（每秒请求数）

// //     return 0;
// // }


// #include "Krpcapplication.h"
// #include "../user.pb.h"
// #include "Krpccontroller.h"
// #include <iostream>
// #include <atomic>
// #include <thread>
// #include <chrono>
// #include "KrpcLogger.h"

// void send_request(int thread_id, std::atomic<int> &success_count, std::atomic<int> &fail_count, int requests_per_thread) {
    
//     Kuser::UserServiceRpc_Stub stub(new KrpcChannel(false));

//     Kuser::LoginRequest request;
//     request.set_name("zhangsan");
//     request.set_pwd("123456");

//     Kuser::LoginResponse response;
//     Krpccontroller controller;
    
//     for (int i = 0; i < requests_per_thread; ++i) {
//         stub.Login(&controller, &request, &response, nullptr);

//         if (controller.Failed()) {
//             KRPC_LOG_ERROR("调用失败: " << controller.ErrorText());
//             fail_count++;
//         } else {
//             if (int{} == response.result().errcode()) {
//                 KRPC_LOG_INFO("调用成功: success=" << response.success());
//                 success_count++;
//             } else {
//                 KRPC_LOG_WARNING("业务错误: " << response.result().errmsg());
//                 fail_count++;
//             }
//         }
//     }
// }

// int main(int argc, char **argv) {
    
//     // 只需这一步！框架自动初始化日志！
//     KrpcApplication::Init(argc, argv);
    
//     const int thread_count = 1;
//     const int requests_per_thread = 1;

//     std::vector<std::thread> threads;
//     std::atomic<int> success_count(0);
//     std::atomic<int> fail_count(0);

//     auto start_time = std::chrono::high_resolution_clock::now();

//     for (int i = 0; i < thread_count; i++) {
//         threads.emplace_back([argc, argv, i, &success_count, &fail_count, requests_per_thread]() {  
//             send_request(i, success_count, fail_count, requests_per_thread);
//         });
//     }

//     for (auto &t : threads) {
//         t.join();
//     }

//     auto end_time = std::chrono::high_resolution_clock::now();
//     std::chrono::duration<double> elapsed = end_time - start_time;

//     KRPC_LOG_INFO("===== 统计结果 =====");
//     KRPC_LOG_INFO("总请求: " << thread_count * requests_per_thread);
//     KRPC_LOG_INFO("成功: " << success_count);
//     KRPC_LOG_INFO("失败: " << fail_count);
//     KRPC_LOG_INFO("耗时: " << elapsed.count() << " 秒");
//     KRPC_LOG_INFO("QPS: " << (thread_count * requests_per_thread) / elapsed.count());

//     return 0;
// }

#include "Krpcapplication.h"
#include "../user.pb.h"
#include "Krpccontroller.h"
#include <iostream>
#include <atomic>
#include <thread>
#include <chrono>
#include <vector>
#include <random>
#include <iomanip>
#include "KrpcLogger.h"

// ============================================
// 测试配置
// ============================================
struct TestConfig {
    int thread_count = 16;           // 并发线程数
    int requests_per_thread = 500;    // 每个线程发送的请求数
    bool test_register = false;     // 是否测试注册功能
    bool test_login = true;         // 是否测试登录功能
    bool verbose = true;            // 是否打印详细日志
    int think_time_ms = 0;          // 请求之间的思考时间（毫秒）
};

// ============================================
// 测试统计数据
// ============================================
struct TestStats {
    std::atomic<int> success_count{0};
    std::atomic<int> fail_count{0};
    std::atomic<int> timeout_count{0};
    
    std::atomic<long long> total_latency_us{0};
    std::atomic<long long> min_latency_us{LLONG_MAX};
    std::atomic<long long> max_latency_us{0};
    
    void RecordLatency(long long latency_us) {
        total_latency_us += latency_us;
        
        // 更新最小值
        long long curr_min = min_latency_us.load();
        while (latency_us < curr_min && 
               !min_latency_us.compare_exchange_weak(curr_min, latency_us)) {
        }
        
        // 更新最大值
        long long curr_max = max_latency_us.load();
        while (latency_us > curr_max && 
               !max_latency_us.compare_exchange_weak(curr_max, latency_us)) {
        }
    }
};

// ============================================
// 单个测试任务
// ============================================
void TestLoginTask(int thread_id, TestStats& stats, const TestConfig& config) {
    Kuser::UserServiceRpc_Stub stub(new KrpcChannel(false));
    
    std::vector<std::string> names = {"zhangsan", "lisi", "wangwu", "zhaoliu", "qianqi"};
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> name_dist(0, names.size() - 1);
    
    for (int i = 0; i < config.requests_per_thread; ++i) {
        auto start = std::chrono::high_resolution_clock::now();
        
        Kuser::LoginRequest request;
        int name_idx = name_dist(gen);
        request.set_name(names[name_idx]);
        request.set_pwd("123456");
        
        Kuser::LoginResponse response;
        Krpccontroller controller;
        
        stub.Login(&controller, &request, &response, nullptr);
        
        auto end = std::chrono::high_resolution_clock::now();
        auto latency = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        
        stats.RecordLatency(latency.count());
        
        if (controller.Failed()) {
            if (config.verbose) {
                KRPC_LOG_ERROR("[Thread-" << thread_id << "] 调用失败: " << controller.ErrorText());
            }
            stats.fail_count++;
        } else {
            if (response.result().errcode() == 0) {
                if (config.verbose) {
                    KRPC_LOG_INFO("[Thread-" << thread_id << "] 登录成功: user=" << names[name_idx] 
                               << ", success=" << response.success()
                               << ", latency=" << latency.count() << "us");
                }
                stats.success_count++;
            } else {
                if (config.verbose) {
                    KRPC_LOG_WARNING("[Thread-" << thread_id << "] 业务错误: " << response.result().errmsg());
                }
                stats.fail_count++;
            }
        }
        
        // 思考时间（模拟真实用户）
        if (config.think_time_ms > 0) {
            std::this_thread::sleep_for(std::chrono::milliseconds(config.think_time_ms));
        }
    }
}

void TestRegisterTask(int thread_id, TestStats& stats, const TestConfig& config) {
    Kuser::UserServiceRpc_Stub stub(new KrpcChannel(false));
    
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> id_dist(1000, 9999);
    
    for (int i = 0; i < config.requests_per_thread; ++i) {
        auto start = std::chrono::high_resolution_clock::now();
        
        Kuser::RegisterRequest request;
        request.set_id(id_dist(gen));
        request.set_name("testuser_" + std::to_string(thread_id) + "_" + std::to_string(i));
        request.set_pwd("123456");
        
        Kuser::RegisterResponse response;
        Krpccontroller controller;
        
        stub.Register(&controller, &request, &response, nullptr);
        
        auto end = std::chrono::high_resolution_clock::now();
        auto latency = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        
        stats.RecordLatency(latency.count());
        
        if (controller.Failed()) {
            if (config.verbose) {
                KRPC_LOG_ERROR("[Thread-" << thread_id << "] 注册失败: " << controller.ErrorText());
            }
            stats.fail_count++;
        } else {
            if (response.result().errcode() == 0) {
                if (config.verbose) {
                    KRPC_LOG_INFO("[Thread-" << thread_id << "] 注册成功: user=" << request.name()
                               << ", success=" << response.success()
                               << ", latency=" << latency.count() << "us");
                }
                stats.success_count++;
            } else {
                if (config.verbose) {
                    KRPC_LOG_WARNING("[Thread-" << thread_id << "] 业务错误: " << response.result().errmsg());
                }
                stats.fail_count++;
            }
        }
        
        if (config.think_time_ms > 0) {
            std::this_thread::sleep_for(std::chrono::milliseconds(config.think_time_ms));
        }
    }
}

// ============================================
// 打印测试结果
// ============================================
void PrintTestResults(const TestStats& stats, const TestConfig& config, 
                     const std::chrono::duration<double>& elapsed) {
    int total_requests = config.thread_count * config.requests_per_thread;
    double qps = total_requests / elapsed.count();
    
    long long avg_latency = stats.total_latency_us.load() / total_requests;
    
    KRPC_LOG_INFO("");
    KRPC_LOG_INFO("┌────────────────────────────────────────────────────┐");
    KRPC_LOG_INFO("│              Krpc 测试结果报告                     │");
    KRPC_LOG_INFO("└────────────────────────────────────────────────────┘");
    KRPC_LOG_INFO("");
    KRPC_LOG_INFO("测试配置:");
    KRPC_LOG_INFO("  并发线程数: " << config.thread_count);
    KRPC_LOG_INFO("  每个线程请求数: " << config.requests_per_thread);
    KRPC_LOG_INFO("  总请求数: " << total_requests);
    KRPC_LOG_INFO("  思考时间: " << config.think_time_ms << "ms");
    KRPC_LOG_INFO("");
    KRPC_LOG_INFO("执行结果:");
    KRPC_LOG_INFO("  成功: " << stats.success_count);
    KRPC_LOG_INFO("  失败: " << stats.fail_count);
    KRPC_LOG_INFO("  成功率: " << std::fixed << std::setprecision(2) 
               << (stats.success_count.load() * 100.0 / total_requests) << "%");
    KRPC_LOG_INFO("");
    KRPC_LOG_INFO("性能指标:");
    KRPC_LOG_INFO("  总耗时: " << std::fixed << std::setprecision(3) << elapsed.count() << " 秒");
    KRPC_LOG_INFO("  QPS: " << std::fixed << std::setprecision(0) << qps);
    KRPC_LOG_INFO("  平均延迟: " << avg_latency << " us (" << avg_latency/1000.0 << " ms)");
    KRPC_LOG_INFO("  最小延迟: " << stats.min_latency_us.load() << " us");
    KRPC_LOG_INFO("  最大延迟: " << stats.max_latency_us.load() << " us");
    KRPC_LOG_INFO("");
}

// ============================================
// 简单冒烟测试（单次调用）
// ============================================
void SmokeTest() {
    KRPC_LOG_INFO("===== 冒烟测试 =====");
    
    Kuser::UserServiceRpc_Stub stub(new KrpcChannel(false));
    
    // 测试登录
    {
        Kuser::LoginRequest request;
        request.set_name("smoke_test");
        request.set_pwd("123456");
        
        Kuser::LoginResponse response;
        Krpccontroller controller;
        
        KRPC_LOG_INFO("测试登录...");
        stub.Login(&controller, &request, &response, nullptr);
        
        if (controller.Failed()) {
            KRPC_LOG_ERROR("登录调用失败: " << controller.ErrorText());
        } else {
            KRPC_LOG_INFO("登录成功! success=" << response.success());
        }
    }
    
    // 测试注册（如果有）
    {
        Kuser::RegisterRequest request;
        request.set_id(8888);
        request.set_name("smoke_register");
        request.set_pwd("123456");
        
        Kuser::RegisterResponse response;
        Krpccontroller controller;
        
        KRPC_LOG_INFO("测试注册...");
        stub.Register(&controller, &request, &response, nullptr);
        
        if (controller.Failed()) {
            KRPC_LOG_ERROR("注册调用失败: " << controller.ErrorText());
        } else {
            KRPC_LOG_INFO("注册成功! success=" << response.success());
        }
    }
    
    KRPC_LOG_INFO("===== 冒烟测试结束 =====");
}

int main(int argc, char **argv) {
    KrpcApplication::Init(argc, argv);
    
    KRPC_LOG_INFO("===== Krpc Client 启动 =====");
    
    // 默认测试配置
    TestConfig config;
    // config.thread_count = 8;
    // config.requests_per_thread = 100;
    // config.test_login = true;
    // config.test_register = false;
    // config.verbose = true;
    // config.think_time_ms = 0;
    
    // 先执行冒烟测试
    SmokeTest();
    
    KRPC_LOG_INFO("");
    KRPC_LOG_INFO("===== 性能测试 =====");
    
    TestStats stats;
    std::vector<std::thread> threads;
    
    auto start_time = std::chrono::high_resolution_clock::now();
    
    // 启动测试线程
    for (int i = 0; i < config.thread_count; i++) {
        if (config.test_login) {
            threads.emplace_back([i, &stats, config]() {
                TestLoginTask(i, stats, config);
            });
        } else if (config.test_register) {
            threads.emplace_back([i, &stats, config]() {
                TestRegisterTask(i, stats, config);
            });
        }
    }
    
    // 等待所有线程结束
    for (auto &t : threads) {
        t.join();
    }
    
    auto end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end_time - start_time;
    
    PrintTestResults(stats, config, elapsed);
    
    return 0;
}