(declare-fun $~flatten0$3 () Bool)
(declare-fun $V275_X$3 () Bool)
(declare-fun $V274_X$3 () Bool)
(declare-fun $V273_X$3 () Bool)
(declare-fun $V60_open_door$3 () Bool)
(declare-fun $V272_warning_start_cant_become_true_when_door_is_opening$3
             ()
             Bool)
(declare-fun $V270_warning_start_and_in_station_go_down_simultaneously$3
             ()
             Bool)
(declare-fun $V276_X$3 () Bool)
(declare-fun $V267_tramway_doesnt_start_if_not_door_ok$3 () Bool)
(declare-fun $V265_door_doesnt_close_if_not_asked$3 () Bool)
(declare-fun $V269_initially_not_in_station$3 () Bool)
(declare-fun $V252_X$0 () Bool)
(declare-fun $V252_X$3 () Bool)
(declare-fun $V253_between_A_and_X$3 () Bool)
(declare-fun $door_is_open$0 () Bool)
(declare-fun $V268_door_initially_closed$3 () Bool)
(declare-fun $V59_prop_ok$3 () Bool)
(declare-fun $V264_env_ok$3 () Bool)
(declare-fun $V266_door_doesnt_open_if_not_asked$3 () Bool)
(declare-fun $V58_env_always_ok$3 () Bool)
(declare-fun $V253_between_A_and_X$0 () Bool)
(declare-fun $~flatten0$0 () Bool)
(declare-fun $in_station$1 () Bool)
(declare-fun $V58_env_always_ok$0 () Bool)
(declare-fun $V61_close_door$3 () Bool)
(declare-fun $V251_door_opens_before_leaving_station$3 () Bool)
(declare-fun $warning_start$0 () Bool)
(declare-fun $V62_door_ok$0 () Bool)
(declare-fun $V276_X$0 () Bool)
(declare-fun $OK$3 () Bool)
(declare-fun $warning_start$1 () Bool)
(declare-fun $V273_X$0 () Bool)
(declare-fun $door_is_open$1 () Bool)
(declare-fun $V271_warning_start_only_in_station$3 () Bool)
(declare-fun $V275_X$0 () Bool)
(declare-fun $V250_door_doesnt_open_out_of_station$3 () Bool)
(declare-fun $V274_X$0 () Bool)
(declare-fun $request_door$0 () Bool)

(assert (let ((a!1 (or (not (or (not $in_station$1) (not $V274_X$0))) $V62_door_ok$0))
      (a!3 (and (or (and $request_door$0 (not $warning_start$0))
                    (and (not $~flatten0$0) $V253_between_A_and_X$0))
                (or (not $in_station$1) (not $V252_X$0))))
      (a!5 (or false (not (or (not $door_is_open$1) (not $V273_X$0)))))
      (a!6 (or $V62_door_ok$0 (not (or (not $in_station$1) (not $V274_X$0)))))
      (a!7 (= (ite false false (or (not $in_station$1) (not $V275_X$0)))
              (ite false false (or (not $warning_start$1) (not $V276_X$0)))))
      (a!8 (or (not (or $warning_start$1 (not $warning_start$0))) (not true)))
      (a!10 (ite (and (not $warning_start$0) $request_door$0)
                 true
                 (ite (ite false false $~flatten0$0)
                      false
                      $V253_between_A_and_X$0))))
(let ((a!2 (and (not (or (not $door_is_open$1) (not $V273_X$0)))
                a!1
                (= (or (not $in_station$1) (not $V275_X$0))
                   (or (not $warning_start$1) (not $V276_X$0)))
                (or (not $warning_start$1) $in_station$1)
                (not (or $warning_start$1 (not $warning_start$0)))
                $V58_env_always_ok$0))
      (a!9 (and true
                true
                true
                a!5
                a!6
                a!7
                (or $in_station$1 (not $warning_start$1))
                a!8))
      (a!11 (and a!10
                 (ite false false (or (not $in_station$1) (not $V252_X$0))))))
(let ((a!4 (or (not a!2)
               (and (or (not $door_is_open$1) $in_station$1) (not a!3))))
      (a!12 (or (not (and $V58_env_always_ok$0 a!9))
                (and (or $in_station$1 (not $door_is_open$1)) (not a!11))))
      (a!13 (= $V59_prop_ok$3
               (and (or $in_station$1 (not $door_is_open$1)) (not a!11)))))
(let ((a!14 (and (= $OK$3 a!12)
                 (= $V58_env_always_ok$3 (and $V58_env_always_ok$0 a!9))
                 a!13
                 (= $V264_env_ok$3 a!9)
                 (= $V250_door_doesnt_open_out_of_station$3
                    (or $in_station$1 (not $door_is_open$1)))
                 (= $V251_door_opens_before_leaving_station$3 (not a!11))
                 (= $V253_between_A_and_X$3 a!10)
                 (= $V252_X$3 (not $in_station$1))
                 (= $V266_door_doesnt_open_if_not_asked$3 a!5)
                 (= $V265_door_doesnt_close_if_not_asked$3 true)
                 (= $V267_tramway_doesnt_start_if_not_door_ok$3 a!6)
                 (= $V268_door_initially_closed$3 true)
                 (= $V269_initially_not_in_station$3 true)
                 (= $V270_warning_start_and_in_station_go_down_simultaneously$3
                    a!7)
                 (= $V271_warning_start_only_in_station$3
                    (or $in_station$1 (not $warning_start$1)))
                 (= $V272_warning_start_cant_become_true_when_door_is_opening$3
                    a!8)
                 (= $V60_open_door$3 true)
                 (= $V273_X$3 (not $door_is_open$1))
                 (= $V61_close_door$3 false)
                 (= $V274_X$3 (not $in_station$1))
                 (= $V275_X$3 (not $in_station$1))
                 (= $V276_X$3 (not $warning_start$1))
                 (= $~flatten0$3 $door_is_open$0))))
  (ite a!4 a!14 true))))))
(check-sat)
