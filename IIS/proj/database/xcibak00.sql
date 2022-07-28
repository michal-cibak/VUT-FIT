-- phpMyAdmin SQL Dump
-- version 4.0.10.20
-- https://www.phpmyadmin.net
--
-- Hostiteľ: localhost
-- Vygenerované: Št 26.Nov 2020, 14:10
-- Verzia serveru: 5.6.40
-- Verzia PHP: 5.3.29

SET SQL_MODE = "NO_AUTO_VALUE_ON_ZERO";
SET time_zone = "+00:00";


/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8 */;

-- ---------------------------------------------------------

--
-- Databáza: `xcibak00`
--

-- --------------------------------------------------------

SET FOREIGN_KEY_CHECKS = 0;
drop table if exists Clen;
drop table if exists Clenove;
drop table if exists Festival;
drop table if exists Hodnoceni;
drop table if exists Interpret;
drop table if exists Rezervace_navstevnika;
drop table if exists Rezervace_uzivatele;
drop table if exists Uzivatel;
drop table if exists Vstupenka;
drop table if exists Vystupujici;
SET FOREIGN_KEY_CHECKS = 1;

-- --------------------------------------------------------

--
-- Štruktúra tabuľky pre tabuľku `Clen`
--

CREATE TABLE IF NOT EXISTS `Clen` (
  `id` int(1) NOT NULL AUTO_INCREMENT,
  `jmeno` varchar(30) COLLATE utf8_czech_ci NOT NULL,
  `prijmeni` varchar(30) COLLATE utf8_czech_ci NOT NULL,
  `pozice` varchar(30) COLLATE utf8_czech_ci NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB  DEFAULT CHARSET=utf8 COLLATE=utf8_czech_ci;

--
-- Štruktúra tabuľky pre tabuľku `Clenove`
--

CREATE TABLE IF NOT EXISTS `Clenove` (
  `interpret_id` int(1) NOT NULL,
  `clen_id` int(1) NOT NULL,
  PRIMARY KEY (`interpret_id`,`clen_id`),
  KEY `fk_clen_id` (`clen_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_czech_ci;

--
-- Štruktúra tabuľky pre tabuľku `Festival`
--

CREATE TABLE IF NOT EXISTS `Festival` (
  `id` int(1) NOT NULL AUTO_INCREMENT,
  `nazev` varchar(30) COLLATE utf8_czech_ci NOT NULL,
  `rocnik` int(1) NOT NULL,
  `misto_konani` varchar(30) COLLATE utf8_czech_ci NOT NULL,
  `datum_od` date NOT NULL,
  `datum_do` date NOT NULL,
  `vstupne_standard` int(1) NOT NULL,
  `vstupne_vip` int(1) NOT NULL,
  `zaplneno` int(1) NOT NULL,
  `kapacita` int(1) NOT NULL,
  `popis` text COLLATE utf8_czech_ci NOT NULL,
  PRIMARY KEY (`id`),
  UNIQUE KEY `nazev_rocnik` (`nazev`,`rocnik`)
) ENGINE=InnoDB  DEFAULT CHARSET=utf8 COLLATE=utf8_czech_ci;

--
-- Štruktúra tabuľky pre tabuľku `Hodnoceni`
--

CREATE TABLE IF NOT EXISTS `Hodnoceni` (
  `interpret_id` int(1) NOT NULL,
  `uzivatel_id` int(1) NOT NULL,
  `hodnoceni` set('1','2','3','4','5') COLLATE utf8_czech_ci NOT NULL,
  PRIMARY KEY (`interpret_id`,`uzivatel_id`),
  KEY `fk_hodnoceni_uzivatel_id` (`uzivatel_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_czech_ci;

--
-- Štruktúra tabuľky pre tabuľku `Interpret`
--

CREATE TABLE IF NOT EXISTS `Interpret` (
  `id` int(1) NOT NULL AUTO_INCREMENT,
  `jmeno` varchar(30) COLLATE utf8_czech_ci NOT NULL,
  `logo_img` varchar(30) COLLATE utf8_czech_ci DEFAULT NULL,
  `datum_vzniku` date NOT NULL,
  `zanr` varchar(30) COLLATE utf8_czech_ci NOT NULL,
  PRIMARY KEY (`id`),
  UNIQUE KEY `jmeno` (`jmeno`)
) ENGINE=InnoDB  DEFAULT CHARSET=utf8 COLLATE=utf8_czech_ci;

--
-- Štruktúra tabuľky pre tabuľku `Rezervace_navstevnika`
--

CREATE TABLE IF NOT EXISTS `Rezervace_navstevnika` (
  `id` int(1) NOT NULL AUTO_INCREMENT,
  `email` varchar(30) COLLATE utf8_czech_ci NOT NULL,
  `festival_id` int(1) NOT NULL,
  `stav` set('neuhrazeno','uhrazeno','zamitnuto') COLLATE utf8_czech_ci NOT NULL DEFAULT 'neuhrazeno',
  PRIMARY KEY (`id`),
  KEY `festival_id` (`festival_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_czech_ci;

--
-- Štruktúra tabuľky pre tabuľku `Rezervace_uzivatele`
--

CREATE TABLE IF NOT EXISTS `Rezervace_uzivatele` (
  `id` int(1) NOT NULL AUTO_INCREMENT,
  `uzivatel_id` int(1) NOT NULL,
  `festival_id` int(1) NOT NULL,
  `stav` set('neuhrazeno','uhrazeno','zruseno','zamitnuto') COLLATE utf8_czech_ci NOT NULL DEFAULT 'neuhrazeno',
  PRIMARY KEY (`id`),
  KEY `uzivatel_id` (`uzivatel_id`,`festival_id`),
  KEY `festival_id` (`festival_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_czech_ci;

--
-- Štruktúra tabuľky pre tabuľku `Uzivatel`
--

CREATE TABLE IF NOT EXISTS `Uzivatel` (
  `id` int(1) NOT NULL AUTO_INCREMENT,
  `prezdivka` varchar(20) COLLATE utf8_czech_ci NOT NULL,
  `heslo` varchar(128) COLLATE utf8_czech_ci NOT NULL,
  `email` varchar(50) COLLATE utf8_czech_ci NOT NULL,
--  `jmeno` varchar(50) COLLATE utf8_czech_ci DEFAULT NULL,
 -- `prijmeni` varchar(50) COLLATE utf8_czech_ci DEFAULT NULL,
--  `vek` int(1) COLLATE utf8_czech_ci DEFAULT NULL,
  `opravneni` set('administrator','poradatel','pokladni','divak') COLLATE utf8_czech_ci NOT NULL DEFAULT 'divak',
  PRIMARY KEY (`id`),
  UNIQUE KEY `prezdivka` (`prezdivka`)
) ENGINE=InnoDB  DEFAULT CHARSET=utf8 COLLATE=utf8_czech_ci;

--
-- Štruktúra tabuľky pre tabuľku `Vstupenka`
--

CREATE TABLE IF NOT EXISTS `Vstupenka` (
  `id` int(1) NOT NULL AUTO_INCREMENT,
  `typ` set('standard','vip') COLLATE utf8_czech_ci NOT NULL DEFAULT 'standard',
  `rezervace_uzivatele_id` int(1) DEFAULT NULL,
  `rezervace_navstevnika_id` int(1) DEFAULT NULL,
  PRIMARY KEY (`id`),
  KEY `fk_rezervace_uzivatele_id` (`rezervace_uzivatele_id`),
  KEY `fk_rezervace_navstevnika_id` (`rezervace_navstevnika_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_czech_ci;

--
-- Štruktúra tabuľky pre tabuľku `Vystupujici`
--

CREATE TABLE IF NOT EXISTS `Vystupujici` (
  `festival_id` int(1) NOT NULL,
  `interpret_id` int(1) NOT NULL,
  `stage` varchar(30) COLLATE utf8_czech_ci NOT NULL,
  `od` datetime NOT NULL,
  `do` datetime NOT NULL,
  PRIMARY KEY (`festival_id`,`interpret_id`),
  KEY `fk_interpret_id` (`interpret_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_czech_ci;

-- ---------------------------------------------------------

--
-- Obmedzenie pre tabuľku `Clenove`
--
ALTER TABLE `Clenove`
  ADD CONSTRAINT `fk_clen_id` FOREIGN KEY (`clen_id`) REFERENCES `Clen` (`id`),
  ADD CONSTRAINT `fk_intrepret_id` FOREIGN KEY (`interpret_id`) REFERENCES `Interpret` (`id`);

--
-- Obmedzenie pre tabuľku `Hodnoceni`
--
ALTER TABLE `Hodnoceni`
  ADD CONSTRAINT `fk_hodnoceni_intrepret_id` FOREIGN KEY (`interpret_id`) REFERENCES `Interpret` (`id`),
  ADD CONSTRAINT `fk_hodnoceni_uzivatel_id` FOREIGN KEY (`uzivatel_id`) REFERENCES `Uzivatel` (`id`);

--
-- Obmedzenie pre tabuľku `Rezervace_navstevnika`
--
ALTER TABLE `Rezervace_navstevnika`
  ADD CONSTRAINT `fk_rezervace_navstevnika_festival_id` FOREIGN KEY (`festival_id`) REFERENCES `Festival` (`id`);

--
-- Obmedzenie pre tabuľku `Rezervace_uzivatele`
--
ALTER TABLE `Rezervace_uzivatele`
  ADD CONSTRAINT `fk_rezervace_uzivatele_festival_id` FOREIGN KEY (`festival_id`) REFERENCES `Festival` (`id`),
  ADD CONSTRAINT `fk_rezervace_uzivatele_uzivatel_id` FOREIGN KEY (`uzivatel_id`) REFERENCES `Uzivatel` (`id`);

--
-- Obmedzenie pre tabuľku `Vstupenka`
--
ALTER TABLE `Vstupenka`
  ADD CONSTRAINT `fk_rezervace_navstevnika_id` FOREIGN KEY (`rezervace_navstevnika_id`) REFERENCES `Rezervace_navstevnika` (`id`),
  ADD CONSTRAINT `fk_rezervace_uzivatele_id` FOREIGN KEY (`rezervace_uzivatele_id`) REFERENCES `Rezervace_uzivatele` (`id`);

--
-- Obmedzenie pre tabuľku `Vystupujici`
--
ALTER TABLE `Vystupujici`
  ADD CONSTRAINT `fk_festival_id` FOREIGN KEY (`festival_id`) REFERENCES `Festival` (`id`),
  ADD CONSTRAINT `fk_interpret_id` FOREIGN KEY (`interpret_id`) REFERENCES `Interpret` (`id`);

-- ---------------------------------------------------------

--
-- Sťahujem dáta pre tabuľku `Clen`
--

INSERT INTO `Clen` (`jmeno`, `prijmeni`, `pozice`) VALUES
('Mark', 'Cici', 'trumpeta'),
('Terry', 'High', 'zpěvák'),
('Nick', 'Melody', 'housle'),
('Petr', 'Soukup', 'kytarista'),
('Terry', 'Rozumberk', 'bubeník'),
('Danny', 'Koumák', 'Flétna'),
('Tomáš', 'Drop', 'zpěvák'),
('Vojtěch', 'Kolumbus', 'zpěvák'),
('Petr', 'Skočnato', 'Trumpeta'),
('Markéta', 'Strašilová', 'Klávesy'),
('David', 'Duše', 'basa'),
('Josef', 'Kotek', 'klávesy'),
('Nikola', 'Smažilová', 'zpěvák'),
('Petr', 'Hiklbery', 'violonchelo'),
('Daniel', 'Landa', 'zpěvák'),
('Tomáš', 'Stajne', 'kytarista'),
('Jerry', 'Hlava', 'kytarista'),
('Aleš', 'Řezivo', 'bubeník'),
('Michal', 'Čibo', 'basa'),
('Roman', 'Skokoč', 'flétna'),
('Frank', 'Lampard', 'bonga'),
('Tomáš', 'Rosický', 'violonchelo'),
('Jan', 'Hašek', 'zpěvák'),
('Lukáš', 'Vykoukal', 'kytarista'),
('Petr', 'Nemehlo', 'kytarista'),
('Jan', 'Trouba', 'bubeník'),
('Alice', 'Palice', 'basa'),
('Michal', 'Čenichal', 'kytara'),
('Pepa', 'Sklepa', 'bubeník'),
('Emil', 'Tlemil', 'Flétna'),
('Jonáš', 'Konáš', 'zpěvák'),
('Livie', 'Palicová', 'zpěvák'),
('Petra', 'Solařová', 'Trumpeta'),
('Amélie', 'Magnézie', 'Klávesy'),
('David', 'Kvapil', 'basa');

--
-- Clenové v kapelách
--
/* zlobí */
/*
INSERT INTO `Clenove` (`interpret_id`, `clen_id`) VALUES
(1, 1),
(1, 2),
(1, 3),
(2, 4),
(2, 5),
(2, 6),
(2, 7),
(3, 8),
(3, 9),
(3, 10),
(3, 11),
(3, 12),
(4, 13),
(4, 14);*/
--
-- Sťahujem dáta pre tabuľku `Festival`
--

INSERT INTO `Festival` (`nazev`, `rocnik`, `misto_konani`, `datum_od`, `datum_do`, `vstupne_standard`, `vstupne_vip`, `zaplneno`, `kapacita`, `popis`) VALUES
('Bubu', 1, 'Brno', '2020-11-16', '2020-11-22', 500, 1500, 0, 2000, 'Best festival in best city question mark?'),
('Hello', 3, 'Hobovník', '2017-06-15', '2017-06-28', 1000, 1800, 0, 750, 'Summer is here.'),
('Hello', 4, 'Hobovník', '2019-06-15', '2019-06-28', 1000, 1800, 0, 750, 'Summer is here again, but year later than expected.'),
('Hello', 5, 'Hobovník', '2020-06-15', '2020-06-28', 1000, 1800, 0, 750, 'Summer is here, this time in time!'),
('Ahoj', 1, 'Praha', '2020-11-10', '2020-12-10', 1000, 2000, 0, 500, 'A long one.'),
('Cesta sem, cesta tam', 1, 'Ostrava', '2021-01-01', '2021-01-14', 500, 1500, 0, 1000, 'New Year yay!'),
('Rock for People', 25, 'Praha', '2019-07-08', '2021-07-15', 1000, 2500, 9000 , 10000, 'Nejlepší rock ve Střední Evropě'),
('Votvírák', 6, 'Milovice', '2021-06-16', '2021-06-18', 400, 1000, 60000 , 100000, 'Největší festival v Česku! Pojďme otevřít prázdniny!');


--
-- Sťahujem dáta pre tabuľku `Interpret`
--

INSERT INTO `Interpret` (`jmeno`, `logo_img`, `datum_vzniku`, `zanr`) VALUES
('Sucho', NULL, '2017-06-15', 'rock'),
('Smrtelníci', NULL, '2010-07-09', 'pop'),
('Hobo Bobo', NULL, '2020-11-15', 'jazz'),
('Kabát', NULL, '1983-8-01', 'rock'),
('Chinaski', NULL, '1993-01-15', 'pop'),
('Divokej Bill', NULL, '1998-06-21', 'pop'),
('Pokáč', NULL, '2020-11-15', 'folk'),
('Michal Horák', NULL, '2020-11-15', 'pop'),
('Traktor', NULL, '2001-02-18try', 'rock'),
('Dymytry', NULL, '2003-04-13', 'metal'),
('Trautenberk', NULL, '2010-12-29', 'metal'),
('Tři sestry', NULL, '1985-11-22', 'punk'),
('No name', NULL, '1996-08-09', 'rock'),
('Daniel Landa', NULL, '1990-06-10', 'rock'),
('Xindl X', NULL, '2008-05-11', 'pop'),
('Atmo music', NULL, '2012-11-01', 'pop'),
('Maniak', NULL, '2016-10-08', 'hip hop');


--
-- Sťahujem dáta pre tabuľku `Uzivatel`
--

INSERT INTO `Uzivatel` (`prezdivka`, `heslo`, `email`, `opravneni`) VALUES
('Leader', '$1$dFKu8SqL$4ztgyI6eWukqyvDYarSvX0', 'administrator@iisproj2020.our', 'administrator'),
('Organizer', '$1$DLMwpUhf$Vyo1be3Q9rHUnHXjk/4LN0', 'poradatel@iisproj2020.our', 'poradatel'),
('CashMan', '$1$khtmCj/x$OIB3mhTJzxeZXuviqA4dz/', 'pokladni@iisproj2020.our', 'pokladni'),
('User', '$1$yDYQMifA$1YrsadGP2m44oAzAcJSJ90', 'divak@iisproj2020.our','divak');

-- ---------------------------------------------------------

/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
