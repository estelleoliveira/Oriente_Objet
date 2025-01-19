set title "Comparaison des solutions Jacobi"
set xlabel "x"
set yrange [14:31]
set ylabel "u(x)"
plot "build/Var_u_kp1.data" using 1:2 with lines title "Jacobi", \
     "build/Var_u_ref.data" using 1:2 with lines title "Solution Exacte", \
     "build/Var_u_k.data" using 1:2 with lines title "Condition Initiale"
pause -1