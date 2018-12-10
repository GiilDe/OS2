#ifndef DS_TESTMACROS_H
#define DS_TESTMACROS_H

#include <exception>
#include <iostream>
#include <sstream>
#include <string>
#include <cstdlib>

using std::ostringstream;
using std::string;
using std::cout;
using std::cerr;
using std::endl;
using std::exception;

/**
 * ANSI-C color "manipulators"
 */
#define GREEN "\033[32m"
#define RED "\033[31m"
#define YELLOW "\033[33m"
#define REVERSED "\033[7m"
#define NORMAL_TEXT "\033[0m"

#define PRINT_ERROR(str) cerr << RED << "[ERROR] " << NORMAL_TEXT << "Error in line: "  \
<< __LINE__ << ". Error is: " << (str) << "." << endl;

#define ASSERT_NO_EXCEPTION(t) \
    try{ \
        t; \
    } catch(const exception& e){ \
        PRINT_ERROR(string("Caught exception ") + e.what()); \
        return false; \
    }
#define ASSERT_EXCEPTION(t, exc) \
    try{ \
        t; \
        PRINT_ERROR("No exception"); \
        return false; \
    } catch(const exc& e){}catch(const exception& e){\
        PRINT_ERROR(string(" Wrong exception caught: ") + e.what());\
        return false; \
    }

#define ASSERT_TRUE(t) \
    try{ \
        if(!(t)){\
            PRINT_ERROR((string(#t) + " is false"));\
            return false;\
        } \
    } catch(const exception& e) { \
        PRINT_ERROR(string("Caught exception ") + e.what());\
        return false; \
    }

#define ASSERT_FALSE(t) \
    try{ \
        if(t){\
            PRINT_ERROR((string(#t) + " is true"));\
            return false;\
        } \
    } catch(const exception& e) { \
        PRINT_ERROR(string("Caught exception ") + e.what());\
        return false; \
    }

#define ASSERT_EQUALS(a, b) \
    try{ \
        if((a) != (b)){\
            PRINT_ERROR((string(#a) + " != " + string(#b))); \
            return false;\
        } \
    } catch(const exception& e) { \
        PRINT_ERROR(string("Caught exception ") + e.what());\
        return false; \
    }

#define ASSERT_NOT_EQUALS(a, b) \
    try{ \
        if((a) == (b)){\
            PRINT_ERROR((string(#a) + " == " + string(#b))); \
            return false;\
        } \
    } catch(const exception& e) { \
        PRINT_ERROR(string("Caught exception ") + e.what());\
        return false; \
    }

// Child process functions

#define CHILD_ASSERT_TRUE(t) \
    try{ \
        if(!(t)){\
            PRINT_ERROR((string(#t) + " is false"));\
            exit(-1);\
        } \
    } catch(const exception& e) { \
        PRINT_ERROR(string("Caught exception ") + e.what());\
        exit(-1); \
    }

#define CHILD_ASSERT_FALSE(t) \
    try{ \
        if(t){\
            PRINT_ERROR((string(#t) + " is true"));\
            exit(-1);\
        } \
    } catch(const exception& e) { \
        PRINT_ERROR(string("Caught exception ") + e.what());\
        exit(-1); \
    }

#define CHILD_ASSERT_EQUALS(a, b) \
    try{ \
        if((a) != (b)){\
            PRINT_ERROR((string(#a) + " != " + string(#b))); \
            exit(-1);\
        } \
    } catch(const exception& e) { \
        PRINT_ERROR(string("Caught exception ") + e.what());\
        exit(-1); \
    }

#define CHILD_ASSERT_NOT_EQUALS(a, b) \
    try{ \
        if((a) == (b)){\
            PRINT_ERROR((string(#a) + " == " + string(#b))); \
            exit(-1);\
        } \
    } catch(const exception& e) { \
        PRINT_ERROR(string("Caught exception ") + e.what());\
        exit(-1); \
    }

#define CHILD_EXIT() \
    exit(0);

#define RUN_TEST(t) \
    try { \
        if(t()){ \
            cout << REVERSED << #t << NORMAL_TEXT << ":" << GREEN << " PASS ✓" <<  NORMAL_TEXT << endl; \
        } else { \
            cerr << REVERSED << #t << NORMAL_TEXT << ":" << RED << " FAIL ✗" << NORMAL_TEXT << endl;\
            return -1; \
        } \
    } catch(const exception& e){ \
        cerr << #t << "has failed with exception: " << e.what(); \
        cerr <<endl;\
        return -1; \
    } catch(...) {\
        cerr << #t << "has failed with unknown exception" << endl;\
        return -1; \
    }

#define RUN_TEST_ISOLATE(t, stat_variable) \
    do { \
        if(CHILD_PROCESS(fork())) { \
            RUN_TEST(t); \
            CHILD_EXIT(); \
        } else { \
            wait(&stat_variable); \
        } \
    } while (0);


#define CHILD_PROCESS(pid) \
    (pid == 0)

#define WAIT_CHILD(stat_variable) \
    wait(&(stat_variable)); \
    if (stat_variable) { \
        return false; \
    }

#define WAIT_CHILD_PID(stat_variable, pid) \
    waitpid(pid, &(stat_variable), 0); \
    if (stat_variable) { \
        return false; \
    }

#define TEST_CHILD(code) \
    do { \
        code \
    } while (0);


bool VerifyOutput(ostringstream& output, const string& expected_output){
    if(expected_output != output.str()){
        cerr << "\nExpected output:\n" << expected_output << endl;
        cerr << "\nActual Output:\n" << output.str() << endl;
        return false;
    }
    output.clear();
    output.str("");
    return true;
}

#endif // DS_TESTMACROS_H
