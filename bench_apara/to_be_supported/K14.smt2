(declare-var x (Array Int Int))
(declare-var e (Array Int Int))
(declare-var e1 (Array Int Int))
(declare-var d (Array Int Int))
(declare-var s (Array Int Int))
(declare-var s1 (Array Int Int))
(declare-var i Int)
(declare-var i1 Int)
(declare-var n Int)
(declare-rel inv ((Array Int Int) (Array Int Int) (Array Int Int) (Array Int Int) Int Int))
(declare-rel inv1 ((Array Int Int) (Array Int Int) (Array Int Int) (Array Int Int) Int Int))
(declare-rel fail ())

(rule (inv e x d s 0 n))

(rule (=> (and (inv e x d s i n) (< i n)
  (= e1 (store e i (select e (- (select x i ) 1))))
  (= i1 (+ i 1))) (inv e1 x d s i1 n)))

(rule (=> (and (inv e x d s i n) (not (< i n))) (inv1 e x d s 0 n) ))

(rule (=> (and (inv1 e x d s i n) (< i n)
  (= s1 (store s (select x i) (+ (select s (select x i)) (select d i))))
  (= i1 (+ i 1))) (inv1 e x d s1 i1 n)))

(rule (=> (and (inv1 e x d s i n) (not (< i n)) true) fail))

(query fail)


;
;    /*
;     *******************************************************************
;     *   K14 -- 1-D PIC (Particle In Cell)
;     *   The values of x[i] may not be linear. Challenge example
;     *   If we can discover an invariant on x and it turns out monotone
;     *   then we can parallelize the first loop.
;     *******************************************************************
;     */
;
;     for ( i=0 ; i<n ; i++ ) {
;        e[i] = e[ x[i] - 1 ];
;     }
;
;     for ( i=0 ; i<n ; i++ ) {
;        s[ x[i] ] += d[i];
;     }
;

