set title "Chainbase vs. Chainrocks (RocksDB) Operating Out Of Physical Memory\n[BENCHMARK ARGS | CPU | PHYSICAL RAM | DISTRO]"
set xlabel offset 0,0.5 "Total Virtual Memory Usage (In MB)"
set ylabel offset 0.75,0 "Transactions Per Second"
set xrange [*:*]
set yrange [*:*]
set grid xtics ytics \
    linetype 3 linewidth 0 linecolor rgb "#545454", \
    linetype 3 linewidth 0 linecolor rgb "#545454"
set object rectangle from screen 0,0 to screen 1,1 behind fillcolor rgb '#e6e6e6' fillstyle solid noborder

# Modify to increase/decrease the size of the bars.
# The chart may come out looking SUPER funky; that's ok. Just play around with this variable until it looks good.
set boxwidth 130

# Modify the `lw' variables to give the boxes a pleasing aesthetic.
plot 'cumulative_dataset.csv' using 1:3:2:6:5 title "Individual Test Run" with candlesticks lt -1 lw 0.7 whiskerbars,\
     '' using 1:4:4:4:4 with candlesticks lt -1 lw 0.7 notitle
