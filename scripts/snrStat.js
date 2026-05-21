//
//    This file is a part of nuSolve. nuSolve is a part of CALC/SOLVE system
//    and is designed to perform data analyis of a geodetic VLBI session.
//    Copyright (C) 2021--2023 Sergei Bolotin.
//
//    This program is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
//
//

//const   isTesting = true;
const   isTesting = false;

//
//
//
const   selfName = 'snrStat.js';

var     filter = 'All';
var     doBln = false;
var     doStn = false;
var     doSrc = false;



//
// it should be a standard function:
function padStart(str, length, filler)
{
    if (length <= str.length)
        return str;
    if (filler.length != 1)
        return str;

    //
    var         dest = [];

    for (var i=0; i<length; i++)
        dest.push(filler);

    var j=0;
    for (var i=str.length-1; -1<i; i--)
    {
        dest[length-j-1] = str[i];
        j++;
    };

    // convert it to string:
    var         ret = "";
    for (var i=0; i<length; i++)
        ret += dest[i];

    return ret;
};



function rad2deg(rad)
{
    return rad*180.0/Math.PI;
};





function initCounters(container, counter)
{
    for (var i=0; i<container.length; i++)
        counter[container[i].key] = [];
};



function filterObs(obs, bandKey)
{
    if (filter == 'Good')
        return obs.isUsable(bandKey);
    else if (filter == 'Used')
        return obs.isProcessed;
    else
        return true;
};





function printSnrStats(counter)
{
    var num=0;
    for (var o in counter)
        if (counter[o].length)
        {
            var snrs = counter[o].sort(function(a, b){return a - b}), idx;
            idx = Math.floor(snrs.length/2);
            print(o + '   ' + padStart( snrs[idx].toFixed(3), 9, " ")  + '   ' + 
                padStart(idx.toString(), 3, " ") + '/' + padStart(snrs.length.toString(), 3, " "));
            num++;
        };
    print('Total: ' + num + ' objects');
    print('');
}


//
//
function collectStats(bandKey)
{
    const       selfName = 'collectStats()';
    if (false && isTesting)
    {
        print(selfName + ': making initial setup.');
        print(selfName + ': Setting the number of polynomial terms for station clocks to 3:');
    };
    session.setNumOfClockPolynoms4Stations(3);
//  print(selfName + ': done.');
    if (!session.hasReferenceClocksStation)
    {
        session.pickupReferenceClocksStation();
//      print(selfName + ': the reference clock station was picked up.');
    };
    //
    config.useDelayType = CFG.VD_GRP_DELAY;
    config.opMode       = CFG.OPM_BASELINE;

    config.useExtAPrioriSitesPositions = true;
    config.useExtAPrioriSitesVelocities = true;
    config.useExtAPrioriSourcesPositions = true;
    config.useExtAPrioriErp = false;
    config.useExtAPrioriAxisOffsets = true;
    config.useExtAPrioriHiFyErp = false;
    config.useExtAPrioriMeanGradients = false;

    config.have2ApplyPxContrib = true;
    config.have2ApplyPyContrib = true;
    config.have2ApplyEarthTideContrib = true;
    config.have2ApplyOceanTideContrib = true;
    config.have2ApplyPoleTideContrib = true;
    config.have2ApplyOceanPoleTideContrib = true;
    config.have2ApplyUt1OceanTideHFContrib = true;
    config.have2ApplyPxyOceanTideHFContrib = true;
//  config.have2ApplyNutationHFContrib = true;
    config.have2ApplyUt1LibrationContrib = true;
    config.have2ApplyPxyLibrationContrib = true;

    config.have2ApplyFeedCorrContrib = false;
    config.have2ApplyTiltRemvrContrib = false;
    config.have2ApplyOldOceanTideContrib = false;
    config.have2ApplyOldPoleTideContrib = false;
    config.have2ApplyAxisOffsetContrib = true;
    config.have2ApplyUnPhaseCalContrib = false;
    config.have2ApplyNdryContrib = false;
    config.have2ApplyNwetContrib = false;

    config.activeBandIdx = session.primaryBandIdx;
    //
    //
    if (bandKey == "")
        bandKey = session.bands[session.primaryBandIdx].key;
    else
    {
        var hasBand = false;
        for (var i=0; i<session.bands.length; i++)
            if (bandKey == session.bands[i].key)
                hasBand = true;
        if (!hasBand)
        {
            print(selfName + ': ERROR: the session does not has ' + bandKey + '-band.');
            logger.returnCode = 1;
            return logger.returnCode;
        };
    };
    //
    // add parameters:
    //
    parsDescript.unsetAllParameters();
    parsDescript.setMode4Parameter(Parameters.Clocks,   Parameters.EstimateLocal);
    parsDescript.setMode4Parameter(Parameters.Zenith,   Parameters.EstimateLocal);
    parsDescript.setMode4Parameter(Parameters.Bl_Length,Parameters.EstimateLocal);

    session.process();

    //
    //
    //
    // baselines:
    var blnByKey = Object;
    for (var i=0; i<session.baselines.length; i++)
      blnByKey[session.baselines[i].key] = session.baselines[i];

    var countByBln = new Object();
    initCounters(session.baselines, countByBln);


    // stations:
    var stnByKey = Object;
    for (var i=0; i<session.stations.length; i++)
      stnByKey[session.stations[i].key] = session.stations[i];

    var countByStn = new Object();
    initCounters(session.stations, countByStn);

    // sources:
    var srcByKey = Object;
    for (var i=0; i<session.sources.length; i++)
      srcByKey[session.sources[i].key] = session.sources[i];

    var countBySrc = new Object();
    initCounters(session.sources, countBySrc);





    // collect statistics:
    var observations=session.observations;
    for (var i=0; i<observations.length; i++)
    {
        var obs = observations[i];

        // baselines:
        var blnName  = obs.baseline.key;
        if (filterObs(obs, bandKey))
            countByBln[blnName].push(obs.snr(bandKey));

        // stations:
        var stn1Name  = obs.station_1.key;
        if (filterObs(obs, bandKey))
            countByStn[stn1Name].push(obs.snr(bandKey));
        var stn2Name  = obs.station_2.key;
        if (filterObs(obs, bandKey))
            countByStn[stn2Name].push(obs.snr(bandKey));

        // sources:
        var srcName  = obs.source.key;
        if (filterObs(obs, bandKey))
            countBySrc[srcName].push(obs.snr(bandKey));
    };

    //
    // print the info:
    print("\nSNR median values of " + bandKey + '-band ' + session.name + '\n');
    // baselines:
    if (doBln)
    {
        print("_____Baseline____     __SNR__   idx/Num");
        printSnrStats(countByBln);
    };
    if (doStn)
    {
        print("Station_     __SNR__   idx/Num");
        printSnrStats(countByStn);
    };
    if (doSrc)
    {
        print("_Source_     __SNR__   idx/Num");
        printSnrStats(countBySrc);
    };
};



















function main()
{
    const       pwd = setup.pwd;
    var         isTmp = false;
    var         bandKey="";

    logger.returnCode = 0;

    // check arguments:
    if (!args.length)
    {
        print('\nscript ' + selfName + ' usage:\n');
        print(selfName + ' input <filter>');
        print('where arguments:');
        print('   input         -- a database name (with or without version part) or a wrapper file name');
        print('   filter        -- what observations to count: all, good or used in the analysis');
        logger.returnCode = 1;
        return logger.returnCode;
    };

    if (1 < args.length)
    {
        if (args[1] == 'all')
            filter = 'All';
        else if (args[1] == 'good')
            filter = 'Good';
        else if  (args[1] == 'used')
            filter = 'Used';
        else
        {
            print('Unknown filter: "' + args[1] + '"');
            print('The filter can be: "all", "good", "used". The default is "all"');
            logger.returnCode = 1;
            return logger.returnCode;
        };
    };
    if (4 < args.length)
    {
        if (args[2] == 'Y')
            doBln = true;
        else
            doBln = false;
        if (args[3] == 'Y')
            doStn = true;
        else
            doStn = false;
        if (args[4] == 'Y')
            doSrc = true;
        else
            doSrc = false;
    }
    else // default:
    {
        doBln = true;
        doStn = false;
        doSrc = false;
    };
    if (5 < args.length)
    {
        bandKey = args[5];
        if (bandKey.length != 1)
        {
            print(selfName + ': ERROR: band key "' + bandKey + '" is not supported');
            logger.returnCode = 1;
            return logger.returnCode;
        };
    };


    handler.fileName  = args[0];
    handler.inputType = 'VDB';
    //
    //
    //
    // set up logging:
//  logger.dirName = pwd + '/Logs';
//  logger.fileName = handler.guessedSessionName + '.log';
//  logger.have2store = true;
//  logger.have2store = false;
//  logger.rmLogFile();
    logger.isMute = true;
    logger.write(Log.Inf, Log.Preproc, selfName + ': Starting processing ' + handler.fileName + ' file');

    handler.importSession();
    if (session.isOk)
    {
        collectStats(bandKey);
    }
    else
    {
        print(selfName + ': ERROR: reading the file ' + handler.fileName + ' as ' + handler.inputType + ' type has failed.');
        logger.returnCode = 1;
        return logger.returnCode;
    };
    return logger.returnCode;
};












main();









