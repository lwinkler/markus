find videos/Fall_videos_Softcom_1/videos -name "*M3204*.mp4" | sort | xargs -i{} echo ./markus -nc projects2/FallDetection.write.xml -p Input.file=\"\{}\" >| cmd
# cat cmd | parallel
