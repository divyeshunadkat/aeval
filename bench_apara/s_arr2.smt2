(declare-var a (Array Int Int))
(declare-var a0 (Array Int Int))
(declare-var a1 (Array Int Int))
(declare-var a2 (Array Int Int))
(declare-var i Int)
(declare-var i1 Int)
(declare-var n Int)
(declare-var C1 Int)
(declare-var C2 Int)
(declare-var C3 Int)
(declare-var elem Int)

(declare-rel inv0 ((Array Int Int) Int Int Int Int Int Int))
(declare-rel inv1 ((Array Int Int) Int Int Int Int Int Int))
(declare-rel inv2 ((Array Int Int) Int Int Int Int Int Int))
(declare-rel fail ())

(rule (inv0 a 0 10 20 (div n 2) n elem))

(rule (=> (and (inv0 a i C1 C2 C3 n elem) (< i n)
  (= a1 (store a i C1))
  (= a0 (ite (= i C3) (store a1 0 C2) a1))
  (= i1 (+ i 1))) (inv0 a0 i1 C1 C2 C3 n (select a0 i))))

(rule (=> (and (inv0 a2 i C1 C2 C3 n elem) (>= i n)) (inv1 a i C1 C2 C3 n elem)))

(rule (=> (and (inv1 a i C1 C2 C3 n elem) (<= i (div n 2))
  (= a1 (store a i C1))
  (= a2 (ite (= i C3) (store a1 0 C2) a1))
  (= i1 (+ i 1))) (inv1 a2 i1 C1 C2 C3 n (select a2 i))))
  
(rule (=> (and (inv1 a i C1 C2 C3 n elem) (> i (div n 2))) (inv2 a i C1 C2 C3 n elem) ))

(rule (=> (and (inv2 a i C1 C2 C3 n elem) (> i (div n 2))
  (= a2 (store a i C1))
  (= i1 (+ i 1))) (inv1 a2 i1 C1 C2 C3 n (select a1 i))))

(rule (=> (and (inv0 a0 i C1 C2 C3 n elem) (inv2 a2 i C1 C2 C3 n elem) (>= i n)
      (<= 0 i1) (< i1 n) (not (= (select a0 i1) (select a2 i1) ))) fail))

(query fail)


;void s_arr2 (int *a, int n) {
;
;  const int C1 = 10;
;  const int C2 = 20;
;  const int C3 = n/2;
;
;  for (int i=0; i<n; i++) {
;    a[i] = C1;
;    if (i == C3) {
;      a[0] = C2;
;    }
;  }
;}

