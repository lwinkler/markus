#! /bin/bash



function deploy {
	read -r -p "Do you want to deploy version "$TARGET" ? [y/N] " response
	case $response in
		[yY][eE][sS]|[yY]) 

		scp -r ${TARGET} vp@smartaid.quality:~
		scp -r ${TARGET} /mnt/releases/Morphean/VideoAID

		;;
		*)
		;;
	esac
}

MARKUS_RELEASE="v0.0.3_"
#`date "+%Y-%m-%d_%H%M%S"`

read -r -p "Do you want to release version "$MARKUS_RELEASE" ? [y/N] " response
case $response in
	[yY][eE][sS]|[yY]) 
	;;
	*)
	exit
	;;
esac

# fall

TARGET=fall_${MARKUS_RELEASE}
rm -rf ${TARGET}
mkdir ${TARGET}
scp -r markus projects2/FallDetectionFromVP.xml modules2/FilterPython/vignettes log4cxx.min.xml ${TARGET}

deploy


# motion

TARGET=motion_${MARKUS_RELEASE}
rm -rf ${TARGET}
mkdir ${TARGET}
scp -r markus projects2/MotionDetectorFromVP.xml log4cxx.min.xml ${TARGET}


deploy
