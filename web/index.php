<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
  <meta http-equiv="Content-Type" content="text/html; charset=UTF-8" />
  <title>Mensen des Studentenwerks München</title>  
</head>
<body>
<h1>RSS-Feeds</h1>
<p>Für alle Mensen und Cafeterias sind RSS-Feeds mit dem aktuellen Speiseplan
verfügbar.</p>

<ul>
<?php
include('mensen.inc.php');
foreach ($mensen as $mensa):
?>
  <li>
    <a href="menu-rss.php?id=<?php echo $mensa['id']?>"><?php echo htmlspecialchars($mensa['name'])?></a><br/>
    <?php echo htmlspecialchars($mensa['address'])?>
  </li>
<?php
endforeach;
?>
</ul>

<h1>Für alle KDE4-Benutzer: Das Mensaplan-Plasmoid</h1>
<p>Mehr Informationen im Blog:
<a href="http://philipp.wagner.name/blog/2009/10/new-version-of-the-cafeteria-plasmoid/">hier</a> und
<a href="http://philipp.wagner.name/blog/2008/10/whats-for-lunch/"/>hier</a></p>
</body>
</html>
