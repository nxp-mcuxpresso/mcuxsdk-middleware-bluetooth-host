<?php
  if(empty($_SERVER['CONTENT_TYPE'])) {
    $type = "application/x-www-form-urlencoded";
    $_SERVER['CONTENT_TYPE'] = $type;
  }

  include 'includes.inc.php';

  $where_cond=array();
  if(isset($_POST["id"]) && $_POST["id"] > 0) {
    if(isset($_POST["asc"])) {
      $where_cond[] = ' id >= '.$_POST["id"];
    } else {
      $where_cond[] = ' id <= '.$_POST["id"];
    }
  }
  if(isset($_POST["msg"]))
    $where_cond[] = " msg <> ''";
  $where = implode(' AND ', $where_cond);
  
  if(isset($_POST["asc"])) {
    $orderby = 'id ASC';
  } else {
    $orderby = 'id DESC';
  }
  if(!isset($_POST["limit"]) || $_POST["limit"] == 0)
    $_POST["limit"] = 25;
  $orderby .= ' LIMIT '.$_POST["limit"];
 
echo '<pre>';
  if(isset($_GET["debug"]))
  {
    print_r($_POST);
    print_r($_GET);
    print_r($where);
    print_r($orderby);
  }
echo '</pre>';
?>

<!DOCTYPE html>
<html lang="en">
    <head>
        <meta http-equiv="content-type" content="text/html; charset=UTF-8">
        <meta charset="utf-8">
        <title>Log</title>

        <link rel="icon" type="image/png" href="img/favicon.ico" />
        <link href="css/style.css" rel="stylesheet" type="text/css"/>
        <link href="css/pure-min.css" rel="stylesheet" type="text/css"/>
        <meta name="viewport" content="width=device-width, height=device-height, initial-scale=1">
    </head>

    <?php echo $header;
    
    if(!$loggedin)
    {
      echo $loginform;  
    } else {
    ?>    
    <form class="pure-form pure-form-aligned" method="post" id="updateform">
    <fieldset>
        <legend>Filter</legend>

        <input type="number" name="limit" step="1" min="1" max="9999" placeholder="25" value="<?php echo $_POST["limit"];?>">
        <input type="number" name="id" step="1" min="1" max="9999" placeholder="1" value="<?php echo $_POST["id"];?>">
        <input type="checkbox" name="asc" <?php if(isset($_POST["asc"])) echo 'checked="checked"'; ?> ><span class='pure-form-message-inline'>Ascending</span>
        <input type="checkbox" name="msg" <?php if(isset($_POST["msg"])) echo 'checked="checked"'; ?> ><span class='pure-form-message-inline'>With Msg</span>
        <button type="submit" class="pure-button pure-button-primary" name="update">Update</button>
        <button type="submit" class="pure-button pure-button-secondary" formaction="download.php" name="csv">csv</button>
        <button type="submit" class="pure-button pure-button-secondary" formaction="download.php" name="log">log</button>
        
        </form>
        <form class="pure-form pure-form-aligned" method="post">
        <legend>Measurements</legend>
    
    <table class="pure-table pure-table-bordered">
    <thead>
        <tr>
            <th>ID</th>
            <th>Result</th>
            <th>Raw</th>
            <th>Distance</th>
        </tr>
    </thead>

    <tbody>
    <?php
        $results = $db->query('measurement', $where, $orderby);
        while ($row = $results->fetchArray()) {
          echo '<tr>';
          echo '<td>'.$row['id'].'</td>';
          echo '<td>'.$row['returncode'].'</td>';
          echo '<td>'.$row['raw'].'</td>';
          echo '<td>'.$row['distance'].'</td>';
          echo '</tr>';
        }
    ?>
    </tbody>
    </table>

    </fieldset>
    </form>
<?php } ?>  
</html>
