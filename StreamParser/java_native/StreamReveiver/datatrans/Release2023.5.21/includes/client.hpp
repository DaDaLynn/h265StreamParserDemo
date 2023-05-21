#pragma once
#include <stdlib.h>
#include <inttypes.h>
#include <stdio.h>
#include <thread>
#include <mutex>

using namespace std;

#define Debug(...) { printf("%s(line:%d): ", __FILE__, __LINE__); printf(__VA_ARGS__); fflush(stdout);}

struct RadarData{
    int32_t obj_id; //目标id
    float dislong;  //经度距离
    float dislat;   //纬度距离
    float vrelong;  //速度
    int32_t status; //目标状态（0绿1黄2红）
};

using PL_RADARDATACB = void (*)(void* pData, int data_len, void* pUserData); //雷达数据上报回调
using PL_BITSCB = void (*)(void* pData, int data_len, bool bIdr, void* pUserData); //视频帧数据回调

#define DLL_EXPORTS __attribute__((visibility("default")))

class DLL_EXPORTS Client{
public:
    Client();
    ~Client();

    int32_t init(PL_BITSCB, void*, PL_RADARDATACB, void*);
    void uninit();
    int32_t restart();

    bool write_data(const string& data_name, void* pData, int32_t data_len);
    bool read_data(const string& data_name, void* pData, int32_t data_len);

    bool is_valid();//判读网络会话对象是否有效

private:
    void recv_main();
    int32_t create_sess(PL_BITSCB, void*);
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
    PL_RADARDATACB pRadarDataCb {nullptr};
    void* pUserdataRadar {nullptr};
};

