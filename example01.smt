(declare-const P Bool)
(declare-const Q Bool)
(declare-const R Bool)

(assert (=> P Q))
(assert (=> Q R))
(assert (not (=> P R)))

(check-sat)
(get-model)
