<?php
/*
<tutorial>
<ol>
<li>Loads the 51Degrees PHP module,
<li>Instanciates a match provider,
<li>Prints a list of meta data concerning
the data set file.
</ol>
</tutorial>
*/
// Snippet Start
require("FiftyOneDegreesPatternV3.php");

$provider = FiftyOneDegreesPatternV3::provider_get();

echo "Dataset Published Date: ".$provider->getDataSetPublishedDate()."<br>\n";
echo "Dataset Next Update Date: ".$provider->getDataSetNextUpdateDate()."<br>\n";
echo "Dataset Version: ".$provider->getDataSetFormat()."<br>\n";
echo "Dataset Name: ".$provider->getDataSetName()."<br>\n";
echo "Dataset Device Combinations: ".$provider->getDataSetDeviceCombinations()."<br>\n";
// Snippet End
?>
