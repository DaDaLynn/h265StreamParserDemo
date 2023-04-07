echo off
adb push ./libs/armeabi-v7a/H265StreamParserDemo32 /data/local/tmp/Lynn
adb push ./libs/armeabi-v7a/libH265StreamParser.so /data/local/tmp/Lynn
adb push ../1920x1080_yuv420p_test0.265 /data/local/tmp/Lynn
adb shell "cd /data/local/tmp/Lynn && export LD_LIBRARY_PATH=./ && chmod +x H265StreamParserDemo32 && ./H265StreamParserDemo32"
adb pull /data/local/tmp/Lynn/out.265 ./out_arm.265
echo  ------------------------------End----------------------------------------------
pause