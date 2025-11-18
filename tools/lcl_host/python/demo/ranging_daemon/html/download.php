<?php

  include 'includes.inc.php';

  $fields = array('id', 'limit', 'asc', 'csv', 'log');

  foreach($fields as $field) {
    if(isset($_POST[$field]))
      ${$field} = $_POST[$field];
    else if(isset($_GET[$field]))
      ${$field} = $_GET[$field];
  }

  $where=null;
  if(isset($id) && $id > 0) {
    if(isset($asc)) {
      $where = ' id >= '.$id;
    } else {
      $where = ' id <= '.$id;
    }
  }

  if(isset($asc)) {
    $orderby = 'id ASC';
  } else {
    $orderby = 'id DESC';
  }
  if(isset($limit) && $limit > 0) {
    $orderby .= ' LIMIT '.$limit;
  }

  if(isset($csv) || !isset($log))
    $type='csv';
  else
    $type='log';

  if($loggedin==false){
    echo  "not logged in..";
  } else if($type=='csv') {
    header('Content-Type: application/csv');
    header('Content-Disposition: attachment; filename="filename.csv"');
    $data = "id,returncode,raw,distance\n";
    $results = $db->query('measurement', $where, $orderby);
    while ($row = $results->fetchArray()) {
      $data .= $row['id'].','.$row['returncode'].','.$row['raw'].','.$row['distance']."\n";
    }
    echo $data; exit();
  } else if($type=='log') {
    header('Content-Type: application/txt');
    header('Content-Disposition: attachment; filename="filename.txt"');
    $data = '';
    $results = $db->query('measurement', $where, $orderby);
    while ($row = $results->fetchArray()) {
      $data .= '---- '.$row['id']." ----\n";
      $data .= $row['msg']."\n";
    }
    echo $data; exit();  
  }
?>