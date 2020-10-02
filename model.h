#ifndef MODEL_H
#define MODEL_H

#include <Eigen/Core>
#include <vector>

class Model
{
public:
    bool isBoundary;
    double max_angle;   // assigned in InitializeFan()
    double max_normal_trac, fracture_angle; // assigned in EvaluateViaBrent()
    double min_normal_trac, max_tangential_trac, min_tangential_trac;
    unsigned iterations;     // how many iterations the evaluation took; assigned in EvaluateViaBrent()

    void AddSector(double ux, double uy, double uz,
                   double vx, double vy, double vz,
                   double sx, double sy, double txy);

    void InitializeFan();
    void EvaluateViaBrent();

    void evaluate_tractions(double angle_fwd, double &t_normal, double &t_tangential) const;
    double normal_traction(const double angle_fwd) const;
    double tangential_traction(const double angle_fwd) const;

    struct Sector
    {
        Eigen::Vector2d u, v; // edges
        Eigen::Vector2d u_normalized, v_normalized; // edges
        Eigen::Vector2d u_p, v_p; // edges
        Eigen::Matrix2d stress;
        Eigen::Vector2d t0, t1;
        double angle0, angle1;
        double angle_span;
    };

    std::vector<Sector> fan;
};

#endif // MODEL_H
