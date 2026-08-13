# Simulation Report

Trials per bet: 100000

Elapsed time: 12.33 seconds

House edge is reported as the expected player loss divided by the initial wager size.

## Executive Summary

The current run is directionally consistent with the configured table math, but it is still noisy for low-edge and long-resolution bets. `38/40` measured edges land within `1.0` percentage point of their theoretical value, while `29/40` land within `0.5` points. The largest gaps show up in volatile proposition bets and a few multi-roll bets (`AnyCraps`, `Hardway6`, `PassLine`), which is expected at `100000` trials but is still large enough that the report should be read as a validation snapshot rather than a final certification.

The strongest qualitative signals are intact: true-odds bets are effectively zero-edge, Big 6/8 and proposition bets remain the most expensive wagers, and place/buy bets sit in the expected middle tier. One important caveat is that the engine currently models lay bets with no commission, which makes their theoretical house edge `0.000%`; if the product intends to represent casino-standard vig on lay bets, that assumption should be revisited before treating these figures as production-ready.

## Expected vs Measured House Edge

Expected values below are based on the current engine configuration in `table_config_vegas.json`, including buy commission charged only on wins and no commission on lay bets.

| Bet | Expected edge | Measured edge | Delta |
| --- | ---: | ---: | ---: |
| PassLine | 1.414% | 2.260% | +0.846 pts |
| DontPass | 1.364% | 1.362% | -0.002 pts |
| Come | 1.414% | 0.882% | -0.532 pts |
| DontCome | 1.364% | 1.068% | -0.296 pts |
| Odds4 | 0.000% | 0.364% | +0.364 pts |
| Odds5 | 0.000% | 0.177% | +0.178 pts |
| Odds6 | 0.000% | 0.404% | +0.404 pts |
| Odds8 | 0.000% | -0.003% | -0.003 pts |
| Odds9 | 0.000% | 0.105% | +0.105 pts |
| Odds10 | 0.000% | 0.598% | +0.598 pts |
| Lay4 | 0.000% | -0.051% | -0.051 pts |
| Lay5 | 0.000% | 0.352% | +0.352 pts |
| Lay6 | 0.000% | -0.104% | -0.104 pts |
| Lay8 | 0.000% | 0.102% | +0.102 pts |
| Lay9 | 0.000% | 0.040% | +0.040 pts |
| Lay10 | 0.000% | -0.008% | -0.008 pts |
| Place4 | 6.667% | 6.564% | -0.103 pts |
| Place5 | 4.000% | 4.130% | +0.130 pts |
| Place6 | 1.515% | 0.808% | -0.707 pts |
| Place8 | 1.515% | 1.250% | -0.265 pts |
| Place9 | 4.000% | 4.473% | +0.473 pts |
| Place10 | 6.667% | 6.598% | -0.069 pts |
| Buy4 | 3.333% | 3.091% | -0.243 pts |
| Buy5 | 3.000% | 2.522% | -0.478 pts |
| Buy6 | 2.727% | 2.698% | -0.029 pts |
| Buy8 | 2.727% | 2.373% | -0.354 pts |
| Buy9 | 3.000% | 2.697% | -0.303 pts |
| Buy10 | 3.333% | 3.276% | -0.057 pts |
| Field | 2.778% | 2.133% | -0.645 pts |
| Big6 | 9.091% | 9.476% | +0.385 pts |
| Big8 | 9.091% | 9.462% | +0.371 pts |
| Hardway4 | 11.111% | 11.032% | -0.079 pts |
| Hardway6 | 9.091% | 10.860% | +1.769 pts |
| Hardway8 | 9.091% | 10.090% | +0.999 pts |
| Hardway10 | 11.111% | 11.208% | +0.097 pts |
| AnyCraps | 11.111% | 9.248% | -1.863 pts |
| AceDeuce | 11.111% | 12.064% | +0.953 pts |
| Yo | 11.111% | 10.496% | -0.615 pts |
| Horn | 12.500% | 13.210% | +0.710 pts |
| Any7 | 16.667% | 16.290% | -0.377 pts |

## Measured Results

| Bet | Avg net / wager | House edge |
| --- | ---: | ---: |
| Lay6 | 0.001037 | -0.103667% |
| Lay4 | 0.000515 | -0.051500% |
| Lay10 | 0.000080 | -0.008000% |
| Odds8 | 0.000032 | -0.003200% |
| Lay9 | -0.000400 | 0.040000% |
| Lay8 | -0.001017 | 0.101667% |
| Odds9 | -0.001050 | 0.105000% |
| Odds5 | -0.001775 | 0.177500% |
| Lay5 | -0.003517 | 0.351667% |
| Odds4 | -0.003640 | 0.364000% |
| Odds6 | -0.004038 | 0.403800% |
| Odds10 | -0.005980 | 0.598000% |
| Place6 | -0.008078 | 0.807833% |
| Come | -0.008820 | 0.882000% |
| DontCome | -0.010680 | 1.068000% |
| Place8 | -0.012498 | 1.249833% |
| DontPass | -0.013620 | 1.362000% |
| Field | -0.021330 | 2.133000% |
| PassLine | -0.022600 | 2.260000% |
| Buy8 | -0.023732 | 2.373200% |
| Buy5 | -0.025223 | 2.522275% |
| Buy9 | -0.026969 | 2.696875% |
| Buy6 | -0.026985 | 2.698480% |
| Buy4 | -0.030907 | 3.090700% |
| Buy10 | -0.032763 | 3.276300% |
| Place5 | -0.041296 | 4.129600% |
| Place9 | -0.044728 | 4.472800% |
| Place4 | -0.065640 | 6.564000% |
| Place10 | -0.065976 | 6.597600% |
| AnyCraps | -0.092480 | 9.248000% |
| Big8 | -0.094620 | 9.462000% |
| Big6 | -0.094760 | 9.476000% |
| Hardway8 | -0.100900 | 10.090000% |
| Yo | -0.104960 | 10.496000% |
| Hardway6 | -0.108600 | 10.860000% |
| Hardway4 | -0.110320 | 11.032000% |
| Hardway10 | -0.112080 | 11.208000% |
| AceDeuce | -0.120640 | 12.064000% |
| Horn | -0.528390 | 13.209750% |
| Any7 | -0.162900 | 16.290000% |

## Recommendations

1. Increase the main run to at least `1000000` trials per bet before using this report to sign off on balance or fairness claims, especially for zero-edge odds/lay bets and long-resolution wagers.
2. Add theoretical edge generation directly to the simulator output so the Markdown report is self-checking instead of relying on manual comparison.
3. Clarify the intended vig model for lay bets. The current rules imply `0.000%` theoretical edge because no commission is charged, which is atypical for casino craps.
4. Track a tolerance threshold in CI, such as flagging any measured result that drifts more than `0.5` or `1.0` percentage point from theory after a high-trial run.
