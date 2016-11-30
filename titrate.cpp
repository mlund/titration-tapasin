#include <faunus/faunus.h>

using namespace Faunus;
using namespace Faunus::Potential;

typedef Space<Geometry::Sphere> Tspace;
typedef CombinedPairPotential<Coulomb, LennardJonesLB> Tpairpot;

int main() {
  InputMap mcp("titrate.json");
  Tspace spc(mcp);
  auto pot = Energy::Nonbonded<Tspace,Tpairpot>(mcp)
    + Energy::EquilibriumEnergy<Tspace>(mcp);

  pot.setSpace(spc);
  spc.load("state",Tspace::RESIZE);

  Move::Propagator<Tspace> mv(mcp,pot,spc);
  Analysis::ChargeMultipole mp;

  EnergyDrift sys;
  sys.init( Energy::systemEnergy(spc,pot,spc.p) );

  cout << atom.info() + spc.info() + pot.info()
    + textio::header("MC Simulation Begins!");

  auto g = spc.groupList().at(0);

  MCLoop loop(mcp);
  while ( loop[0] ) {
    while ( loop[1] ) {
      sys+=mv.move();
      mp.sample( *g, spc );
    } // end of micro loop

    sys.checkDrift( Energy::systemEnergy(spc,pot,spc.p) );
    cout << loop.timing();

  } // end of macro loop

  cout << loop.info() + sys.info() + mv.info() + mp.info();

  FormatPQR().save("confout.pqr", spc.p);
  spc.save("state");
}
