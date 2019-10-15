set title "Chainbase vs. Chainrocks (RocksDB) Operating Out Of Physical Memory"
# set xlabel offset 0,1 "hi"
# set xtics font "Verdana,10"
set xlabel offset 0,0.5 "Total Virtual Memory Usage (In MB)"
set ylabel offset 0.75,0 "Transactions Per Second"
set xrange [0:5128]
set label font "Verdana,8" "Physical\nMemory\nLimit (3768MB)" at 2625,4.75 # Make sure to manually draw arrow and dotted line here.
set grid xtics ytics \
    linetype 3 linewidth 0 linecolor rgb "#545454", \
    linetype 3 linewidth 0 linecolor rgb "#545454"

set object rectangle from screen 0,0 to screen 1,1 behind fillcolor rgb '#e6e6e6' fillstyle solid noborder
set object rectangle from screen 0.72,0.12 to screen 0.962,0.87 behind fillcolor rgb '#ff5959' fillstyle solid noborder

plot "data.csv"

# set multiplot
# set size 1, 0.3

# set title "Title goes here" offset 0,2 textcolor "#FFFFFF"

# set key right bottom textcolor "#FFFFFF"

# set grid xtics ytics \
#     linetype 3 linewidth 0 linecolor rgb "#3366AA", \
#     linetype 3 linewidth 0 linecolor rgb "#3366AA"

# set border linecolor rgb "#3366AA"
# set ticslevel 0

# set origin 0.0,0.65
# plot "data.csv" using 1:2 axis x1y1 title "tps"       with linespoints linewidth 0 linecolor rgb "#C466FF"

# set origin 0.0,0.35
# plot "data.csv" using 1:3 axis x1y2 title "cpu load"  with linespoints linewidth 0 linecolor rgb "#FF8266"

# set origin 0.0,0.05
# plot "data.csv" using 1:4 axis x1y2 title "ram usage" with linespoints linewidth 0 linecolor rgb "#EEFF66"
