#ifndef __LEARNER_HPP
#define __LEARNER_HPP

#include <fstream>
#include "apara/Options.hpp"
#include "deep/RndLearnerV2.hpp"
#include "deep/RndLearnerV3.hpp"

using namespace std;
using namespace ufo;

namespace apara
{
  class Learner : public RndLearnerV3
  {
  protected:
    Options& o;

  public:

    Learner (ExprFactory &efac, EZ3 &z3, CHCs& r, bool freqs, bool aggp, Options& opt) :
      RndLearnerV3 (efac, z3, r, freqs, aggp), o(opt) {}

    inline ExprVector& getDecls() { return decls; }
    inline vector<vector<SamplFactory>>& getSFS() { return sfs; }
    inline map<int, Expr>& getIterators() { return iterators; }
    inline map<int, bool>& getIterGrows() { return iterGrows; }
    inline map<int, Expr>& getPreConds() { return preconds; }
    inline map<int, Expr>& getPostConds() { return postconds; }
    inline void setPrintLog(bool pLog) { printLog = pLog; }

  };

}

#endif
