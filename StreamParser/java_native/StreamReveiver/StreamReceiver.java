
public class StreamReceiver
{
    public static native int init();
    public static native int NextOnePicLen();
    public static native void readOnePic(Object data);
    public static native int deinit();
    public static native boolean write_data(String data_name, Object pData, int data_len);
    public static native boolean read_data(String data_name, Object pData, int data_len);

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
