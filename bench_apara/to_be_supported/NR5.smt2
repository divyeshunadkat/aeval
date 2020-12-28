(declare-var a (Array Int Int))
(declare-var a1 (Array Int Int))
(declare-var i Int)
(declare-var i1 Int)
(declare-var j Int)
(declare-var j1 Int)
(declare-var C1 Int)
(declare-var n Int)
(declare-rel inv ((Array Int Int) Int Int Int Int))
(declare-rel inv1 ((Array Int Int) Int Int Int Int))
(declare-rel fail ())

(rule (inv a 0 1 4 n))

(rule (=> (and (inv a i j C1 n) (<= i (div n 5)) (= i1 (+ i 1)))
          (inv1 a i1 1 n) ))

(rule (=> (and (inv1 a i j C1 n) (<= j 5)
  (= a1 (ite (>= j C1) (store a (- (* i 5) j) 0) (store a (- (* i 5) j) j))
  (= j1 (+ j 1))) (inv1 a i j1 n) ))

(rule (=> (and (inv1 a i j C1 n) (> j 5) (inv a i j C1 n) ))

(rule (=> (and (inv a i j C1 n) (> i (div n 5)) true) fail ))

(query fail)


;
;  int C1=4;
;  for(int i = 1; i <= n/5; i++) {
;    for(int j = 1; j <= 5; j++) {
;      if(j >= C1) {
;        a[i*5 - j] = 0;
;      } else {
;        a[i*5 - j] = j;
;      }
;    }
;  }
;
