#ifndef INVKIN
#define INVKIN

/* System 1
 *                B_1
 *      **********************
 *       *                    *
 *        *                     *
 *         *                     *  C_1
 *      A_1 *                     *
 *           *                     *
 *            *  )alpha             *
 *             *  D_1, init_offsetA  *
 */
 
double A_1[] = {18};
double B_1[] = {19};
double C_1[] = {27};
double D_1[] = {27};
double init_offsetA[] = {7*PI/180};  //Radians

/* System 2
 *  
 *      *  
 *      * *
 *      * )init_offsetC 
 *      *     *
 *  B_2 *       *
 *      *         *
 *  X_1(*) )init_offsetD
 *        *           *
 *          *         *
 *    A_2     *       *
 *              *     *
 *                *  init_offsetB
 *                  * *   )alpha,  D_1
 *            theta(  * * * * * * * * * *
 */               

double A_2[] = {54};
double B_2[] = {18.5};
double init_offsetB[] = {80*PI/180};  //Radians
double init_offsetC[] = {80*PI/180};  //Radians
double init_offsetD[] = {106*PI/180};  //Radians


/* System 3
 *     
 *     *                            *
 *     *  *                      *
 *     *    *  B_3             *
 *     *      *             *
 * D_3 *        *        *
 *     *          *   *         B_2
 *     *           * 
 *     *       *      *
 *     *   *  C_3       *    
 *     *                  *
 *                          *
 */

double B_3[] = {51};
double C_3[] = {51};
double D_3[] = {15.5};

/* System 4
 * 
 *      *
 *      * *
 *      *   *   D_3
 *  A_4 *     *
 *      *       *
 *      *
 *      *
 */

double A_4[] = {20};
double ang_4[] = {145/(2*PI)};

double flexion = 0;

/* Thumb - System 5
 *                      *   
 *                    *****  axis of rotation: theta1
 *                      *   
 *                      *
 *                      * A_5  
 *                      *
 *                  *   *
 *           B_5      * *
 *  ******************* *   
 *                        *  axis of rotation: theta2
 *                          *
 *                        
 */

double A_5[] = {2};
double B_5[] = {2};



#endif
