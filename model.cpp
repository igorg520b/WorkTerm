#include "model.h"
#include "boost/math/tools/minima.hpp"
#include <iostream>
#include <iomanip>
#include <cmath>
#include <cfloat>
#include <stdexcept>
#include <Eigen/Geometry>
using namespace std;


void Model::AddSector(double ux, double uy, double vx, double vy, double sx, double sy, double txy)
{
    Sector s;
    s.u << ux, uy;
    s.v << vx, vy;
    s.stress << sx, txy, txy, sy;
    s.u_normalized = s.u.normalized();
    s.v_normalized = s.v.normalized();

    s.u_p << -s.u_normalized.y(), s.u_normalized.x();
    s.v_p << -s.v_normalized.y(), s.v_normalized.x();

    fan.push_back(s);
}


void Model::InitializeFan()
{
    auto get_angle = [](Eigen::Vector2d u, Eigen::Vector2d v)
    {
        double dot = u.dot(v)/(u.norm()*v.norm());
        if(dot > 1) dot = 1.0;
        else if(dot < -1.0) dot = -1.0;
        return acos(dot);
    };

    // precompute t0, t1 for each sector
    double end_angle =  0;
    for(Sector &s : fan)
    {
        s.angle0 = end_angle;
        s.angle_span = get_angle(s.u,s.v);
        end_angle += s.angle_span;
        s.angle1 = end_angle;
        s.t0 = s.stress * s.u_p;
        s.t1 = s.stress * s.v_p;
    }
    max_angle = end_angle;
}

void Model::evaluate_tractions(double angle_fwd, double &trac_normal, double &trac_tangential) const
{
    if(angle_fwd > max_angle || angle_fwd < 0) throw std::range_error("angle is out of range");
    if(angle_fwd == max_angle) angle_fwd -= 1e-10;
    Eigen::Vector2d traction[2];
    traction[0] = traction[1] = Eigen::Vector2d::Zero();
    Eigen::Vector2d tn, tn_p;

    double angle_bwd = angle_fwd+M_PI;
    if (angle_bwd >= 2*M_PI) angle_bwd -= 2*M_PI;

    // integrate traction
    int sector = (isBoundary || angle_fwd < angle_bwd) ? 0 : 1;

    for (std::size_t f=0; f < fan.size(); f++)
    {
        const Sector &fp = fan[f];
        if (angle_fwd >= fp.angle0 && angle_fwd < fp.angle1)
        {
            double ratio = (angle_fwd - fp.angle0)/fp.angle_span;
            tn = (fp.u_normalized*(1-ratio) + fp.v_normalized*ratio).normalized();
            tn_p = (fp.u_p*(1-ratio) + fp.v_p*ratio).normalized(); // perpendicular to tn
            Eigen::Vector2d tmult = fp.stress * tn_p;
            traction[sector] += tmult - fp.t0;
            sector = 1-sector;
            traction[sector] += fp.t1 - tmult;
        }
        else if (!isBoundary && angle_bwd >= fp.angle0 && angle_bwd < fp.angle1)
        {
            double ratio = (angle_bwd - fp.angle0)/fp.angle_span;
            Eigen::Vector2d tn_p_bwd = (fp.u_p*(1-ratio) + fp.v_p*ratio).normalized();
            Eigen::Vector2d tmult = fp.stress * tn_p_bwd;
            traction[sector] += tmult - fp.t0;
            sector = 1-sector;
            traction[sector] += fp.t1 - tmult;
        }
        else
        {
            traction[sector] += fp.t1 - fp.t0;
        }
    }

    double t0_tangential = traction[0].dot(tn);
    double t1_tangential = traction[1].dot(tn);
    double t0_normal = tn_p.dot(traction[0]);
    double t1_normal = tn_p.dot(traction[1]);

    trac_normal = t0_normal-t1_normal;
    trac_tangential = t0_tangential-t1_tangential;

    if(!isBoundary)
    {
        trac_normal/=2;
        trac_tangential/=2;
    }
}

double Model::normal_traction(const double angle_fwd) const
{
    double trac_normal, trac_tangential;
    evaluate_tractions(angle_fwd, trac_normal, trac_tangential);
    return trac_normal;
}

double Model::tangential_traction(const double angle_fwd) const
{
    double trac_normal, trac_tangential;
    evaluate_tractions(angle_fwd, trac_normal, trac_tangential);
    return trac_tangential;
}

void Model::EvaluateViaBrent()
{
    int bits = std::numeric_limits<float>::digits;

    boost::uintmax_t max_iter = 100;
    auto r = boost::math::tools::brent_find_minima(
                [=](double x){return -normal_traction(x);},
    0.0, isBoundary ? max_angle : M_PI, bits, max_iter);

    max_normal_trac = -r.second; // sign inverted since we need the maximum
    fracture_angle = r.first;
    iterations = max_iter;
}

