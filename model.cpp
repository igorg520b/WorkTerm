#include "model.h"

Model::Model()
{

}

void Model::AddSector(double ux, double uy, double uz,
               double vx, double vy, double vz,
               double sx, double sy, double txy)
{
    Sector s;
    s.u << ux, uy, uz;
    s.v << vx, vy, vz;
    s.stress << sx, txy, 0, txy, sy, 0, 0, 0, 0;
    fan.push_back(s);
}

void Model::Evaluate()
{
    // average normal

    // perform calculations on sectors

    // evaluate the function for different angles

}


