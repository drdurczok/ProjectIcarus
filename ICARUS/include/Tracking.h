#ifndef TRACKING_H
#define TRACKING_H

#include "EyeBase.h"
#include "GUI.h"

class Tracking: public EyeBase, public GUI
{
	public:
		Tracking();
		~Tracking();
		void LucasKanade();
		void ScanFingerTips();
		float innerAngle(float px1, float py1, float px2, float py2, float cx1, float cy1);

		void GUI();

	private:
		//VideoCapture *vid;
		int minH = 0, maxH = 20, minS = 30, maxS = 150, minV = 60, maxV = 255;
		cv::Mat frame;
		int count = 0;


};
#endif