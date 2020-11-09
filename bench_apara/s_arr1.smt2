(declare-var a (Array Int Int))
(declare-var a0 (Array Int Int))
(declare-var a1 (Array Int Int))
(declare-var a2 (Array Int Int))
(declare-var i Int)
(declare-var i1 Int)
(declare-var n Int)
(declare-var C1 Int)
(declare-var C2 Int)
(declare-var elem Int)

(declare-rel inv0 ((Array Int Int) Int Int Int Int Int))
(declare-rel inv1 ((Array Int Int) Int Int Int Int Int))
(declare-rel inv2 ((Array Int Int) Int Int Int Int Int))
(declare-rel fail ())

(rule (inv0 a 0 20 10 n 0))

(rule (=> (and (inv0 a i C1 C2 n elem) (< i n)
  (= a0 (ite (= i 0) (store a i C1) (store a i (+ (select a (- i 1)) C2))))
  (= i1 (+ i 1))) (inv0 a0 i1 C1 C2 n (select a0 i)) ))

(rule (=> (and (inv0 a i C1 C2 n elem) (>= i n)) (inv1 a1 0 C1 C2 n elem)))

(rule (=> (and (inv1 a1 i C1 C2 n elem) (< i (div n 2))
  (= a2 (store a1 i (+ (* i C2) C1)))
  (= i1 (+ i 1))) (inv1 a2 i1 C1 C2 n (select a2 i)) ))

(rule (=> (and (inv1 a1 i C1 C2 n elem) (>= i (div n 2))) (inv2 a1 i C1 C2 n elem))) 

(rule (=> (and (inv21 a i C1 C2 n elem) (< i n)
  (= a2 (store a1 i (+ (* i C2) C1)))
  (= i1 (+ i 1))) (inv2 a2 i1 C1 C2 n (select a2 i)) ))

(rule (=> (and (inv2 a2 i C1 C2 n elem) (inv0 a i C1 C2 n elem) (>= i n)
      (<= 0 i1) (< i1 n) (not (= (select a i1) (select a2 i1) ))) fail))

(query fail)

;
;void s_arr1 (int *a, int n) {
;
;  const int C1 = 20;
;  const int C2 = 10;
;
;  for (int i = 0; i < n; i++) {
;    if ( i == 0 ) {
;      a[i] = C1;
;    } else {
;      a[i] = a[i-1] + C2;
;    }
;  }
;}
;

