#pragma once
#include <stdlib.h>
#include <inttypes.h>
#include <stdio.h>
#include <thread>
#include <mutex>

using namespace std;

#define Debug(...) { printf("%s(line:%d): ", __FILE__, __LINE__); printf(__VA_ARGS__); fflush(stdout);}


using PL_BITSCB = void (*)(void* pData, int data_len, bool bIdr, void* pUserData);

#define DLL_EXPORTS __attribute__((visibility("default")))

class DLL_EXPORTS Client{
public:
    Client();
    ~Client();

    int32_t init(PL_BITSCB, void*);
    void uninit();

    bool write_data(const string& data_name, void* pData, int32_t data_len);
    bool read_data(const string& data_name, void* pData, int32_t data_len);

private:
    void recv_main();
    thread main;
    volatile bool flag_exit;
    volatile int32_t dcs{0}; // data_channel_status
    mutex mtx;
    uint16_t rw_no{0};
    uint8_t rw_buf[1024];
    int32_t rw_size;
    mutex mtx_sess; //need to ensure thread safe reading & writing of sess
    int32_t rw_shake_time{3};

private:
    void *sess {nullptr};
    PL_BITSCB pBitsCb {nullptr};
    void* pUserdata {nullptr};
};

