
public class StreamReceiver
{
    public static native int init();
    public static native int NextOnePicLen();
    public static native void readOnePic(Object data);
    public static native int deinit();
    public static native boolean write_int(String data_name, int pData);
    public static native int read_int(String data_name);
    public static native boolean write_float(String data_name, float pData);
    public static native float read_float(String data_name);
	
	public static native boolean videostart();
	public static native boolean videostop();

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
