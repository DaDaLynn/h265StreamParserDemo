
public class DataChannel {
    // client setup
    public static native int init();
    public static native int deinit();

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


    public static class AlarmHorn {
        public int ah_no = 0; 
        public int ah_v = 0;  
        public AlarmHorn(int _no, int _v) {
            this.ah_no = _no;
            this.ah_v = _v;
        }
        public AlarmHorn() {
            this(0, 0);
        }
    }

    public static class RvParam {
        public float lane_width;  
        public float near_field_bound; 
        public float near_field_speed_limit; 
        public float far_field_bound; 
        public float far_field_speed_limit; 

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
        public int obj_id; 
        public float dislong;  
        public float dislat;  
        public float vrelong;  
        public int status; 

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
            System.loadLibrary("DataChannelJni_Android");
        } catch(UnsatisfiedLinkError e)
        {
            System.err.println("Cannot load DataChannel library:\n " +
                    e.toString());
        }
    }
}
