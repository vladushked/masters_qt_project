    #ifndef SU_ROV_H
#define SU_ROV_H

#include <QObject>
#include "KX_pult_src/kx_protocol.h"
#include "KX_pult_src/qkx_coeffs.h"
#include "math.h"

#define ANPA_MOD_CNT 23

extern double X[2000][2];
extern QVector<double> K;

struct InitData {
    double k_gamma;
    double m;
    double delta_m;
    double cv1[3];
    double cv2[3];
    double cw1[3];
    double cw2[3];
    double lambda[6][6];
    double J[3];
    double kd;
    double h;
    double Td;
    double l1,l2;
    double depth_limit;
    double max_depth;
}; //struct InitData


class SU_ROV : public QObject
{
    Q_OBJECT

public:
    explicit SU_ROV(QObject *parent = nullptr);
    virtual ~SU_ROV() {};
    QTimer timer;

public slots:
    void tick();

private:
    Qkx_coeffs * K_Protocol;
    x_protocol * X_Protocol;

    float timer_period; //период таймера

    void BFS_DRK_KUSTO_3(double Upsi, double Uy, double Uz);
    //метод, который ограничивает значение input по величине max
    float saturation(float input, float max);
    int sign(float input){
        return (input>=0) ? 1 : -1;
    }
    void yawControlChannel();
    void lagControlChannel();
    void depthControlChannel();
    // rov model init
    void modelKusto3(const float Ulz, const float Ulp, const float Ugl, const float Ugp);
    void rungeKusto3(const float Ulz, const float Ulp, const float Ugl, const float Ugp, const float dt);
    void resetModel();

    double Plz,Plp,Pgl,Pgp; //упоры движителей
    double Pgl_y, Pgp_y, Plz_z, Plp_z; // проекции упоров

    void getDataFromModel();
    void pitchControlChannel();
    //void BFS_DRK(double Upsi, double Uteta, double Ugamma, double Ux);
    //метод, который ограничивает значение input по величине max
    // rov model init
    void model(const float Umvl,const float Umnl,const float Umvp,const float Umnp);
    void runge(const float Umvl,const float Umnl,
               const float Umvp,const float Umnp, const float dt=0.01);


    double a[ANPA_MOD_CNT];
    double da[ANPA_MOD_CNT];
    double delta_f;
    //константы
    double k_gamma;
    double m;
    double g;
    double G;
    double delta_m;
    double cv1[3];
    double cv2[3];
    double cw1[3];
    double cw2[3];
    double lambda[7][7];
    double J[3];
    double kd;
    double h;
    double Td;
    double l1,l2;
    double depth_limit;
    double max_depth;
    //переменные
    double sumX, sumZ;
    double cur_depth, Wx, Wy, Wz;
    double Psi_g, Gamma_g, Tetta_g;

    double Psi_gi, W_Psi_g, W_Gamma_g, W_Tetta_g;

    //double Pmvp, Pmvl, Pmnp, Pmnl;
    // double Pmvp_x, Pmnl_x, Pmvl_x, Pmnp_x;
    double Ulz, Ulp, Umpr, Uml; //напряжения движителей

    double FloatageX, FloatageY, FloatageZ, Fdx, Fdy, Fdz, Fgx, Fgy, Fgz;
    double Mdx, Mdy, Mdz, Mgx, Mgy, Mgz;
    double Max,May,Maz; // моменты от силы Архимеда

    double x_global, y_global, z_global;
    double vx_local,  vy_local, vz_local;  //lineinye skorosti SPA v svyazannyh osyah
    double vx_global, vy_global, vz_global;

    float Fx,Fy,Fz; //total forces for XYZ-axis
    float Mx,My,Mz; //total moments for XYZ-axis
};

#endif // SU_ROV_H
