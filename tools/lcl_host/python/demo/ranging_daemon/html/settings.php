<?php
  if(empty($_SERVER['CONTENT_TYPE'])) {
    $type = "application/x-www-form-urlencoded";
    $_SERVER['CONTENT_TYPE'] = $type;
  }

  include 'includes.inc.php';
  if($loggedin)
  {
    $config = $db->apply_submit($_POST);
    if(isset($_POST["dbrotate"]))
    {
      $result = $db->update('config', "value = 'dbrotate'", "name = 'command'");
      $config['command'] = 'dbrotate';
      if(!$result)
        echo $db->db->lastErrorMsg();
    }
  }

echo '<pre>';
  if(isset($_GET["debug"]))
  {
    print_r($_POST);
    print_r($_SESSION);
    print_r($config);
  }
echo '</pre>';
?>

<!DOCTYPE html>
<html lang="en">
    <head>
        <meta http-equiv="content-type" content="text/html; charset=UTF-8">
        <meta charset="utf-8">
        <title>Settings</title>

        <link rel="icon" type="image/png" href="img/favicon.ico" />
        <link href="css/style.css" rel="stylesheet" type="text/css"/>
        <link href="css/pure-min.css" rel="stylesheet" type="text/css"/>
        <meta name="viewport" content="width=device-width, initial-scale=1">
    </head>
    <body>
    <?php echo $header; ?>
<?php

    function run_cmd($cmd)
    {
      $output = array();
      $return_var = 0;
      exec($cmd, $output, $return_var);
      return $return_var;
    }
    function label($name, $short, $value, $description)
    {
        echo "<div class='pure-control-group'>\n";
        echo "<label for='$name' class='pure-input-1-4'>$short</label>\n";
        echo "<input type='text' id='$name' class='pure-input-1-4' placeholder='$value' readonly>\n";
        echo "<span class='pure-input-2-4'>$description</span>\n";
        echo "</div>\n";
    }

    function submit_button($name, $short, $description)
    {
        echo "<div class='pure-control-group'>\n";
        echo "<label for='$name' class='pure-input-1-4'>$short</label>\n";
        echo "<button type='submit' name='$name' class='pure-button pure-input-1-4 pure-button-primary'>$name</button>\n";
        echo "<span class='pure-input-2-4'>$description</span>\n";
        echo "</div>\n";
    }

    function input_bool($name, $short, $description)
    {
        global $config;
        echo "<div class='pure-control-group'>\n";
        echo "<label for='$name' class='pure-input-1-4'>$short</label>\n";
        echo "<input name='$name' id='$name' type='checkbox' align='right' class='pure-input-1-4'";
        if($config[$name]=='true') echo ' checked';
        echo ">\n<span class='pure-input-2-4'>$description</span>\n";
        echo "</div>\n";
    }

    function input_number($name, $short, $description, $min, $max, $default, $step=1)
    {
        global $config;
        echo "<div class='pure-control-group'>\n";
        echo "<label for='$name' class='pure-input-1-4'>$short</label>\n";
        echo "<input class='pure-input-1-4' name='$name' id='$name' type='number' step='$step' min='$min' max='$max' placeholder='$default' value='$config[$name]'>\n";
        echo "<span class='pure-input-2-4'>$description</span>\n";
        echo "</div>\n";
    }

    function input_combobox($name, $short, $description, $items)
    {
        global $config;
        echo "<div class='pure-control-group'>\n";
        echo "<label for='$name' class='pure-input-1-4'>$short</label>\n";
        echo "<select name='$name' id='$name' class='pure-input-1-4'>\n";
        foreach($items as $item) {
            if($item==$config[$name])
                echo "<option selected='selected'>$item</option>\n";
            else
                echo "<option>$item</option>\n";
        }
        echo "</select>\n<span class='pure-input-2-4'>$description</span>\n";
        echo "</div>\n";
    }

    if($loggedin==false)
    {
      echo $loginform;
    } else {
 ?>
    <form class="pure-form pure-form-aligned" method="post">
    <fieldset>

        <legend>User Status</legend>
        <?php
          submit_button('logout', 'Logout', '');
        ?>
        <legend>Server Status</legend>
        <?php
            if(run_cmd('cat /var/run/rangingd.pid | xargs ps -p')==0)
              label('running', 'Daemon', 'Running', '');
            else
              label('running', 'Daemon', 'Exited', 'Please restart the platform!');

            $load = sys_getloadavg();
            label('load_avg', 'CPU Load', number_format($load[1]*100, 1), '');

            label('ip_server', 'Local IP', shell_exec('hostname -I'), '');

            label('db_size', 'Database size', number_format(filesize($config_filename)/(1024*1024), 2), 'Size in  Megabytes');
            if($config['command'] == '')
              submit_button('dbrotate', 'DB Rotate', 'Store a copy of current DB and start with a empty DB');
         ?>
        <legend>Generic</legend>
        <?php
            $am_description = '0=Single antenna, ';
            $am_description .= '1=Initiator HV polarized, ';
            $am_description .= '2=Reflector HV polarized, ';
            $am_description .= '3=Initiator + Reflector HV polarized, ';
            $am_description .= '4=Initiator ULA (5 pairs), ';
            $am_description .= '5=Reflector ULA (5 pairs), ';
            $am_description .= '6=Initiator ULA (4 pairs), ';
            $am_description .= '7=Reflector ULA (4 pairs)';
            input_combobox('board', 'Board Type', '', $config['board_lst']);
            $bn_description = 'It is possible to range with multiple devices (only unique board numbers are taken into account)';
            input_combobox('board_number_0', 'Remote Board Number (device-0)', $bn_description, $config['known_board_number_lst']);
            input_combobox('board_number_1', 'Remote Board Number (device-1)', $bn_description, $config['known_board_number_lst']);
            input_combobox('ant_mode', 'Antenna mode', $am_description, [0, 1, 2, 3, 4, 5, 6, 7]);
            input_combobox('no_tones', '#Tones', 'Number of tones to use in the measurements', [80, 40, 20]);
            input_bool('ena_tof_bl', 'Enable ToF', 'Enable ToF measurements (MCIQ measurements are always enabled)');
        ?>
        <legend>Visual</legend>
        <?php
          // input_combobox('graph_type', 'Graph type', 'Select which graph to display.', $config['graph_type_lst']);
          input_number('graph_width', 'Depth', 'Maximum number of values in graph', 10, 250, 40, 1);
          input_number('y_axis_range', 'Y Axis range', 'Fixed Y axis max level', 0, 200, 10, 1);
          input_bool('plot_mciq_raw_bl', 'MCIQ', 'Plot raw MCIQ values?');
          input_bool('plot_tof_bl', 'ToF', 'Plot ToF values? If available.');
          input_bool('plot_tof_raw_bl', 'ToF', 'Plot raw ToF values? If available.');
		  input_bool('plot_likeliness_bl', 'Likeliness', 'Plot Likeliness?');
          input_bool('unit_feet_bl', 'Imperial', 'Use feet as distance unit (meters otherwise)');
          input_bool('display_tof_bl', 'ToF', 'Display ToF distance? If available.');
          input_bool('display_rssi_bl', 'RSSI', 'Display RSSI?');
          input_bool('display_velocity_bl', 'Velocity', 'Display Velocity (only available with Kalman tracking)?');
          input_bool('display_likeliness_bl', 'Likeliness', 'Display Likeliness? If available.');
          input_bool('display_security_bl', 'Security Level', 'Display Security Level? If available.');
          input_bool('display_authentication_bl', 'Authentication', 'Display Authentication? If available.');
        ?>
        <legend>Parameters</legend>
        <?php
          input_number('mciq_offset', 'Distance offset', 'Constant distance offset [m]', -15, 15, -1.25, 0.1);
          input_number('tof_offset', 'ToF Distance offset', 'Constant ToF distance offset [m]', -15, 15, -1.25, 0.25);
          input_number('tof_margin', 'ToF Distance Margin', 'Margin of ToF values to determine security-level [m]', 0, 25, 10.0, 0.25);
        ?>
        <div class="pure-controls">
            <button type="submit" name="submit" class="pure-button pure-button-primary">Submit</button>
            <a class="pure-button pure-button-secondary" href="/">Back</a>
        </div>
    </fieldset>
    </form>
<?php } ?>
    </body>
</html>
