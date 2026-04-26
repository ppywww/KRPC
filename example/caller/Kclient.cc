
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
        
        // 检查是否超时
        if (controller.IsTimeout()) {
            if (config.verbose) {
                KRPC_LOG_ERROR("[Thread-" << thread_id << "] 趃用超时: " << controller.ErrorText());
            }
            stats.timeout_count++;
            continue;
        }
        
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