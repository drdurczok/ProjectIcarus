#ifndef DISPARITYEYE_H
#define DISPARITYEYE_H

#include "EyeBase.h"

class DisparityEye: public EyeBase
{
    public:
        DisparityEye();
        virtual ~DisparityEye();
        void depthMap();
        void GUI(int &a, int &b, int &c, int &d, int &e, int &f, int &g, int &h, int &i, int &j, bool &k, char arrow, int &state);
        void LoadBar(int min, int max, int &val, int steps);

    protected:

    private:

};

#endif // DISPARITYEYE_H
