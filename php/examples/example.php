<?php
/* fiftyone_match function example */
printName("fiftyone_match()");
$result = fiftyone_match($_SERVER['HTTP_USER_AGENT']);
printArrayToTable($result);

/* fiftyone_match_with_headers example */
printName("fiftyone_match_with_headers()");
$headers = "";
//Get all HTTP headers from $_SERVER
foreach ($_SERVER as $key => $value) {
  if (strpos($key, "HTTP_") !== false) {
    $headers = $headers.$key." ".$value."\n";
  }
}
$result_1 = fiftyone_match_with_headers($headers);
printArrayToTable($result_1);

/* fiftyone_info() function example */
printName("fiftyone_info()");
$info = fiftyone_info();
printArrayToTable($info);

/* fiftyone_http_headers function example */
printName("fiftyone_http_headers()");
fiftyone_http_headers();

/**
 * Function prints a string surrounded by formatted div tags.
 * @param $name string of text to print.
 */
function printName($name) {
  echo "<div style=\"width: 100%; border-bottom: 1px solid black; border-top: 1px solid black; text-align: center;\">";
  echo $name;
  echo "</div>";
}

/**
 * Function prints the contents of a supplied array as a table.
 * @param $array is the array to print out. Mandatory.
 * @param $colNames a one dimensional array that containes names of columns. Optional.
 *	  Default values will be used if parameter not provided.
 */
function printArrayToTable($array, $colNames=array("Property","Value","Data Type")) {
  if (!empty($array)) {
    echo "<table>";
    echo "<tbody>";
    //Print table headers
    echo "<tr>";
    foreach($colNames as $key) {
      echo "<th>$key</th>";
    }
    echo "</tr>";
    //Print table contents.
    foreach ($array as $key => $value) {
      echo "<tr>";
      echo "<td>$key</td>";
      echo "<td>";
      //If value is an aray print all entries. Otherwise print the entry.
      if(is_array($value)) {
        foreach($value as $k => $v) {
          echo $v.", ";
        }
      } else {
        echo $value;
      }
      echo "</td>";
      echo "<td>".gettype($value)."</td>";
      echo "</tr>";
    }
    echo "</tbody>";
    echo "</table>";
  }
}
?>

