<?php
/*
 * Copyright 2008-2009  Philipp Wagner <mail@philipp-wagner.com>
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

/**
 * Mensafood class
 *
 * This provides a simple method of getting the food plan from the
 * Studentenwerk Muenchen homepage together with the prices.
 * It parses the page by transforming it into valid XML using tidy and applying
 * XPath expressions to get the important information out of it.
 *
 * It requires PHP 5 with the following extensions:
 * - tidy (install using PECL or (on Debian/Ubuntu) apt-get install php5-tidy)
 *
 * @author Philipp Wagner <mail@philipp-wagner.com>
 */
class MensaFood
{
    const CATEGORY_NORMAL = 1;  // Tagesgericht
    const CATEGORY_ORGANIC = 2; // Biogericht
    const CATEGORY_SPECIAL = 3; // Aktionsessen

    private $date;
    private $prices = null;
    /**
     * Main courses ("Gerichte")
     *
     * @var array
     */
    private $food = null;
    /**
     * Side dishes ("Beilagen")
     *
     * @var array
     */
    private $sides = null;

    private $locations;
    private $locationId;

    /**
     * Default constructor
     *
     * @param DateTime for which date should we get the plan?
     */
    public function __construct($date=null)
    {
        if ($date !== null) {
            $this->date = $date;
        }

        include 'mensen.inc.php';
        $this->locations = $mensen;
    }

    /**
     * Set the date for the food plan
     */
    public function setDate(DateTime $date)
    {
        $this->date = $date;
    }

    private function getDomDocument($url)
    {
        $data = @file_get_contents($url);
        if (!$data) {
            throw new Exception("Unable to fetch document with URL $url");
        }
        $page = $data;

        $tidy = new tidy;
        $tidy->parseString($page, array('output-xml' => true, 'numeric-entities' => true, 'indent' => true, 'add-xml-decl' => true), 'utf8');
        $tidy->cleanRepair();

        $domPage = new DomDocument();
        $domPage->loadXML($tidy);
        return $domPage;
    }

    /**
     * Fetch all prices
     *
     * The prices are available using the $this->prices variable.
     */
    protected function fetchPrices()
    {
        $this->prices = array();
        $url = 'http://www.studentenwerk-muenchen.de/mensa/unsere-preise/';
        $domPage = $this->getDomDocument($url);
        $xpath = new DOMXPath($domPage);
        $categories = array(self::CATEGORY_NORMAL, self::CATEGORY_ORGANIC, self::CATEGORY_SPECIAL);

        foreach ($categories as $category) {
            switch ($category) {
            case self::CATEGORY_NORMAL:
                $query = "//table[@class='essenspreise']/tbody/tr[position()>=2][position()<=4]";
                break;
            case self::CATEGORY_ORGANIC:
            case self::CATEGORY_SPECIAL:
                $query = "//table[@class='essenspreise']/tbody/tr[position()>=11][position()<=10]";
                break;
            }
            $entries = $xpath->query($query);

            $this->prices[$category] = array();
            foreach ($entries as $entry) {
                // number
                $tmp = trim($xpath->query("th[1]", $entry)->item(0)->nodeValue);
                $nr = preg_replace('/^.+gericht\s+(\d+)+.*$/sm', '\1', $tmp);
                // price
                $tmp = trim($xpath->query("td[1]", $entry)->item(0)->nodeValue);
                $price = preg_replace('/^.*(\d+),(\d{2}).*$/sm', '\1.\2', $tmp);

                $this->prices[$category][$nr] = $price;
            }
        }
    }

    /**
     * Get the food prices
     *
     * @return array [category][number]
     */
    public function getPrices()
    {
        if ($this->prices === null) {
            $this->fetchPrices();
        }
        return $this->prices;
    }

    /**
     * Get the official URL of the Studentenwerk page for this day
     *
     * Make sure to set locationId and date first!
     *
     * @return string
     */
    public function getOfficialUrl()
    {
        if (empty($this->locationId) || empty($this->date)) {
           throw new Exception('You need to set locationId and date first');
        }

        // speiseplan_2008-03-10_421_-de.html
        $datef = $this->date->format('Y-m-d');
        return 'http://www.studentenwerk-muenchen.de/mensa/speiseplan/speiseplan_'.$datef.'_'.$this->locationId.'_-de.html';
    }

    /**
     * Fetch all dishes and sides from the Studentenwerk homepage
     *
     * The result is available through getFood() and getSides().
     *
     * @return bool false if there is no plan for this day or problems while fetching data
     */
    public function fetchFood()
    {
        $url = $this->getOfficialUrl();
        try {
            $domPage = $this->getDomDocument($url);
        } catch (Exception $e) {
            return false;
        }

        // get relevant data from DOM document
        $xpath = new DOMXPath($domPage);
        $query = "//table[@class='menu']//tr[position()>1]";
        $entries = $xpath->query($query, $domPage);
        $food = array();
        $sides = array();
        $nowSides = false;
        foreach ($entries as $entry) {
            $leftCell = $xpath->query("td[1]", $entry)->item(0)->nodeValue;
            $rightCell = $xpath->query("td[@class='beschreibung']/span[1]", $entry)->item(0)->nodeValue;

            if ($nowSides || (empty($leftCell) || preg_match('/^Beilagen$/', $leftCell))) {
                // Side dishes
                $nowSides = true;
                $sideDish = trim(
                    preg_replace("/\s+/", ' ',
                        str_replace("\n", ' ', $rightCell)
                    )
                );
                if (preg_match('/^Aktion$/', $leftCell)) {
                    $sideDish .= ' (Aktion)';
                }
                $sides[] = $sideDish;
            } else {
                // Dishes
                // category
                if (strpos($leftCell, 'Tagesgericht') === 0) {
                    $category = self::CATEGORY_NORMAL;
                } elseif (strpos($leftCell, 'Biogericht') === 0) {
                    $category = self::CATEGORY_ORGANIC;
                } elseif (strpos($leftCell, 'Aktionsessen') === 0) {
                    $category = self::CATEGORY_SPECIAL;
                } else {
                    throw new Exception("Unknown category $leftCell");
                }
                // number
                $categoryNumber = preg_replace('/^\w+(?:gericht|essen)\s+(\d).*$/sm', '\1', $leftCell);
                // food name
                $foodName = trim(
                    preg_replace("/\s+/", ' ',
                        str_replace("\n", ' ', $rightCell)
                    )
                );
                $food[] = array(
                    'category' => $category,
                    'categoryNumber' => $categoryNumber,
                    'name' => $foodName
                );
            }
        }

        $this->sides = $sides;
        $this->food = $food;
        
        return (count($this->food) != 0);
    }

    /**
     * Get all dishes
     *
     * Use fetchFood() before you use this method, otherwise you'll get
     * an empty array.
     *
     * @return array
     */
    public function getFood()
    {
        if ($this->food === null) {
            return array();
        }
        return $this->food;
    }

    /**
     * Get all sides
     *
     * Use fetchFood() before you use this method, otherwise you'll get
     * an empty array.
     *
     * @return array
     */
    public function getSides()
    {
        if ($this->sides === null) {
            return array();
        }
        return $this->sides;
    }

    public function getAllLocations()
    {
        return $this->locations;
    }

    /**
     * Set the location for which we want to get the information
     *
     * @param int location ID
     * @return bool false if $locationId is invalid
     */
    public function setLocation($locationId)
    {
        $valid = false;
        foreach ($this->locations as $location) {
            if ($location['id'] == $locationId) {
                $valid = true;
                break;
            }
        }
        if (!$valid) {
            return false;
        }
        $this->locationId = (int)$locationId;
        return true;
    }

    /**
     * Get the location name
     *
     * @return string|false false if the given location ID is invalid
     */
    public function getLocationName()
    {
        foreach ($this->locations as $location) {
            if ($location['id'] === $this->locationId) {
                return $location['name'];
            }
        }
        return false;
    }
}
?>
