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

        Eigen::Vector3d traction[2];
        double angle_fwd, angle_bwd;
        double phi;
        double theta;
        double t0_normal, t0_tangential;
        double t1_normal, t1_tangential;
        double trac_normal, trac_tangential;

        Eigen::Vector3d tn, tn_perp;
    };

    static const unsigned number_of_ponts = 200;
    Result values[number_of_ponts];

    void Evaluate();    // calculate the result

    Eigen::Vector3d normal, dir;
    unsigned evaluation_time;
    double span_angle;
    unsigned fwd_sector, bwd_sector;
    unsigned idx_max_sector;

};

#endif // MODEL_H
