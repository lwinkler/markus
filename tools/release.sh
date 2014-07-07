#! /bin/bash



function deploy {
	read -r -p "Do you want to deploy "$TARGET" on quality ? [y/N] " response
	if [[ $response =~ ^[Yy]$ ]] ; then
		scp -r ${TARGET} vp@smartaid.quality:~
	fi

	read -r -p "Do you want to release "$TARGET" ? [y/N] " response
	if [[ $response =~ ^[Yy]$ ]] ; then
		scp -r ${TARGET} /mnt/releases/Morphean/VideoAID/markus
	fi
}

#### BEGIN ####
if [ "$#" -ne 2 ]; then
	echo "usage: $0 <branch> <tag>"
	exit
fi

MARKUS_RELEASE=$1
MARKUS_TAG=$2
#`date "+%Y-%m-%d_%H%M%S"`

# Quit on the first error

# Preparation
read -r -p "Do you want to clone the Markus repository to a fresh directory ./markus_v$MARKUS_RELEASE ? [y/N] " response
if [[ $response =~ ^[Yy]$ ]] ; then
	git clone git@github.com:lwinkler/markus.git markus_v$MARKUS_RELEASE
	cd markus_v$MARKUS_RELEASE

	
	read -r -p "Do you want to clone the vp-detection repository ? [y/N] " response
	if [[ $response =~ ^[Yy]$ ]] ; then
		git clone git@github.com:softcom-technologies/vp-detection.git
		ln -s vp-detection/markus/projects projects2
		ln -s vp-detection/markus/modules modules2
	fi
fi


read -r -p "Do you want to create a new branch $MARKUS_RELEASE ? [y/N] " response
if [[ $response =~ ^[Yy]$ ]] ; then
	git checkout -b $MARKUS_RELEASE
	git push origin HEAD
	cd vp-detection
	git checkout -b $MARKUS_RELEASE
	git push origin HEAD
	cd ..
else
	git checkout $MARKUS_RELEASE
	git pull
	cd vp-detection
	git checkout $MARKUS_RELEASE
	git pull
	cd ..
fi

read -r -p "Do you want to create a new tag $MARKUS_TAG ? [y/N] " response
if [[ $response =~ ^[Yy]$ ]] ; then
	git tag $MARKUS_TAG
	git push origin $MARKUS_TAG
	cd vp-detection
	git tag $MARKUS_TAG
	git push origin $MARKUS_TAG
	cd ..
else
	if [[ `git describe --tag` != $MARKUS_TAG ]]; then
		echo "Warning: Markus repo must be on tag $MARKUS_TAG and not"
		git describe --tag
	fi
	cd vp-detection
	if [[ `git describe --tag` != $MARKUS_TAG ]]; then
		echo "Warning: vp-detection repo must be on tag $MARKUS_TAG and not"
		git describe --tag
	fi
	cd ..
fi


read -r -p "Do you want to compile ? [y/N] " response
if [[ $response =~ ^[Yy]$ ]] ; then
	cmake . -DCMAKE_BUILD_TYPE=Release -DMarkus_NO_GUI=ON
	make clean
	make -j9 update_modules_list
	make update_projects_list
fi

echo "Check version of executable:"
./markus -v
read -r -p "Is the version number correct ? [y/N] " response
if [[ $response =~ ^[Nn]$ ]] ; then
	echo "Please fix the version number."
	exit
fi


read -r -p "Do you want to run unit tests ? [y/N] " response
if [[ $response =~ ^[Yy]$ ]] ; then
	make tests
fi

# Release all kinds of detectors

# fall

TARGET=fall_v${MARKUS_TAG}
rm -rf ${TARGET}
mkdir ${TARGET}
scp -r markus projects2/FallDetectionFromVP.xml modules2/FilterPython/vignettes log4cxx.min.xml projects2/Calibration ${TARGET}

deploy


# motion

TARGET=motion_v${MARKUS_TAG}
rm -rf ${TARGET}
mkdir ${TARGET}
scp -r markus projects2/MotionDetectorFromVP.xml log4cxx.min.xml ${TARGET}

deploy

