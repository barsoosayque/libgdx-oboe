#!/usr/bin/sh

DATA=$(grep -E 'name|median' build/libgdx-oboe-report/*.json \
        | paste -d " " - - \
        | tr -d ',"' \
        | awk '{print $2 " " $4;}' \
        | sed -e 's/GDX\|Oboe/: &/g' \
        | sort -r \
        | awk -F':' 'NF>1{a[$1] = a[$1]" "$2};END{for(i in a)print i""a[i]}' \
        )

CHART_PAYLOAD=$(echo "$DATA" | awk '{print "[\"" $1 "\"," $3*0.000000001 "," $5*0.000000001 "],";}')

cat <<EOF > build/libgdx-oboe-report/chart.html
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
    <div id="columnchart_material" style="width: 800; height: 500px;"></div>
  </body>
</html>
EOF