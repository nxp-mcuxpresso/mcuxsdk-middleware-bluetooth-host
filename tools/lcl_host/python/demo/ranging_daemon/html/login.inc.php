<?php

    ob_start();
    session_start();

    function check_login($info, $nopwd)
    {
        if (((!empty($info['password']) && 
            $info['password'] == '753951') || $nopwd == true))
            return true;
        return false;
    }
    
    $loggedin = false;
    if (isset($_POST['login']) && check_login($_POST, false)) {
      $_SESSION['valid'] = true;
      $_SESSION['timeout'] = time();
      $_SESSION['username'] = $_POST['username'];
      $loggedin = true;
      //echo '<pre>submit ok<br></pre>';
    }
    if (isset($_SESSION['valid']) && check_login($_SESSION, true)) {
      $loggedin = true;
      //echo '<pre>session ok<br></pre>';
    }
    if (isset($_POST['logout'])) {
      unset($_SESSION['valid']);
      unset($_SESSION['timeout']);
      unset($_SESSION['username']);
      $loggedin = false;
      //echo '<pre>logout ok<br></pre>';
    }
    
    $msg  = '<form class="pure-form pure-form-aligned"  method="post">'."\n";
    $msg  .= '<fieldset>'."\n";
    $msg  .= '    <legend>Login</legend>'."\n";
    $msg  .= '    <div class="pure-control-group">'."\n";
    $msg  .= '        <input name="password" id="password" type="password" placeholder="Password">'."\n";
    $msg  .= '        <button type="submit" class="pure-button pure-button-primary" name="login">Login</button>'."\n";
    $msg  .= '        <a class="pure-button pure-button-secondary" href="/">Back</a>'."\n";
    $msg  .= '    </div>'."\n";
    $msg  .= '</fieldset>'."\n";
    $msg  .= '</form>'."\n";
    $msg  .= ''."\n";
    //$msg  .= '<form class="pure-form pure-form-aligned">'."\n";
    $msg  .= '<fieldset>'."\n";
    $msg  .= '    <legend>Input</legend>'."\n";
    $msg  .= '    <div class="pure-control-group">'."\n";
    $msg  .= '    <table class="pure-table">'."\n";
    $msg  .= '        <tbody>'."\n";
    $msg  .= '            <tr class="pure-table-odd">'."\n";
    $msg  .= '                <td><button class="pure-button" onclick="keyinput(7);">7</button></td>'."\n";
    $msg  .= '                <td><button class="pure-button" onclick="keyinput(8);">8</button></td>'."\n";
    $msg  .= '                <td><button class="pure-button" onclick="keyinput(9);">9</button></td>'."\n";
    $msg  .= '            </tr>'."\n";
    $msg  .= ''."\n";
    $msg  .= '            <tr>'."\n";
    $msg  .= '                <td><button class="pure-button" onclick="keyinput(4);">4</button></td>'."\n";
    $msg  .= '                <td><button class="pure-button" onclick="keyinput(5);">5</button></td>'."\n";
    $msg  .= '                <td><button class="pure-button" onclick="keyinput(6);">6</button></td>'."\n";
    $msg  .= '            </tr>'."\n";
    $msg  .= ''."\n";
    $msg  .= '            <tr class="pure-table-odd">'."\n";
    $msg  .= '                <td><button class="pure-button" onclick="keyinput(1);">1</button></td>'."\n";
    $msg  .= '                <td><button class="pure-button" onclick="keyinput(2);">2</button></td>'."\n";
    $msg  .= '                <td><button class="pure-button" onclick="keyinput(3);">3</button></td>'."\n";
    $msg  .= '            </tr>'."\n";
    $msg  .= ''."\n";
    $msg  .= '            <tr>'."\n";
    $msg  .= '                <td><button class="pure-button" onclick="keyreset();">Reset</button></td>'."\n";
    $msg  .= '                <td><button class="pure-button" onclick="keyinput(0);">0</button></td>'."\n";
    $msg  .= '                <td></td>'."\n";
    $msg  .= '            </tr>'."\n";
    $msg  .= ''."\n";
    $msg  .= '        </tbody>'."\n";
    $msg  .= '    </table>'."\n";
    $msg  .= '    </div>'."\n";
    $msg  .= '</fieldset>'."\n";
    //$msg  .= '</form>'."\n";
    $msg  .= ''."\n";
    $msg  .= '<script type="text/javascript">'."\n";
    $msg  .= ''."\n";
    $msg  .= 'function keyreset() {'."\n";
    $msg  .= '  document.getElementById("password").value  = "";'."\n";
    $msg  .= '}'."\n";
    $msg  .= ''."\n";
    $msg  .= 'function keyinput(val) {'."\n";
    $msg  .= '  var newtxt;'."\n";
    $msg  .= '  newtxt = document.getElementById("password").value + val;'."\n";
    $msg  .= '  document.getElementById("password").value  = newtxt;'."\n";
    $msg  .= '  //document.getElementById("test-info").innerHTML  = newtxt;'."\n";
    $msg  .= '}'."\n";
    $msg  .= ''."\n";
    $msg  .= '</script>'."\n";
    
    //$msg  = '<form class="pure-form pure-form-aligned"  method="post">'."\n";
    //$msg .= '<fieldset>'."\n";
    //$msg .= '    <legend>Login</legend>'."\n";
    //$msg .= '    <div class="pure-control-group">'."\n";
    //$msg .= '        <label for="username">Username</label>'."\n";
    //$msg .= '        <input name="username" id="username" type="text" placeholder="Username">'."\n";
    //$msg .= '    </div>'."\n";
    //$msg .= ''."\n";
    //$msg .= '    <div class="pure-control-group">'."\n";
    //$msg .= '        <label for="password">Password</label>'."\n";
    //$msg .= '        <input name="password" id="password" type="password" placeholder="Password">'."\n";
    //$msg .= '    </div>'."\n";
    //$msg .= '    <div class="pure-controls">'."\n";
    //$msg .= '        <button type="submit" class="pure-button pure-button-primary" name="login">Login</button>'."\n";
    //$msg .= '        <a class="pure-button pure-button-secondary" href="/">Back</a>'."\n";
    //$msg .= '    </div>'."\n";
    //$msg .= '</fieldset>'."\n";
    //$msg .= '</form>'."\n";
    
    $GLOBALS["loginform"] = $msg;
    $GLOBALS["loggedin"] = $loggedin;
?>
