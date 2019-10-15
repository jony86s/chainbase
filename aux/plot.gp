set title "Chainbase vs. Chainrocks (RocksDB) Operating Out Of Physical Memory"
set xlabel offset 0,0.5 "Total Virtual Memory Usage (In MB)"
set ylabel offset 0.75,0 "Transactions Per Second"
set xrange [0:5128]
set yrange [*:*]
set label font "Verdana,8" "Physical\nMemory\nLimit (3768MB)" at 2625,4.75
set grid xtics ytics \
    linetype 3 linewidth 0 linecolor rgb "#545454", \
    linetype 3 linewidth 0 linecolor rgb "#545454"
set object rectangle from screen 0,0 to screen 1,1 behind fillcolor rgb '#e6e6e6' fillstyle solid noborder
set object rectangle from screen 0.72,0.12 to screen 0.962,0.87 behind fillcolor rgb '#ff5959' fillstyle solid noborder

plot "data.csv"
