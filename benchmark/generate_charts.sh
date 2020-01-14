#!/usr/bin/sh

DATA=$(grep -E 'name|median' build/benchmark/*.json \
        | paste -d " " - - \
        | tr -d ',"' \
        | awk '{print $2 " " $4;}'\
        | sed -e 's/GDX\|Oboe/ &/g' \
        | sort -r \
        | sed -E '$!N;/^(\S+)(.*)\n\1/!P;s//\n\1\2/;D' \
        )

CHART_PAYLOAD=$(echo "$DATA" | awk '{print "[\"" $1 "\"," $3*0.000000001 "," $5*0.000000001 "],";}')

cat <<EOF > build/benchmark/chart.html
<html>
  <head>
    <script type="text/javascript" src="https://www.gstatic.com/charts/loader.js"></script>
    <script type="text/javascript">
      google.charts.load('current', {'packages':['bar']});
      google.charts.setOnLoadCallback(drawChart);

      function drawChart() {
        var data = google.visualization.arrayToDataTable([
          ['Method', 'libGDX-Oboe', 'Default'],
          $CHART_PAYLOAD
        ]);

        var options = {
          chart: { title: 'libGDX-Oboe benchmarks' },
          vAxis: { title: 'Time (ms)' }
        };

        var chart = new google.charts.Bar(document.getElementById('columnchart_material'));
        chart.draw(data, google.charts.Bar.convertOptions(options));
      }
    </script>
  </head>
  <body>
    <div id="columnchart_material" style="width: 800px; height: 300px;"></div>
  </body>
</html>
EOF