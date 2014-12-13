package org.example.prueba;
 
import org.opencv.android.BaseLoaderCallback;
import org.opencv.android.CameraBridgeViewBase;
import org.opencv.android.LoaderCallbackInterface;
import org.opencv.android.OpenCVLoader;
import org.opencv.android.CameraBridgeViewBase.CvCameraViewFrame;
import org.opencv.android.CameraBridgeViewBase.CvCameraViewListener2;
import org.opencv.core.CvType;
import org.opencv.core.Mat;

import android.app.Activity;
import android.os.Bundle;
import android.util.Log;
import android.view.WindowManager;
 
public class FrmSaludo extends Activity implements CvCameraViewListener2 {
	
	private static final String    TAG = "Pruebas";
	private Mat mRgba;
	
	private CameraBridgeViewBase   mOpenCvCameraView;
	
	private BaseLoaderCallback  mLoaderCallback = new BaseLoaderCallback(this) {
		@Override
		public void onManagerConnected(int status) {
			switch (status) {
			case LoaderCallbackInterface.SUCCESS:
			{
				Log.i(TAG, "OpenCV loaded successfully");
				// Load native library after(!) OpenCV initialization
				System.loadLibrary("Prueba");
				mOpenCvCameraView.enableView();
				Log.i(TAG,"Camara iniciada");
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
    	Log.i(TAG, "called onCreate");
        super.onCreate(savedInstanceState);
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
        
        setContentView(R.layout.camera);
        mOpenCvCameraView = (CameraBridgeViewBase) findViewById(R.id.camara);
		mOpenCvCameraView.setCvCameraViewListener(this);
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
		mRgba = inputFrame.rgba();
		findSquares(mRgba.getNativeObjAddr());
		return mRgba;
	}

	private native void findSquares(long nativeObjAddr);
}