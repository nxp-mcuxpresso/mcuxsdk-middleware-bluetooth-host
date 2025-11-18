<?php

class sqldb
{
  //PDO DB object
  var $db = NULL;

  //Array to store result handles from simultaneous queries
  var $results = NULL;
  /*
  * Class Constructor
  * Init SQLite and connect to server based on supplied arguments
  *
  * $db_file  - (str)      - The SQLite file to load (absolute or relative path)
  */
  function __construct( $db_file = '')
  {
    //init default values across the board
    $this->results = array();

    //check the file exists
    if( !is_file( $db_file ) )
      throw new Exception( 'SQite3Database: File '.$db_file.' wasn\'t found.' );

    //set up database properties
    $this->db = new SQLite3($db_file, SQLITE3_OPEN_READWRITE);
    if(!$this->db)
      throw new Exception( 'SQite3Database: could not open: '.$this->db->lastErrorMsg() );
    $this->db->busyTimeout(2000);
  }

  function count( $table)
  {
    $result = $this->db->querySingle("SELECT id FROM $table ORDER BY id DESC");
    if(!$result)
      return 0;
    return $result['id'];
  }

  function delete($table)
  {
    $query = 'DELETE FROM '.$table;
    $result = $this->db->exec($query);
    if(!$result)
      echo $this->db->lastErrorMsg();
    $result = $this->db->exec('VACUUM');
    if(!$result)
      echo $this->db->lastErrorMsg();
  }

  function query( $table, $where = NULL, $order = NULL)
  {
    $query = 'SELECT * FROM '.$table;
    if($where)
      $query = $query . ' WHERE '.$where;
    if($order)
      $query = $query . ' ORDER BY '.$order;
    return $this->db->query($query);
  }

  function update( $table, $set, $where)
  {
    $query = 'UPDATE '.$table;
    $query = $query . ' SET '.$set;
    $query = $query . ' WHERE '.$where.';';
    return $this->db->exec($query);
  }

  function reload()
  {
    $tmp = array();
    $results = $this->query('config');
    while ($row = $results->fetchArray()) {
      $tmp[$row["name"]] = $row["value"];
      switch(substr(strrchr($row["name"],"_"),1))
      {
        case 'lst': // convert serialized list to array
          $tmp[$row["name"]] = explode('|', $row["value"]);
          break;
      }
    }
    return $tmp;
  }

  function apply_submit($post)
  {
    if(!isset($_POST["submit"]))
      return $this->reload();

    $results = $this->query('config');
    while ($row = $results->fetchArray()) {
      $name = $row["name"];
      $sql_value = $row["value"];

      $post_value = '';
      $update = 0;
      if(isset($post[$name]))
      {
        $update = 1;
        $post_value = $post[$name];
      }
      if(count($post)) // is there any post information?
      {
        switch(substr(strrchr($name,"_"),1))
        {
          case 'bl':
            if($post_value != 'on')
              $post_value = 'false';
            else
              $post_value = 'true';
            $update = 1;
            break;
        }
      }
      if($update)
      {
        if($post_value != $sql_value)
        {
          $result = $this->update('config', "value = '" . $post_value . "'", "name = '" . $name . "'");
          if(!$result)
            echo $this->db->lastErrorMsg();
        }
      }
    }
    return $this->reload();
  }
}

$db_file='data/ranging.db';
$db = new sqldb($db_file);
$config = $db->reload();

$GLOBALS["db"] = $db;
$GLOBALS["config"] = $config;
$GLOBALS["config_filename"] = $db_file;

?>
