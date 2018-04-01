#include "../include/EyeBase.h"

EyeBase::EyeBase()
{
    //ctor
}

bool EyeBase::loadCameraCalibration(string name, Mat& cameraMatrix, Mat& distanceCoefficient){
    ifstream inStream(name);
    if(inStream){
        uint16_t rows;
        uint16_t columns;

        inStream >> rows;
        inStream >> columns;

        cameraMatrix = Mat(Size(columns, rows), CV_64F);

        for(int r=0; r<rows; r++){
            for(int c=0; c<columns; c++){
                double read = 0.0f;
                inStream >> read;
                cameraMatrix.at<double>(r,c) = read;
                //cout << cameraMatrix.at<double>(r,c) << "\n";
            }
        }
        //Distance Coefficients
        inStream >> rows;
        inStream >> columns;

        distanceCoefficient = Mat::zeros(rows, columns, CV_64F);

        for(int r=0; r<rows; r++){
            for(int c=0; c<columns; c++){
                double read = 0.0f;
                inStream >> read;
                distanceCoefficient.at<double>(r,c) = read;
                //cout << distanceCoefficient.at<double>(r,c) << "\n";
            }
        }
        inStream.close();
        return true;
    }
    return false;
}

bool EyeBase::saveCameraCalibration(string name, Mat& cameraMatrix, Mat& distanceCoefficients){
    ofstream outStream(name);
    if(outStream){
        uint16_t rows = cameraMatrix.rows;
        uint16_t columns = cameraMatrix.cols;

        outStream << rows << endl;
        outStream << columns << endl;

        for(int r=0; r<rows; r++){
            for(int c=0; c<columns; c++){
                double value = cameraMatrix.at<double>(r,c);
                outStream<<value<<endl;
            }
        }

        rows = distanceCoefficients.rows;
        columns = distanceCoefficients.cols;

        outStream << rows << endl;
        outStream << columns << endl;

         for(int r=0; r<rows; r++){
            for(int c=0; c<columns; c++){
                double value = distanceCoefficients.at<double>(r,c);
                outStream<<value<<endl;
            }
        }

        outStream.close();
        return true;
    }

    return false;
}

void EyeBase::createRMap(Mat& LeftImgOrg, Mat& RightImgOrg){
    Mat distanceCoefficients[2];
    Mat cameraMatrix[2], R[2], P[2];

    FileStorage fs1("../CalibrationParam.xml", FileStorage::READ);
    fs1["K1"] >> cameraMatrix[0];
    fs1["K2"] >> cameraMatrix[1];
    fs1["D1"] >> distanceCoefficients[0];
    fs1["D2"] >> distanceCoefficients[1];
    fs1["R1"] >> R[0];
    fs1["R2"] >> R[1];
    fs1["P1"] >> P[0];
    fs1["P2"] >> P[1];
    fs1.release();

    initUndistortRectifyMap(cameraMatrix[0], distanceCoefficients[0], R[0], P[0], LeftImgOrg.size(), CV_16SC2, rmap[0][0], rmap[0][1]);
    initUndistortRectifyMap(cameraMatrix[1], distanceCoefficients[1], R[1], P[1], RightImgOrg.size(), CV_16SC2, rmap[1][0], rmap[1][1]);

}
