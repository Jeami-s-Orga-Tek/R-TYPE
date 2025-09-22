/*
** EPITECH PROJECT, 2025
** G-CPP-500-BDX-5-1-rtype-1
** File description:
** Log
*/

#ifndef LOG_HPP_
#define LOG_HPP_

#include <string>
#include <iostream>
#include <cstdio>

namespace RtypeServer {
    enum LogLevel {
        DEBUG,
        INFO,
        WARNING,
        ERROR
    };

    class Log {
    public:
        Log();
        ~Log();
        
        template<typename... Args>
        void infof(const char* format, Args... args) {
            printf("[INFO] ");
            printf(format, args...);
            printf("\n");
        }
        
        template<typename... Args>
        void warnf(const char* format, Args... args) {
            printf("[WARN] ");
            printf(format, args...);
            printf("\n");
        }
        
        template<typename... Args>
        void errorf(const char* format, Args... args) {
            printf("[ERROR] ");
            printf(format, args...);
            printf("\n");
        }

    protected:
    private:
    };

    template<typename... Args>
    void infof(const char* format, Args... args) {
        printf("[INFO] ");
        printf(format, args...);
        printf("\n");
    }

    template<typename... Args>
    void warnf(const char* format, Args... args) {
        printf("[WARN] ");
        printf(format, args...);
        printf("\n");
    }

    template<typename... Args>
    void errorf(const char* format, Args... args) {
        printf("[ERROR] ");
        printf(format, args...);
        printf("\n");
    }
}



#endif /* !LOG_HPP_ */
