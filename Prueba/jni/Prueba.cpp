#include <jni.h>
#include <dirent.h>
#include <string>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <vector>

#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include "opencv2/highgui/highgui.hpp"

#include <android/log.h>




using namespace std;
using namespace cv;


Size tamano(8,5);
int width_points=5;
int height_points=8;
int chess_size=29;

/*vector<vector<Point> > findConcentricSquares(vector<vector<Point> > cuadrados){
	vector<vector<Point> > marca=NULL;
	for(int i=0; i<cuadrados.size();i++){
		float center1=getCenter(cuadrados[i]);
		Point2f centro1=new Point2f(center1[0],center1[1]);
		Point2f punto1=new Point2f(cuadrados[i][0].x,cuadrados[i][0].y);
		float pt1[]={cuadrados[i][0].x,cuadrados[i][0].y};
		float threshold=distance(centro1,punto1)/10;
		for(int j=0;j<cuadrados.size();j++){
			float center2=getCenter(cuadrados[j]);
			int distancia=distance(cuadrados[i][0],cuadrados[j][0]);
			if(distancia<threshold){
			}
		}
	}
	return marca;
}*/

float* getCenter(vector<Point> contorno){
	float centro[2];
	for(int i=0; i++;i< contorno.size()){
		centro[0]=centro[0]+contorno[i].x;
		centro[1]=centro[1]+contorno[i].y;
	}
	centro[0]=centro[0]/4;
	centro[1]=centro[1]/4;
	return centro;
}

float distance (Point p1, Point p2){
	float distance=0;
	distance=pow(p1.x-p2.x,2)+pow(p1.y-p2.y,2);
	return distance;
}

float distance (float* p1, float* p2){
	float distance=0;
	distance=pow(p1[0]-p2[0],2)+pow(p1[1]-p2[1],2);
	return distance;
}

void getObjectPoints(vector<Point3f>& corners)
{
	for( int i = 0; i < tamano.height; ++i )
		for( int j = 0; j < tamano.width; ++j )
			corners.push_back(Point3f(float( j*chess_size ), float( i*chess_size ), 0));
}

vector<vector<Point> > filtraContornos(vector<vector<Point> > contornos){
	vector<vector<Point> > cuadrangulos;
	cuadrangulos.clear();
	vector<Point> aprox;
	Point punto;
	for(int i=0; i < contornos.size(); i++){
		approxPolyDP(Mat(contornos[i]), aprox, arcLength(Mat(contornos[i]), true)*0.02, true);

		if(aprox.size()==4 && isContourConvex(aprox) && (contourArea(aprox)>200)){
			cuadrangulos.push_back(aprox);
		}
	}
	return cuadrangulos;
}

extern "C" {
JNIEXPORT jstring JNICALL Java_org_example_prueba_MainActivity_apellido(JNIEnv *env, jobject thisObj, jstring nombre);
JNIEXPORT void JNICALL Java_org_example_prueba_FrmSaludo_findSquares(JNIEnv *env, jobject thisObj, jlong imagen);

JNIEXPORT void JNICALL Java_org_example_prueba_MainActivity_calibrate(JNIEnv *env, jobject thisObj, jobjectArray archivos, jlong matPtr);

JNIEXPORT void JNICALL Java_org_example_prueba_FrmSaludo_findSquares(JNIEnv *env, jobject thisObj, jlong imagen){
	Mat& src  = *(Mat*)imagen;
	vector<vector<Point> > contours;
	vector<vector<Point> > cuadrados;
	vector<Vec4i> hierarchy;
	Mat src_gray, canny_output;

	cvtColor( src, src_gray, CV_BGR2GRAY );
	blur( src_gray, src_gray, Size(3,3) );
	int thresh=190;
	Canny( src_gray, canny_output, thresh, thresh*2, 3 );
	findContours( canny_output, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );
	cuadrados=filtraContornos(contours);
	drawContours(src, cuadrados, -1, Scalar(0,255,0),2,8);
}


JNIEXPORT void JNICALL Java_org_example_prueba_MainActivity_calibrate(JNIEnv *env, jobject thisObj, jobjectArray archivos, jlong matPtr){
	Mat& cameraMatrix=*(Mat*) matPtr;
	Mat distCoeffs;
	vector<string> imageList;
	vector<vector<Point2f> > imagePoints;
	vector<vector<Point3f> > objectPoints(1);
	vector<Point2f> pointBuf;

	int stringCount = env->GetArrayLength(archivos);

	for (int i=0; i<stringCount; i++) {
		jstring imgPath = (jstring) env->GetObjectArrayElement(archivos, i);
		const char *rawString = env->GetStringUTFChars(imgPath, 0);
		string img(rawString);
		imageList.push_back(img);
		// Don't forget to call `ReleaseStringUTFChars` when you're done.
		env->ReleaseStringUTFChars(imgPath,rawString);
	}

	//loadImages(imageList,directorio);
	for(int i=0;i<(int)imageList.size();i++){
		bool found=false;
		//string ruta(strdir+"\\"+imageList[i]);
		Mat view=imread(imageList[i]);
		Mat dst;
		resize(view, dst, Size(), 0.3, 0.3, CV_INTER_AREA);
		found=findChessboardCorners(dst,Size(8,5),pointBuf,0);
		if(found){
			imagePoints.push_back(pointBuf);
		}
	}
	objectPoints[0].clear();
	getObjectPoints(objectPoints[0]);
	objectPoints.resize(imagePoints.size(),objectPoints[0]);
	distCoeffs = Mat::zeros(8, 1, CV_64F);
	vector<Mat> rvecs, tvecs;
	double rms = calibrateCamera(objectPoints, imagePoints, Size(8,5), cameraMatrix, distCoeffs, rvecs, tvecs);

}

JNIEXPORT jstring JNICALL Java_org_example_prueba_MainActivity_apellido(JNIEnv *env, jobject thisObj, jstring nombre) {

	const char *s = env->GetStringUTFChars(nombre,NULL);
	string nom(s);
	const char *fullName = nom.append(" Valverde").c_str();
	env->ReleaseStringUTFChars(nombre,s);
	jstring resultado=env->NewStringUTF(fullName);
	return resultado;
}
}
