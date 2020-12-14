(declare-var a (Array Int Int))
(declare-var a0 (Array Int Int))
(declare-var a1 (Array Int Int))
(declare-var a2 (Array Int Int))
(declare-var i Int)
(declare-var i1 Int)
(declare-var n Int)
(declare-var min Int)
(declare-var min1 Int)
(declare-var minp Int)
(declare-var minp1 Int)
(declare-var elem Int)

(declare-rel inv0 ((Array Int Int) Int Int Int Int))
(declare-rel inv1 ((Array Int Int) Int Int Int Int))
(declare-rel inv2 ((Array Int Int) Int Int Int Int))
(declare-rel fail ())

(rule (inv0 a 0 (select a 0) n elem))

(rule (=> (and (inv0 a i min n elem) (< i n)
  (= min1 (ite (<= min (select a i)) (select a i) min))
  (= a0 (ite (<= min (select a i)) (store a 0 min1) a))
  (= i1 (+ i 1))) (inv0 a0 i1 min1 n (select a0 i)) ))

(rule (=> (and (inv0 a i min n elem) (>= i n)) (inv1 a1 0 (select a1 0) n elem)))

(rule (=> (and (inv1 a1 i minp n elem) (<= i (div n 2))
  (= minp1 (ite (<= minp (select a1 i)) (select a1 i) minp))
  (= a2 (ite (<= minp (select a1 i)) (store a1 0 minp1) a1))
  (= i1 (+ i 1))) (inv1 a2 i1 minp1 n (select a2 i)) ))

(rule (=> (and (inv2 a1 i minp n elem) (> i (div n 2))) (inv2 a1 i minp n elem)))

(rule (=> (and (inv2 a1 i minp n elem) (< i n)
  (= minp1 (ite (<= minp (select a1 i)) (select a1 i) minp))
  (= a2 (ite (<= minp (select a1 i)) (store a1 0 minp1) a1))
  (= i1 (+ i 1))) (inv2 a2 i1 minp1 n (select a2 i)) ))

(rule (=> (and (inv0 a i min n elem) (inv2 a2 i minp n elem) (>= i n)
      (<= 0 i1) (< i1 n) (not (= (select a i1) (select a2 i1) ))) fail))

(query fail)


;
;void s_arr4 (int *a, int n) {
;  int min = a[0];
;  for (int i=0; i<n; i++) {
;    if (min <= a[i]) {
;      min = a[i];
;      a[0] = min;
;    }
;  }
;}
;

