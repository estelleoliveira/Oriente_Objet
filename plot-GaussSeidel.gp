set title "Comparaison des solutions Gauss-Seidel"
set xlabel "x"
set ylabel "u(x)"
set yrange [14:31]
plot "build/Var_u_kp1_gs.data" using 1:2 with lines title "Gauss-Seidel", \
     "build/Var_u_ref_gs.data" using 1:2 with lines title "Solution Exacte", \
     "build/Var_u_k_gs.data" using 1:2 with lines title "Condition Initiale"
pause -1