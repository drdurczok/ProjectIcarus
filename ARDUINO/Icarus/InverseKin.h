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
 *            *                     *
 *             *  D_1, init_offsetA  *
 */
 
double A_1 = 1;
double B_1 = 1;
double C_1 = 1;
double D_1 = 1;
double init_offsetA = 5*PI/180;  //Radians

/* System 2
 *  
 *      *  
 *      * *
 *      *   *
 *      *     *
 *  B_2 *       *
 *      *         *
 *  X_1(*)           *
 *        *           *
 *          *         *
 *    A_2     *       *
 *              *     *
 *                *  init_offsetB
 *                  * *       D_1
 *            alpha(  * * * * * * * * * *
 */               

double A_2 = 1.1;
double B_2 = 1;
double init_offsetB = 80*PI/180;  //Radians

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

double B_3 = 1;
double C_3 = 1;
double D_3 = 1;

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

double A_4 = 1;
double ang_4 = 145/(2*PI);

double flexion = 0;

#endif