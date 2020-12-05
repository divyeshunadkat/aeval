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
    map<int, ExprSet> invExprSet;
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
      if(o.getVerbosity() > 1)
        cout << "\n\nBootstrapping Invariants\n\n";
      bool res = ds.bootstrap();
      if(o.getVerbosity() > 1) ds.printSolution();
      return res;
    }

    bool learnInvs()
    {
      if(o.getVerbosity() > 1)
        cout << "\n\nLearning Invariants \n\n";
      std::srand(std::time(0));
      ds.synthesize(maxAttempts, (char*)"");
      if(o.getVerbosity() > 1) ds.printSolution();
      return true;
    }

    void transformCHCs()
    {
      if(o.getVerbosity() > 1)
        cout << "\n\nTransforming CHCs for output \n\n";
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
      if(o.getVerbosity() > 1)
        cout << "\n\nSimplifying Invariant Expressions \n\n";
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

    void getEqualityInvs()
    {
      if(o.getVerbosity() > 1) cout << "\n\nFetching Equality Invariants\n\n";
      for (int i = 0; i < ds.getDecls().size(); i++)
      {
        Expr rel = ds.getDecls()[i];
        SamplFactory& sf = ds.getSFS()[i].back();
        ExprSet eqInvs;
        std::set<Expr>::iterator it1, it2;
        for (it1 = sf.learnedExprs.begin(); it1 != sf.learnedExprs.end(); ++it1) {
          for (it2 = it1; it2 != sf.learnedExprs.end(); ++it2) {
            if(it1 == it2) continue;
            Expr e1 = *it1, e2 = *it2;
            if (!(containsOp<FORALL>(e1) && containsOp<FORALL>(e2))) continue;
            if (e1->arity() != e2->arity()) continue;
            if (!(isOpX<IMPL>(e1->last()) && isOpX<IMPL>(e2->last()))) continue;

            // Following is available by construction for single loop CHC programs
            // if (!u.checkSameExpr(e1->last()->left(), e2->last()->left())) continue;
            // TODO: Support the above for multiple loops
            Expr impl1 = e1->last(), impl2 = e2->last();
            if (!(isOp<ComparissonOp>(impl1->last()) &&
                  isOp<ComparissonOp>(impl2->last())) ) continue;
            if(!u.isSat(mk<EQ>(mkMPZ(0, m_efac), mk<PLUS>(impl1->last()->left(), impl2->last()->left())),
                        mk<EQ>(mkMPZ(0, m_efac), mk<PLUS>(impl1->last()->right(), impl2->last()->right())))) continue;
            // isIntConst(e1->last()->right(), e2->last()->right());
            if(o.getVerbosity() > 1) {
              outs () << "\n\nIdentified the following two expressions that form a equality\n\n";
              u.print(impl1->last()); outs () << "\n\n"; u.print(impl2->last());
            }
            // eqInvs.insert( changeOperatortoEQ(e1) );
          }
        }
        invExprSet[i] = eqInvs;
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
      if(o.getVerbosity() > 1) ds.setPrintLog(true);
      initializeLearner();
    }

    bool makeParallel()
    {
      if(o.getVerbosity() > 1) outs () << "\nInvoked the Parallelization Engine\n";
      KSynthesizer ksynth(m_efac, ruleManager, ds.getDecls(), ds.getIterators(),
                          ds.getIterGrows(), ds.getPreConds(), ds.getPostConds(), o);
      bool co = ksynth.checkOverlap();
      if(!co) {
        bool ca = ksynth.runKSynthesizer();
      } else {
        bool bs = bootstrapInvs();
        if(!bs) {
          if(o.getVerbosity() > 1) outs () << "\nBootstrapping worked\n";
        } else {
          if(o.getVerbosity() > 1) outs () << "\nInvariant synthesis invoked\n";
          learnInvs();
          if(o.getVerbosity() > 1) outs () << "\nInvariant synthesis successful\n";
        }
      }
      getEqualityInvs();
      // printInvs();
      /*
      getSimplifiedInvExpr();
      transformCHCs();
      outputParallelVersion();
      */
      return parallelized;
    }

    void printInvs(bool simplify = true)
    {
      for (int i = 0; i < ds.getDecls().size(); i++)
      {
        Expr res = invExpr[i];
        u.print(res);
        outs () << "\n\n";
      }
    }

  };

  inline static bool parallelizeCHCs(Options& o)
  {
    Parallelize par(o);
    return par.makeParallel();
  }

}

#endif
