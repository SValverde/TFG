#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include "opencv2/highgui/highgui.hpp"


#include <iostream>
#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <errno.h>
#include <sys/types.h>
#include <vector>

using namespace std;
using namespace cv;

Size tamano(8,5);
int width_points=5;
int height_points=8;
int chess_size=29;

int loadImages(vector<string>& imageList, char* directorio){
	DIR *d;
	struct dirent *dir;
	imageList.resize(0);
	//d = opendir("D:/workspace/TFG/src/right");
	//d = opendir("D:/workspace/");
	d=opendir(directorio);
	if (d)
	{
		while ((dir = readdir(d)) != NULL)
		{
			string conv(dir->d_name);
			//if((conv.compare(".")!=0) && (conv.compare("..")!=0)){
			if (conv.find(".jpg") != std::string::npos) {
				imageList.push_back(conv);
			}
		}
		closedir(d);
	}
	else{
		cout << "No se pudo abrir el fichero error" <<endl;
		return(1);
	}
	for (int i = 0; i < (int)imageList.size(); i++){
		cout << imageList[i] << endl;
	}
	return(0);
}

void getObjectPoints(vector<Point3f>& corners)
{
	for( int i = 0; i < tamano.height; ++i )
	    for( int j = 0; j < tamano.width; ++j )
	        corners.push_back(Point3f(float( j*chess_size ), float( i*chess_size ), 0));
}


int main (int argc, char** argv){
	cout << "Hola" << endl;
	Mat cameraMatrix, distCoeffs;
	vector<string> imageList;
	vector<vector<Point2f> > imagePoints;
	vector<vector<Point3f> > objectPoints(1);
	vector<Point2f> pointBuf;
	//char* directorio=argv[1];
	char* directorio="D:/workspace/TFG/src/imgMovil";
	string strdir(directorio);

	loadImages(imageList,directorio);
	for(int i=0;i<(int)imageList.size();i++){
		bool found=false;
		string ruta(strdir+"\\"+imageList[i]);
		cout << ruta << endl;
		Mat view=imread(ruta);
		Mat dst;
		resize(view, dst, Size(), 0.3, 0.3, CV_INTER_AREA);
		namedWindow( "imagen", CV_WINDOW_NORMAL );
		imshow("imagen",dst);
		waitKey(250);
		found=findChessboardCorners(dst,Size(8,5),pointBuf,0);
		if(found){
			Mat viewGray;
			cvtColor(dst, viewGray, CV_BGR2GRAY);
			imagePoints.push_back(pointBuf);
			drawChessboardCorners(dst,Size(8,5),imagePoints[i],found);
			imshow("imagen",dst);
			waitKey(250);
		}
		else{
			cout << "Not found" << endl;
			return 0;
		}
	}
	objectPoints[0].clear();
	getObjectPoints(objectPoints[0]);
	objectPoints.resize(imagePoints.size(),objectPoints[0]);
	cameraMatrix = Mat::eye(3, 3, CV_64F);
	distCoeffs = Mat::zeros(8, 1, CV_64F);
	vector<Mat> rvecs, tvecs;
	//double rms = calibrateCamera(objectPoints, imagePoints, Size(8,5), cameraMatrix, distCoeffs, rvecs, tvecs);
	calibrateCamera(objectPoints, imagePoints, Size(8,5), cameraMatrix, distCoeffs, rvecs, tvecs);
	cout << "camera_matrix" << cameraMatrix << endl;
	cout << "distCoeffs" << distCoeffs << endl;
}
