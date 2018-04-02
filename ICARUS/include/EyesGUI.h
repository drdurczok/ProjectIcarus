#ifndef EYESGUI_H
#define EYESGUI_H

#include "Eyes.h"
#include "GUI.h"

#include <iostream>
#include <sstream>
#include <fstream>
#include <string>

class EyesGUI: public GUI
{
    public:
        EyesGUI();
        void calibrationGUI();
        void swapCameras(Eyes*);
        void intrinsicParam(Eyes*);
        void extrinsicParam(Eyes*);
        void calibration(Eyes*);
        void undistImage(Eyes*);
        void rectImage(Eyes*);

    private:
        unsigned camVid[2];
};

#endif // EYESGUI_H
