#ifndef MODEL_H
#define MODEL_H

#include <Eigen/Core>
#include <vector>

class Model
{
public:
    Model();
    bool isBoundary;

    void AddSector(double ux, double uy, double uz,
                   double vx, double vy, double vz,
                   double sx, double sy, double txy);

    void Evaluate();    // calculate the result

    struct Sector
    {
        Eigen::Vector3d u, v; // edges
        Eigen::Matrix3d stress;
        Eigen::Vector3d frame_x, frame_y;
        Eigen::Vector3d t0, t1;
        double angle0, angle1;
    };

    std::vector<Sector> fan;

    struct Result
    {
        Eigen::Vector3d traction[2];    // traction of the "left" and "right" side of the fracture plane
        double angle_fwd, angle_bwd;
        double phi[2];
        double theta[2];
        double t0_normal, t0_tangential;
        double t1_normal, t1_tangential;
        double trac_normal, trac_tangential;
        Eigen::Vector3d tn, tn_perp;
    };

    static const unsigned number_of_ponts = 200;
    Result results[number_of_ponts];


    Eigen::Vector3d normal, dir;
    unsigned evaluation_time;
    double max_angle;
    unsigned fwd_sector, bwd_sector;
    unsigned idx_max_sector;
    double max_normal_traction;
    double approx_radius;

};

#endif // MODEL_H
