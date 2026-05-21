/*
 *
 *    This file is a part of Space Geodetic Library. The library is used by
 *    nuSolve, a part of CALC/SOLVE system, and designed to make analysis of
 *    geodetic VLBI observations.
 *    Copyright (C) 2010-2020 Sergei Bolotin.
 *
 *    This program is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation, either version 3 of the License, or
 *    (at your option) any later version.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include <iostream>
#include <stdlib.h>


#include <SgVlbiNetworkId.h>
#include <SgLogger.h>



/*=======================================================================================================
*
*                           METHODS:
* 
*======================================================================================================*/
//
// static first:
const QString SgVlbiNetworkId::className()
{
  return "SgVlbiNetworkId";
};



//
int SgVlbiNetworkId::countMatches(const QString& key) const
{
  int                           count(0);
  for (int i=0; i<size(); i++)
    if (key.contains(at(i), Qt::CaseInsensitive))
      count++;
  return count;
};




/*=====================================================================================================*/
//
//                           FRIENDS:
// 
/*=====================================================================================================*/
//

/*=====================================================================================================*/
//
// aux functions:
//

/*=====================================================================================================*/
//
// constants:
const QString                   defaultNetIdName("DEFAULT");
QList<SgVlbiNetworkId>          networks(QList<SgVlbiNetworkId>() <<
// this is an unofficial assignments of networks:

//  SgVlbiNetworkId(defaultNetIdName,
//                                  QList<QString>() << "noname" << "default") <<
//
  SgVlbiNetworkId(defaultNetIdName,
                                  QList<QString>() << "noname" << "default") <<
  //
  SgVlbiNetworkId("AOV",          QList<QString>() << "aov") <<
  SgVlbiNetworkId("APSG",         QList<QString>() << "apsg-") <<
  SgVlbiNetworkId("APT",          QList<QString>() << "apt-") <<
  SgVlbiNetworkId("ASIA",         QList<QString>() << "xasia" << "c-asia" << "casia" << "trns-asia") <<
  SgVlbiNetworkId("ASTROM",       QList<QString>() << "astrom-" << "astro " << "astr-") << 
  SgVlbiNetworkId("Atlantic",     QList<QString>() << "e-atl-" << "e.atl-" << "NORTH-ATL" <<
                  "s.atlantic" << "satl" << "sth-atl" << "STH-ATL" << "satlant" << "TRANS-ATL" <<
                  "TRAS-ATL" << "W.ATL-" << "X-ATLNTIC" << "XATL") <<
  SgVlbiNetworkId("AUS-AST",      QList<QString>() << "aus-ast") <<
  SgVlbiNetworkId("AUS-MIX",      QList<QString>() << "aus-mix") <<
  SgVlbiNetworkId("California",   QList<QString>() << "ARI83A" << "CALIFRNIA" << "CDO83O" <<
                  "YUMP" << "JPL83F" << "GSFCB") <<
  SgVlbiNetworkId("CDP",          QList<QString>() << "CDP-") <<
  SgVlbiNetworkId("CONT",         QList<QString>() << "cont" << "rd94-") <<
  SgVlbiNetworkId("CORE",         QList<QString>() << "core" << "core-" << "cor-oh" << "core-naps") <<
  SgVlbiNetworkId("CRF",          QList<QString>() << "crf" << "ivs-crf") <<
  SgVlbiNetworkId("CRL",          QList<QString>() << "crl-") <<
  SgVlbiNetworkId("DSN",          QList<QString>() << "dsn " << "dsn-" << "net-p" << "net-r") <<
  SgVlbiNetworkId("EUROPE",       QList<QString>() << "europe" << "ONSALA-2" << "SHAWE") <<
  SgVlbiNetworkId("FRAME",        QList<QString>() << "frame-") <<
  SgVlbiNetworkId("GEOCAT",       QList<QString>() << "geocat" << "geocat-" << "geoct-") <<
  SgVlbiNetworkId("GEODETIC",     QList<QString>() << "geodetic") <<
  SgVlbiNetworkId("GLOBAL",       QList<QString>() << "global-" << "glob" << "globl-trf" << 
                  "golden" << "gglb" << "glbl-trf" << "GLBL-TRF" << "gldn-glbl" << "x-global" <<
                  "xglobal") <<
  SgVlbiNetworkId("GRAV",         QList<QString>() << "grav") <<
  SgVlbiNetworkId("INT",          QList<QString>() << "INT" << "IN1" << "IN2" << "IN3" << "IN4" <<
                  "iris int" << "irisa int" << "tswz" << "int01-test" << "R&DINT") <<
  SgVlbiNetworkId("IRIS-A",       QList<QString>() << "iris-a" << "iris a" << "irisa") <<
  SgVlbiNetworkId("IRIS-P",       QList<QString>() << "iris-p" << "iris p" << "irisp") <<
  SgVlbiNetworkId("IRIS-S",       QList<QString>() << "iris-s" << "iris s" << "iriss") <<
  SgVlbiNetworkId("IVS-CRF",      QList<QString>() << "ivs-crf" << "ivs-crds" << "ivs-crms" <<
                  "ivs-crfds" << "ivs-crfms") <<
  SgVlbiNetworkId("IVS-E3",       QList<QString>() << "ivs-e3") <<
  SgVlbiNetworkId("IVS-OHIG",     QList<QString>() << "ivs-ohig" << "ivs-ohg") <<
  SgVlbiNetworkId("IVS-R&D",      QList<QString>() << "ivs-r&d" << "ivs-r&d-") <<
  SgVlbiNetworkId("IVS-R1",       QList<QString>() << "IVS-R1" << "IVSR1") << 
  SgVlbiNetworkId("IVS-R4",       QList<QString>() << "IVS-R4" << "IVSR4") << 
  SgVlbiNetworkId("IVS-SUR",      QList<QString>() << "ivs-sur") <<
  SgVlbiNetworkId("IVS-T2",       QList<QString>() << "T2" << "ivs-t2") <<
  SgVlbiNetworkId("JADE",         QList<QString>() << "jade-") <<
  SgVlbiNetworkId("Japan",        QList<QString>() << "gsi-" << "JMS91") <<
  SgVlbiNetworkId("JUPITER",      QList<QString>() << "jupiter") <<
  SgVlbiNetworkId("KSP",          QList<QString>() << "ksp-" << "ksp0" << "ksp9") <<
  SgVlbiNetworkId("Locals",       QList<QString>() << "CDP-85M" << "CDP-86PA" << "FD-TIES" << "GORF-" <<
                  "HK-TIES" << "MO-TIES" << "MOJ13-TIE") <<
  SgVlbiNetworkId("MARS",         QList<QString>() << "mars") <<
  SgVlbiNetworkId("MERIT",        QList<QString>() << "merit") <<
  SgVlbiNetworkId("MV-EUR",       QList<QString>() << "BREST-" << "CARNSTY-" << "GRASSE-" << 
                  "HOHNFRG-" << "METSOVI-" << "TROMSO-" << "mveur-") <<
  SgVlbiNetworkId("NAPS",         QList<QString>() << "CDP-90KA" << "CDP-90KB" << "NAPS-") <<
  SgVlbiNetworkId("NAVex",        QList<QString>() << "navex" << "navyex") <<
  SgVlbiNetworkId("NAVINT",       QList<QString>() << "navint") <<
  SgVlbiNetworkId("NAVNET",       QList<QString>() << "navnet") <<
  SgVlbiNetworkId("NAVtest",      QList<QString>() << "GNUT-" << "NJ2") <<
  SgVlbiNetworkId("NCMN",         QList<QString>() << "ncmn" << "BERM" << "CDP-86N1" << "SEATLE-90") <<
  SgVlbiNetworkId("NEOS-A",       QList<QString>() << "neos-a" << "neos a" << "neosa") <<
  SgVlbiNetworkId("NEOS-B",       QList<QString>() << "neos b" << "neosb" << "neos-b") <<
  SgVlbiNetworkId("Pacific",      QList<QString>() << "E.PACIFC" << "e.pacif" << "east-pac" <<
                  "KASHMOJ" << "KASHIMA" << "north-pac" << "ppm-e" << "ppm-n" << "ppm-s" << "ppm-w" <<
                  "trans-pac" << "W.PACIFC" << "west-pac" << "W.PACIFC" << "xpac" << "n-pac" <<
                  "n.pac-" << "npac-" << "w-pac" << "w.pac-" << "w.pacifc" << "wpac-" << "t-pac-" <<
                  "t.pac-" << "trns-pac" << "x.pac-" << "x-pac" << "pacific") <<
  SgVlbiNetworkId("POLARIS",      QList<QString>() << "POLA-") <<
  SgVlbiNetworkId("POLAR-N",      QList<QString>() << "polar" << "polar-n" << "polar n" << "polarn" <<
                  "POLAR-N") <<
  SgVlbiNetworkId("POLAR-S",      QList<QString>() << "polar-s" << "polar-s" << "polar s" << "polars") <<
  SgVlbiNetworkId("R&D",          QList<QString>() << "r&d-" << "rdv" << "bonn-r&d" << "ES-TEST" <<
                  "ATD-" << "ext-r&d" << "low-elev" << "low-elv" << "phase delay" << "phs-dely" << 
                  "phse-dely" << "phsedly-" << "CDP-85M" << "CDP-86PA" << "R&DINT" << "ggao-r" <<
                  "polrzaton" << "relativity") <<
  SgVlbiNetworkId("REFRAME",      QList<QString>() << "reframe" << "reference" << "ref-") <<
  SgVlbiNetworkId("S2IMAG",       QList<QString>() << "s2imag") <<
  SgVlbiNetworkId("SOUTH",        QList<QString>() << "south") <<
  SgVlbiNetworkId("SYOWA",        QList<QString>() << "syowa") <<
  SgVlbiNetworkId("SHS",          QList<QString>() << "sh-s" << "shs-" << "shs0") <<
  SgVlbiNetworkId("SURVEY",       QList<QString>() << "survey" << "suvey" << "survy" << "sth" << 
                  "surn" << "ss1") <<
  SgVlbiNetworkId("TRF",          QList<QString>() << "trf" << "strf" << "sth-trf" << "gtrf") <<
  SgVlbiNetworkId("U-TIES",       QList<QString>() << "gb-ties" << "haw2-ties" << "hawai-tie" <<
                  "hh-45tie" << "hw-ties" << "ties" << "japan-tie" << "jpn-ties" << "k4-tie" <<
                  "-ties" << " ties" << "wties" << "johannesburg-dss45") <<
  SgVlbiNetworkId("UNIQUE",       QList<QString>() << "azores" << "brm87a" << "bermuda" << "crimea" <<
                  "daito" << "hawaii" << "hofn" << "hd-survey" << "hohen" << "jeg" << "kash-nobey" <<
                  "kirsberg" << "marcus" << "mrcs" << "nj2" << "onsala-tie" << "onsl-wetzl" <<
                  "quake" << "sesh" << "seattle" << "seshan" << "sde" << "suwon" << "tau28" << "tja13" <<
                  "toulouse" << "ussr" << "ussurisk" << "ussuriisk" << "wesk" << "yellow") <<
  SgVlbiNetworkId("VEGA",         QList<QString>() << "vega") <<
  SgVlbiNetworkId("VGOS",         QList<QString>() << "vgos" << "vgos-") <<
  SgVlbiNetworkId("VLBA",         QList<QString>() << "vla-" << "geo-vlba" << "geovlba" << "bb0" <<
                  "bf0" << "vlba-" << "vlba" << "TAU28") <<
  SgVlbiNetworkId("WAPS",         QList<QString>() << "waps") <<
  SgVlbiNetworkId("X-US",         QList<QString>() << "x-us" << "gsfca" << "jpl83e")
  
  );
//
/*=====================================================================================================*/
