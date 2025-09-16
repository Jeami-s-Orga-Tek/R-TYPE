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

    protected:
    private:
};
}



#endif /* !LOG_HPP_ */
