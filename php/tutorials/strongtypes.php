<?php
/*
<tutorial>
<ol>
<li>Loads the 51Degrees PHP module,
<li>Instanciates a match provider,
<li>Performs a match with requests User-Agent,
<li>Fetches and prints value of SupportedBearers
as a string,
<li>Fetches and prints values of SupportedBearers
as an array.
</ol>
</tutorial>
*/
// Snippet Start
require("FiftyOneDegreesPatternV3.php");

$provider = FiftyOneDegreesPatternV3::provider_get();

$match = $provider->getMatch($_SERVER["HTTP_USER_AGENT"]);

echo "Supported Bearers as String:<br>\n";
$supportedBearers = $match->getValue("SupportedBearers");
echo $supportedBearers."<br>\n";

echo "Supported Bearers as Array:<br>\n";
$supportedBearersArray_in = $match->getValues("SupportedBearers");
for($i=0;$i<$supportedBearersArray_in->size();$i++) {
	$supportedBearersArray[$i] = $supportedBearersArray_in->get($i);
}
foreach($supportedBearersArray as $value) {
	echo $value."<br>\n";
}
// Snippet End
?>
