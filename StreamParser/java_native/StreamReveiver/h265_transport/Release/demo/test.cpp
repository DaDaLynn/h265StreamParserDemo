#include "client.hpp"
using namespace std;

/*
   需要封装的接口：
      （1）回调函数，见demo中DataProcCb
      （2）数据通道读写函数，见demo中client.read_data() & client.write_data
*/
void DataProcCb(void* pData, int data_len, bool bIdr, void* pUserData){
      
}



int main(int argc, char** argv) {
    
    Client client;
    int times = 0;
    
    if (!client.init(DataProcCb, nullptr)){
        while(1) {
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





