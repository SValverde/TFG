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
int chess_size=29;

//Hallar los 2 menores Y y luego encontrar el menor X entre esos 2 para que sea la primera esquina
vector<Point> sortCoord(vector<Point> contorno){
	vector<Point> ordenado;
	Point primero=contorno[0];
	Point segundo;
	Point tercero;
	int indice=0;

	//Vertice de menor coordenada y
	for(int i=1;i<contorno.size();i++){
		if(contorno[i].y<primero.y){
			primero=contorno[i];
			indice=i;
		}
	}
	contorno.erase(contorno.begin()+indice);

	//Segundo vertice de menor coordenada y
	segundo=contorno[0];
	indice=0;
	for(int i=0;i<contorno.size();i++){
		if(contorno[i].y<segundo.y){
			segundo=contorno[i];
			indice=i;
		}
	}
	contorno.erase(contorno.begin()+indice);

	//Decidimos el primer y segundo vertice
	if(primero.x<segundo.x){
		ordenado.push_back(primero);
		ordenado.push_back(segundo);
	}
	else{
		ordenado.push_back(segundo);
		ordenado.push_back(primero);
	}

	//Decidimos el tercer y cuarto vertice
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

//Debugging
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

void putVector(Mat imagen,string mensaje,Mat vector,int flag){
	stringstream str1,str2,str3;
	str1  << vector.row(0);
	str2  << vector.row(1);
	str3  << vector.row(2);
	putText(imagen,mensaje,Point(5,60+flag),FONT_HERSHEY_SIMPLEX,1,Scalar::all(255),2);
	putText(imagen,str1.str(),Point(5,90+flag),FONT_HERSHEY_SIMPLEX,1,Scalar::all(255),2);
	putText(imagen,str2.str(),Point(5,120+flag),FONT_HERSHEY_SIMPLEX,1,Scalar::all(255),2);
	putText(imagen,str3.str(),Point(5,150+flag),FONT_HERSHEY_SIMPLEX,1,Scalar::all(255),2);
}

extern "C" {
JNIEXPORT void JNICALL Java_upm_tfg_Localizacion_findSquares(JNIEnv *env, jobject thisObj, jlong imagen,jlong cM, jlong dC);
//JNIEXPORT void JNICALL Java_org_example_prueba_FrmSaludo_findSquares(JNIEnv *env, jobject thisObj, jlong imagen);

JNIEXPORT void JNICALL Java_upm_tfg_Calibracion_calibrate(JNIEnv *env, jobject thisObj, jobjectArray archivos, jlong matPtr, jlong coeffPtr);

JNIEXPORT void JNICALL Java_upm_tfg_Localizacion_findSquares(JNIEnv *env, jobject thisObj, jlong imagen,jlong cM, jlong dC){
	//JNIEXPORT void JNICALL Java_org_example_prueba_FrmSaludo_findSquares(JNIEnv *env, jobject thisObj, jlong imagen){
	Mat& src  = *(Mat*)imagen;
	Mat& cameraMatrix  = *(Mat*)cM;
	Mat& distCoeffs  = *(Mat*)dC;
	vector<vector<Point> > contours, cuadrados, marca, ordenados;
	vector<Vec4i> hierarchy;
	Mat src_gray, canny_output, rvec, tvec, dst;

	//Transformacion de RGB a escala de grises
	cvtColor( src, src_gray, CV_BGR2GRAY );

	//Filtro
	blur( src_gray, src_gray, Size(3,3) );

	//Definicion del umbral y transformacion a binaria
	int thresh=150;
	Canny( src_gray, canny_output, thresh, thresh*2, 3 );

	findContours( canny_output, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );
	cuadrados=filtraVertices(contours);
	marca=eliminateDoubleContours(cuadrados);
	if(!marca.empty()){
		//Obtenemos los objectPoints
		vector<Point3f> objectPoints=getObjectPoints();

		//Ordenamos los imagePoints y los juntamos en un solo vector
		ordenados.push_back(sortCoord(marca[0]));
		ordenados.push_back(sortCoord(marca[1]));
		vector<Point2f> imagePoints;
		for(int i=0;i<ordenados[0].size();i++){
			imagePoints.push_back(ordenados[0][i]);
		}
		for(int i=0;i<ordenados[1].size();i++){
			imagePoints.push_back(ordenados[1][i]);
		}

		//Instanciamos rvec y tvec y llamamos a solvePnP
		rvec=Mat::zeros(3, 1, CV_64F);
		tvec=Mat::zeros(3, 1, CV_64F);
		solvePnP(objectPoints,imagePoints,cameraMatrix,distCoeffs,rvec,tvec);
		drawContours(src, marca, -1, Scalar(0,255,0),2,8);
		putVector(src,"Rvec",rvec,0);
		putVector(src,"Tvec",tvec,150);
	}
}


JNIEXPORT void JNICALL Java_upm_tfg_Calibracion_calibrate(JNIEnv *env, jobject thisObj, jobjectArray archivos, jlong matPtr, jlong coeffPtr){

	//Recuperamos los objetos y parametros
	Mat& cameraMatrix=*(Mat*) matPtr;
	Mat& distCoeffs=*(Mat*) coeffPtr;
	int stringCount = env->GetArrayLength(archivos);

	//Declaramos variables
	vector<string> imageList;
	vector<vector<Point2f> > imagePoints;
	vector<vector<Point3f> > objectPoints(1);
	vector<Point2f> pointBuf;

	//Transformamos el array de rutas de java en un vector de strings de C++
	for (int i=0; i<stringCount; i++) {
		jstring imgPath = (jstring) env->GetObjectArrayElement(archivos, i);
		const char *rawString = env->GetStringUTFChars(imgPath, 0);
		string img(rawString);
		imageList.push_back(img);
		env->ReleaseStringUTFChars(imgPath,rawString);
	}

	//loadImages(imageList,directorio);
	for(int i=0;i<(int)imageList.size();i++){
		bool found=false;
		//Leemos la imagen
		Mat view=imread(imageList[i]);
		Mat dst;

		//Reescalamos la imagen
		resize(view, dst, Size(), 0.3, 0.23, CV_INTER_AREA);

		//Hallamos los imagePoints y los guardamos en el vector
		found=findChessboardCorners(dst,Size(8,5),pointBuf,0);
		if(found){
			imagePoints.push_back(pointBuf);
		}
	}
	objectPoints[0].clear();
	getObjectPoints(objectPoints[0]);
	objectPoints.resize(imagePoints.size(),objectPoints[0]);
	vector<Mat> rvecs, tvecs;
	double rms = calibrateCamera(objectPoints, imagePoints, Size(8,5), cameraMatrix, distCoeffs, rvecs, tvecs);

}
}
