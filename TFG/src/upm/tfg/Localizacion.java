package upm.tfg;

import org.opencv.android.BaseLoaderCallback;
import org.opencv.android.CameraBridgeViewBase;
import org.opencv.android.LoaderCallbackInterface;
import org.opencv.android.OpenCVLoader;
import org.opencv.android.CameraBridgeViewBase.CvCameraViewFrame;
import org.opencv.android.CameraBridgeViewBase.CvCameraViewListener2;
import org.opencv.core.CvType;
import org.opencv.core.Mat;
import upm.tfg.R;

import android.app.Activity;
import android.content.Context;
import android.content.SharedPreferences;
import android.os.Bundle;
import android.util.Log;
import android.view.WindowManager;

public class Localizacion extends Activity implements CvCameraViewListener2 {

	private static final String    TAG = "Pruebas";
	private Mat mRgba;
	private Mat cameraMatrix;
	private Mat distCoeffs;

	private CameraBridgeViewBase   mOpenCvCameraView;

	private BaseLoaderCallback  mLoaderCallback = new BaseLoaderCallback(this) {
		@Override
		public void onManagerConnected(int status) {
			switch (status) {
			case LoaderCallbackInterface.SUCCESS:
			{
				Log.i("Pruebas", "OpenCV loaded successfully");
				// Load native library after(!) OpenCV initialization
				System.loadLibrary("NativeLib");
				mOpenCvCameraView.enableView();
				Log.i("Pruebas","Camara iniciada");
			} break;
			default:
			{
				super.onManagerConnected(status);
			} break;
			}
		}
	};

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		Log.i("Pruebas", "called onCreate");
		super.onCreate(savedInstanceState);
		//Mantenemos la pantalla encendida
		getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);

		//Cargamos el layout
		setContentView(R.layout.camara);

		//Cargamos los parametros intrinsecos
		cameraMatrix=loadMatrix();
		distCoeffs=loadCoeffs();

		//Obtenemos la referencia a la camara y la activamos
		mOpenCvCameraView = (CameraBridgeViewBase) findViewById(R.id.camara);
		mOpenCvCameraView.setCvCameraViewListener(this);
	}

	private Mat loadMatrix() {

		//Inicializamos la matriz y obtenemos la referencia a las preferencias
		Mat resultado=Mat.zeros(3,3,CvType.CV_64F);
		SharedPreferences prefs=getSharedPreferences("Calibracion",Context.MODE_PRIVATE);
		
		//Obtenemos los parametros
		float aux[]= new float[4];
		aux[0]=prefs.getFloat("elem1", 1);
		aux[1]=prefs.getFloat("elem2", 1);
		aux[2]=prefs.getFloat("elem3", 1);
		aux[3]=prefs.getFloat("elem4", 1);
		
		//Colocamos los parametros en sus sitios
		resultado.put(0, 0, aux[0]);
		resultado.put(0, 2, aux[1]);
		resultado.put(1, 1, aux[2]);
		resultado.put(1, 2, aux[3]);
		resultado.put(2, 2, 1.0);
		return resultado;
	}

	private Mat loadCoeffs() {

		Mat resultado=Mat.zeros(5,1,CvType.CV_64F);
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
	
	
	@Override
	public void onPause()
	{
		super.onPause();
		if (mOpenCvCameraView != null)
			mOpenCvCameraView.disableView();
	}

	@Override
	public void onResume()
	{
		super.onResume();
		OpenCVLoader.initAsync(OpenCVLoader.OPENCV_VERSION_2_4_3, this, mLoaderCallback);
	}

	public void onDestroy() {
		super.onDestroy();
		if (mOpenCvCameraView != null)
			mOpenCvCameraView.disableView();
	}

	@Override
	public void onCameraViewStarted(int width, int height) {
		mRgba = new Mat(height, width, CvType.CV_8UC4);
	}

	@Override
	public void onCameraViewStopped() {
		mRgba.release();
	}

	@Override
	public Mat onCameraFrame(CvCameraViewFrame inputFrame) {
		//Transformamos el fotograma en un objeto tipo Mat
		mRgba = inputFrame.rgba();
		Log.e("Pruebas",mRgba.size().toString());
		
		//Llamamos a la funcion definida en la libreria nativa
		findSquares(mRgba.getNativeObjAddr(),cameraMatrix.getNativeObjAddr(),distCoeffs.getNativeObjAddr());
		//findSquares(mRgba.getNativeObjAddr());
		return mRgba;
	}

	private native void findSquares(long nativeObjAddr, long cM, long dC);
	//private native void findSquares(long nativeObjAddr);
}