// // #include <iostream>
// // #include <string>
// // #include "../user.pb.h"
// // #include "Krpcapplication.h"
// // #include "Krpcprovider.h"
// // #include "KrpcLogger.h"

// // /*
// // UserService 原本是一个本地服务，提供了两个本地方法：Login 和 GetFriendLists。
// // 现在通过 RPC 框架，这些方法可以被远程调用。
// // */
// // class UserService : public Kuser::UserServiceRpc // 继承自 protobuf 生成的 RPC 服务基类
// // {
// // public:
// //     // 本地登录方法，用于处理实际的业务逻辑
// //     bool Login(std::string name, std::string pwd) {
// //         std::cout << "doing local service: Login" << std::endl;
// //         std::cout << "name:" << name << " pwd:" << pwd << std::endl;  
// //         return true;  // 模拟登录成功
// //     }

// //     /*
// //     重写基类 UserServiceRpc 的虚函数，这些方法会被 RPC 框架直接调用。
// //     1. 调用者（caller）通过 RPC 框架发送 Login 请求。
// //     2. 服务提供者（callee）接收到请求后，调用下面重写的 Login 方法。
// //     */
// //     void Login(::google::protobuf::RpcController* controller,
// //               const ::Kuser::LoginRequest* request,
// //               ::Kuser::LoginResponse* response,
// //               ::google::protobuf::Closure* done) {
// //         // 从请求中获取用户名和密码
// //         std::string name = request->name();
// //         std::string pwd = request->pwd();

// //         // 调用本地业务逻辑处理登录
// //         bool login_result = Login(name, pwd); 

// //         // 将响应结果写入 response 对象
// //         Kuser::ResultCode *code = response->mutable_result();
// //         code->set_errcode(0);  // 设置错误码为 0，表示成功
// //         code->set_errmsg("");  // 设置错误信息为空
// //         response->set_success(login_result);  // 设置登录结果

// //         // 执行回调操作，框架会自动将响应序列化并发送给调用者
// //         done->Run();
// //     }
// // };

// // int main(int argc, char **argv) {

// //     // // 初始化日志
// //     // KrpcLogger logger("KrpcServer");
// //     // logger.SetLogLevel(KrpcLogLevel::INFO);

// //     // 调用框架的初始化操作，解析命令行参数并加载配置文件
// //     KrpcApplication::Init(argc, argv);

// //     // 创建一个 RPC 服务提供者对象
// //     KrpcProvider provider;

// //     // 将 UserService 对象发布到 RPC 节点上，使其可以被远程调用
// //     provider.NotifyService(new UserService());

// //     // 启动 RPC 服务节点，进入阻塞状态，等待远程的 RPC 调用请求
// //     provider.Run();

// //     return 0;
// // }


// #include <iostream>
// #include <string>
// #include "../user.pb.h"
// #include "Krpcapplication.h"
// #include "Krpcprovider.h"
// #include "KrpcLogger.h"

// class UserService : public Kuser::UserServiceRpc
// {
// public:
//     bool Login(std::string name, std::string pwd) {
//         std::cout << "doing local service: Login" << std::endl;
//         std::cout << "name:" << name << " pwd:" << pwd << std::endl;
        
//         // 使用宏调用日志！
//         KRPC_LOG_INFO("收到登录请求: name=" << name);
        
//         return true;
//     }

//     void Login(::google::protobuf::RpcController* controller,
//                const ::Kuser::LoginRequest* request,
//                ::Kuser::LoginResponse* response,
//                ::google::protobuf::Closure* done) {
        
//         std::string name = request->name();
//         std::string pwd = request->pwd();

//         bool login_result = Login(name, pwd);

//         Kuser::ResultCode *code = response->mutable_result();
//         code->set_errcode(0);
//         code->set_errmsg("");
//         response->set_success(login_result);

//         done->Run();
//     }
// };

// int main(int argc, char **argv) {
    
//     // 框架自动初始化日志！
//     KrpcApplication::Init(argc, argv);
    
//     // 创建 RPC 服务提供者对象
//     KrpcProvider provider;

//     // 注册服务
//     provider.NotifyService(new UserService());

//     // 启动 RPC 服务节点
//     KRPC_LOG_INFO("RPC 服务节点启动中...");
//     provider.Run();

//     return 0;
// }

#include <iostream>
#include <string>
#include <chrono>
#include <thread>
#include "../user.pb.h"
#include "Krpcapplication.h"
#include "Krpcprovider.h"
#include "KrpcLogger.h"

// 可配置的服务端设置
struct ServerConfig {
    int io_threads = 4;           // muduo IO 线程数
    int worker_threads = 1;       // 业务处理线程数（预留）
    bool enable_sleep = false;    // 是否在处理时添加延迟（模拟慢服务）
    int sleep_ms = 0;            // 延迟毫秒数
    bool verbose_log = false;     // 是否打印详细日志
};

// 全局配置
ServerConfig g_config;

class UserService : public Kuser::UserServiceRpc
{
public:
    // 本地登录方法
    bool Login(std::string name, std::string pwd) {
        if (g_config.verbose_log) {
            KRPC_LOG_INFO("收到登录请求: name=" << name << ", pwd=" << pwd);
        }
        
        // 模拟慢服务（可选）
        if (g_config.enable_sleep) {
            std::this_thread::sleep_for(std::chrono::milliseconds(g_config.sleep_ms));
        }
        
        return true;
    }

    // 重写基类的虚函数
    void Login(::google::protobuf::RpcController* controller,
               const ::Kuser::LoginRequest* request,
               ::Kuser::LoginResponse* response,
               ::google::protobuf::Closure* done) {
        
        auto start = std::chrono::high_resolution_clock::now();
        
        std::string name = request->name();
        std::string pwd = request->pwd();

        bool login_result = Login(name, pwd);

        Kuser::ResultCode *code = response->mutable_result();
        code->set_errcode(0);
        code->set_errmsg("登录成功");
        response->set_success(login_result);

        // 记录处理时间
        auto end = std::chrono::high_resolution_clock::now();
        auto latency = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        
        if (g_config.verbose_log) {
            KRPC_LOG_INFO("登录处理完成: latency=" << latency.count() << "us");
        }

        done->Run();
    }
    
    // 注册方法
    bool Register(uint32_t id, std::string name, std::string pwd) {
        if (g_config.verbose_log) {
            KRPC_LOG_INFO("收到注册请求: id=" << id << ", name=" << name);
        }
        
        if (g_config.enable_sleep) {
            std::this_thread::sleep_for(std::chrono::milliseconds(g_config.sleep_ms));
        }
        
        return true;
    }
    
    void Register(::google::protobuf::RpcController* controller,
                  const ::Kuser::RegisterRequest* request,
                  ::Kuser::RegisterResponse* response,
                  ::google::protobuf::Closure* done) {
        
        auto start = std::chrono::high_resolution_clock::now();
        
        uint32_t id = request->id();
        std::string name = request->name();
        std::string pwd = request->pwd();

        bool register_result = Register(id, name, pwd);

        Kuser::ResultCode *code = response->mutable_result();
        code->set_errcode(0);
        code->set_errmsg("注册成功");
        response->set_success(register_result);

        auto end = std::chrono::high_resolution_clock::now();
        auto latency = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        
        if (g_config.verbose_log) {
            KRPC_LOG_INFO("注册处理完成: latency=" << latency.count() << "us");
        }

        done->Run();
    }
};

int main(int argc, char **argv) {
    
    // 初始化 RPC 框架
    KrpcApplication::Init(argc, argv);
    
    KRPC_LOG_INFO("===== Krpc Server 启动 =====");
    
    // 服务端配置（可以改成命令行参数）
    g_config.io_threads = 8;         // 增加 IO 线程数
    g_config.enable_sleep = false;   // 不添加延迟
    g_config.verbose_log = false;    // 生产环境关闭详细日志
    g_config.sleep_ms = 0;
    
    KRPC_LOG_INFO("服务端配置:");
    KRPC_LOG_INFO("  IO 线程数: " << g_config.io_threads);
    KRPC_LOG_INFO("  模拟延迟: " << (g_config.enable_sleep ? "是" : "否") 
               << " (" << g_config.sleep_ms << "ms)");
    
    // 创建 RPC 服务提供者对象
    KrpcProvider provider;

    // 注册服务
    provider.NotifyService(new UserService());
    
    // 设置 muduo 线程数（在 Run 之前设置）
    // 注意：需要在 provider.Run() 之前调用
    // provider.SetThreadNum(g_config.io_threads);

    // 启动 RPC 服务节点
    KRPC_LOG_INFO("RPC 服务节点启动中...");
    provider.Run();

    return 0;
}