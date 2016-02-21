export LD_LIBRARY_PATH=/usr/lib
/usr/bin/mjpg_streamer -i "/usr/lib/input_uvc.so -d /dev/video0" -o "/usr/lib/output_http.so -w /var/www"

