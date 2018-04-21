#ifndef DISPARITYEYE_H
#define DISPARITYEYE_H

#include "EyeBase.h"
#include "GUI.h"


class DisparityEye: public EyeBase, public GUI
{
    public:
        DisparityEye();
        virtual ~DisparityEye();
        void showDepthMap();
        void createDataSet();
        void deleteDataSet();
        unsigned stereoCalibration(unsigned);

        void cameraFeed(unsigned char);
        void GUI();

    private:
		const unsigned char UNDISTORT = 0b0000'0001;
		const unsigned char RECTIFY   = 0b0000'0010;

};

#endif // DISPARITYEYE_H
