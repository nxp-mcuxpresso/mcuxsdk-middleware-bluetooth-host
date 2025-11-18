<?php
  if(empty($_SERVER['CONTENT_TYPE'])) {
      $type = "application/x-www-form-urlencoded";
      $_SERVER['CONTENT_TYPE'] = $type;
  }

  include 'includes.inc.php';
  
  echo '<pre></pre>'; // for some reason required to get the height of the graph working properly!
?>

<!DOCTYPE html>
<html lang="en">
    <head>
        <meta http-equiv="content-type" content="text/html; charset=UTF-8">
        <meta charset="utf-8">
        <title>Ranging</title>

        <script type="text/javascript" src="js/dygraph.js"></script>
        <script type="text/javascript" src="js/smooth-plotter.js"></script>

        <link rel="icon" type="image/png" href="img/favicon.ico" />
        <link href="css/style.css" rel="stylesheet" type="text/css"/>
        <link href="css/pure-min.css" rel="stylesheet" type="text/css"/>
        <meta name="viewport" content="width=device-width, height=device-height, initial-scale=1">
        <link href="css/dygraph.css" rel="stylesheet" type="text/css" />
    </head>
    <body onload="init();">
    
    <?php echo $header; ?>

    <div class="pure-g" style="height:70%;">
        <div class="pure-u-1-24">
        </div>
        <div class="pure-u-22-24" id="graphdiv" style="width:91%;height:100%;">
        </div>
        <div class="pure-u-1-24">
        </div>
    </div>
    
    <div class="pure-g" >
        <div class="pure-u-2-24">
        </div>
        <div class="pure-u-3-24">
            <button id="zoom-btn" class="pure-button pure-button-secondary " onclick="toggleZoom(this);"></button>
        </div>
        <div class="pure-u-18-24">
            <h4 align="right" id="range-info"> </h4>
        </div>
        <div class="pure-u-1-24">
        </div>
    </div>

<script type="text/javascript">
  const y_axis_range = <?php echo $config['y_axis_range']; ?>;
  const zoomLabelAuto = 'imec'; // 'Auto Y Scale';
  const zoomLabelFixed = '.imec.'; // 'Max('+y_axis_range+') Y Scale';

  if (!window.location.origin) {
    window.location.origin = window.location.protocol + "//" + window.location.hostname + (window.location.port ? ':' + window.location.port: '');
  }

  var redrawEnabled = true;
  var unchangedCount = 2;
  var lastValue = null;
  var lastTimestampValid = Date.now();
  var lastTimestampError = Date.now();
  var rng_info = null;
  var g = null;
  
  function XHR(file, callback) {
    var xhr = new XMLHttpRequest();
    xhr.onreadystatechange = function(){
        if(xhr.readyState === 4 && xhr.status === 200){
            callback(xhr.responseText);
        }
    }
    xhr.open('GET', file, true);
    xhr.send();
  }
  
  function init() {
    redraw();
  }
  
  //g.ready(init);

  function updateActivity()
  {
    var newValue = null;
    var newWave;
    if(g != null && g.numRows() >= 2)
      newValue = g.getValue(g.numRows()-1, 0);
    if(newValue == lastValue || newValue == null || lastValue == null)
    {
      unchangedCount += 1;
      lastTimestampError = Date.now();
    }
    else
    {
      unchangedCount = 0;
      lastTimestampValid = Date.now();
    }
    lastValue = newValue;
    
    if(unchangedCount >= 2 || redrawEnabled==false)
      unchangedCount = 2;
    //if(unchangedCount >= 2)
    if((lastTimestampError - lastTimestampValid) > 2500)
      newWave="img/wireless-off.png";
    else
      newWave="img/wireless-signal.png";
    if(document.getElementById("wave").src != newWave)
      document.getElementById("wave").src=newWave;
  }
  
  function redraw() {
    XHR('data/info.json', function(response) {
      // Parse JSON string into object
      rng_info = JSON.parse(response);
      
      if(rng_info.summary != null)
        document.getElementById("range-info").innerText = rng_info.summary;
      
      if(g == null)
      {
        g = new Dygraph(
          document.getElementById("graphdiv"),
          rng_info.graph_data,
          {             
            legend: 'onmouseover',
            ylabel: 'Distance [m]',
            plotter: [Dygraph.Plotters.fillPlotter, Dygraph.Plotters.errorPlotter, smoothPlotter],
            labelsSeparateLines: true,
            customBars: false,
            labels: rng_info.graph_labels,
            fillAlpha: 0.10, // default = 0.15
            strokeWidth: 2.5, // default = 1.0
          }          // options
        );
        toggleZoom(document.getElementById("zoom-btn"), false);
      }
      else
      {
        if(redrawEnabled)
        {
          g.updateOptions({labels: rng_info.graph_labels, file : rng_info.graph_data});
          if(rng_info.graph_bar != null)
            g.updateOptions({customBars : rng_info.graph_bar});
          else
            g.updateOptions({customBars : false});
        }
      }
      g.updateOptions({ylabel: 'Distance (' + rng_info.unit + ')'});
    });
 
    updateActivity();

    if(rng_info != null)
      setTimeout(redraw, rng_info.period + 10);
    else
      setTimeout(redraw, 1000);
  }

  function toggleZoom(ref, toggle=true) {
    if(g == null)
      return;
    if(ref.innerText == zoomLabelFixed || toggle == false) {
      if(g.numRows() >= 2)
        g.updateOptions({
          dateWindow: null,
          valueRange: [0, y_axis_range]
        });
      ref.innerText = zoomLabelAuto;
      ref.style.backgroundColor = 'lightblue';
    } else {
      if(g.numRows() >= 2)
        g.updateOptions({
          dateWindow: null,
          valueRange: null
        });
      ref.innerText = zoomLabelFixed;
      ref.style.backgroundColor = '';
    }
  }

  function pauzeGraph(ref) {
    if(redrawEnabled) {
      ref.innerText = 'Run';
      ref.style.backgroundColor = 'red';
    } else {
      ref.innerText = 'Pauze';
      ref.style.backgroundColor = '';
    }
    redrawEnabled = !redrawEnabled;
  }

</script>
    
    


    </body>
</html>
