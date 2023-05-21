package com.example.hevcdeocderlibrary;

public class StreamReceiver {
    // client setup
    public static native int init();
    public static native int deinit();

    // video control
    public static native boolean videostart();
    public static native int NextOnePicLen();
    public static native void readOnePic(Object data);
    public static native boolean videostop();

    // parameters transfer functions
    public static native boolean write_int(String data_name, int pData);
    public static native int read_int(String data_name);

    public static native boolean write_float(String data_name, float pData);
    public static native float read_float(String data_name);

    public static native boolean write_AlarmHorn(AlarmHorn data_name);
    public static native boolean read_AlarmHorn(AlarmHorn data_name);

    public static native boolean write_RvParam(RvParam data_name);
    public static native boolean read_RvParam(RvParam data_name);

    public static native boolean read_RadarData(RadarData data_name);

    /*public static native void set_radar_video_param(RadarVideoParam param);

    public class RadarVideoParam {
        public float lane_offset = 0f; //车道偏移
        public AlarmHorn alarm_horn = new AlarmHorn(); //报警喇叭
        public int work_mode = 0; //0:全关 1：实测開 2：模拟开
        public RvParam rv_param = new RvParam(); //雷达参数
        public byte[] extension = new byte[256];
    }*/

    public static class AlarmHorn {
        public int ah_no = 0; //1~3报警喇叭继电器序号，4~5：LED1/手环触发LED2
        public int ah_v = 0;  //0:断开；1：闭合
        public AlarmHorn(int _no, int _v) {
            this.ah_no = _no;
            this.ah_v = _v;
        }
        public AlarmHorn() {
            this(0, 0);
        }
    }

    public static class RvParam {
        public float lane_width;  //车道宽
        public float near_field_bound; //近场边界
        public float near_field_speed_limit; //近场限速
        public float far_field_bound; //远场边界
        public float far_field_speed_limit; //远场限速

        public RvParam(float _w, float _nb, float _nl, float _fb, float _fl) {
            this.lane_width = _w;
            this.near_field_bound = _nb;
            this.near_field_speed_limit = _nl;
            this.far_field_bound = _fb;
            this.far_field_speed_limit = _fl;
        }
        public RvParam() {
            this(0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
        }
    }

    public static class RadarData {
        public int obj_id; //目标id
        public float dislong;  //经度距离
        public float dislat;   //纬度距离
        public float vrelong;  //速度
        public int status; //目标状态（0绿1黄2红）

        public RadarData(int _o, float _d1, float _d2, float _v, int _s){
            this.obj_id = _o;
            this.dislong = _d1;
            this.dislat = _d2;
            this.vrelong = _v;
            this.status = _s;
        }

        public RadarData() {this(0, 0.0f, 0.0f, 0.0f, 0);}
    }


    static
    {
        try{
            System.loadLibrary("StreamReceiverJni_Android");
        } catch(UnsatisfiedLinkError e)
        {
            System.err.println("Cannot load StreamReceiver library:\n " +
                    e.toString());
        }
    }
}
