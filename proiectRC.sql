-- phpMyAdmin SQL Dump
-- version 3.5.2.2
-- http://www.phpmyadmin.net
--
-- Host: fenrir
-- Generation Time: Jan 01, 2015 at 04:46 PM
-- Server version: 5.1.73-1
-- PHP Version: 5.2.6-1+lenny16

SET SQL_MODE="NO_AUTO_VALUE_ON_ZERO";
SET time_zone = "+00:00";


/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8 */;

--
-- Database: `proiectRC`
--

-- --------------------------------------------------------

--
-- Table structure for table `chat_groups`
--

CREATE TABLE IF NOT EXISTS `chat_groups` (
  `username` varchar(50) NOT NULL,
  `descriptor` int(11) NOT NULL,
  `nume_grup` varchar(50) DEFAULT NULL
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

-- --------------------------------------------------------

--
-- Table structure for table `descriptori_utilizatori_logati_pe_sv`
--

CREATE TABLE IF NOT EXISTS `descriptori_utilizatori_logati_pe_sv` (
  `id` int(11) NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

--
-- Dumping data for table `descriptori_utilizatori_logati_pe_sv`
--

INSERT INTO `descriptori_utilizatori_logati_pe_sv` (`id`) VALUES
(5),
(6);

-- --------------------------------------------------------

--
-- Table structure for table `postari`
--

CREATE TABLE IF NOT EXISTS `postari` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `username` varchar(50) NOT NULL,
  `catre_grup` varchar(20) NOT NULL,
  `titlu` varchar(100) NOT NULL,
  `continut` varchar(500) NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=MyISAM  DEFAULT CHARSET=latin1 AUTO_INCREMENT=51 ;

--
-- Dumping data for table `postari`
--

INSERT INTO `postari` (`id`, `username`, `catre_grup`, `titlu`, `continut`) VALUES
(1, 'a', 'familie', 'Titlu prima stire', 'Continutul primei stiri /////////////////////////////////////////////////////////////////////////////////////////////  Continutul primei stiri '),
(2, 'ionel', 'familie', 'Acesta este un post catre familie', 'Contimnut : doar prietenii care sunt de tip familie il pot vedea. stefanuca este unul dintre ei'),
(3, 'user1', 'coleg', 'Post catre colegii mei.', 'Acesta este un post catre colegii mei. Doar acestea pot vedea ce am postat eu'),
(4, 'a', 'familie', 'Titlu a doua postare', 'Acesta este continutul postarii nr 2'),
(5, 'stefanuca', 'public', 'Postare publica', 'Acesta este continutul unei postari publice'),
(6, 'stefanuca', 'coleg', 'Titlu postare inserare', 'Continut postare inserare'),
(7, 'stefanuca', 'familie', 'titlu stire', 'continut stire'),
(8, 'stefanuca', 'coleg', 'Postare stire testa baza de date', 'Continut postare stire testare baza de date'),
(9, 'stefanuca', 'coleg', 'tddel', 'dfgadaugat cu succes un nou prieten'),
(10, 'a', 'coleg', 'asd', 'asd'),
(11, 'asd', 'coleg', 'titlu stire', 'ana are mere,continutun nou prieten'),
(12, 'ionel', 'public', 'Titlu postare public pentru op=3', 'Continutul POstarii publice . Postat de ionel pentru op =3 '),
(13, 'proiectRc', 'familie', 'Ziua cand prezint proiectul', 'COntinutul dupa ce o sa prezint proiectull\n# Tipul postarii: public\n# Titlul postarii: Titlu postare'),
(14, 'a', 'publice', 'Postare publica nr 3oiectul', 'Continutul postarii publice nr 3%d]=== Setare Profil ===\n'),
(15, 'a', 'publice', 'postare publica nr 3oiectul', 'continutare nr: 79\n'),
(16, 'a', 'colegce', 'asdasdasdublica nr 3oiectul', 'asdasdasare nr: 79\n'),
(17, 'a', 'public', 'asdasdfdfdg sdfsdfsd', 'sdfsdf sdfsdfsdf'),
(18, 'a', 'coleg', 'incercare de a posta', 'continut incercare de a postarieten'),
(19, 'a', 'coleg', 'titlu stire', ''),
(20, 'a', 'coleg', 'titlu stire', 'continutat cu succes un nou prieten'),
(21, 'a', 'coleg', 'titlu stire', 'continutat cu succes un nou prieten'),
(22, 'a', 'coleg', 'ana re mere', 'ana are mereu succes un nou prieten'),
(23, 'a', 'public', 'titlu', 'continut test'),
(24, 'a', 'colegc', 'titlul', 'continutul postarii'),
(25, 'a', 'coleg', 'titlu dupa bzero', 'continut dupa bzero'),
(26, 'a', 'public', 'anabela stire', 'continut stire anabela'),
(27, 'a', 'coleg', 'titlu dipa modificare meniu', 'continut dupa modificare meniu'),
(28, 'a', 'public', 'Postare facuta de Mos Craciun', 'Continutul postarii facute de mos craciun'),
(29, 'stefanuca', 'public', 'titlu mutex stire', 'continut mutex stire user2'),
(30, 'user1', 'coleg', 'titlu user 1 mutex', 'continut stire 1 mutex'),
(31, 'stefanuca', 'familie', 'sdgdfhfghffghfgh', 'sdfsdfsdf'),
(32, 'user1', 'public', 'hojkhdgfdfg dfgdfgd dfgdfgdfg', 'sdfsd sdfsdfsdf sdfsdfs'),
(33, 'p', 'coleg', 's', 'a'),
(34, 'a', 'familie', 'i', 'd'),
(35, 'a', 'coleg', 'a', 's'),
(36, 'p', 'familie', '3', 'h'),
(37, 'a', 'familie', 'ana ', 'are'),
(38, 'a', 'familie', 'postare titlu', 'continut postae'),
(39, 'a', 'coleg', 'titlu postare', 'continut postare'),
(40, 'stefanuca', 'public', 'test postare send public', 'continut postare test send public '),
(41, 'a', 'public', 'titlu test 2 send postare publica', 'continut test 2 send postare public'),
(42, 'a', 'public', 'asdasd', 'bggggggggggggg'),
(43, 'stefanuca', 'familie', 'postare stire dual public', 'continut postare dual public'),
(44, 'stefanuca', 'colegie', 'postare stire dual public apropiati', 'continut stire dual public apropiati'),
(45, 'stefanuca', 'familie', 'titlu', 'stire'),
(46, 'stefanuca', 'coleg', 'titlu', 'continut'),
(47, 'a', 'public', 'titlu postare publica', 'continut postare publica'),
(48, 'stefanuca', 'public', 'titlu postare p', 'continut postare p'),
(49, 'stefanuca', 'public', 'titlu postare pp', 'continut psotare pp'),
(50, 'stefanuca', 'coleg', 'catre colegi postare', 'continut catre colegi psotare');

-- --------------------------------------------------------

--
-- Table structure for table `prieteni`
--

CREATE TABLE IF NOT EXISTS `prieteni` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `cod1_uname` varchar(50) NOT NULL,
  `cod2_uname` varchar(50) NOT NULL,
  `tip` varchar(50) NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=MyISAM  DEFAULT CHARSET=latin1 AUTO_INCREMENT=59 ;

--
-- Dumping data for table `prieteni`
--

INSERT INTO `prieteni` (`id`, `cod1_uname`, `cod2_uname`, `tip`) VALUES
(1, 'stefanuca', 'user1', 'familie'),
(2, 'stefanuca', 'a', 'familie'),
(3, 'stefanuca', 'user1', 'coleg'),
(4, 'stefanuca', 'user1', 'coleg'),
(5, 'stefanuca', 'user1', 'coleg'),
(6, 'ionel', 'stefanuca', 'familie'),
(7, 'a', 'test1', 'coleg'),
(8, 'a', 'nush', 'apropiat'),
(9, 'a', 'mouse', 'coleg'),
(10, 'a', 'ionel', 'coleg'),
(11, 'a', 'df', 'coleg'),
(12, 'a', 'banana', 'coleg'),
(13, 'a', 'aurel', 'coleg'),
(14, 'a', 'fdgdfg', 'familie'),
(15, 'a', 'asd', 'coleg'),
(16, 'a', 'adsa', 'coleg'),
(17, 'a', 'ionel', 'familie'),
(18, 'a', 'ionel', 'familie'),
(19, 'a', 'stefanuca', 'coleg'),
(50, 'user1', 'ionel', 'coleg'),
(22, 'stefanuca', 'ionel', 'familie'),
(23, 'a', 'ionel', 'coleg'),
(24, 'a', 'user6', 'coleg'),
(25, 'asd', 'user6', 'coleg'),
(26, 'proiectRc', 'casetofon', 'coleg'),
(27, 'proiectRc', 'mouse', 'familie'),
(28, 'a', 'user1', 'coleg'),
(29, 'a', 'g', 'coleg'),
(30, 'a', 'vasiliu', 'coleg'),
(31, 'a', 'radu', 'coleg'),
(32, 'a', 'sd', 'coleg'),
(33, 'birsan', 'user1', 'coleg'),
(34, 'stefanuca', 'sd', 'coleg'),
(35, 'a', 'casetofon', 'coleg'),
(36, 'a', 'ghj', 'coleg'),
(37, 'a', 'anabela', 'coleg'),
(38, 'a', 'gh', 'coleg'),
(39, 'a', 'hhj', 'coleg'),
(40, 'a', 'mosCraciun', 'coleg'),
(41, 'a', 'asd', 'coleg'),
(42, 'a', 'asd', 'familie'),
(43, 'a', 'sad', 'coleg'),
(44, 'a', 'p', 'coleg'),
(45, 'p', 'mouse', 'coleg'),
(46, 'a', 'user2', 'coleg'),
(47, 'a', 'asdasd', 'coleg'),
(48, 'stefanuca', 'g', 'coleg'),
(49, 'user1', 'fd', 'coleg'),
(51, 'stefanuca', 'user2', 'coleg'),
(52, 'a', 'medalion', 'coleg'),
(53, 'p', 'g', 'familie'),
(54, 'a', 'aasd', 'coleg'),
(55, 'p', 'medalion', 'cunostinta'),
(56, 'user1', 'vasiliu', 'coleg'),
(57, 'a', 'dfg', 'coleg'),
(58, 'a', 'user3', 'familie');

-- --------------------------------------------------------

--
-- Table structure for table `UTILIZATORI`
--

CREATE TABLE IF NOT EXISTS `UTILIZATORI` (
  `username` varchar(50) NOT NULL,
  `password` varchar(50) NOT NULL,
  `nume_prenume` varchar(100) NOT NULL,
  `tip` int(2) NOT NULL,
  `profil` varchar(6) DEFAULT NULL,
  PRIMARY KEY (`username`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

--
-- Dumping data for table `UTILIZATORI`
--

INSERT INTO `UTILIZATORI` (`username`, `password`, `nume_prenume`, `tip`, `profil`) VALUES
('birsan', 'birsan', 'birsan iulian', 2, '3'),
('vasiliu', 'parolavasi', 'vasiliu cristian', 2, '3'),
('medalion', 'parola', 'medalion prenume', 2, '3'),
('a', 'a', 'df df', 2, '1'),
('asd', 'asd', 'asd asd', 2, '1'),
('casetofon', 'fdg', 'fg sdf', 2, '3'),
('user6', 'sdf', 'dfg gfsd', 2, '3'),
('proiectRc', 'parola', 'numele prenumele', 2, '1'),
('mouse', 'dfs', 'fg fg', 2, '3'),
('dfgdfg', 'dfg', 'sdfsd dfg', 2, '3'),
('ionel', 'parola', 'stefanuca ionel', 2, '3'),
('fgh', 'fgh', 'sdf dfg', 2, '3'),
('dfg', 'hj', 'sfd hgf', 2, '3'),
('hhj', 'g', 'hj gh', 2, '3'),
('gh', 'jh', 'hj hj', 2, '3'),
('sd', 'dg', 'g asd', 2, '3'),
('ghj', 'sdf', 'dgf dgf', 2, '3'),
('g', 'd', 'g d', 2, '3'),
('user2', 'p', 'a a', 2, '2'),
('user1', 'p', 'sdf hg', 2, '1'),
('stefanuca', 'parola', 'stefanuca ionel', 2, '2'),
('radu', 'parolaradu', 'radu andreea', 2, '3'),
('mititeluUser', 'parolamicu', 'mititelu ionut', 2, '3'),
('anabela', 'parola', 'anabela ana', 2, '3'),
('mosCraciun', 'craciunita', 'mos craciun', 2, '3'),
('fd', 'fgasd', 'dfg asd', 2, '3'),
('p', 'p', 'p p', 2, '2'),
('aasd', 'dfgd', 'dfg asd', 2, '3'),
('gfhfghf', 'sdfsd', 'asd asfdg', 2, '3'),
('hkhjkhj', 'fdgdfg', 'asdas dt', 2, '3'),
('hjkghjkhkjh', 'sdfsdfsdf', 'dfgdfgdfg dfgdfdfg', 2, '3'),
('fghgfgggggg', 'asd', 'fg s', 2, '3'),
('dfgdfgdfgdfg', 's', 's s', 2, '3'),
('h', 'd', 's s', 2, '3'),
('user3', 'p', 'p p', 2, '3'),
('jhgh', 's', 'ghg wre', 2, '3'),
('ooooooooo', 'a', 'a a', 2, '3'),
('ooooooooooohhh', 'a', 'a a', 1, '3'),
('ppppppppppppp', 'j', 'j j', 2, '3'),
('admin', 'admin', 'nume prenume', 1, '3'),
('admin1', 'admin1', 'admin parola', 1, '3'),
('metin2', 'parola', 'nume prenume', 2, '3'),
('mmm', 'mmm', 'mmm mmm', 1, '3');

-- --------------------------------------------------------

--
-- Table structure for table `utilizatori_online`
--

CREATE TABLE IF NOT EXISTS `utilizatori_online` (
  `username` varchar(50) NOT NULL,
  `sd_client` int(11) NOT NULL,
  PRIMARY KEY (`username`),
  UNIQUE KEY `username_2` (`username`),
  KEY `username_3` (`username`),
  FULLTEXT KEY `username` (`username`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
