(declare-var a (Array Int Int))
(declare-var a0 (Array Int Int))
(declare-var a1 (Array Int Int))
(declare-var a2 (Array Int Int))
(declare-var i Int)
(declare-var i1 Int)
(declare-var j Int)
(declare-var j1 Int)
(declare-var n Int)
(declare-var C1 Int)
(declare-var x Int)
(declare-var x1 Int)
(declare-var xp Int)
(declare-var xp1 Int)
(declare-var elem Int)

(declare-rel inv0 ((Array Int Int) Int Int Int Int Int Int))
(declare-rel inv1 ((Array Int Int) Int Int Int Int Int Int))
(declare-rel inv2 ((Array Int Int) Int Int Int Int Int))
(declare-rel fail ())

(rule (inv0 a 0 0 10 5 n elem))

(rule (=> (and (inv0 a i j C1 x n elem) (< i n)
  (= a1 (store a i C1))
  (= j1 (mod (+ j 1) 10))
  (= a0 (ite (= i x) (store a1 j1 (+ (select a1 j1) 1)) a1))
  (= x1 (* x 2))
  (= i1 (+ i 1))) (inv0 a0 i1 j1 C1 C2 x1 n (select a0 i))))

(rule (=> (and (inv0 a i j C1 x n elem) (>= i n)) (inv1 a2 0 0 C1 5 n elem)))

(rule (=> (and (inv1 a1 i j C1 xp n elem) (<= i (div n 2))
  (= a2 (store a1 i C1))
  (= j1 (mod (+ j 1) 10))
  (= a0 (ite (= i x) (store a2 j1 (+ (select a2 j1) 1)) a2))
  (= xp1 (* xp 2))
  (= i1 (+ i 1))) (inv1 a0 i1 j1 C1 xp1 n (select a0 i))))
  
(rule (=> (and (inv1 a1 i j C1 xp n elem) (> i (div n 2))) (inv2 a1 i j C1 xp n elem) ))

(rule (=> (and (inv2 a1 i j C1 xp n elem) (> i (div n 2))
  (= a2 (store a1 i C1))
  (= jp1 (mod (+ j 1) 10))
  (= a0 (ite (= i x) (store a2 j1 (+ (select a2 j1) 1)) a1))
  (= xp1 (* xp 2))
  (= i1 (+ i 1))) (inv2 a0 i1 j1 C1 xp1 n (select a0 i))))

(rule (=> (and (inv0 a i j C1 x n elem) (inv2 a2 i j C1 xp n elem) (>= i n)
      (<= 0 i1) (< i1 n) (not (= (select a i1) (select a2 i1) ))) fail))

(query fail)


;
;void s_arr8 (int *a, int n) {
;
;  const int C1 = 10;
;  int j = 0;
;  int x = 5;
;
;  for (int i=0; i<n; i++) {
;    a[i] = C1;
;    j = (j + 1) % 10;
;    if (i == x) {
;      a[j]++;
;      x = x*2;
;    }
;  }
;
;}
;

