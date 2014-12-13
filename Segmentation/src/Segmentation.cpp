//============================================================================
// Name        : Segmentation.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================
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

RNG rng(12345);



float* getMoment(vector<Point> contorno){
	Moments M=moments(contorno);
	float *centro= new float[2];
	centro[0]=float(M.m10/M.m00);
	centro[1]=float(M.m01/M.m00);
	//cout << "centro: " << centro[0] << " " << centro[1] << endl;
	return centro;
}

void printPoint(Point punto){
	cout << "Punto: " << punto.x << "," << punto.y << endl;
}

void printPoint(Point3f punto){
	cout << "Punto: " << punto.x << "," << punto.y << "," << punto.z << endl;
}


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
		cout << "Punto: " << objectPoints[i].x << "," << objectPoints[i].y << "," << objectPoints[i].z << endl;
	}

	return objectPoints;
}

float* getCenter(vector<Point> contorno){
	float *centro= new float[2];
	for(int i=0;i<contorno.size();i++){
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

vector<vector<Point> > eliminateDoubleContours(vector<vector<Point> > cuadrados){
	vector<vector<Point> > ncuadrados;
	int encontrado=0;
	for(int i=0; i<cuadrados.size();i++){
		for(int j=0; j<cuadrados.size() && encontrado==0;j++){
			if(i!=j){
				float* centro1=getMoment(cuadrados[i]);
				cout << "centro1: " << centro1[0] << " " << centro1[1] << endl;
				float* centro2=getMoment(cuadrados[j]);
				cout << "centro2: " << centro2[0] << " " << centro2[1] << endl;
				float punto1[2];
				float punto2[2];
				punto1[0]=cuadrados[i][0].x;
				punto1[1]=cuadrados[i][0].y;
				punto2[0]=cuadrados[j][0].x;
				punto2[1]=cuadrados[j][0].y;
				float distancia1 = distance(punto1,centro1);
				cout << "distancia1: " << distancia1 << endl;
				float distancia2 = distance(punto2,centro1);
				cout << "distancia2: " << distancia2 << endl;
				float threshold=distancia1/10;
				cout << "threshold: " << threshold << endl;
				if(distancia2<(distancia1-threshold)){
					ncuadrados.push_back(cuadrados[i]);
					ncuadrados.push_back(cuadrados[j]);
					encontrado=1;
				}
			}
		}
	}
	return ncuadrados;
}

/*vector<vector<Point> > findConcentricSquares(vector<vector<Point> > cuadrados){
	vector<vector<Point> > marca;
	for(int i=0; i<cuadrados.size();i++){
		float* center1=getCenter(cuadrados[i]);
		Point centro1;
		centro1.x=center1[0];
		centro1.y=center1[1];
		cout << "centro " << centro1.x << " " << centro1.y << endl;
		Point2f punto1;
		punto1.x=cuadrados[i][0].x;
		punto1.y=cuadrados[i][0].y;
		float pt1[]={cuadrados[i][0].x,cuadrados[i][0].y};
		float threshold=distance(centro1,punto1)/10;
		for(int j=0;j<cuadrados.size();j++){
			float* center2=getCenter(cuadrados[j]);
			int distancia=distance(cuadrados[i][0],cuadrados[j][0]);
			if(distancia<threshold){
			}
		}
	}
	return marca;
}*/

vector<vector<Point> > filtraContornos(vector<vector<Point> > contornos){
	vector<vector<Point> > cuadrangulos;
	cuadrangulos.clear();
	vector<Point> aprox;

	for(int i=0; i < contornos.size(); i++){
		approxPolyDP(Mat(contornos[i]), aprox, arcLength(Mat(contornos[i]), true)*0.02, true);

		if(aprox.size()==4 && isContourConvex(aprox) && (contourArea(aprox)>200)){
			cuadrangulos.push_back(aprox);
		}
	}
	return cuadrangulos;
}

vector<vector<Point> > findsquares(vector<vector<Point> > contornos){
	vector<vector<Point> > cuadrados;

	return cuadrados;
}

void printContours(vector<vector<Point> > contornos){
	for (int i=0; i <contornos.size(); i++){
		cout << contornos[i] << endl;
	}
}

int main() {
	vector<vector<Point> > contours;
	vector<vector<Point> > cuadrados;
	vector<Vec4i> hierarchy;
	Mat src_gray, canny_output, rvec, tvec;
	Mat cameraMatrix = (Mat_<double>(3,3) << 1029.349905517402, 0, 652.8466773386904, 0, 1028.950876835721, 497.8438614575827, 0, 0, 1);
	Mat distCoeffs=(Mat_<double>(5,1) << 0.06672331799566605, -0.02689524218988657, 0.01641824430817169, 0.01241975878674349, -0.4402234662696096);
	Mat m = (Mat_<double>(3,3) << 770.334773350491, 0, 338.315995148628, 0, 771.3731199420957, 227.020436722669, 0, 0, 1);
	Mat src=imread("D:/workspace/Segmentation/src/markers/flecha-2.jpg");
	cvtColor( src, src_gray, CV_BGR2GRAY );
	blur( src_gray, src_gray, Size(3,3) );
	char* source_window = "Source";
	//namedWindow( source_window, CV_WINDOW_AUTOSIZE );
	//imshow( source_window, src );
	waitKey(200);
	int thresh=190;
	Canny( src_gray, canny_output, thresh, thresh*2, 3 );
	findContours( canny_output, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );
	cuadrados=filtraContornos(contours);
	Mat drawing = src;
	//Mat drawing = Mat::zeros( canny_output.size(), CV_8UC3 );
	for( int i = 0; i< cuadrados.size(); i++ )
	{
		Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
		drawContours( drawing, cuadrados, i, color, 2, 8, hierarchy, 0, Point() );
	}
	//printContours(contours);
	printContours(cuadrados);
	//drawContours(drawing, cuadrados, -1, Scalar(0,255,0),2,8);
	//namedWindow( "Contours", CV_WINDOW_AUTOSIZE );
	//imshow( "Contours", drawing );
	//waitKey(100);
	vector<vector<Point> > marca=eliminateDoubleContours(cuadrados);
	drawContours(drawing, marca, -1, Scalar(0,255,0),2,8);

	printContours(marca);
	vector<Point3f> objectPoints=getObjectPoints();
	marca[0]=sortCoord(marca[0]);
	marca[1]=sortCoord(marca[1]);
	vector<Point2f> imagePoints;
	for(int i=0;i<marca[0].size();i++){
		imagePoints.push_back(marca[0][i]);
	}
	for(int i=0;i<marca[1].size();i++){
		imagePoints.push_back(marca[1][i]);
	}
	for(int i=0;i<imagePoints.size();i++){
		printPoint(imagePoints[i]);
	}
	rvec=Mat::zeros(3, 1, CV_64F);
	tvec=Mat::zeros(3, 1, CV_64F);
	cout << objectPoints.size() << ", " << imagePoints.size() << endl;
	solvePnP(objectPoints,imagePoints,cameraMatrix,distCoeffs,rvec,tvec);
	cout << "rvec" << rvec << endl;
	cout << "tvec" << tvec << endl;

	//Inicializo los valores del texto que quiero escribir
	Point org; org.x=5; org.y=20;
	CvFont * font = new CvFont;
	cvInitFont(font, CV_FONT_VECTOR0, 0.5f, 1.0f, 0, 1, 8);
	putText(drawing,"Prueba",org,FONT_HERSHEY_SIMPLEX,0.7,Scalar::all(255),2);
	namedWindow( "Contours", CV_WINDOW_AUTOSIZE );
	imshow( "Contours", drawing );
	waitKey(0);

	return 0;
}


