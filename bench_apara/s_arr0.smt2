(declare-var a (Array Int Int))
(declare-var a0 (Array Int Int))
(declare-var a1 (Array Int Int))
(declare-var a2 (Array Int Int))
(declare-var i Int)
(declare-var i1 Int)
(declare-var n Int)
(declare-var c Int)
(declare-var elem Int)
(declare-rel inv0 ((Array Int Int) Int Int Int Int))
(declare-rel inv1 ((Array Int Int) (Array Int Int) Int Int Int Int))
(declare-rel inv2 ((Array Int Int) (Array Int Int) Int Int Int Int))
(declare-rel fail ())

(rule (inv0 a 0 50 n 0))

(rule (=> (and (inv0 a i c n elem) (< i n)
  (= a0 (ite (= i 0) (store a i c) (store a i (+ 1 (select a (- i 1))))))
  (= i1 (+ i 1))) (inv0 a0 i1 c n (select a0 i))))

(rule (=> (and (inv0 a0 i c n elem) (not (< i n))) (inv1 a0 a 0 c n 0)))

(rule (=> (and (inv1 a0 a i c n elem) (< i (div n 2))
  (= a1  (store a i (+ i c)))
  (= i1 (+ i 1))) (inv1 a0 a1 i1 c n (select a1 i))))

(rule (=> (and (inv1 a0 a i c n elem) (not (< i (div n 2)))) (inv2 a0 a i c n elem)))

(rule (=> (and (inv2 a0 a i c n elem) (< i n)
  (= a2  (store a i (+ i c)))
  (= i1 (+ i 1))) (inv2 a0 a2 i1 c n (select a2 i))))

(rule (=> (and (inv2 a0 a2 i c n elem) (not (< i n))
  (<= 0 i1) (< i1 n) (not (= (select a0 i1) (select a2 i1)))) fail))

(query fail)

;void s_arr0 (int *a, int n) {
;
;  const int C = 50;
;
;  for (int i = 0; i < n; i++) {
;    if ( i == 0 ) {
;      a[i] = C;
;    } else {
;      a[i] = a[i-1] + 1;
;    }
;  }
;}
