/********************************************************************
	功能 :	摄像机标定与校正类实现代码;
*********************************************************************/
#include "StdAfx.h"
#include "StereoCalib.h"

StereoCalib::StereoCalib(void)
{
}


StereoCalib::~StereoCalib(void)
{
}


//	功能 : 初始化棋盘角点数据信息;
int StereoCalib::initCornerData(int nImages, cv::Size imageSize, cv::Size boardSize, float squareWidth, CornerDatas& cornerDatas)
{
	cornerDatas.nImages = nImages;
	cornerDatas.imageSize = imageSize;
	cornerDatas.boardSize = boardSize;
	cornerDatas.nPoints = nImages * boardSize.width * boardSize.height;
	cornerDatas.nPointsPerImage = boardSize.width * boardSize.height;
	cornerDatas.objectPoints.resize(nImages, vector<cv::Point3f>(cornerDatas.nPointsPerImage, cv::Point3f(0,0,0)));
	cornerDatas.imagePoints1.resize(nImages, vector<cv::Point2f>(cornerDatas.nPointsPerImage, cv::Point2f(0,0)));
	cornerDatas.imagePoints2.resize(nImages, vector<cv::Point2f>(cornerDatas.nPointsPerImage, cv::Point2f(0,0)));

	//	计算棋盘角点的世界坐标值;
	int i,j,k,n;
	for( i = 0; i < nImages; i++ )
	{
		n = 0;
		for( j = 0; j < boardSize.height; j++ )
			for( k = 0; k < boardSize.width; k++ )
				cornerDatas.objectPoints[i][n++] = cv::Point3f(j*squareWidth, k*squareWidth, 0);
	}

	return 1;
}

//	功能 : 根据成功检测的棋盘数目修改棋盘角点数据;
int StereoCalib::resizeCornerData(int nImages, CornerDatas& cornerDatas)
{
	cornerDatas.nImages = nImages;
	cornerDatas.nPoints = nImages * cornerDatas.nPointsPerImage;
	cornerDatas.objectPoints.resize(nImages);
	cornerDatas.imagePoints1.resize(nImages);
	cornerDatas.imagePoints2.resize(nImages);

	return 1;
}

//	功能 : 载入棋盘角点数据信息;
int StereoCalib::loadCornerData(const char* filename, CornerDatas& cornerDatas)
{
	cv::FileStorage fs(filename, cv::FileStorage::READ);
	if (fs.isOpened())
	{		
		fs["nPoints"]			>> cornerDatas.nPoints;
		fs["nImages"]			>> cornerDatas.nImages;
		fs["nPointsPerImage"]	>> cornerDatas.nPointsPerImage;

		cv::FileNodeIterator it = fs["imageSize"].begin(); 
		it >> cornerDatas.imageSize.width >> cornerDatas.imageSize.height;

		cv::FileNodeIterator bt = fs["boardSize"].begin(); 
		bt >> cornerDatas.boardSize.width >> cornerDatas.boardSize.height;
		
		for (int i=0; i<cornerDatas.nImages;i++) 
		{
			std::stringstream imagename; 
			imagename << "image" << i;

			cv::FileNode img = fs[imagename.str()];
			vector<cv::Point3f> ov;
			vector<cv::Point2f> iv1, iv2;
			for (int j=0; j<cornerDatas.nPointsPerImage; j++) 
			{
				std::stringstream nodename; 
				nodename << "node" << j;

				cv::FileNode pnt = img[nodename.str()];
				cv::Point3f op;
				cv::Point2f ip1, ip2;
				cv::FileNodeIterator ot = pnt["objectPoints"].begin(); 
				ot >> op.x >> op.y >> op.z;
				cv::FileNodeIterator it1 = pnt["imagePoints1"].begin(); 
				it1 >> ip1.x >> ip1.y;
				cv::FileNodeIterator it2 = pnt["imagePoints2"].begin(); 
				it2 >> ip2.x >> ip2.y;

				iv1.push_back(ip1);
				iv2.push_back(ip2);
				ov.push_back(op);
			}
			cornerDatas.imagePoints1.push_back(iv1);
			cornerDatas.imagePoints2.push_back(iv2);
			cornerDatas.objectPoints.push_back(ov);
		}

		fs.release();
		return 1;
	} 
	else
	{
		return 0;
	}
}


//	功能 : 保存棋盘角点数据信息;
int StereoCalib::saveCornerData(const char* filename, const CornerDatas& cornerDatas)
{
	cv::FileStorage fs(filename, cv::FileStorage::WRITE);
	if (fs.isOpened())
	{
		time_t rawtime; 
		time(&rawtime);
		fs << "calibrationDate" << asctime(localtime(&rawtime));
		
		fs << "nPoints"			<< cornerDatas.nPoints;
		fs << "nImages"			<< cornerDatas.nImages;
		fs << "nPointsPerImage" << cornerDatas.nPointsPerImage;

		fs << "imageSize" << "[" << cornerDatas.imageSize.width << cornerDatas.imageSize.height << "]";

		fs << "boardSize" << "[" << cornerDatas.boardSize.width << cornerDatas.boardSize.height << "]";

		for (int i=0; i<cornerDatas.nImages;i++) 
		{
			std::stringstream imagename; 
			imagename << "image" << i;

			fs << imagename.str() << "{";

			for (int j=0; j<cornerDatas.nPointsPerImage; j++) 
			{
				std::stringstream nodename; 
				nodename << "node" << j;

				fs << nodename.str() << "{";

				cv::Point3f op = cornerDatas.objectPoints[i][j];
				cv::Point2f ip1 = cornerDatas.imagePoints1[i][j];
				cv::Point2f ip2 = cornerDatas.imagePoints2[i][j];

				fs << "objectPoints" << "[:"; 
				fs << op.x << op.y << op.z << "]";

				fs << "imagePoints1" << "[:"; 
				fs << ip1.x << ip1.y << "]";

				fs << "imagePoints2" << "[:"; 
				fs << ip2.x << ip2.y << "]";

				fs << "}";
			}

			fs << "}";
		}

		fs.release();
		return 1;
	} 
	else
	{
		return 0;
	}
}


//	功能 : 检测棋盘角点;
int StereoCalib::detectCorners(cv::Mat& img1, cv::Mat& img2, CornerDatas& cornerDatas, int imageCount)
{
	bool stereoMode = true;
	if (img2.empty())	stereoMode = false;

	vector<cv::Point2f>& corners1 = cornerDatas.imagePoints1[imageCount];
	vector<cv::Point2f>& corners2 = cornerDatas.imagePoints2[imageCount];

	bool found1 = false, found2 = true;
	int flags = cv::CALIB_CB_ADAPTIVE_THRESH + cv::CALIB_CB_NORMALIZE_IMAGE + cv::CALIB_CB_FAST_CHECK;

	found1 = findChessboardCorners(img1, cornerDatas.boardSize, corners1, flags);
	if (stereoMode) 
		found2 = findChessboardCorners(img2, cornerDatas.boardSize, corners2, flags);

	if ( found1 && found2 )
	{
		cv::Mat gray1, gray2;
		cvtColor(img1, gray1, cv::COLOR_RGB2GRAY);
		if (stereoMode) 
			cvtColor(img2, gray2, cv::COLOR_RGB2GRAY);

		cv::Size regionSize(11, 11);
		cornerSubPix(gray1, corners1, regionSize, cv::Size(-1,-1), cv::TermCriteria(cv::TermCriteria::EPS|cv::TermCriteria::MAX_ITER, 30, 0.05));
		if (stereoMode) 
			cornerSubPix(gray2, corners2, regionSize, cv::Size(-1,-1), cv::TermCriteria(cv::TermCriteria::EPS|cv::TermCriteria::MAX_ITER, 30, 0.05));
	}

	drawChessboardCorners(img1, cornerDatas.boardSize, corners1, found1);
	if (stereoMode) 
		drawChessboardCorners(img2, cornerDatas.boardSize, corners2, found2);

	char info[10];
	sprintf_s(info, "%02d/%02d", imageCount+1, cornerDatas.nImages);
	string text = info;
	showText(img1, text);
	if (stereoMode)	showText(img2, text);

	if (stereoMode)
		return (found1 && found2) ? 1 : 0;
	else
		return found1 ? 1 : 0;
}


//	功能 : 在图像右下角显示指定文字信息;
void StereoCalib::showText(cv::Mat& img, string text)
{
	int fontFace = cv::FONT_HERSHEY_PLAIN;
	double fontScale = 1;
	int fontThickness = 2;

	int textBaseline=0;
	cv::Size textSize = cv::getTextSize(text, fontFace, fontScale, fontThickness, &textBaseline);
	textBaseline += fontThickness;

	cv::Point textOrg((img.cols - textSize.width - 10), (img.rows - textSize.height - 10));

	rectangle(img, textOrg + cv::Point(0, textBaseline), textOrg + cv::Point(textSize.width, -textSize.height), cv::Scalar(0,255,0));

	line(img, textOrg + cv::Point(0, fontThickness), textOrg + cv::Point(textSize.width, fontThickness), cv::Scalar(0,255,0));

	putText(img, text, textOrg, fontFace, fontScale, cv::Scalar(0,0,255), fontThickness, 8);
}


//	功能 : 载入已标定好的摄像机内部参数;
int StereoCalib::loadCameraParams(const char* filename, CameraParams& cameraParams)
{
	cv::FileStorage fs(filename, cv::FileStorage::READ);
	if (fs.isOpened())
	{
		cv::FileNodeIterator it = fs["imageSize"].begin(); 
		it >> cameraParams.imageSize.width >> cameraParams.imageSize.height;

		fs["cameraMatrix"]				>> cameraParams.cameraMatrix;
		fs["distortionCoefficients"]	>> cameraParams.distortionCoefficients;
		fs["flags"]						>> cameraParams.flags;

		int nImages = 0;
		fs["nImages"] >> nImages;

		for (int i = 0; i < nImages; i++)
		{
			char matName[50];
			sprintf_s(matName, "rotaionMatrix_%d", i);

			cv::Mat rotMat;
			fs[matName] >> rotMat;
			cameraParams.rotations.push_back(rotMat);
		}

		for (int i = 0; i < nImages; i++)
		{
			char matName[50];
			sprintf_s(matName, "translationMatrix_%d", i);

			cv::Mat tranMat;
			fs[matName] >> tranMat;
			cameraParams.translations.push_back(tranMat);
		}

		fs.release();
		return 1;
	} 
	else
	{
		return 0;
	}
}


//	功能 : 保存已标定好的摄像机内部参数;
int StereoCalib::saveCameraParams(const CameraParams& cameraParams, const char* filename /* = "cameraParams.yml" */)
{
	std::string filename_ = filename;

	if (filename_ == "")
	{
		int strLen = 20;
		char *pCurrTime = (char*)malloc(sizeof(char)*strLen);
		memset(pCurrTime, 0, sizeof(char)*strLen);
		time_t now;
		time(&now);
		strftime(pCurrTime, strLen , "%Y_%m_%d_%H_%M_%S_", localtime(&now));

		filename_ =  pCurrTime;
		filename_ += "cameraParams.yml";
	}

	cv::FileStorage fs(filename_.c_str(), cv::FileStorage::WRITE);
	if (fs.isOpened())
	{
		time_t rawtime; 
		time(&rawtime);
		fs << "calibrationDate" << asctime(localtime(&rawtime));

		char flagText[1024];
		sprintf_s( flagText, "flags: %s%s%s%s%s",
			cameraParams.flags & cv::CALIB_FIX_K3 ? "fix_k3" : "",
			cameraParams.flags & cv::CALIB_USE_INTRINSIC_GUESS ? " + use_intrinsic_guess" : "",
			cameraParams.flags & cv::CALIB_FIX_ASPECT_RATIO ? " + fix_aspect_ratio" : "",
			cameraParams.flags & cv::CALIB_FIX_PRINCIPAL_POINT ? " + fix_principal_point" : "",
			cameraParams.flags & cv::CALIB_ZERO_TANGENT_DIST ? " + zero_tangent_dist" : "" );
		cvWriteComment(*fs, flagText, 0);
		
		fs << "flags"					<< cameraParams.flags;

		fs << "imageSize" << "[" << cameraParams.imageSize.width << cameraParams.imageSize.height << "]";
		
		fs << "cameraMatrix"			<< cameraParams.cameraMatrix;
		fs << "distortionCoefficients"	<< cameraParams.distortionCoefficients;

		int nImages = cameraParams.rotations.size();
		fs << "nImages"	<< nImages;
		for (UINT i = 0; i < nImages; i++)
		{
			char matName[50];
			sprintf_s(matName, "rotaionMatrix_%d", i);

			fs << matName << cameraParams.rotations[i];
		}
		for (UINT i = 0; i < nImages; i++)
		{
			char matName[50];
			sprintf_s(matName, "translationMatrix_%d", i);

			fs << matName << cameraParams.translations[i];
		}

		fs.release();
		return 1;
	} 
	else
	{
		return 0;
	}
}


//	功能 : 执行单目摄像机标定, 注意此函数不能在 calibrateStereoCamera 中调用，只适合外部调用, 棋盘角点的图像坐标默认存放在 cornerDatas.imagePoints1 中;
int StereoCalib::calibrateSingleCamera(CornerDatas& cornerDatas, CameraParams& cameraParams)
{
	cameraParams.imageSize = cornerDatas.imageSize;

	cv::calibrateCamera(
		cornerDatas.objectPoints, 
		cornerDatas.imagePoints1, 
		cornerDatas.imageSize, 
		cameraParams.cameraMatrix, 
		cameraParams.distortionCoefficients,
		cameraParams.rotations, 
		cameraParams.translations,
		cameraParams.flags
		);

	return 1;
}


//	功能 : 执行双目摄像机标定, 若每个摄像机尚未标定，则首先进行单目标定，再进行双目标定;
int StereoCalib::calibrateStereoCamera(CornerDatas& cornerDatas, StereoParams& stereoParams, bool cameraUncalibrated /* = false */)
{
	if (cameraUncalibrated)
	{
//	执行单目定标;
		cv::calibrateCamera(
			cornerDatas.objectPoints, cornerDatas.imagePoints1, cornerDatas.imageSize, 
			stereoParams.cameraParams1.cameraMatrix, stereoParams.cameraParams1.distortionCoefficients,
			stereoParams.cameraParams1.rotations, stereoParams.cameraParams1.translations,
			stereoParams.cameraParams1.flags
			);

		cv::calibrateCamera(
			cornerDatas.objectPoints, cornerDatas.imagePoints2, cornerDatas.imageSize, 
			stereoParams.cameraParams2.cameraMatrix, stereoParams.cameraParams2.distortionCoefficients,
			stereoParams.cameraParams2.rotations, stereoParams.cameraParams2.translations,
			stereoParams.cameraParams2.flags
			);
		saveCameraParams(stereoParams.cameraParams1, (m_workDir + "cameraParams_left.yml").c_str());
		saveCameraParams(stereoParams.cameraParams2, (m_workDir + "cameraParams_right.yml").c_str());
	}

	stereoParams.imageSize = cornerDatas.imageSize;

	cv::stereoCalibrate(
		cornerDatas.objectPoints, cornerDatas.imagePoints1, cornerDatas.imagePoints2,
		stereoParams.cameraParams1.cameraMatrix, stereoParams.cameraParams1.distortionCoefficients,
		stereoParams.cameraParams2.cameraMatrix, stereoParams.cameraParams2.distortionCoefficients,
		cornerDatas.imageSize,
		stereoParams.rotation, stereoParams.translation, stereoParams.essential, stereoParams.foundational,
		cv::TermCriteria(cv::TermCriteria::COUNT+cv::TermCriteria::EPS, 30, 1e-6),
		stereoParams.flags + cv::CALIB_FIX_K3 + cv::CALIB_FIX_K4 + cv::CALIB_FIX_K5
		);

	return 1;
}


//	功能 : 计算单目标定误差;
int StereoCalib::getCameraCalibrateError(vector<vector<cv::Point3f> >& _objectPoints, vector<vector<cv::Point2f> >& _imagePoints, CameraParams& cameraParams, double& err)
{
	cv::Mat imagePoints2;
	int totalPoints = 0;
	double totalErr = 0;
	size_t nImages = _objectPoints.size();

	for (int i = 0; i < nImages; i++)
	{
		vector<cv::Point3f>& objectPoints = _objectPoints[i];
		vector<cv::Point2f>& imagePoints = _imagePoints[i];
		totalPoints += objectPoints.size();

		projectPoints(
			objectPoints, cameraParams.rotations[i], cameraParams.translations[i],
			cameraParams.cameraMatrix, cameraParams.distortionCoefficients, imagePoints2);

        cv::Mat imagePoints1 = cv::Mat(imagePoints);
		double erri = norm(imagePoints1, imagePoints2, cv::NORM_L2);
		totalErr += erri * erri;
	}

	err = std::sqrt(totalErr / totalPoints);

	return 1;
}


//	功能 : 计算双目标定误差;
int StereoCalib::getStereoCalibrateError(CornerDatas& cornerDatas, StereoParams& sterepParams, double& err)
{
	//	利用对极线约束检查立体校正效果;
	vector<cv::Vec3f> epilines[2];
	vector<vector<cv::Point2f> > imagePoints[2];
	cv::Mat cameraMatrix[2], distCoeffs[2];
	int npoints = 0;
	int i,j,k;

	imagePoints[0] = cornerDatas.imagePoints1;
	imagePoints[1] = cornerDatas.imagePoints2;
	cameraMatrix[0] = sterepParams.cameraParams1.cameraMatrix;
	cameraMatrix[1] = sterepParams.cameraParams2.cameraMatrix;
	distCoeffs[0] = sterepParams.cameraParams1.distortionCoefficients;
	distCoeffs[1] = sterepParams.cameraParams2.distortionCoefficients;

	for( i = 0; i < cornerDatas.nImages; i++ )
	{
		int npt = (int)imagePoints[0][i].size();
		cv::Mat imgpt[2];

		for( k = 0; k < 2; k++ )
		{
			imgpt[k] = cv::Mat(imagePoints[k][i]);
			undistortPoints(imgpt[k], imgpt[k], cameraMatrix[k], distCoeffs[k], cv::Mat(), cameraMatrix[k]);
			computeCorrespondEpilines(imgpt[k], k+1, sterepParams.foundational, epilines[k]);
		}
		for( j = 0; j < npt; j++ )
		{
			double errij = fabs(imagePoints[0][i][j].x * epilines[1][j][0] + imagePoints[0][i][j].y * epilines[1][j][1] + epilines[1][j][2]) +
				fabs(imagePoints[1][i][j].x * epilines[0][j][0] + imagePoints[1][i][j].y * epilines[0][j][1] + epilines[0][j][2]);
			err += errij;
		}
		npoints += npt;
	}
	err /= npoints;

	return 1;
}


//	功能 : 生成单个摄像头的校正矩阵;
int StereoCalib::rectifySingleCamera(CameraParams& cameraParams, RemapMatrixs& remapMatrixs)
{
	cv::initUndistortRectifyMap(cameraParams.cameraMatrix, cameraParams.distortionCoefficients, cv::Mat(),
		getOptimalNewCameraMatrix(cameraParams.cameraMatrix, cameraParams.distortionCoefficients, cameraParams.imageSize, 1, cameraParams.imageSize, 0),
		cameraParams.imageSize, CV_16SC2, remapMatrixs.mX1, remapMatrixs.mY1);

	return 1;
}


//	功能 : 执行双目摄像机校正，生成双目校正数据;
 int StereoCalib::rectifyStereoCamera(CornerDatas& cornerDatas, StereoParams& stereoParams, RemapMatrixs& remapMatrixs, RECTIFYMETHOD method)
{
	remapMatrixs.mX1 = cv::Mat(stereoParams.imageSize, CV_32FC1);
	remapMatrixs.mY1 = cv::Mat(stereoParams.imageSize, CV_32FC1);
	remapMatrixs.mX2 = cv::Mat(stereoParams.imageSize, CV_32FC1);
	remapMatrixs.mY2 = cv::Mat(stereoParams.imageSize, CV_32FC1);

	cv::Mat R1, R2, P1, P2, Q;
	cv::Rect roi1, roi2;
    if (stereoParams.alpha < 0 || stereoParams.alpha > 1)
        stereoParams.alpha = -1;

	//	执行双目校正;
	stereoRectify(
		stereoParams.cameraParams1.cameraMatrix, stereoParams.cameraParams1.distortionCoefficients,
		stereoParams.cameraParams2.cameraMatrix, stereoParams.cameraParams2.distortionCoefficients,
		stereoParams.imageSize, stereoParams.rotation, stereoParams.translation,
		R1,R2, P1, P2, Q,
		cv::CALIB_ZERO_DISPARITY,
		stereoParams.alpha, 
		stereoParams.imageSize,
		&roi1, &roi2);

	//使用HARTLEY方法的额外处理
	if (method == RECTIFY_HARTLEY)
	{
		cv::Mat F, H1, H2;
		F = findFundamentalMat(
			cornerDatas.imagePoints1,
			cornerDatas.imagePoints2,
			cv::FM_8POINT, 0, 0);
		stereoRectifyUncalibrated(
			cornerDatas.imagePoints1,
			cornerDatas.imagePoints2,
			F, stereoParams.imageSize, H1, H2, 3);

		R1 = stereoParams.cameraParams1.cameraMatrix.inv() * H1 * stereoParams.cameraParams1.cameraMatrix;
		R2 = stereoParams.cameraParams2.cameraMatrix.inv() * H2 * stereoParams.cameraParams2.cameraMatrix;
		P1 = stereoParams.cameraParams1.cameraMatrix;
		P2 = stereoParams.cameraParams2.cameraMatrix;
	}

	//	生成图像校正所需的像素映射矩阵;
	initUndistortRectifyMap(
		stereoParams.cameraParams1.cameraMatrix, stereoParams.cameraParams1.distortionCoefficients,
		R1, P1, 
		stereoParams.imageSize,
		CV_16SC2,
		remapMatrixs.mX1, remapMatrixs.mY1);

	initUndistortRectifyMap(
		stereoParams.cameraParams2.cameraMatrix, stereoParams.cameraParams2.distortionCoefficients,
		R2, P2, 
		stereoParams.imageSize,
		CV_16SC2,
		remapMatrixs.mX2, remapMatrixs.mY2);

	Q.copyTo(remapMatrixs.Q);
	remapMatrixs.roi1 = roi1;
	remapMatrixs.roi2 = roi2;

	return 1;
}


//	功能 : 保存双目校正参数;
int StereoCalib::saveCalibrationDatas(const char* filename, RECTIFYMETHOD method, CornerDatas& cornerDatas, StereoParams& stereoParams, RemapMatrixs& remapMatrixs)
{
	cv::FileStorage fs(filename, cv::FileStorage::WRITE);

	if (fs.isOpened())
	{
		time_t rawtime; 
		time(&rawtime);
		fs << "calibrationDate" << asctime(localtime(&rawtime));

		fs << "num_boards"	<< cornerDatas.nImages;
		fs << "imageSize" << "[" << cornerDatas.imageSize.width << cornerDatas.imageSize.height << "]";

		char flagText[1024];
		sprintf_s( flagText, "flags: %s%s%s%s%s",
			stereoParams.flags & cv::CALIB_USE_INTRINSIC_GUESS ? "+ use_intrinsic_guess" : "",
			stereoParams.flags & cv::CALIB_FIX_ASPECT_RATIO ? " + fix_aspect_ratio" : "",
			stereoParams.flags & cv::CALIB_FIX_PRINCIPAL_POINT ? " + fix_principal_point" : "",
			stereoParams.flags & cv::CALIB_FIX_INTRINSIC ? " + fix_intrinsic" : "",
			stereoParams.flags & cv::CALIB_SAME_FOCAL_LENGTH ? " + same_focal_length" : "" );

		cvWriteComment(*fs, flagText, 0);

		fs << "stereoCalibrateFlags"		<< stereoParams.flags;
		fs << "leftCameraMatrix"			<< stereoParams.cameraParams1.cameraMatrix;
		fs << "leftDistortCoefficients"		<< stereoParams.cameraParams1.distortionCoefficients;
		fs << "rightCameraMatrix"			<< stereoParams.cameraParams2.cameraMatrix;
		fs << "rightDistortCoefficients"	<< stereoParams.cameraParams2.distortionCoefficients;
		fs << "rotationMatrix"				<< stereoParams.rotation;
		fs << "translationVector"			<< stereoParams.translation;
		fs << "foundationalMatrix"			<< stereoParams.foundational;

		if (method == RECTIFY_BOUGUET)
		{
			fs << "rectifyMethod" << "BOUGUET";
			fs << "leftValidArea" << "[:"
				<< remapMatrixs.roi1.x << remapMatrixs.roi1.y
				<< remapMatrixs.roi1.width << remapMatrixs.roi1.height << "]";
			fs << "rightValidArea" << "[:"
				<< remapMatrixs.roi2.x << remapMatrixs.roi2.y
				<< remapMatrixs.roi2.width << remapMatrixs.roi2.height << "]";
			fs << "QMatrix" << remapMatrixs.Q;
		}
		else
			fs << "rectifyMethod" << "HARTLEY";

		fs << "remapX1" << remapMatrixs.mX1;
		fs << "remapY1" << remapMatrixs.mY1;
		fs << "remapX2" << remapMatrixs.mX2;
		fs << "remapY2" << remapMatrixs.mY2;

		fs.release();
		return 1;
	} 
	else
	{
		return 0;
	}
}


//	功能 : 对图像进行校正;
int StereoCalib::remapImage(cv::Mat& img1, cv::Mat& img2, cv::Mat& img1r, cv::Mat& img2r, RemapMatrixs& remapMatrixs)
{
	if ( !remapMatrixs.mX1.empty() && !remapMatrixs.mY1.empty() )
		cv::remap( img1, img1r, remapMatrixs.mX1, remapMatrixs.mY1, cv::INTER_LINEAR );

	if ( !remapMatrixs.mX2.empty() && !remapMatrixs.mY2.empty() )
		cv::remap( img2, img2r, remapMatrixs.mX2, remapMatrixs.mY2, cv::INTER_LINEAR );

	return 1;
}


