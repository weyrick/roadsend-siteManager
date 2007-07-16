<?php 

// set $suite to test suite
include("siteManager_test.php");
//our formater
require_once('formatedTestResults.php');

?>
	<head>
    <STYLE TYPE="text/css">
<?php
    include ("stylesheet.css");
?>
    </STYLE>

  </head>
  <body>
    <h2>Test Results</h2>
<?php
	$result = new formatedTestResult;
	$suite->run($result);

	print('</table>');

	$result->report();
	?>
  </body>
</html>
