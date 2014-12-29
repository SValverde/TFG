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

#define  LOG_TAG    "Native"
#define  ALOG(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)

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

float* getMoment(vector<Point> contorno){
	Moments M=moments(contorno);
	float *centro= new float[2];
	centro[0]=float(M.m10/M.m00);
	centro[1]=float(M.m01/M.m00);
	//cout << "centro: " << centro[0] << " " << centro[1] << endl;
	return centro;
}

/*void printPoint(Point punto){
	cout << "Punto: " << punto.x << "," << punto.y << endl;
}*/

/*void printPoint(Point3f punto){
	cout << "Punto: " << punto.x << "," << punto.y << "," << punto.z << endl;
}*/


//Hallar los 2 menores Y y luego encontrar el menor X entre esos 2 para que sea la primera esquina
vector<Point> sortCoord(vector<Point> contorno){
	vector<Point> ordenado;
	Point primero=contorno[0];
	Point segundo;
	Point tercero;
	int indice=0;

	//Primer vertice
	for(int i=1;i<contorno.size();i++){
		if(contorno[i].y<primero.y){
			primero=contorno[i];
			indice=i;
		}
	}
	//ordenado.push_back(primero);
	contorno.erase(contorno.begin()+indice);

	//Segundo vertice
	segundo=contorno[0];
	indice=0;
	for(int i=0;i<contorno.size();i++){
		if(contorno[i].y<segundo.y){
			segundo=contorno[i];
			indice=i;
		}
	}
	//ordenado.push_back(segundo);
	contorno.erase(contorno.begin()+indice);

	if(primero.x<segundo.x){
		ordenado.push_back(primero);
		ordenado.push_back(segundo);
	}
	else{
		ordenado.push_back(segundo);
		ordenado.push_back(primero);
	}

	if(contorno[0].x<contorno[1].x){
		ordenado.push_back(contorno[0]);
		ordenado.push_back(contorno[1]);
	}
	else{
		ordenado.push_back(contorno[1]);
		ordenado.push_back(contorno[0]);
	}

	for(int i=0;i<ordenado.size();i++){
		//printPoint(ordenado[i]);
	}
	return ordenado;
}

vector<vector<Point> > eliminateDoubleContours(vector<vector<Point> > cuadrados){
	vector<vector<Point> > ncuadrados;
	int encontrado=0;
	for(int i=0; i<cuadrados.size();i++){
		for(int j=0; j<cuadrados.size() && encontrado==0;j++){
			if(i!=j){
				double area1=contourArea(cuadrados[i]);
				double area2=contourArea(cuadrados[j]);
				double threshold=area1/10.0;
				double inside=pointPolygonTest(cuadrados[i],cuadrados[j][0],false);
				if(area2<area1-threshold && inside>0){
					//__android_log_print(ANDROID_LOG_INFO,"Native","Area1: %f",area1);
					//__android_log_print(ANDROID_LOG_INFO,"Native","Area2: %f",area2);
					//__android_log_print(ANDROID_LOG_INFO,"Native","Threshold: %f",threshold);
					ncuadrados.push_back(cuadrados[i]);
					ncuadrados.push_back(cuadrados[j]);
					encontrado=1;
				}
			}
		}
	}
	return ncuadrados;
}

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

vector<vector<Point> > filtraVertices(vector<vector<Point> > contornos){
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

void printContour(vector<Point> contorno){
	__android_log_print(ANDROID_LOG_INFO,"Native","Comienzo");
	for(int i=0;i<contorno.size();i++){
		__android_log_print(ANDROID_LOG_INFO,"Native","(%d, %d)",contorno[i].x,contorno[i].y);
	}
	__android_log_print(ANDROID_LOG_INFO,"Native","Fin");
}

vector<Point3f> getObjectPoints(){
	vector<Point3f> objectPoints;
	Point3f *puntos=new Point3f[8];
	puntos[0].x=0;puntos[0].y=0;puntos[0].z=0;
	puntos[1].x=200;puntos[1].y=0;puntos[1].z=0;
	puntos[2].x=0;puntos[2].y=200;puntos[2].z=0;
	puntos[3].x=200;puntos[3].y=200;puntos[3].z=0;
	puntos[4].x=50;puntos[4].y=50;puntos[4].z=0;
	puntos[5].x=150;puntos[5].y=50;puntos[5].z=0;
	puntos[6].x=50;puntos[6].y=150;puntos[6].z=0;
	puntos[7].x=150;puntos[7].y=150;puntos[7].z=0;
	for(int i=0;i<8;i++){
		objectPoints.push_back(puntos[i]);
	}

	return objectPoints;
}

extern "C" {
JNIEXPORT jstring JNICALL Java_org_example_prueba_MainActivity_apellido(JNIEnv *env, jobject thisObj, jstring nombre);

JNIEXPORT void JNICALL Java_org_example_prueba_FrmSaludo_findSquares(JNIEnv *env, jobject thisObj, jlong imagen,jlong cM, jlong dC);
//JNIEXPORT void JNICALL Java_org_example_prueba_FrmSaludo_findSquares(JNIEnv *env, jobject thisObj, jlong imagen);

JNIEXPORT void JNICALL Java_org_example_prueba_MainActivity_calibrate(JNIEnv *env, jobject thisObj, jobjectArray archivos, jlong matPtr, jlong coeffPtr);

JNIEXPORT void JNICALL Java_org_example_prueba_FrmSaludo_findSquares(JNIEnv *env, jobject thisObj, jlong imagen,jlong cM, jlong dC){
//JNIEXPORT void JNICALL Java_org_example_prueba_FrmSaludo_findSquares(JNIEnv *env, jobject thisObj, jlong imagen){
	Mat& src  = *(Mat*)imagen;
	Mat& cameraMatrix  = *(Mat*)cM;
	Mat& distCoeffs  = *(Mat*)dC;
	vector<vector<Point> > contours, cuadrados, marca, ordenados;
	vector<Vec4i> hierarchy;
	Mat src_gray, canny_output, rvec, tvec, dst;

	//resize(src, dst, Size(), 0.3, 0.3, CV_INTER_AREA);
	cvtColor( src, src_gray, CV_BGR2GRAY );
	blur( src_gray, src_gray, Size(3,3) );
	int thresh=150;
	Canny( src_gray, canny_output, thresh, thresh*2, 3 );
	findContours( canny_output, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );
	cuadrados=filtraVertices(contours);
	marca=eliminateDoubleContours(cuadrados);
	if(!marca.empty()){
		//printContour(marca[0]);
		//printContour(marca[1]);
		vector<Point3f> objectPoints=getObjectPoints();
		ordenados.push_back(sortCoord(marca[0]));
		ordenados.push_back(sortCoord(marca[1]));
		//printContour(ordenados[0]);
		vector<Point2f> imagePoints;
		for(int i=0;i<ordenados[0].size();i++){
			imagePoints.push_back(ordenados[0][i]);
		}
		for(int i=0;i<ordenados[1].size();i++){
			imagePoints.push_back(ordenados[1][i]);
		}
		rvec=Mat::zeros(3, 1, CV_64F);
		tvec=Mat::zeros(3, 1, CV_64F);
		solvePnP(objectPoints,imagePoints,cameraMatrix,distCoeffs,rvec,tvec);
		stringstream str1;
		str1  << "Distance: " << tvec.row(2);
		drawContours(src, marca, -1, Scalar(0,255,0),2,8);
		putText(src,str1.str(),Point(5,60),FONT_HERSHEY_SIMPLEX,0.5,Scalar::all(255),1.5);
	}
}


JNIEXPORT void JNICALL Java_org_example_prueba_MainActivity_calibrate(JNIEnv *env, jobject thisObj, jobjectArray archivos, jlong matPtr, jlong coeffPtr){
	Mat& cameraMatrix=*(Mat*) matPtr;
	Mat& distCoeffs=*(Mat*) coeffPtr;
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
		resize(view, dst, Size(), 0.3, 0.23, CV_INTER_AREA);
		found=findChessboardCorners(dst,Size(8,5),pointBuf,0);
		if(found){
			imagePoints.push_back(pointBuf);
		}
	}
	objectPoints[0].clear();
	getObjectPoints(objectPoints[0]);
	objectPoints.resize(imagePoints.size(),objectPoints[0]);
	//distCoeffs = Mat::zeros(8, 1, CV_64F);
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
