# 基于并行BP神经网络的人脸识别系统

此为 **QT版** 的人脸识别系统

并行版请戳：[Github . PAC-P2P/BPNN-Face-Recognition-For-Parallel](https://github.com/PAC-P2P/BPNN-Face-Recognition-For-Parallel)

串行版请戳：[Github . PAC-P2P/BPNN-Face-Recognition](https://github.com/PAC-P2P/BPNN-Face-Recognition)

Qt 版请戳：[Github . PAC-P2P/BPNN-Face-Recognition-For-Qt](https://github.com/PAC-P2P/BPNN-Face-Recognition-For-Qt)

## 注意事项

* 确保程序目录结构正确（如下**目录结构**）。
* 确保训练集与测试集（包括识别）的图片分辨率相同。
* `User ID`必须唯一，否则会覆盖原用户数据。

## 目录结构

请保持以下目录结构，以便程序能正常运行。

	.
	├── BPNN-Face-Recognition.pro
	├── Release
	│   └── BPNN-Face-Recognition.app
	├── data
	│   ├── facenet.net
	│   ├── images
	│   │   ├── imagesRec
	│   │   │   └── rec_01.pgm
	│   │   └── imagesTrain
	│   │       ├── an2i_1_1.pgm
	│   │       ├── ...(images)
	│   │       └── cheyer_8_3.pgm
	│   ├── imagesSet_rec.list
	│   ├── imagesSet_testAll.list
	│   └── straightrnd_train.list
	└── src
    	├── backprop.cpp
    	├── backprop.h
    	├── camera.cpp
    	├── camera.h
    	├── camera.ui
    	├── dir.cpp
    	├── dir.h
    	├── facetrain.cpp
    	├── facetrain.h
    	├── imagenet.cpp
    	├── imagenet.h
    	├── imagesettings.cpp
    	├── imagesettings.h
    	├── imagesettings.ui
    	├── main.cpp
    	├── mainwindow.cpp
    	├── mainwindow.h
    	├── mainwindow.ui
    	├── pgmimage.cpp
    	├── pgmimage.h
    	├── src.pro
    	├── trainingsetting.cpp
    	├── trainingsetting.h
    	└── trainingsetting.ui

## 数据集

[Neural Networks for Face Recognition](http://www.cs.cmu.edu/afs/cs.cmu.edu/user/mitchell/ftp/faces.html)