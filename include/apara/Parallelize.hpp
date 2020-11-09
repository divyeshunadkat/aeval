#ifndef __PARALLELIZE_HPP
#define __PARALLELIZE_HPP

#include <fstream>
#include "apara/Options.hpp"
#include "apara/Learner.hpp"
#include "apara/KSynthesizer.hpp"

using namespace std;
using namespace ufo;

namespace apara
{
  class Parallelize
  {
  protected:
    ExprFactory m_efac;
    EZ3 m_z3;
    CHCs ruleManager;
    ZSolver<EZ3> m_smt_solver;
    SMTUtils u;
    Options& o;
    Learner ds;

    map<int, Expr> invExpr;
    int maxAttempts = 2000000;
    bool parallelized = false;

    void initializeLearner()
    {
      BndExpl bnd(ruleManager);
      if (!ruleManager.hasCycles())
      {
        bnd.exploreTraces(1, ruleManager.chcs.size(), true);
        return;
      }
      map<Expr, ExprSet> cands;
      for (auto& dcl: ruleManager.decls) ds.initializeDecl(dcl);
      for (int i = 0; i < ruleManager.cycles.size(); i++)
      {
        Expr pref = bnd.compactPrefix(i);
        cands[ruleManager.chcs[ruleManager.cycles[i][0]].srcRelation].insert(pref);
        ds.initArrayStuff(bnd, i, pref);
      }
      vector<string> behaviorfiles;
      ds.getDataCandidates(cands, behaviorfiles); // Data Learning enabled by default!
      for (auto& dcl: ruleManager.wtoDecls) ds.getSeeds(dcl, cands);
      ds.refreshCands(cands);
      for (auto& dcl: ruleManager.decls) ds.doSeedMining(dcl->arg(0), cands[dcl->arg(0)], false);
      ds.calculateStatistics();
    }

    bool bootstrapInvs()
    {
      if(ds.bootstrap()) return true;
      if(o.getVerbosity() > 1) ds.printSolution();
      return false;
    }

    void learnInvs()
    {
      std::srand(std::time(0));
      ds.synthesize(maxAttempts, (char*)"");
    }

    void transformCHCs()
    {
      for (auto & hr : ruleManager.chcs)
      {
        if(hr.isFact || hr.isQuery) continue;
        if(hr.srcRelation != hr.dstRelation) continue;
        int invNum = getVarIndex(hr.srcRelation, ds.getDecls());
        hr.body = invExpr[invNum];
      }
      for (auto & hr : ruleManager.chcs)  // Cannot be merged with loop above due to continue stmts
        hr.rewriteToOrigVars();
    }

    void getSimplifiedInvExpr()
    {
      for (int i = 0; i < ds.getDecls().size(); i++)
      {
        Expr rel = ds.getDecls()[i];
        SamplFactory& sf = ds.getSFS()[i].back();
        ExprSet lms;
        for( auto & e : sf.learnedExprs )
          if (containsOp<FORALL>(e)) lms.insert( e->last() );
          else lms.insert(e);
        /*
        // TODO: Replace the qv with the iterator
        for (auto &hr: ruleManager.chcs)
        {
          int iNum = getVarIndex(hr.srcRelation, decls);
          if(iNum != i) continue;
          if(hr.isFact || hr.isQuery) continue;
          for (int h = 0; h < hr.srcVars.size(); h++)
          {
              if(qvar == hr.srcVars[h])
                outs () << "Original iterator variable:" << *(hr.origSrcArgs[h]) << "\n\n";
            }
          }
        */
        Expr tmp = conjoin(lms, m_efac);
        if (!containsOp<FORALL>(tmp)) u.removeRedundantConjuncts(lms);
        Expr res = simplifyArithm(tmp);
        invExpr[i] = res;
      }
    }

    bool outputParallelVersion()
    {
      if (!parallelized) {
        if(o.getVerbosity() > 20)
          cout << "\n\nUnable to parallelize the given input file " << o.getInputFile() << "\n\n";
        return false;
      }
      if(o.getVerbosity() > 20)
        cout << "\nWriting the parallelized version to " << o.getOutputFile() << "\n";
      std::ofstream outputFileStream;
      outputFileStream.open(o.getOutputFile());
      outputFileStream << "#Parallelized version of " << o.getInputFile() << "\n\n";
      outputFileStream << ruleManager;
      outputFileStream.flush();
      if(o.getVerbosity() > 20)
        cout << "\nParallelized version written to " << o.getOutputFile() << "\n\n";
      return true;
    }

  public:
    Parallelize (Options& opt) :
      m_z3(m_efac), ruleManager(m_efac, m_z3), m_smt_solver (m_z3), u(m_efac),
      o(opt), ds(m_efac, m_z3, ruleManager, false, false, o)
    {
      ruleManager.parse(o.getInputFile());
      initializeLearner();
    }

    bool makeParallel()
    {
      if(o.getVerbosity() > 1) outs () << "\nInvoked the Parallelization Engine\n";
      bool bs = bootstrapInvs();
      ds.checkAccesses();
      if(!bs) learnInvs();
      getSimplifiedInvExpr();
      transformCHCs();
      outputParallelVersion();
      return parallelized;
    }

  };

  inline static bool parallelizeCHCs(Options& o)
  {
    Parallelize par(o);
    return par.makeParallel();
  }

}

#endif
