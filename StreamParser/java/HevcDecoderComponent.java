package com.example.hevcdeocderlibrary;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Canvas;
import android.graphics.RectF;
import android.media.MediaCodec;
import android.media.MediaFormat;

import android.util.Log;
import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import androidx.annotation.NonNull;

import com.alibaba.fastjson.JSONObject;

import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.nio.ByteBuffer;
import io.dcloud.feature.uniapp.UniSDKInstance;
import io.dcloud.feature.uniapp.annotation.UniJSMethod;
import io.dcloud.feature.uniapp.bridge.UniJSCallback;
import io.dcloud.feature.uniapp.ui.action.AbsComponentData;
import io.dcloud.feature.uniapp.ui.component.AbsVContainer;
import io.dcloud.feature.uniapp.ui.component.UniComponent;

/**
 * 自定义组件
 */
public class HevcDecoderComponent extends UniComponent<SurfaceView> implements SurfaceHolder.Callback {
    //private static final String SAMPLE = "/data/local/tmp/1920x1080_yuv420p_test0.265";
    private static final String SAMPLE = "/data/local/tmp/rec_100frm.265";
    private static final String PNG_SAMPLE = "/data/local/tmp/show.png";
    private int int_val = 0;
    private float float_val = 1.0f;

    //private StreamReceiver.AlarmHorn alarmHorn;
    private PlayerThread mPlayer;
    private SurfaceView surfaceView;

    public HevcDecoderComponent(UniSDKInstance instance, AbsVContainer parent, int type, AbsComponentData componentData) {
        super(instance, parent, type, componentData);
    }

    public HevcDecoderComponent(UniSDKInstance instance, AbsVContainer parent, AbsComponentData componentData) {
        super(instance, parent, componentData);
    }

/*    private Bitmap loadOneBitmap() {
        try {
            FileInputStream fis = new FileInputStream(PNG_SAMPLE);
            return BitmapFactory.decodeStream(fis);
        } catch (IOException e) {
            e.printStackTrace();
        }
        return null;
    }

    public void showOneFrame(byte[] bytes) {
        Canvas canvas = surfaceView.getHolder().lockCanvas();
        if (canvas != null) {
            Log.i("frame", "canvas width:" + canvas.getWidth() + " height:" + canvas.getHeight());
            canvas.drawColor(0, android.graphics.PorterDuff.Mode.CLEAR);
            Bitmap cacheBitmap = loadOneBitmap();
            RectF rectF = new RectF(0, 0, canvas.getWidth(), canvas.getHeight());
            canvas.drawBitmap(cacheBitmap, null, rectF, null);
            surfaceView.getHolder().unlockCanvasAndPost(canvas);
        }
    }*/

    @UniJSMethod
    public void videoPlay() {
        /*StreamReceiver.AlarmHorn alarmHorn = new StreamReceiver.AlarmHorn();
        int_val++;
        alarmHorn.ah_no = int_val;
        int_val++;
        alarmHorn.ah_v = int_val;
        boolean write_ret = StreamReceiver.write_AlarmHorn(alarmHorn);
        Log.d("DecodeActivity", "StreamReceiver.write_AlarmHorn return: " + write_ret);*/

        boolean start_rec = StreamReceiver.videostart();
        Log.d("DecodeActivity", "StreamReceiver.videostart return: " + start_rec);

        //showOneFrame(null);
        if (mPlayer != null) {
            if (Thread.State.RUNNABLE.equals(mPlayer.getState())) {
                return;
            }
            if (Thread.State.TERMINATED.equals(mPlayer.getState())) {
                mPlayer = new PlayerThread(surfaceView.getHolder().getSurface());
            }
        } else {
            mPlayer = new PlayerThread(surfaceView.getHolder().getSurface());
        }
        mPlayer.start();
    }

    @UniJSMethod
    public void videoStop() {
        /*StreamReceiver.AlarmHorn alarmHorn = new StreamReceiver.AlarmHorn();
        StreamReceiver.read_AlarmHorn(alarmHorn);
        Log.d("DecodeActivity", "StreamReceiver.set_AlarmHorn return: " + alarmHorn.ah_no + " "+ alarmHorn.ah_v);*/

        if (mPlayer != null) {
            mPlayer.signal();
        }
    }

    @UniJSMethod
    public void RvParamWrite(JSONObject json) {
        float l = json.getFloatValue("lane_width");
        float nb = json.getFloatValue("near_field_bound");
        float nl = json.getFloatValue("near_field_speed_limit");
        float fb = json.getFloatValue("far_field_bound");
        float fl = json.getFloatValue("far_field_speed_limit");
        StreamReceiver.RvParam rvParam = new StreamReceiver.RvParam(l, nb, nl, fb, fl);
        boolean write_ret = StreamReceiver.write_RvParam(rvParam);
        Log.d("DecodeActivity", "StreamReceiver.RvParamWrite return: " + write_ret);
    }

    @UniJSMethod
    public void RvParamRead(UniJSCallback callback) {
        StreamReceiver.RvParam rvParam = new StreamReceiver.RvParam();
        boolean read_ret = StreamReceiver.read_RvParam(rvParam);
        Log.d("DecodeActivity", "StreamReceiver.read_RvParam return: " + read_ret);

        JSONObject res = new JSONObject();
        res.put("lane_width", rvParam.lane_width);
        res.put("near_field_bound", rvParam.near_field_bound);
        res.put("near_field_speed_limit", rvParam.near_field_speed_limit);
        res.put("far_field_bound", rvParam.far_field_bound);
        res.put("far_field_speed_limit", rvParam.far_field_speed_limit);
        callback.invoke(res);
    }

    @UniJSMethod
    public void RadarDataRead(UniJSCallback callback) {
        StreamReceiver.RadarData radarData = new StreamReceiver.RadarData();
        boolean read_ret = StreamReceiver.read_RadarData(radarData);
        Log.d("DecodeActivity", "StreamReceiver.read_RadarData return: " + read_ret);

        JSONObject res = new JSONObject();
        res.put("obj_id", radarData.obj_id);
        res.put("dislong", radarData.dislong);
        res.put("dislat", radarData.dislat);
        res.put("vrelong", radarData.vrelong);
        res.put("status", radarData.status);
        callback.invoke(res);
    }

    @Override
    public void surfaceCreated(SurfaceHolder holder) {
        int init_rec = StreamReceiver.init();
        Log.d("DecodeActivity", "StreamReceiver.init return: " + init_rec);
    }

    @Override
    public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {

    }

    @Override
    public void surfaceDestroyed(SurfaceHolder holder) {
        if (mPlayer != null) {
            mPlayer.interrupt();
        }
        int deinit_rec = StreamReceiver.deinit();
        Log.d("DecodeActivity", "StreamReceiver.deinit return: " + deinit_rec);
    }

    @Override
    protected SurfaceView initComponentHostView(@NonNull Context context) {
        surfaceView = new SurfaceView(context);
        surfaceView.getHolder().addCallback(this);
        return surfaceView;
    }

    private class PlayerThread extends Thread {
        //private MediaExtractor extractor;
        private MediaCodec decoder;
        private MediaFormat format;
        private Surface surface;

        public PlayerThread(Surface surface) {
            this.surface = surface;
            this.running = true;
        }

        private volatile boolean running;

        public void openDecoder(String mime, int width ,int height)
        {
            format = MediaFormat.createVideoFormat(mime, width, height);
            try {
                decoder = MediaCodec.createDecoderByType(mime);
            } catch (IOException e) {
                throw new RuntimeException("Failed to create codec", e);
            }
            if (decoder == null) {
                Log.e("DecodeActivity", "Can't find video info!");
                return;
            }

            decoder.configure(format, surface, null, 0);
            decoder.start();
        }

        public void closeDecoder()
        {
            decoder.stop();
            decoder.release();
        }

        public void ReStartDecoder()
        {
            decoder.stop();
            decoder.configure(format, surface, null, 0);
            decoder.start();
        }


        public void signal () {
            this.running = false;
        }

        @Override
        public void run() {
            /*FileOutputStream fos2 = null;// 第二個参数为true表示程序每次运行都是追加字符串在原有的字符上
            try {
                fos2 = new FileOutputStream("/data/data/com.android.UniPlugin/input_stream.265", true);
            } catch (FileNotFoundException e) {
                throw new RuntimeException(e);
            }*/

            //openDecoder("video/hevc", 1920, 1080);
            openDecoder("video/avc", 1920, 1080);

            ByteBuffer[] inputBuffers = decoder.getInputBuffers();
            ByteBuffer[] outputBuffers = decoder.getOutputBuffers();
            MediaCodec.BufferInfo info = new MediaCodec.BufferInfo();
            boolean isEOS = false;
            long startMs = System.currentTimeMillis();
            long startNano = System.nanoTime();

            //int init_rec = StreamReceiver.init();

            //int streamLen = StreamParser.initStream(SAMPLE);
            //int Pos = 0;
            long lastMs = -1;
            while (running) {
                //int sliceLen = StreamParser.nextNalLen(Pos);
                int sliceLen = StreamReceiver.NextOnePicLen();
                Log.d("DecodeActivity", "sliceLen: " + sliceLen);

                int retry_times = 10;

                if(sliceLen <= 0)
                    continue;

                while(retry_times-- >= 0) {
                    if (!isEOS) {
                        int inIndex = decoder.dequeueInputBuffer(10000);
                        if (inIndex >= 0) {
                            ByteBuffer buffer = inputBuffers[inIndex];

                            /*StreamParser.readOneNal(buffer, Pos, sliceLen);

                            byte[] bytesArray = new byte[sliceLen];
                            buffer.get(bytesArray, 0, bytesArray.length);

                            try {
                                fos2.write(bytesArray);
                            } catch (IOException e) {
                                throw new RuntimeException(e);
                            }

                            Pos += sliceLen;*/


                            if (sliceLen <= 0) {
                                // We shouldn't stop the playback at this point, just pass the EOS
                                // flag to decoder, we will get it again from the
                                // dequeueOutputBuffer
                                //Log.d("DecodeActivity", "InputBuffer BUFFER_FLAG_END_OF_STREAM");
                                //ecoder.queueInputBuffer(inIndex, 0, 0, 0, MediaCodec.BUFFER_FLAG_END_OF_STREAM);
                                //isEOS = true;
                            } else {
                                StreamReceiver.readOnePic(buffer);
                                /*byte[] bytesArray = new byte[sliceLen];
                                buffer.get(bytesArray, 0, bytesArray.length);

                                try {
                                    fos2.write(bytesArray);
                                } catch (IOException e) {
                                    throw new RuntimeException(e);
                                }*/
                                if(lastMs == -1)
                                    lastMs = System.currentTimeMillis();
                                else
                                {
                                    while(System.currentTimeMillis() - lastMs < 3)
                                    {
                                        Log.v("DecodeActivity", "before sleep... ");
                                        try {
                                            sleep(2);
                                        } catch (InterruptedException e) {
                                            throw new RuntimeException(e);
                                        }
                                    }
                                    lastMs = System.currentTimeMillis();
                                }
                                long pt = (System.nanoTime() - startNano)/1000;
                                decoder.queueInputBuffer(inIndex, 0, sliceLen, pt, 0);
                                Log.d("DecodeActivity", "pt: " + pt);

                            }

                            break;
                        }
                    }
                }

                if(retry_times <= 0){
                    ReStartDecoder();
                    continue;
                }

                while(running)
                {
                    int outIndex = decoder.dequeueOutputBuffer(info, 10000);
                    if(outIndex >= 0)
                    {
                        ByteBuffer buffer = outputBuffers[outIndex];
                        Log.v("DecodeActivity", "We can't use this buffer but render it due to the API limit, " + buffer);

                        // We use a very simple clock to keep the video FPS, or the video
                        // playback will be too fast
                        long pt = System.currentTimeMillis() - startMs;
                        while (info.presentationTimeUs / 1000 > pt) {
                            try {
                                Log.v("DecodeActivity", "sleep... ");
                                sleep(2);
                            } catch (InterruptedException e) {
                                e.printStackTrace();
                                break;
                            }
                        }
                        decoder.releaseOutputBuffer(outIndex, true);
                    } else if (outIndex == MediaCodec.INFO_OUTPUT_BUFFERS_CHANGED) {
                        Log.d("DecodeActivity", "INFO_OUTPUT_BUFFERS_CHANGED");
                        outputBuffers = decoder.getOutputBuffers();
                        break;
                    } else if (outIndex == MediaCodec.INFO_OUTPUT_FORMAT_CHANGED) {
                        Log.d("DecodeActivity", "New format " + decoder.getOutputFormat());
                        break;
                    } else if (outIndex == MediaCodec.INFO_TRY_AGAIN_LATER) {
                        Log.d("DecodeActivity", "dequeueOutputBuffer timed out!");
                        break;
                    }else{
                        Log.d("DecodeActivity", "dequeueOutputBuffer error!");
                        break;
                    }

                    // All decoded frames have been rendered, we can stop playing now
                    if ((info.flags & MediaCodec.BUFFER_FLAG_END_OF_STREAM) != 0) {
                        Log.d("DecodeActivity", "OutputBuffer BUFFER_FLAG_END_OF_STREAM");
                        signal();
                        break;
                    }
                }
            }
            Log.d("DecodeActivity", "closeDecoder");
            closeDecoder();

            boolean stop_rec = StreamReceiver.videostop();
            Log.d("DecodeActivity", "StreamReceiver.videostop return: " + stop_rec);


            // 释放资源
            /*try {
                fos2.close();
            } catch (IOException e) {
                throw new RuntimeException(e);
            }*/
        }
    }
}

