
public class StreamReceiver
{
    public static native int init();
	public static native int NextOnePicLen();
    public static native void readOnePic(Object data);

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
