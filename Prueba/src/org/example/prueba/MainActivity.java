package org.example.prueba;

import java.io.File;

import org.opencv.android.BaseLoaderCallback;
import org.opencv.android.LoaderCallbackInterface;
import org.opencv.android.OpenCVLoader;
import org.opencv.core.Mat;
import org.opencv.core.CvType;
import org.opencv.highgui.Highgui;
import android.os.Bundle;
import android.os.Environment;
import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.content.SharedPreferences;
import android.util.Log;
import android.view.Menu;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.hardware.*;

public class MainActivity extends Activity {

	private SharedPreferences prefs;
	private BaseLoaderCallback  mLoaderCallback = new BaseLoaderCallback(this) {
		@Override
		public void onManagerConnected(int status) {
			//Log.i("Pruebas","Esto arranca");
			switch (status) {
			case LoaderCallbackInterface.SUCCESS:
			{
				Log.i("Pruebas", "OpenCV loaded successfully");
				// Load native library after(!) OpenCV initialization
				System.loadLibrary("Prueba");

			} break;
			default:
			{
				Log.e("Pruebas","Error al cargar manager");
				super.onManagerConnected(status);
			} break;
			}
		}
	};

	/*@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main);
		//Obtenemos una referencia a los controles de la interfaz
		//final ImageView imagen = (ImageView)findViewById(R.id.ImgShow);
		final Button btnHola = (Button)findViewById(R.id.BtnCalib);
		btnHola.setOnClickListener(new OnClickListener() {
			@Override
			public void onClick(View v) {
				File prueba=Environment.getExternalStorageDirectory();
				String path=prueba.getAbsolutePath()+"/Pictures";
				File pictures= new File(path);	
				String lista1[]=pictures.list();
				String pathList[]= new String[lista1.length];
				for(int i=0;i<lista1.length;i++){
					pathList[i]=path+"/"+lista1[i];
				}
				Log.e("Pruebas",pathList[0]);
				String imgpath=pathList[0];

				Mat matriz=Mat.eye(3, 3, CvType.CV_64F);
				calibrate(pathList,matriz.getNativeObjAddr());
				Log.e("Pruebas",matriz.dump());
			}
		});
	}*/

	@Override
	public void onResume()
	{
		super.onResume();
		if (!OpenCVLoader.initAsync(OpenCVLoader.OPENCV_VERSION_2_4_9, this, mLoaderCallback))
		{
			Log.e("Pruebas", "Cannot connect to OpenCV Manager");
		}
		else
			Log.e("Pruebas", "Conectado a OpenCV Manager");
	}


	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main);
		prefs=getSharedPreferences("Calibracion",Context.MODE_PRIVATE);
		
		File wallpaperDirectory = new File(Environment.getExternalStorageDirectory()+"/Calibracion");
		wallpaperDirectory.mkdirs();

		//Obtenemos una referencia a los controles de la interfaz
		final Button btnCalib = (Button)findViewById(R.id.BtnCalib);
		
		final Button btnReset = (Button)findViewById(R.id.BtnReset);
		btnReset.setOnClickListener(new OnClickListener(){
			public void onClick(View v) {
				prefs.edit().clear().commit();
				btnCalib.setText("Calibrar");
			}
		});

		btnCalib.setOnClickListener(new OnClickListener() {
			@Override
			public void onClick(View v) {
				Mat matriz=Mat.eye(3, 3, CvType.CV_64F);
				Mat coeffs=Mat.zeros(8,1,CvType.CV_64F);
				btnCalib.setText("Calibrando");
				if(!isCalibrated()){
					File prueba=Environment.getExternalStorageDirectory();
					String path=prueba.getAbsolutePath()+"/Calibracion";
					File pictures= new File(path);	
					String lista1[]=pictures.list();
					String pathList[]= new String[lista1.length];
					for(int i=0;i<lista1.length;i++){
						pathList[i]=path+"/"+lista1[i];
					}
					//Log.e("Pruebas",pathList[0]);
					Mat img=Highgui.imread(pathList[0]);
					Log.e("Pruebas",img.size().toString());
					calibrate(pathList,matriz.getNativeObjAddr(),coeffs.getNativeObjAddr());

					Log.e("Pruebas",matriz.dump());
					Log.e("Pruebas",coeffs.dump());
					saveMatrix(matriz);
					saveCoeffs(coeffs);
					btnCalib.setText("Calibrado!");
				}
				else{
					btnCalib.setText("Calibrado!");
					matriz=loadMatrix();
					coeffs=loadCoeffs();
					Log.e("Prueba",matriz.dump());
					Log.e("Prueba",coeffs.dump());
				}
				//Creamos el Intent
				Intent intent = new Intent(MainActivity.this, FrmSaludo.class);
				//Iniciamos la nueva actividad
				startActivity(intent);
			}
		});
	}

	protected void saveCoeffs(Mat coeffs) {
		// TODO Auto-generated method stub
		SharedPreferences.Editor editor= prefs.edit();
		double[] elemen1=coeffs.get(0, 0);
		double[] elemen2=coeffs.get(1, 0);
		double[] elemen3=coeffs.get(2, 0);
		double[] elemen4=coeffs.get(3, 0);
		double[] elemen5=coeffs.get(4, 0);
		float e1=(float) elemen1[0];
		float e2=(float) elemen2[0];
		float e3=(float) elemen3[0];
		float e4=(float) elemen4[0];
		float e5=(float) elemen5[0];
		editor.putFloat("coef1", e1);
		editor.putFloat("coef2", e2);
		editor.putFloat("coef3", e3);
		editor.putFloat("coef4", e4);
		editor.putFloat("coef5", e5);
		editor.commit();
	}


	protected void saveMatrix(Mat matriz) {
		// TODO Auto-generated method stub
		SharedPreferences.Editor editor= prefs.edit();
		double[] elemen1=matriz.get(0, 0);
		double[] elemen2=matriz.get(0, 2);
		double[] elemen3=matriz.get(1, 1);
		double[] elemen4=matriz.get(1, 2);
		float e1=(float) elemen1[0];
		float e2=(float) elemen2[0];
		float e3=(float) elemen3[0];
		float e4=(float) elemen4[0];
		editor.putFloat("elem1", e1);
		editor.putFloat("elem2", e2);
		editor.putFloat("elem3", e3);
		editor.putFloat("elem4", e4);
		editor.commit();
	}


	private Mat loadMatrix() {

		Mat resultado=Mat.zeros(3,3,CvType.CV_64F);
        SharedPreferences prefs=getSharedPreferences("Calibracion",Context.MODE_PRIVATE);
        float aux[]= new float[4];
		aux[0]=prefs.getFloat("elem1", 1);
		aux[1]=prefs.getFloat("elem2", 1);
		aux[2]=prefs.getFloat("elem3", 1);
		aux[3]=prefs.getFloat("elem4", 1);
		resultado.put(0, 0, aux[0]);
		resultado.put(0, 2, aux[1]);
		resultado.put(1, 1, aux[2]);
		resultado.put(1, 2, aux[3]);
		resultado.put(2, 2, 1.0);
		return resultado;
	}
	
	private Mat loadCoeffs() {

		Mat resultado=Mat.zeros(8,1,CvType.CV_64F);
        SharedPreferences prefs=getSharedPreferences("Calibracion",Context.MODE_PRIVATE);
        float aux[]= new float[5];
		aux[0]=prefs.getFloat("coef1", 1);
		aux[1]=prefs.getFloat("coef2", 1);
		aux[2]=prefs.getFloat("coef3", 1);
		aux[3]=prefs.getFloat("coef4", 1);
		aux[4]=prefs.getFloat("coef5", 1);
		resultado.put(0, 0, aux[0]);
		resultado.put(1, 0, aux[1]);
		resultado.put(2, 0, aux[2]);
		resultado.put(3, 0, aux[3]);
		resultado.put(4, 0, aux[4]);
		return resultado;
	}

	private boolean isCalibrated() {

		float valor=prefs.getFloat("elem1", 0);
		if(valor==0)
			return false;
		else
			return true;
	}

	private void storeMat(Mat matriz){
	}

	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		getMenuInflater().inflate(R.menu.main, menu);
		return true;
	}

	private native String apellido(String nombre);
	private native double getAt(long l,int posicion);
	private native void calibrate(Object[] path, long l, long m);
}