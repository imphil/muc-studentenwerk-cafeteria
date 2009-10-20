<?php
/*
 * Mensaplan XML output for communication with the Mensaplan plasmoid
 * Copyright 2008  Philipp Wagner <mail@philipp-wagner.com>
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
require_once 'mensafood.php';

// make sure that we get the right output formatting for floats
setlocale(LC_ALL, 'C');

$categoryNames = array(
    MensaFood::CATEGORY_NORMAL => 'Tagesgericht',
    MensaFood::CATEGORY_ORGANIC => 'Biogericht',
    MensaFood::CATEGORY_SPECIAL => 'Aktionsessen');

// get input parameters: action
if (empty($_REQUEST['action']) ||
    !in_array($_REQUEST['action'], array('get_locations', 'get_menu'))) {
    $action = 'get_menu';
} else {
    $action = $_REQUEST['action'];
}

$xml = '<?xml version="1.0" encoding="utf-8"?>
    <cafeteriamenu version="1.0">';

if ($action === 'get_menu') {
    $mensafood = new MensaFood();

    // get input parameters: date
    if (preg_match('/^\d{4}-\d{2}-\d{2}$/', $_REQUEST['date'])) {
        $date = new DateTime($_REQUEST['date']);
    } else {
        $date = new DateTime();
    }
    // get input parameters: location
    if (empty($_REQUEST['location']) || !ctype_digit($_REQUEST['location'])) {
        die('location request parameter missing or not a number');
    } else {
        $mensafood->setLocation($_REQUEST['location']) || die('Invalid location ID');
    }

    $mensafood->setDate($date);

    $xml .= '<menu>
      <location>'.$mensafood->getLocationName().'</location>
      <date>'.$date->format('Y-m-d').'</date>';

    if ($mensafood->fetchFood() === false) {
        $xml .= '<status>closed</status>';
    } else {
        $xml .= '<status>ok</status>';
        $prices = $mensafood->getPrices();
        $dishes = $mensafood->getFood();
        // dishes
        foreach ($dishes as $foodItem) {
            $xml .= '<item>';
            $xml .= '<name>'.$categoryNames[$foodItem['category']].' '.$foodItem['categoryNumber'].'</name>';
            $xml .= '<value>'.$foodItem['name'].'</value>';
            $xml .= '<price>'.$prices[$foodItem['category']][$foodItem['categoryNumber']].'</price>';
            $xml .= "</item>\n";
        }

        // sides
        $xml .= '<item>';
        $xml .= '<name>Beilagen</name>';
        $xml .= '<value>'.implode(', ', $mensafood->getSides()).'</value>';
        $xml .= '<price></price>';
        $xml .= "</item>\n";
    }

    $xml .= '</menu>';
} elseif ($action == 'get_locations') {
    $mensafood = new MensaFood();
    $xml .= '<locations>';
    foreach ($mensafood->getAllLocations() as $location) {
        $xml .= '<location>';
        $xml .= '<name>'.$location['name'].'</name>';
        $xml .= '<address>'.$location['address'].'</address>';
        $xml .= '<id>'.$location['id'].'</id>';
        $xml .= '</location>';
    }
    $xml .= '</locations>';
}

$xml .= '</cafeteriamenu>';

header('Content-Type: text/xml');
echo $xml;
?>
