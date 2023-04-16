package com.example.hevcdeocderlibrary;

public class StreamParser
{
    public static native int initStream(String pStreamFile);
    public static native int nextNalLen(int nStreamLen, int Pos);
    public static native void readOneNal(Object data, int Pos, int sliceLen);

    public static native int add(int a,int b);

    static
    {
        try{
            System.loadLibrary("StreamParserJni_Android");
        } catch(UnsatisfiedLinkError e)
        {
            System.err.println("Cannot load StreamParser library:\n " +
                    e.toString());
        }
    }

    public static void  main(String[] args)
    {
        StreamParser a = new StreamParser();
        System.out.println("hello" + a.add(3,1));
    }
}
