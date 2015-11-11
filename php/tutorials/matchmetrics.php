<?php
/*
<tutorial>
<ol>
<li>Loads the 51Degrees PHP module,
<li>Instanciates a match provider,
<li>Performs a match with requests User-Agent,
<li>Prints a list of match metrics relating
to the match.
</ol>
</tutorial>
*/
// Snippet Start
require("FiftyOneDegreesPatternV3.php");

$provider = FiftyOneDegreesPatternV3::provider_get();

$match = $provider->getMatch($_SERVER["HTTP_USER_AGENT"]);

echo "Device Id: ".$match->getDeviceId()."<br>\n";
echo "Method: ".$match->getMethod()."<br>\n";
echo "Difference: ".$match->getDifference()."<br>\n";
echo "Rank: ".$match->getRank()."<br>\n";
// Snippet End
?>
