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
