(declare-var px (Array Int Int Int))
(declare-var px1 (Array Int Int Int))
(declare-var i Int)
(declare-var i1 Int)
(declare-var n Int)
(declare-rel inv ((Array Int Int Int) Int Int))
(declare-rel fail ())

(rule (inv px 0 n))

(rule (=> (and (inv px i n) (< i n)
  (= px1 (store px1 i 0 (+ (* 8 (select px i 12))
                           (* 7 (select px i 11))
                           (* 5 (select px i 9))
                           (* 4 (select px i 8))
                           (* 3 (select px i 7))
                           (* 2 (select px i 6))
                           (* 9 (+ (select px i 4) (select px i 5)))
                           (select px i 2)  ) ))
  (= i1 (+ i 1))) (inv px1 i1 n)))

(rule (=> (and (inv px i n) (not (< i n)) true) fail) )

(query fail)


;
;    /*
;     *******************************************************************
;     *   K9 -- integrate predictors
;     *   This is simple example with no dependence between iterations.
;     *   The array elements can be converted to scalar variables.
;     *   Easy to convert the loop in to two loops with disjoint chunks.
;     *   
;     *******************************************************************
;     */
;
;     for ( i=0 ; i<n ; i++ ) {
;        px[i][0] = 8*px[i][12] + 7*px[i][11] + 6*px[i][10] +
;                   5*px[i][ 9] + 4*px[i][ 8] + 3*px[i][ 7] +
;                   2*px[i][ 6] + 9*( px[i][ 4] + px[i][ 5]) + px[i][ 2];
;     }
;

