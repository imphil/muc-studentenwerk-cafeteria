<?php
/*
 * Copyright 2008-2013  Philipp Wagner <mail@philipp-wagner.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

define('CACHE_DIR', dirname(__FILE__).'/cache');
define('CACHE_TIMEOUT', 24*60*60); // cache timeout in seconds (24h)
define('DEBUG', false);

// don't do any input validation here, it's done after the cache lookup
// int cast is enough for now
if (empty($_GET['id'])) {
    $id = 421; // Mensa Arcisstraße - backwards compatibility
} else {
    $id = (int)$_GET['id'];
}

// we cache the results for 24 hours to reduce the load on the Studentenwerk
// homepage and improve our response time
$cacheFile = CACHE_DIR."/menu-$id.rss";
if (!DEBUG && file_exists($cacheFile) && filemtime($cacheFile) > time()-CACHE_TIMEOUT) {
    echo file_get_contents($cacheFile);
    exit;
}

require_once 'mensafood.php';
include 'mensen.inc.php';

setlocale(LC_ALL, 'de_DE');
$categoryNames = array(
    MensaFood::CATEGORY_NORMAL => 'Tagesgericht',
    MensaFood::CATEGORY_ORGANIC => 'Biogericht',
    MensaFood::CATEGORY_SPECIAL => 'Aktionsessen',
    MensaFood::CATEGORY_SELFSERVICE => 'Self-Service',
    MensaFood::CATEGORY_DESSERT => 'Dessert');

// input validation
$idExists = false;
foreach ($mensen as $mensa) {
    if ($mensa['id'] == $id) {
        $idExists = true;
        break;
    }
}
if (!$idExists) {
    // redirect to overview page
    $host  = $_SERVER['HTTP_HOST'];
    $uri   = rtrim(dirname($_SERVER['PHP_SELF']), '/\\');
    $extra = 'index.php';
    header("Location: http://$host$uri/$extra");
}


$rss = '<?xml version="1.0" encoding="utf-8"?>

<rss version="2.0" xmlns:atom="http://www.w3.org/2005/Atom">

  <channel>
    <title>'.htmlspecialchars($mensa['name']).'</title>
    <link>http://www.philipp.wagner.name/cafeteriamenu</link>
    <atom:link href="http://philipp.wagner.name/cafeteriamenu/menu-rss.php?id='.$id.'" rel="self" type="application/rss+xml" />
    <description>Speiseplan des Studentenwerks München - '.htmlspecialchars($mensa['name']).'</description>
    <language>de-DE</language>
    <copyright>Philipp Wagner, mail@philipp-wagner.com</copyright>
    <pubDate>'.date('r').'</pubDate>';

$mensafood = new MensaFood(new DateTime());
$mensafood->setLocation($id);
$prices = $mensafood->getPrices();

for ($i=0; $i<=14; $i++) {
    $date = new DateTime("+$i day");
    $mensafood->setDate($date);
    $res = $mensafood->fetchFood();
    if (!$res) {
        continue;
    }

    // RSS header
    $rss .= '<item><title>'.strftime('%Y-%m-%d %A', $date->format('U')).'</title>';
    $rss .= '<guid isPermaLink="false">'.uniqid('mensafood').'</guid>';
    $rss .= '<link>'.$mensafood->getOfficialUrl().'</link>';
    $rss .= '<description>';

    $desc = '';
    foreach ($mensafood->getFood() as $foodItem) {
        $desc .= '<b>';

        $desc .= $categoryNames[$foodItem['category']];
        if ($foodItem['categoryNumber'] !== null) {
            $desc .= ' '.$foodItem['categoryNumber'];
        }
        $desc .= '</b>';
        if (!empty($prices[$foodItem['category']]) && !empty($prices[$foodItem['category']][$foodItem['categoryNumber']])) {
            $desc .= ' ('.number_format($prices[$foodItem['category']][$foodItem['categoryNumber']], 2, ',', '.').' EUR)';
        }
        $desc .= ': '.$foodItem['name'].'<br>';
    }
    $desc .= '<br>';
    $sides = $mensafood->getSides();
    if (!empty($sides)) {
        $desc .= '<b>Beilagen:</b> '.implode(', ', $mensafood->getSides());
    }

    $rss .= htmlspecialchars($desc);
    $rss .= '</description></item>';
}

$mensafood->fetchFood();
$rss .= '</channel></rss>';
if (!DEBUG) {
    file_put_contents($cacheFile, $rss);
}

header('Content-Type: application/rss+xml');
echo $rss;
?>
