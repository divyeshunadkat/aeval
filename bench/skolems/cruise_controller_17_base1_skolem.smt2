(declare-fun $cruiseThrottle$2 () Real)
(declare-fun $mode$2 () Int)
(declare-fun $cruiseThrottle$~1 () Real)
(declare-fun $OK$2 () Bool)

(assert (let ((a!1 (= 0.0 (ite true (to_real (div 0 10)) $cruiseThrottle$~1))))
(let ((a!2 (= $OK$2 (or (not (= 5 4)) a!1))))
  (and a!2 (= $mode$2 5) (= $cruiseThrottle$2 0.0)))))
(check-sat)
