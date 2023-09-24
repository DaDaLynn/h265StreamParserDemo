#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <chrono>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <string>
#include <string.h>
#include <inttypes.h>
#include <memory>
#include <vector>


template<typename T>
class RingBufferFixsize{//push定长数据
public:
    typedef struct circ_bbuf_t{
        T* const buffer{nullptr};
        intptr_t head{0};
        intptr_t tail{0};
        const int64_t maxlen{0};

        ~circ_bbuf_t(){
            if (buffer != nullptr) delete buffer;
        }
    } circ_bbuf_t;

private:
    circ_bbuf_t m_ring;
    size_t m_size{0};

public:
    int32_t init(int64_t max_size=64){
        *(T**)&m_ring.buffer = new(std::nothrow) T[max_size+1];
        if(m_ring.buffer != nullptr) {
            m_ring.head = 0;
            m_ring.tail = 0;
            *(int64_t*)&m_ring.maxlen = max_size+1;

            return 0;
        }

        return -1;
    }

    int32_t push(T& data)
    {
        intptr_t next;
        auto *c = &m_ring;

        next = c->head + 1;  // next is where head will point to after this write.
        if (next >= c->maxlen)
            next = 0;

        if (next == c->tail)  // if the head + 1 == tail, circular buffer is full
            return -1;

        c->buffer[c->head] = data;  // Load data and then move
        c->head = next;             // head to next data offset.
        m_size++;
        return 0;  // return success to indicate successful push.
    }

    int32_t push( T&& data){
        return push((T&)data);
    }

    int32_t pop(T *data)
    {
        intptr_t next;
        auto *c = &m_ring;

        if (c->head == c->tail)  // if the head == tail, we don't have any data
            return -1;

        next = c->tail + 1;  // next is where tail will point to after this read.
        if(next == c->maxlen)
            next = 0;

        *data = c->buffer[c->tail];  // Read data and then move
        c->tail = next;              // tail to next offset.
        m_size--;
        return 0;  // return success to indicate successful push.
    }

    size_t size(){
        return m_size;
    }

    bool full() {
        return m_size==(m_ring.maxlen-1);
    }

    bool empty() {
        return m_size==0;
    }
};

template<typename T>
class RingBufferFixsize_s{//push定长数据
private:
    RingBufferFixsize<T> m_rb;
    std::mutex m_mtx;

public:
    int32_t init(int64_t max_size=64){
        return m_rb.init(max_size);
    }

    size_t size(){
        std::unique_lock<std::mutex> m(m_mtx);
        return m_rb.m_size;
    }

    int32_t pop(T *data){
        std::unique_lock<std::mutex> m(m_mtx);
        return m_rb.pop(data);
    }

    int32_t push(T& data){
        std::unique_lock<std::mutex> m(m_mtx);
        return m_rb.push(data);
    }

    int32_t push(const T&& data){
        return push((T&)data);
    }

    bool full() {
        std::unique_lock<std::mutex> m(m_mtx);
        return m_rb.full();
    }

    bool empty() {
        std::unique_lock<std::mutex> m(m_mtx);
        return m_rb.empty();
    }
};

template<typename T>
class CircularBuffer{ //push不定长数据
public:
    struct Pointer{
        intptr_t pos{0};
        int32_t size{0};
		Pointer(intptr_t p=0, int32_t s=0){ pos = p; size = s; }
    };

private:
    RingBufferFixsize<Pointer> m_disc;
    std::vector<T> m_buf;
    size_t head{0}, tail{0}, maxlen{0};
    T* m_Pbuf{nullptr};
    size_t m_size{0};
    size_t m_pkg_num{0};

public:
    int32_t init(size_t element_num, int32_t pkg_max_num=128) {//前者对应总容量，后者对应最多可以push次数
        element_num += 1;
        if ( m_disc.init(pkg_max_num) ) {
            return -1;
        }

        m_buf.resize(element_num);
        if(m_buf.size() != element_num){
            return -2;
        }

        m_Pbuf = m_buf.data();
        maxlen = element_num;

        return 0;
    }

    int32_t push(T* data, int32_t size)
    {
        size_t next;

        next = head + size;  // next is where head will point to after this write.
        if (next >= maxlen)
            next = 0;

        if (next == tail)  // if the head + 1 == tail, circular buffer is full
            return -1;

		Pointer ps = {(intptr_t)head, (int32_t)size} ;
        if(m_disc.push( ps )){
            return -2;
        }

        // Load data and then move
        auto p = &m_Pbuf[head];
        for(auto i=0; i<size; i++) p[i] = data[i];
        //memcpy(&m_Pbuf[head], data, sizeof(T)*size);
        head = next;// head to next data offset.

        m_size += size;
        m_pkg_num++;
        return 0;  // return success to indicate successful push.
    }

    int32_t pop(T* &data, int32_t& size) //只是返回了数据指针和长度，没有做拷贝，调用者进行拷贝
    {
        intptr_t next;
        if (head == tail || m_size==0){  // if the head == tail, we don't have any data
            return -1;
        }

        Pointer pt;
        if(m_disc.pop(&pt)) {
            return -2;
        }

        next = tail + pt.size;  // next is where tail will point to after this read.
        if(next >= maxlen){
            next = 0;
        }

        data = &m_Pbuf[tail];  // Read data and then move
        size = pt.size;
        tail = next;              // tail to next offset.

        m_size -= pt.size;
        m_pkg_num--;

        return 0;  // return success to indicate successful push.
    }

    size_t size(){
        return m_size;
    }

    size_t pkg_num(){
        return m_pkg_num;
    }

    bool empty() {
        return m_size==0;
    }
};

template<typename T>
class CircularBuffer_s{//线程安全类，pop接口含义和非安全的类不一样
private:
    CircularBuffer<T> m_rb;
    std::mutex m_mtx;
    std::condition_variable m_cv;
    volatile bool m_pop_sig{false}, m_push_sig{false};

public:
    int32_t init(size_t element_num, int32_t pkg_max_num=128){
        return m_rb.init(element_num, pkg_max_num);
    }

    int32_t push(T* data, int32_t size, int32_t timeout_ms=0, bool force_push=false){//push multiple elements
        std::unique_lock<std::mutex> m(m_mtx);
        int ir = -1;
        for(auto i=0; i<2 && ir != 0; i++){
            ir = m_rb.push(data, size);
            if (i==0 && ir != 0 && timeout_ms>0){
                m_push_sig = true;
                m_cv.wait_for(m, std::chrono::milliseconds(timeout_ms));
            }
            if (timeout_ms < 1) break;
        }

        if (ir !=0 && force_push){
            T* p;
            int32_t s;
            while(ir!=0 && m_rb.pkg_num()>0){
                m_rb.pop(p, s);
                ir = m_rb.push(data, size);
            }
        }

        if (ir == 0) m_push_sig = false;

        if (m_pop_sig)
            m_cv.notify_all();

        return ir;
    }

    int32_t push(T&& data, int32_t timeout_ms=0, bool force_push=false){ //push one element
        return push(&data, 1, timeout_ms, force_push);
    }
	
	int32_t push(T& data, int32_t timeout_ms=0, bool force_push=false){ //push one element
        return push(&data, 1, timeout_ms, force_push);
    }

    size_t size() {
        std::unique_lock<std::mutex> m(m_mtx);
        return m_rb.size();
    }

    size_t pkg_num(){
        std::unique_lock<std::mutex> m(m_mtx);
        return m_rb.pkg_num();
    }

    int32_t pop(T* data/*out*/, int32_t& size/*inout*/, int32_t timeout_ms=0){ //pop one element
        T* p;
        int32_t s;

        std::unique_lock<std::mutex> m(m_mtx);

        int ir = -1;
        for(auto i=0; i<2 && ir != 0; i++){
            ir = m_rb.pop(p, s);
            if (i==0 && ir != 0 && timeout_ms>0){
                m_pop_sig = true;
                m_cv.wait_for(m, std::chrono::milliseconds(timeout_ms));
            }
            if (timeout_ms < 1) break;
        }

        if(ir == 0) {
            m_pop_sig = false;
            if( s <= size ) {
                //memcpy(data, p, sizeof(T)*s);
                for(auto i=0; i<s; i++) data[i] = p[i];
                size = s;
            }else{
                ir = 1; //用户buffer不够大
            }
            if (m_push_sig)
                m_cv.notify_all();
        }

        return ir; //没有数据
    }

    bool empty() {
        std::unique_lock<std::mutex> m(m_mtx);
        return m_rb.empty();
    }
};
