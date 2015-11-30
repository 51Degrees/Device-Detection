<?php
if (extension_loaded(FiftyOneDegreesPatternV3)) {
	require("/home/ben/Device-Detection/php/pattern/module/FiftyOneDegreesPatternV3.php");
	$provider = FiftyOneDegreesPatternV3::provider_get();
}
elseif (extension_loaded(FiftyOneDegreesTrieV3)) {
	require("/home/ben/Device-Detection/php/trie/module/FiftyOneDegreesTrieV3.php");
	$provider = FiftyOneDegreesTrieV3::provider_get();
}
else {
	echo "No 51Degrees PHP detector module found!<br>\n Please install and add entry to your php.ini file<br>\n";
}
// List of all relevant HTTP headers
$allHttpHeaders_in = $provider->getHttpHeaders();
for($i=0;$i<$allHttpHeaders_in->size();$i++) {
	$allHttpHeaders[$i] = $allHttpHeaders_in->get($i);
}
// Information about data set.

// Determine if Pattern or Trie detector is used.
$isPattern = True;
$algorithmUsed = "Pattern";
if (strpos($provider->getDataSetName(), 'Trie') !== false) {
	$isPattern = FALSE;
	$algorithmUsed = "Trie";
}

// List of properties as declared in PHP.ini file.
// List may be different to properties contained in the initialised data set.
// Some properties may not be in the data set you are trying to use.
// I.e. Lite data set does not contain properties like IsTablet or DeviceType.
if ($isPattern) {
$iniProperties = ini_get('FiftyOneDegreesPatternV3.property_list');
$properties = explode(",", $iniProperties);
}
elseif ($algorithmUsed == "Trie") {
$iniProperties = ini_get('FiftyOneDegreesTrieV3.property_list');
$properties = explode(",", $iniProperties);
}


if ($properties[1] == null) {
	$properties_in = $provider->getAvailableProperties();
	for ($i=0;$i<$properties_in->size();$i++) {
		$properties[$i] = $properties_in->get($i);
	}
}

// 51Degrees Logo image link.
$logoImage = getWebsiteLink("image", $isPattern);

?>
<!DOCTYPE html>
<html>
<head>
	<link rel="stylesheet" type="text/css" href="https://51degrees.com/Demos/examples.css" class="inline"/>
</head>
<body>
<div class="content">
<p>
<img src="<?=$logoImage; ?>"
alt="51Degrees logo image for PHP C Extension."/>
</p>
<h1>PHP <?=$algorithmUsed; ?> - Device Detection Example</h1>
<?php
printInfoTable($provider, $isPattern);

printHttpHeaders($allHttpHeaders, "User-Agent", array("HTTP_USER_AGENT"));
$match_ua = $provider->getMatch($_SERVER['HTTP_USER_AGENT']);
printArrayToTable($match_ua, $properties, $isPattern, "ua");

printHttpHeaders($allHttpHeaders, "HTTP Headers");
$httpHeaders = "";
//Get all HTTP headers from $_SERVER
foreach ($_SERVER as $key => $value) {
  if (strpos($key, "HTTP_") !== false) {
    $httpHeaders = $httpHeaders.$key." ".$value."\n";
  }
}
$result_1 = $provider->getMatch($httpHeaders);
printArrayToTable($result_1, $properties, $isPattern, 2);


/**
 * Function prints the contents of a supplied array as a table.
 * @param $array is the array to print out. Mandatory.
 * @param $colNames a one dimensional array that containes names of columns. Optional.
 *	  Default values will be used if parameter not provided.
 */
function printArrayToTable($match, $properties, $isPattern, $occurance, $colNames=array("Property","Value","Data Type")) {
	// Columns in the array.
	$columnsInTable = count($colNames);
	// Extra column for the button.
	$columnsInArrayWithButton = $columnsInTable + 1;
	// Number of rows for properties.
	// Id, Rank, Method and Difference are not displayed in the properties
	// section, so we need less rows.
	// Default is for Trie and Trie only provides Id.
	$propertiesRows = count($properties) + 2;
	// Number of Match Metrics rows. Pattern provides more metrcs stats.
	$metricsRowspan = 2;
	if ($isPattern) {
		// Pattern provides more metrics than Trie, hence more rows.
		$metricsRowspan = 5;
	}

    echo "<table>";
    echo "<tbody>";

	// Print metrics section.
	echo "<tr>";
	echo "<th colspan='".$columnsInTable."'>Match Metrics</th>";
	// Print 'About metrics' button.
	echo "<th rowspan='".$metricsRowspan."'>";
	$link = getWebsiteLink("metrics", $isPattern, $occurance);
	echo "<a class='button' target='_blank' href='".$link."'>About Metrics</a>";
	echo "</th>";
	echo "</tr>";
	//Print device Id.
	echo "<tr>";
	echo "<td>Id</td>";
	if (NULL !== $match->getDeviceId()) {
		//For Trie Id is an optional property. May not necessarily be set.
		echo "<td>".$match->getDeviceId()."</td>";
		echo "<td>".gettype($match->getDeviceId())."</td>";
	} else {
		echo "<td>N/A</td>";
		echo "<td>N/A</td>";
	}
	echo "</tr>";
	if ($isPattern) {
		//Print detection method.
		echo "<tr>";
		echo "<td>Method</td>";
		echo "<td>".$match->getMethod()."</td>";
		echo "<td>".gettype($match->getMethod())."</td>";
		echo "</tr>";
		//Difference.
		echo "<tr>";
		echo "<td>Difference</td>";
		echo "<td>".$match->getDifference()."</td>";
		echo "<td>".gettype($match->getDifference())."</td>";
		echo "</tr>";
		//Rank.
		echo "<tr>";
		echo "<td>Rank</td>";
		echo "<td>".$match->getRank()."</td>";
		echo "<td>".gettype($match->getRank())."</td>";
		echo "</tr>";
	}

    //Print column names
	//Print Device properties header
	echo "<tr>";
	echo "<th colspan='".$columnsInTable."'>Device Properties</th>";
	echo "<th rowspan='".$propertiesRows."'>";
	$link = getWebsiteLink("properties", $isPattern, $occurance);
	echo "<a class='button' target='_blank' href='".$link."'>More Properties</a>";
	echo "</th>";
	echo "</tr>";
    echo "<tr>";
    foreach($colNames as $key) {
      echo "<td>$key</td>";
    }
    echo "</tr>";
    //Print table contents.

	if ($properties != NULL) {
		//Dataset was in fact initialised with a list of properties.
		foreach($properties as $value) {
			//Ignore Rank, Id, Method and Difference as they are printed elsewhere.
			if ($value === "SignatureRank" || $value === "Rank" ||
				$value === "Difference" || $value === "Method" || $value ==="Id") {
				continue;
			}
			echo "<tr>";
			echo "<td><a href='https://51degrees.com/resources/property-dictionary#$value'>$value</a></td>";
			if ($match->getValue($value) != NULL) {

				echo "<td>".$match->getValue($value)."</td>";
			} else {
				echo "<td><a href='https://51degrees.com/compare-data-options'>Switch Data Set</a></td>";
			}
			echo "<td>".gettype($match->getValue($value))."</td>";
			echo "</tr>";
		}
	} else {
		//NULL was provided, meaning all properties selected.
		foreach($match->getAvailableProperties() as $key => $value) {
			//Ignore Rank, Id, Method and Difference as they are printed elsewhere.
			if ($key === "SignatureRank" || $key === "Rank" ||
				$key === "Difference" || $key === "Method" || $key ==="Id") {
				continue;
			}
			echo "<tr>";
			echo "<td><a href='https://51degrees.com/resources/property-dictionary#$key'>$key</a></td>";
			echo "<td>".$value."</td>";
			echo "<td>".gettype($value)."</td>";
			echo "</tr>";
		}
	}
    echo "</tbody>";
    echo "</table>";
}

/**
 * Function prints information related to the data file currently in use.
 */
function printInfoTable($provider, $isPattern) {
	//How many columns will be required.
	$columns = 3;

	echo "<table>";
	echo "<tbody>";

	echo "<tr>";
	echo "<th colspan='$columns' class='fod_table_header'>Data Set Information</th>";
	echo "</tr>";

	//Print dataset initialisation message.

	//Third column spans all rows and is used to print 'Compare Data Options' button.
	if ($columns == 3) {
		echo "<td rowspan='6'>";
		$link = getWebsiteLink("compare", $isPattern);
		echo "<a class='button' target='_blank' href='".$link."'>Compare Data Options</a>";
		echo "</td>";
	}
	echo "</tr>";

	//Print dataset Published date. Pattern only.
	echo "<tr>";
	echo "<td>Dataset published:</td>";
	if ($isPattern) {
		echo "<td>".$provider->getDataSetPublishedDate()."</td>";
	} else {
		echo "<td>Not available for Trie.</td>";
	}
	echo "</tr>";
	//Print dataset next update date. Pattern only.
	echo "<tr>";
	echo "<td>Dataset next update:</td>";
	if ($isPattern) {
		echo "<td>".$provider->getDataSetNextUpdateDate()."</td>";
	} else {
		echo "<td>Not available for Trie.</td>";
	}
	echo "</tr>";
	//Print dataset version. Pattern only.
	echo "<tr>";
	echo "<td>Dataset version:</td>";
	if ($isPattern) {
		echo "<td>".$provider->getDataSetFormat()."</td>";
	} else {
		echo "<td>Not available for Trie.</td>";
	}
	echo "</tr>";
	//Print dataset name. Exists for both Pattern and Trie. Both Pattern and Trie.
	echo "<tr>";
	echo "<td>Dataset name:</td>";
	echo "<td>".$provider->getDataSetName()."</td>";
	echo "</tr>";
	//Print device combinations. Pattern only.
	echo "<tr>";
	echo "<td>Dataset device combinations:</td>";
	if ($isPattern) {
		echo "<td>".$provider->getDataSetDeviceCombinations()."</td>";
	} else {
		echo "<td>Not available for Trie.</td>";
	}
	echo "</tr>";

	echo "</tbody>";
	echo "</table>";
}

/**
 * Function prints all relevant headers and values for the relevant HTTP
 * headers that were set in this request.
 */
function printHttpHeaders($array, $name, $headers=array()) {
	if (empty($array)) {
		return null;
	}
	$selective = FALSE;
	if(!empty($headers)) {
		$selective = TRUE;
	}

	echo "<table>";
	echo "<tbody>";

	echo "<tr>";
	echo "<th colspan='2'>Match With $name</th>";
	echo "</tr>";

	foreach ($array as $value) {
		if ($selective) {
			//Only some headers are required.
			//If current header not in the array of required headers skip.
			if (!in_array("HTTP_".strtoupper(str_replace("-","_",$value)), $headers)) {
				continue;
			}
		}
		echo "<tr>";
		echo "<td>";
		echo $value;
		echo "</td>";
		echo "<td>";
		if ($_SERVER["HTTP_".strtoupper(str_replace("-","_",$value))] != NULL) {
		    echo $_SERVER["HTTP_".strtoupper(str_replace("-","_",$value))];
		} else {
		    echo "<i>header not set</i>";
		}
		echo "</td>";
		echo "</tr>";
	}

	echo "</tbody>";
	echo "</table>";
}

/**
 * Function generates a link to 51Degrees based on the purpose of the link, wheather
 * the detector is operating in Pattern or Trie modes and if the link is created for
 * the match with User-Agent function or the Match with Headers function. Guaranteed
 * to return a link, even if the parameter is now known.
 * @returns a link to 51Degrees.com
 */
function getWebsiteLink($purpose, $isPattern, $occurance=1) {
	$url = "https://51degrees.com";
	$utmSource = "utm_source=Github";
	$utmMedium = "utm_medium=repository";
	$utmContent = "example_trie";
	if($isPattern) {
		$utmContent = "example_pattern";
	}
	$utmCampaign = "utm_campaign=php-open-source";
	if ($purpose === 'image') {
		$url = "https://51degrees.com/DesktopModules/FiftyOne/Distributor/Logo.ashx";
	} else if ($purpose === 'metrics') {
		$url = "https://51degrees.com/support/documentation/howdevicedetectionworks/trie";
		if ($isPattern) {
			$url = "https://51degrees.com/support/documentation/pattern";
		}
		if ($occurance == 2) {
			$utmContent .= "_header";
		} else {
			$utmContent .= "_ua";
		}
	} else if ($purpose === 'compare') {
		$url = "https://51degrees.com/compare-data-options";
		$utmContent .= "_compare";
	} else if ($purpose === 'properties') {
		$url = "https://51degrees.com/resources/property-dictionary";
		if ($occurance == 2) {
			$utmContent .= "_properties_header";
		} else {
			$utmContent .= "_properties_ua";
		}
	}
	return $url."?".$utmSource."&".$utmMedium."&".$utmContent."&".$utmCampaign;
}

?>
</div>
</body>
</html>
