#ifndef __KSYNTHESIZER_HPP
#define __KSYNTHESIZER_HPP

#include "apara/Options.hpp"
#include "deep/Horn.hpp"
#include "ae/SMTUtils.hpp"

using namespace std;
using namespace ufo;

namespace apara
{
  class KSynthesizer
  {
  protected:

    ExprFactory &m_efac;
    CHCs& ruleManager;
    ExprVector& decls;
    map<int, Expr> iterators;
    map<int, bool> iterGrows;
    map<int, Expr> preconds;
    map<int, Expr> postconds;
    SMTUtils u;

    Options& o;

    map<int, ExprVector> itesPerLoop;
    map<int, map<Expr, ExprVector>> allArrStoreAccess;
    map<int, map<Expr, ExprVector>> allArrSelectAccess;
    map<Expr, map<int, map<Expr, ExprVector>>> arrStoreAccessInITEBr;
    map<Expr, map<int, map<Expr, ExprVector>>> arrSelectAccessInITEBr;
    map<Expr, map<Expr, ExprVector>> arrStoreAccessInITE;
    map<Expr, map<Expr, ExprVector>> arrSelectAccessInITE;
    map<int, map<Expr, ExprVector>> arrStoreAccessOutsideAllITE;
    map<int, map<Expr, ExprVector>> arrSelectAccessOutsideAllITE;

    map<int, map<Expr, map<Expr, ExprVector>>> FM;
    map<int, map<Expr, map<Expr, vector<bool>>>> redundantStores;
    map<int, ExprVector> KS;
    map<int, ExprVector> KSResult;

    void populateAccesses()
    {
      populateITESNAccess();
      populateAllArrAccess();
      populateOutsideAccessLists();
    }

    void getITES(const Expr term, ExprVector& iteVec)
    {
      if(o.getVerbosity() > 10) outs () << "\nFetching Top Level ITEs from " << *term << "\n\n";
      if (isOpX<ITE>(term))
        iteVec.push_back(term);
      else
        for (auto it = term->args_begin(), end = term->args_end(); it != end; ++it)
          getITES(*it, iteVec);
    }

    void populateAllArrAccess()
    {
      if(o.getVerbosity() > 1) outs () << "\nPopulating Array Accesses\n";
      for (auto & hr : ruleManager.chcs) {
        if (hr.isFact || hr.isQuery || hr.srcRelation != hr.dstRelation) continue;
        int invNum = getVarIndex(hr.srcRelation, decls);
        if(invNum < 0) continue;
        getArrAccess(hr.body, allArrStoreAccess[invNum], allArrSelectAccess[invNum]);
      }
    }

    void getArrAccess(const Expr term, map<Expr, ExprVector>& arrStoreAccess,
                      map<Expr, ExprVector>& arrSelectAccess)
    {
      if(o.getVerbosity() > 10) outs () << "\nFetching Array Accesses\n";
      if (select && isOpX<SELECT>(term))
        arrSelectAccess[term->left()].push_back(term->right());
      else if (!select && isOpX<STORE>(term))
        arrStoreAccess[term->left()].push_back(term->right());
      else
        for (auto it = term->args_begin(), end = term->args_end(); it != end; ++it)
          getArrAccess(*it, arrStoreAccess, arrSelectAccess);
    }

    void populateOutsideAccessLists()
    {
      if(o.getVerbosity() > 1) outs () << "\nPopulating Lists of Access Outside ITEs\n";
      for (int invNum=0; invNum<decls.size(); invNum++) {
        map<Expr, ExprVector>::iterator allstoit = allArrStoreAccess[invNum].begin();
        while(allstoit != allArrStoreAccess[invNum].end()) {
          for(auto & arrAcc : allstoit->second) {
            bool add = true;
            for(auto & ite : itesPerLoop[invNum]) {
              map<Expr, ExprVector>::iterator it = arrStoreAccessInITE[ite].begin();
              while(it != arrStoreAccessInITE[ite].end()) {
                if (it->first != allstoit->first) { it++; continue; }
                for(auto & iteArrAcc : it->second) {
                  if(o.getVerbosity() > 5)
                    outs () << "arrAcc:" << *arrAcc << "\t iteArrAcc: " << *iteArrAcc << "\n\n";
                  if(arrAcc == iteArrAcc) add = false;
                  if(add == false) break;
                }
                it++;
              }
              if(add == false) break;
            }
            if(add) arrStoreAccessOutsideAllITE[invNum][allstoit->first].push_back(arrAcc);
          }
          allstoit++;
        }

        map<Expr, ExprVector>::iterator allselit = allArrSelectAccess[invNum].begin();
        while( allselit != allArrSelectAccess[invNum].end() ) {
          for(auto & arrAcc : allselit->second) {
            bool add = true;
            for(auto & ite : itesPerLoop[invNum]) {
              map<Expr, ExprVector>::iterator it = arrSelectAccessInITE[ite].begin();
              while(it != arrSelectAccessInITE[ite].end()) {
                if (it->first != allselit->first) { it++; continue; }
                for(auto & iteArrAcc : it->second) {
                  if(o.getVerbosity() > 5)
                    outs () << "arrAcc:" << *arrAcc << "\t iteArrAcc: " << *iteArrAcc << "\n\n";
                  if(arrAcc == iteArrAcc) add = false;
                  if(add == false) break;
                }
                it++;
              }
              if(add == false) break;
            }
            if(add) arrSelectAccessOutsideAllITE[invNum][allselit->first].push_back(arrAcc);
          }
          allselit++;
        }
      }
    }

    void runKSynth()
    {
      if(o.getVerbosity() > 1) outs () << "\nGenerating Formulas for K Synth\n";
      for (auto & hr : ruleManager.chcs) {
        if (hr.isFact || hr.isQuery || hr.srcRelation != hr.dstRelation) continue;
        int invNum = getVarIndex(hr.srcRelation, decls);
        if(invNum < 0) continue;
        ExprVector encCons = encode(invNum);
        // generateFM(invNum);
        // generateKSynthFormulas(invNum);
        // synthesizeK(invNum);

      }
    }

    ExprVector encode(const int invNum)
    {
      Expr k1Var = bind::intConst(mkTerm<string> ("_APARA_K1_", m_efac));
      Expr k2Var = bind::intConst(mkTerm<string> ("_APARA_K2_", m_efac));
      Expr k3Var = bind::intConst(mkTerm<string> ("_APARA_K3_", m_efac));
      Expr k4Var = bind::intConst(mkTerm<string> ("_APARA_K4_", m_efac));
      Expr igeqk3 = mk<GEQ>(iterators[invNum], k3Var);
      Expr iltk4 = mk<LT>(iterators[invNum], k4Var);
      ExprVector conj_min_constraints;
      ExprVector conj_max_constraints;
      map<Expr, ExprVector>::iterator itst = allArrStoreAccess[invNum].begin();
      while (itst != allArrStoreAccess[invNum].end()) {
        for(auto & e2 : itst->second) {
          conj_min_constraints.push_back(mk<GEQ>(e2, k1Var));
          conj_max_constraints.push_back(mk<LT>(e2, k2Var));
        }
        itst++;
      }
      map<Expr, ExprVector>::iterator itsel = allArrSelectAccess[invNum].begin();
      while (itsel != allArrSelectAccess[invNum].end()) {
        for(auto & e2 : itsel->second) {
          conj_min_constraints.push_back(mk<GEQ>(e2, k1Var));
          conj_max_constraints.push_back(mk<LT>(e2, k2Var));
        }
        itsel++;
      }

      ExprSet qVars;
      qVars.insert(iterators[invNum]);
      Expr minCons  = mkNeg(eliminateQuantifiers(mkNeg(mk<IMPL>(igeqk3, conjoin(conj_min_constraints, m_efac))), qVars));
      Expr maxCons  = mkNeg(eliminateQuantifiers(mkNeg(mk<IMPL>(iltk4, conjoin(conj_max_constraints, m_efac))), qVars));

      qVars.clear();
      qVars.insert(k3Var);
      AeValSolver ae1(mk<TRUE>(m_efac), mk<AND>(mk<GEQ>(k3Var, k1Var), minCons), qVars);

      if (ae1.solve()) assert(0);
        else minCons = simplifyArithm(ae1.getSkolemFunction(false));

      qVars.clear();
      qVars.insert(k4Var);
      AeValSolver ae2(mk<TRUE>(m_efac), mk<AND>(mk<LEQ>(k4Var, k2Var), maxCons), qVars);

      if (ae2.solve()) assert(0);
        else maxCons = simplifyArithm(ae2.getSkolemFunction(false));

      if(o.getVerbosity() > 1) {
        outs () << "Printing min constraint: \n" << *minCons << "\n\n";
        outs () << "Printing max constraint: \n" << *maxCons << "\n\n";
      }
      ExprVector res;
      res.push_back(minCons);
      res.push_back(maxCons);
      res.push_back(iterators[invNum]);
      res.push_back(k1Var);
      res.push_back(k2Var);
      res.push_back(k3Var);
      res.push_back(k4Var);
      return res;
    }

    void populateITESNAccess()
    {
      if(o.getVerbosity() > 1) outs () << "\nPopulating ITEs\n";
      for (auto & hr : ruleManager.chcs) {
        if (hr.isFact || hr.isQuery || hr.srcRelation != hr.dstRelation) continue;
        int invNum = getVarIndex(hr.srcRelation, decls);
        if(invNum < 0) continue;
        getITES(hr.body, itesPerLoop[invNum]);
        for(int i=0; i<itesPerLoop[invNum].size(); i++) {
          if(o.getVerbosity() > 2) outs () << "ITE:" << *itesPerLoop[invNum][i] << "\n";
          getArrAccess(itesPerLoop[invNum][i]->right(),
                       arrStoreAccessInITEBr[itesPerLoop[invNum][i]][0],
                       arrSelectAccessInITEBr[itesPerLoop[invNum][i]][0]);
          getArrAccess(itesPerLoop[invNum][i]->last(),
                       arrStoreAccessInITEBr[itesPerLoop[invNum][i]][1],
                       arrSelectAccessInITEBr[itesPerLoop[invNum][i]][1]);
          map<Expr, ExprVector>::iterator it = arrStoreAccessInITEBr[itesPerLoop[invNum][i]][0].begin();
          while (it != arrStoreAccessInITEBr[itesPerLoop[invNum][i]][0].end()) {
            for (auto & e : it->second)
              arrStoreAccessInITE[itesPerLoop[invNum][i]][it->first].push_back(e);
            it++;
          }
          it = arrStoreAccessInITEBr[itesPerLoop[invNum][i]][1].begin();
          while (it != arrStoreAccessInITEBr[itesPerLoop[invNum][i]][1].end()) {
            for (auto & e : it->second)
              arrStoreAccessInITE[itesPerLoop[invNum][i]][it->first].push_back(e);
            it++;
          }
          it = arrSelectAccessInITEBr[itesPerLoop[invNum][i]][0].begin();
          while (it != arrSelectAccessInITEBr[itesPerLoop[invNum][i]][0].end()) {
            for (auto & e : it->second)
              arrSelectAccessInITE[itesPerLoop[invNum][i]][it->first].push_back(e);
            it++;
          }
          it = arrSelectAccessInITEBr[itesPerLoop[invNum][i]][1].begin();
          while(it != arrSelectAccessInITEBr[itesPerLoop[invNum][i]][1].end()) {
            for (auto & e : it->second)
              arrSelectAccessInITE[itesPerLoop[invNum][i]][it->first].push_back(e);
            it++;
          }
        }
      }
    }

    void generateFM(const int invNum)
    {
      if(o.getVerbosity() > 1) outs () << "\nGenerating the formula map for INV: " << invNum << "\n";
      for(int i=0; i<itesPerLoop[invNum].size(); i++) {
        map<Expr, ExprVector>& cfvec = FM[invNum][itesPerLoop[invNum][i]];
        Expr C = itesPerLoop[invNum][i]->left();
        if(o.getVerbosity() > 5) outs () << "\nGenerating for ITE: " << *C << "\n";
        map<Expr, ExprVector> allThenAccessConjunct;
        map<Expr, ExprVector> allThenAccessDisjunct;
        map<Expr, ExprVector>::iterator it = arrStoreAccessInITEBr[itesPerLoop[invNum][i]][0].begin();
        while (it != arrStoreAccessInITEBr[itesPerLoop[invNum][i]][0].end()) {
          for (auto & e1 : it->second) {
            map<Expr, ExprVector> singleAccessDisjunct;
            map<Expr, ExprVector> singleAccessConjunct;
            map<Expr, ExprVector>::iterator outit = arrStoreAccessOutsideAllITE[invNum].begin();
            while (outit != arrStoreAccessOutsideAllITE[invNum].end()) {
              if(it->first != outit->first) { outit++; continue; }
              for(auto & e2 : outit->second) {
                singleAccessDisjunct[it->first].push_back(mk<LT>(e1, e2));
                singleAccessConjunct[it->first].push_back(mk<GT>(e1, e2));
              }
              outit++;
            }
            if(singleAccessDisjunct[it->first].size() > 1)
              allThenAccessConjunct[it->first].push_back(disjoin(singleAccessDisjunct[it->first], m_efac));
            else if (singleAccessDisjunct[it->first].size() == 1)
              allThenAccessConjunct[it->first].push_back(singleAccessDisjunct[it->first][0]);
            else {}
            if(singleAccessConjunct[it->first].size() > 1)
              allThenAccessDisjunct[it->first].push_back(conjoin(singleAccessConjunct[it->first], m_efac));
            else if (singleAccessConjunct[it->first].size() == 1)
              allThenAccessDisjunct[it->first].push_back(singleAccessConjunct[it->first][0]);
            else {}
          }
          Expr fmlc1;
          if(allThenAccessConjunct[it->first].size() == 0)
            fmlc1 = mk<IMPL>(C, mk<FALSE>(m_efac));
          else
            fmlc1 = mk<IMPL>(C, conjoin(allThenAccessConjunct[it->first], m_efac));
          cfvec[it->first].push_back(fmlc1);
          if(o.getVerbosity() > 5) outs () << "\n FMLC1: " << *fmlc1 << "\n";
          Expr fmlc2 = mk<IMPL>(C, disjoin(allThenAccessDisjunct[it->first], m_efac));
          cfvec[it->first].push_back(fmlc2);
          if(o.getVerbosity() > 5) outs () << "\n FMLC2: " << *fmlc2 << "\n";
          it++;
        }

        map<Expr, ExprVector> allElseAccessConjunct;
        map<Expr, ExprVector> allElseAccessDisjunct;
        it = arrStoreAccessInITEBr[itesPerLoop[invNum][i]][1].begin();
        while (it != arrStoreAccessInITEBr[itesPerLoop[invNum][i]][1].end()) {
          for (auto & e1 : it->second) {
            map<Expr, ExprVector> singleAccessDisjunct;
            map<Expr, ExprVector> singleAccessConjunct;
            map<Expr, ExprVector>::iterator outit = arrStoreAccessOutsideAllITE[invNum].begin();
            while (outit != arrStoreAccessOutsideAllITE[invNum].end()) {
              if(it->first != outit->first) { outit++; continue; }
              for(auto & e2 : outit->second) {
                singleAccessDisjunct[it->first].push_back(mk<LT>(e1, e2));
                singleAccessConjunct[it->first].push_back(mk<GT>(e1, e2));
              }
              outit++;
            }
            if(singleAccessDisjunct[it->first].size() > 1)
              allElseAccessConjunct[it->first].push_back(disjoin(singleAccessDisjunct[it->first], m_efac));
            else if (singleAccessDisjunct[it->first].size() == 1)
              allElseAccessConjunct[it->first].push_back(singleAccessDisjunct[it->first][0]);
            else {}
            if(singleAccessConjunct[it->first].size() > 1)
              allElseAccessDisjunct[it->first].push_back(conjoin(singleAccessConjunct[it->first], m_efac));
            else if (singleAccessConjunct[it->first].size() == 1)
              allElseAccessDisjunct[it->first].push_back(singleAccessConjunct[it->first][0]);
            else {}
          }
          Expr fmlc3;
          if(allElseAccessConjunct[it->first].size() == 0)
            fmlc3 = mk<IMPL>(mk<NEG>(C), mk<FALSE>(m_efac));
          else
            fmlc3 = mk<IMPL>(mk<NEG>(C), conjoin(allElseAccessConjunct[it->first], m_efac));
          cfvec[it->first].push_back(fmlc3);
          if(o.getVerbosity() > 5) outs () << "\n FMLC3: " << *fmlc3 << "\n";
          Expr fmlc4 = mk<IMPL>(mk<NEG>(C), disjoin(allElseAccessDisjunct[it->first], m_efac));
          cfvec[it->first].push_back(fmlc4);
          if(o.getVerbosity() > 5) outs () << "\n FMLC4: " << *fmlc4 << "\n";
          it++;
        }

        checkRedundant(invNum, i);
      }
    }

    void checkRedundant(const int invNum, const int iteIndex)
    {
      if(o.getVerbosity() > 2) outs () << "\nChecking for Redundant Stores in Branches\n";
      Expr ite = itesPerLoop[invNum][iteIndex];
      map<Expr, vector<bool>>& rvec = redundantStores[invNum][ite];
      map<Expr, ExprVector>& cfvec = FM[invNum][ite];
      map<Expr, ExprVector>::iterator it = cfvec.begin();
      while (it != cfvec.end()) {
        assert(it->second.size() == 4);
        Expr fmlc1 = it->second[0];
        Expr fmlc3 = it->second[2];
        Expr prerange, postrange;
        genIteratorRanges(invNum, prerange, postrange);
        Expr range = conjoinRanges(prerange, postrange);
        Expr erlc1 = mk<AND>(range, mk<NEG>(fmlc1));
        bool rlc1 = bool(!u.isSat(erlc1));
        rvec[it->first].push_back(rlc1);
        if(o.getVerbosity() > 5) outs () << "\n Are Stores Redundant in THEN: " << rlc1 << "\n" << *erlc1 << "\n";
        bool rlc2 = bool(!u.isSat(mk<AND>(range, mk<NEG>(fmlc3))));
        rvec[it->first].push_back(rlc2);
        if(o.getVerbosity() > 5) outs () << "\n Are Stores Redundant in ELSE: " << rlc2 << "\n";
        it++;
      }
    }

    void genIteratorRanges(const int invNum, Expr& prerange, Expr& postrange)
    {
      if(o.getVerbosity() > 2) outs () << "\nCreating Range Formula for Iterator:" << *iterators[invNum] << "\n";
      Expr pre = ineqSimplifier(iterators[invNum], preconds[invNum]);
      if (pre->right() == iterators[invNum])
        prerange = (iterGrows[invNum]) ? mk<GEQ>(pre->right(), pre->left()) :
          mk<LEQ>(pre->right(), pre->left());
      else if (pre->left() == iterators[invNum])
        prerange = (iterGrows[invNum]) ? mk<GEQ>(pre->left(), pre->right()) :
          mk<LEQ>(pre->left(), pre->right());
      if(o.getVerbosity() > 3) outs () << "\nPre Formula: " << *prerange << "\n";
      ExprSet postcnjs;
      getConj(postconds[invNum], postcnjs);
      for(auto & e : postcnjs)
        if (contains(e, iterators[invNum]) && !isOpX<EQ>(e))
          postrange = e;
      postrange = ineqSimplifier(iterators[invNum], postrange);
      if(o.getVerbosity() > 3) outs () << "\nPost Formula: " << *postrange << "\n";
    }

    inline Expr conjoinRanges(const Expr prerange, const Expr postrange)
    {
      ExprSet extr;
      extr.insert(postrange);
      extr.insert(prerange);
      Expr ret = conjoin(extr, m_efac);
      return ret;
    }

    void genForallSubranges(const int invNum, const Expr kVar, const Expr iter,
                            Expr& pre, Expr& post, Expr& sr1, Expr& sr2)
    {
      if(o.getVerbosity() > 3) outs () << "\nCreating Forall Subranges\n";
      ExprVector forallsr1, forallsr2;
      forallsr1.push_back(pre);
      if (pre->right() == iterators[invNum]) {
        forallsr1.push_back((iterGrows[invNum]) ? mk<LT>(pre->right(), kVar) :
                            mk<GT>(pre->right(), kVar));
        forallsr2.push_back((iterGrows[invNum]) ? mk<GEQ>(pre->right(), kVar) :
                            mk<LEQ>(pre->right(), kVar));
      } else if (pre->left() == iterators[invNum]) {
        forallsr1.push_back((iterGrows[invNum]) ? mk<LT>(pre->left(), kVar) :
                            mk<GT>(pre->left(), kVar));
        forallsr2.push_back((iterGrows[invNum]) ? mk<GEQ>(pre->left(), kVar) :
                            mk<LEQ>(pre->left(), kVar));
      } else {}
      forallsr2.push_back(post);
      sr1 = conjoin(forallsr1, m_efac);
      sr2 = conjoin(forallsr2, m_efac);
      if(o.getVerbosity() > 0) outs () << "\nForall Subrange 1: " << *sr1 << "\n";
      if(o.getVerbosity() > 0) outs () << "\nForall Subrange 2: " << *sr2 << "\n";
    }

    void generateKSynthFormulas(const int invNum)
    {
      if(o.getVerbosity() > 2) outs () << "\nGenerating K Synthesis Formula for inv:" << invNum << "\n";
      map<Expr, map<int, ExprVector>> consMap;
      for(int i=0; i<itesPerLoop[invNum].size(); i++) {
        map<Expr, ExprVector>& cfvec_m = FM[invNum][itesPerLoop[invNum][i]];
        map<Expr, ExprVector>::iterator it = cfvec_m.begin();
        while (it != cfvec_m.end()) {
          ExprVector& cfvec = it->second;
          assert(cfvec.size() == 4);
          consMap[it->first][0].push_back(mk<AND>(mkNeg(cfvec[1]), mkNeg(cfvec[3])));
          consMap[it->first][1].push_back(mk<AND>(cfvec[1], mkNeg(cfvec[3])));
          consMap[it->first][2].push_back(mk<AND>(mkNeg(cfvec[1]), cfvec[3]));
          consMap[it->first][3].push_back(mk<AND>(cfvec[1], cfvec[3]));
          it++;
        }
      }
      Expr prerange, postrange;
      genIteratorRanges(invNum, prerange, postrange);
      Expr iterrange = conjoinRanges(prerange, postrange);
      Expr kVar = bind::intConst(mkTerm<string> ("_APARA_K_", m_efac));
      Expr ksrange = replaceAll(iterrange, iterators[invNum], kVar);
      Expr qVar = bind::intConst(mkTerm<string> ("_APARA_arr_it_", m_efac));
      // Expr e0 = replaceAll(conjoin(consMap[0], m_efac), iterators[invNum], qVar);
      // Expr e1 = replaceAll(conjoin(consMap[1], m_efac), iterators[invNum], qVar);
      // Expr e2 = replaceAll(conjoin(consMap[2], m_efac), iterators[invNum], qVar);
      // Expr e3 = replaceAll(conjoin(consMap[3], m_efac), iterators[invNum], qVar);
      ExprVector& ksvec = KS[invNum];
      Expr forallSubrange1, forallSubrange2;
      genForallSubranges(invNum, kVar, iterators[invNum], prerange, postrange,
                         forallSubrange1, forallSubrange2);
      ExprVector args;
      args.push_back(iterators[invNum]->left());
      map<Expr, map<int, ExprVector>>::iterator it = consMap.begin();
      while(it != consMap.end()) {
        args.push_back(mk<AND>(mk<IMPL>(forallSubrange1, conjoin(consMap[it->first][2], m_efac)),
                               mk<IMPL>(forallSubrange2, conjoin(consMap[it->first][1], m_efac))));
        it++;
      }
      Expr forallSubformula1 = mknary<FORALL>(args);
      ksvec.push_back(mk<AND>(ksrange, forallSubformula1));
      if(o.getVerbosity() > 0) outs () << "\nForall Subformula 1: " << *forallSubformula1 << "\n";
      args.clear();
      args.push_back(iterators[invNum]->left());
      it = consMap.begin();
      while(it != consMap.end()) {
        args.push_back(mk<AND>(mk<IMPL>(forallSubrange1, conjoin(consMap[it->first][1], m_efac)),
                               mk<IMPL>(forallSubrange2, conjoin(consMap[it->first][2], m_efac))));
        it++;
      }
      Expr forallSubformula2 = mknary<FORALL>(args);
      ksvec.push_back(mk<AND>(ksrange, forallSubformula2));
      if(o.getVerbosity() > 0) outs () << "\nForall Subformula 2: " << *forallSubformula2 << "\n";
      args.clear();
      args.push_back(iterators[invNum]->left());
      it = consMap.begin();
      while(it != consMap.end()) {
        args.push_back(mk<AND>(mk<IMPL>(forallSubrange1, conjoin(consMap[it->first][3], m_efac)),
                               mk<IMPL>(forallSubrange2, conjoin(consMap[it->first][0], m_efac))));
        it++;
      }
      Expr forallSubformula3 = mknary<FORALL>(args);
      ksvec.push_back(mk<AND>(ksrange, forallSubformula3));
      if(o.getVerbosity() > 0) outs () << "\nForall Subformula 3: " << *forallSubformula3 << "\n";
      args.clear();
      args.push_back(iterators[invNum]->left());
      it = consMap.begin();
      while(it != consMap.end()) {
        args.push_back(mk<AND>(mk<IMPL>(forallSubrange1, conjoin(consMap[it->first][0], m_efac)),
                               mk<IMPL>(forallSubrange2, conjoin(consMap[it->first][3], m_efac))));
        it++;
      }
      Expr forallSubformula4 = mknary<FORALL>(args);
      ksvec.push_back(mk<AND>(ksrange, forallSubformula4));
      if(o.getVerbosity() > 0) outs () << "\nForall Subformula 4: " << *forallSubformula4 << "\n";
    }

    void synthesizeK(const int invNum)
    {
      if(o.getVerbosity() > 1) outs () << "\nSynthesizing K for inv: " << invNum << "\n";
      ExprVector& kresvec = KSResult[invNum];
      ExprVector& ksvec = KS[invNum];
      for(int i=0; i<4; i++)
        kresvec.push_back(u.isSat(ksvec[i]) ? getSkolemExpr(ksvec[i]) : mkMPZ(-1, m_efac));
      Expr K = mkMPZ(-1, m_efac);
      for(int i=0; i<4; i++)
        if(kresvec[i] != mkMPZ(-1, m_efac)) K = kresvec[i];
      if(o.getVerbosity() > 1) outs () << "\nThe synthesized value of K is: " << K << "\n";
    }

    Expr getSkolemExpr(const Expr e)
    {
      // TODO: Interface with skolem generation
      return mkMPZ(-1, m_efac);
    }

    bool checkAllAccessesProt()
    {
      if(o.getVerbosity() > 1) outs () << "\nChecking Array Accesses\n";
      populateAccesses();
      runKSynth();

      for (auto & hr : ruleManager.chcs)
      {
        if (hr.isFact || hr.isQuery || hr.srcRelation != hr.dstRelation) continue;
        int invNum = getVarIndex(hr.srcRelation, decls);
        if(invNum < 0) continue;
      }

      if(o.getVerbosity() > 1) outs () << "\nAll Accesses Checked\n";
      return true;
    }

  public:
    KSynthesizer (ExprFactory &efac, CHCs& r, vector<Expr>& d, map<int, Expr>& iters,
                  map<int, bool>& iterg, map<int, Expr>& prec, map<int, Expr>& postc,
                  Options& opt) :
      m_efac(efac), ruleManager(r), decls(d), iterators(iters), iterGrows(iterg),
      preconds(prec), postconds(postc), o(opt), u(efac) {}

    inline bool checkAllAccesses() { return checkAllAccessesProt(); }

  };

}

#endif

