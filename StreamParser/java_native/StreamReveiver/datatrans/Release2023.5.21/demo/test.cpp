#include "client.hpp"
using namespace std;

int64_t get_ts_us() //us
{
    using namespace std::chrono;
    return duration_cast<microseconds>(system_clock::now().time_since_epoch()).count();
}

/*
   需要封装的接口：
      （1）回调函数，见demo中DataProcCb & RadarDataCb
      （2）数据通道读写函数，见demo中client.read_data() & client.write_data
       (3) 检查会话通道是否有效，client.is_valid()
*/
void VideoDataCb(void* pData, int data_len, bool bIdr, void* pUserData){
    static int64_t ts = get_ts_us();
    auto interval = get_ts_us() - ts;
    ts = get_ts_us();
    printf("%s==>frame_len: %d, %.2fms\n", bIdr?"   IDR":"P", data_len, interval/1000.0f);
}


void RadarDataCb(void* pData, int data_len, void* pUserData){
    static int64_t ts = get_ts_us();
    auto interval = get_ts_us() - ts;
    ts = get_ts_us();
    auto radar_data = *(RadarData*)pData; //需要将radar_data送给界面展示
    printf("RadarData[%d]==>data_len: %d, %.2fms\n", radar_data.obj_id, data_len, interval/1000.0f);
}


int main(int argc, char** argv) {
    
    Client client;
    int times = 0;
    int ctrl = 1;
    client.write_data("VideoCtrl", &ctrl, sizeof(ctrl));
    if (!client.init(VideoDataCb, nullptr, RadarDataCb, nullptr)){
        for(auto k=0; k<50*30; k++) {
            this_thread::sleep_for(chrono::milliseconds(20));	
            if (times++ % 150 == 0|| times % 300==0){
                int w;                
                auto b = client.read_data("width", &w, 4); // read data
                printf("read %d\n", b);
                printf("==============>:%d\n", w); fflush(stdout);
            }else if (times % 50==0 || times % 250==0){
                int w = times == 50? 1280:1920;
                auto b = client.write_data("width", &w, 4);
                printf("write %d\n", b);
            }
        }
    }else{
        printf("Err.\n");
    }
    
    return 0;
}





